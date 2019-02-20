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

    this->ui->propertySpace->setMargin(0);
    this->ui->propertySpace->setSpacing(5);
    this->ui->propertySpace->setContentsMargins(5,1,2,0);

     addObjComponentBtn.setText("Add property");
     managePropButton.setText("Manage");

     line.setFrameShape(QFrame::HLine);
     line.setFrameShadow(QFrame::Sunken);

     connect(&addObjComponentBtn, SIGNAL(clicked()), this, SLOT(onAddComponentBtnPressed()));
     connect(&managePropButton, SIGNAL(clicked()), this, SLOT(onManagePropButtonPressed()));

     QWidget* widget = new QWidget;
     widget->setLayout(getContentLayout());
     delete this->ui->scrollAreaWidgetContents;
     ui->scrollArea->setWidget(widget);
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
    dialog->exec(); //show dialog
    updateObjectProperties(); //update prop interface
    delete dialog; //Free dialog
}

void InspectorWin::onManagePropButtonPressed(){
    ManageComponentDialog* dialog = new ManageComponentDialog(gameobject_ptr);
    dialog->win = this;
    dialog->exec();
    updateObjectProperties();
    delete dialog;
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
    this->property_areas.clear(); //No areas in list
    this->additional_objects.clear(); //No objects in list
    //remove divider
    getContentLayout()->removeWidget(&line);
    //remove buttons
    getContentLayout()->removeWidget(&addObjComponentBtn);
    getContentLayout()->removeWidget(&managePropButton);

}

void InspectorWin::addPropertyArea(PropertyEditArea* area){
    area->setup(); //Call setup of area
    area->addToInspector(this);
    this->property_areas.push_back(area); //Add area pointer to list of areas
}

void InspectorWin::registerUiObject(QObject* object){
    this->additional_objects.push_back(object);
}

void InspectorWin::addPropButtons(){
    //add divider
    ui->propertySpace->addWidget(&line);
    //add buttons
    ui->propertySpace->addWidget(&addObjComponentBtn);
    ui->propertySpace->addWidget(&managePropButton);
}

void InspectorWin::ShowObjectProperties(void* object_ptr){
    clearContentLayout(); //Clears everything in content layout
    GameObject* obj_ptr = static_cast<GameObject*>(object_ptr);
    obj_ptr->world_ptr->unpickObject();
    obj_ptr->pick(); //Object is picked now

    unsigned int props_num = static_cast<unsigned int>(obj_ptr->props_num);
    for(unsigned int prop_it = 0; prop_it < props_num; prop_it ++){ //iterate over all properties and send them to inspector
        GameObjectProperty* property_ptr = (obj_ptr->properties[prop_it]); //Obtain pointer to object property

        AreaPropertyTitle* prop_title = new AreaPropertyTitle;
        prop_title->prop_title.setText(getPropertyString(property_ptr->type));
        getContentLayout()->addLayout(&prop_title->layout);
        this->registerUiObject(prop_title);

        property_ptr->addPropertyInterfaceToInspector(this); //Add its interface to inspector
    }
    addPropButtons(); //add buttons
    gameobject_ptr = static_cast<void*>(obj_ptr);
}

void InspectorWin::updateObjectProperties(){
    ShowObjectProperties(gameobject_ptr);
}

void InspectorWin::onPropertyEdited(){
    if(updateAreas == false) return; //if trigger is false, then exit
    unsigned int areas_num = static_cast<unsigned int>(this->property_areas.size());
    for(unsigned int area_i = 0; area_i < areas_num; area_i ++){
        PropertyEditArea* pea_ptr = this->property_areas[area_i]; //Obtain pointer to area
        pea_ptr->writeNewValues(); //Update state on it.
        areas_num = static_cast<unsigned int>(this->property_areas.size()); //recount amount
    }
}

void InspectorWin::updateAreasChanges(){
    if(updateAreas == false) return; //if trigger is false, then exit
    unsigned int areas_num = static_cast<unsigned int>(this->property_areas.size());
    for(unsigned int area_i = 0; area_i < areas_num; area_i ++){
        PropertyEditArea* pea_ptr = this->property_areas[area_i]; //Obtain pointer to area
        pea_ptr->updateValues(); //Update state on it.
    }
}

void InspectorWin::onPropertyChange(){
    this->onPropertyEdited();
}

void AddGoComponentDialog::onAddButtonPressed(){
    GameObject* object_ptr = static_cast<GameObject*>(this->g_object_ptr);
    object_ptr->addProperty(comp_type->text().toInt());

    auto prop_ptr = object_ptr->getPropertyPtrByType(comp_type->text().toInt());
    prop_ptr->onAddToObject();

    accept(); //Close dialog with true
}

ManageComponentDialog::ManageComponentDialog(void* g_object_ptr, QWidget* parent) :
    QDialog (parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint) {

    ctx_menu = new PropertyCtxMenu(win, this);
    this->g_object_ptr = g_object_ptr;
    GameObject* obj_ptr = static_cast<GameObject*>(g_object_ptr); //cast pointer
    for(int prop_i = 0; prop_i < obj_ptr->props_num; prop_i ++){
        GameObjectProperty* prop_ptr = obj_ptr->properties[prop_i]; //obtain property pointer
        new QListWidgetItem(getPropertyString(prop_ptr->type), &this->property_list);
    }

    this->close_btn.setText("Close");

    contentLayout.addWidget(&property_list, 0, 0);
    contentLayout.addWidget(&close_btn, 1, 1);

    connect(&close_btn, SIGNAL(clicked()), this, SLOT(reject()));
    connect(&property_list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onPropertyDoubleClick()));

    setLayout(&contentLayout);
    this->setWindowTitle("Manage properties");
}

ManageComponentDialog::~ManageComponentDialog(){

}

void ManageComponentDialog::onPropertyDoubleClick(){
    this->ctx_menu->show(QPoint(0,0));
}

void ManageComponentDialog::deleteProperty(){
    GameObject* obj_ptr = static_cast<GameObject*>(g_object_ptr); //cast pointer

        QListWidgetItem* item = property_list.currentItem();
        QString text = item->text();
        int item_ind = 0;
        for(int i = 0; i < obj_ptr->props_num; i ++){
            GameObjectProperty* prop_ptr = obj_ptr->properties[i];
            if(getPropertyString(prop_ptr->type).compare(text) == 0){
                item_ind = i;
            }
        }

        obj_ptr->removeProperty(item_ind);
        accept();
}

PropertyCtxMenu::PropertyCtxMenu(InspectorWin* win, ManageComponentDialog* dialog, QWidget* parent) : QObject(parent){
    //Allocate Qt stuff
    this->menu = new QMenu(win);
    this->win = win;
    this->dialog = dialog;

    this->action_delete = new QAction("Delete", win);
    this->action_paint_prop = new QAction("Paint", win);

    menu->addAction(action_delete);
    menu->addAction(action_paint_prop);

    QObject::connect(this->action_delete, SIGNAL(triggered(bool)), this, SLOT(onDeleteClicked()));
    QObject::connect(this->action_paint_prop, SIGNAL(triggered(bool)), this, SLOT(onPainClicked()));
}
void PropertyCtxMenu::show(QPoint point){
    this->menu->popup(point);
}
void PropertyCtxMenu::onDeleteClicked(){
    dialog->deleteProperty();
}
void PropertyCtxMenu::onPainClicked(){

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

