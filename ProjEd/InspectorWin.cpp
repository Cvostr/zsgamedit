#include "headers/InspectorWin.h"
#include "headers/ProjectEdit.h"
#include "ui_inspector_win.h"

#include "../World/headers/World.h"
#include <QDoubleValidator>
#include <QObject>

InspectorWin::InspectorWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::InspectorWin)
{
    updateAreas = true; //update areas
    ui->setupUi(this);
    addObjComponentBtn = nullptr;
    this->ui->propertySpace->setMargin(0);
    this->ui->propertySpace->setSpacing(0);
    this->ui->propertySpace->setContentsMargins(0,0,0,0);
}

InspectorWin::~InspectorWin()
{
    delete ui;
}

QVBoxLayout* InspectorWin::getContentLayout(){
    return ui->propertySpace;
}

void InspectorWin::onAddComponentBtnPressed(){
    AddGoComponentDialog* dialog = new AddGoComponentDialog; //Creating dialog instance
    dialog->g_object_ptr = gameobject_ptr; //Assign this pointer to make property adding work
    dialog->exec();

    ShowObjectProperties(gameobject_ptr);

    delete dialog; //Free dialog
}

void InspectorWin::clearContentLayout(){
    unsigned int areas_num = static_cast<unsigned int>(this->property_areas.size());
    unsigned int objs_num = static_cast<unsigned int>(this->additional_objects.size());
    for(unsigned int area_i = 0; area_i < areas_num; area_i ++){ //Iterate over all added areas
        property_areas[area_i]->destroyContent();
        property_areas[area_i]->destroyLayout(); //remove all
        delete property_areas[area_i];
    }
    for(unsigned int obj_i = 0; obj_i < objs_num; obj_i ++){ //Iterate over all added areas
       delete additional_objects[obj_i]; //remove all
    }
    this->property_areas.resize(0); //No areas in list
    this->additional_objects.resize(0); //No objects in list
    if(addObjComponentBtn != nullptr){
        delete addObjComponentBtn; //Remove button
        addObjComponentBtn = 0x0; //Avoid crashes
    }
}

void InspectorWin::addPropertyArea(PropertyEditArea* area){
    area->setup(); //Call setup of area
    area->addToInspector(this);
    this->property_areas.push_back(area); //Add area pointer to list of areas
}

void InspectorWin::registerUiObject(QObject* object){
    this->additional_objects.push_back(object);
}

void InspectorWin::makeAddObjComponentBtn(){
    addObjComponentBtn = new QPushButton; //Allocation of button
    addObjComponentBtn->setText("Add property");
    ui->propertySpace->addWidget(addObjComponentBtn);
    connect(addObjComponentBtn, SIGNAL(clicked()), this, SLOT(onAddComponentBtnPressed()));
}

void InspectorWin::ShowObjectProperties(void* object_ptr){
    clearContentLayout(); //Clears everything in content layout
    GameObject* obj_ptr = static_cast<GameObject*>(object_ptr);
    obj_ptr->world_ptr->unpickObject();
    obj_ptr->pick(); //Object is picked now

    unsigned int props_num = static_cast<unsigned int>(obj_ptr->props_num);
    for(unsigned int prop_it = 0; prop_it < props_num; prop_it ++){ //iterate over all properties and send them to inspector
        GameObjectProperty* property_ptr = (obj_ptr->properties[prop_it]); //Obtain pointer to object property
        property_ptr->addPropertyInterfaceToInspector(this); //Add its interface to inspector
    }
    makeAddObjComponentBtn();
    gameobject_ptr = static_cast<void*>(obj_ptr);
}

void InspectorWin::updateObjectProperties(){
    ShowObjectProperties(gameobject_ptr);
}


void InspectorWin::area_update(){
    if(updateAreas == false) return; //if trigger is false, then exit
    unsigned int areas_num = static_cast<unsigned int>(this->property_areas.size());
    for(unsigned int area_i = 0; area_i < areas_num; area_i ++){
        PropertyEditArea* pea_ptr = this->property_areas[area_i]; //Obtain pointer to area
        pea_ptr->updateState(); //Update state on it.
    }
}


void AddGoComponentDialog::onAddButtonPressed(){
    GameObject* object_ptr = static_cast<GameObject*>(this->g_object_ptr);
    object_ptr->addProperty(comp_type->text().toInt());
    accept(); //Close dialog with true
}

AddGoComponentDialog::AddGoComponentDialog(QWidget* parent)
    : QDialog (parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint){
    //Allocating buttons
    this->add_btn = new QPushButton;
    this->close_btn = new QPushButton;

    this->comp_type = new QLineEdit;
    //Allocation of main layout
    contentLayout = new QGridLayout;

    contentLayout->addWidget(comp_type, 0, 0);
    contentLayout->addWidget(add_btn, 1, 0);
    contentLayout->addWidget(close_btn, 1, 1);
    setLayout(contentLayout);

    add_btn->setText("Add");
    close_btn->setText("Close");
    //Connect to slot
    connect(add_btn, SIGNAL(clicked()), this, SLOT(onAddButtonPressed()));
    connect(close_btn, SIGNAL(clicked()), this, SLOT(reject()));

    this->setWindowTitle("Add component");
}
AddGoComponentDialog::~AddGoComponentDialog(){
    delete this->comp_type;
    delete this->add_btn;
    delete this->close_btn;
}

