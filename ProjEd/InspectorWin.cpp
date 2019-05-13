#include "headers/InspectorWin.h"
#include "headers/InspEditAreas.h"
#include "headers/ProjectEdit.h"
#include "ui_inspector_win.h"

#include "../World/headers/World.h"
#include <QDoubleValidator>
#include <QObject>


InspectorWin::InspectorWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::InspectorWin)
{
    updateRequired = false;
    ui->setupUi(this);

    this->ui->propertySpace->setMargin(0);
    this->ui->propertySpace->setSpacing(5);
    this->ui->propertySpace->setContentsMargins(5,1,2,0);

     managePropButton = nullptr;
     line = nullptr;
     this->addObjComponentBtn = nullptr;

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
    ManageComponentDialog* dialog = new ManageComponentDialog(this, gameobject_ptr);
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

    //remove buttons
    if(managePropButton != nullptr){
        delete managePropButton;
        managePropButton = nullptr;
    }
    if(addObjComponentBtn != nullptr){
        delete addObjComponentBtn;
        addObjComponentBtn = nullptr;
    }
    if(line != nullptr){
        delete line;
        line = nullptr;
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

void InspectorWin::addPropButtons(){
    //add divider
    line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    ui->propertySpace->addWidget(line);
    //add buttons
    addObjComponentBtn = new QPushButton;
    addObjComponentBtn->setText("Add Property");
    ui->propertySpace->addWidget(addObjComponentBtn);

    managePropButton = new QPushButton;
    managePropButton->setText("Manage");
    ui->propertySpace->addWidget(managePropButton);

    connect(addObjComponentBtn, SIGNAL(clicked()), this, SLOT(onAddComponentBtnPressed()));
    connect(managePropButton, SIGNAL(clicked()), this, SLOT(onManagePropButtonPressed()));
}

void InspectorWin::ShowObjectProperties(void* object_ptr){
    clearContentLayout(); //Clears everything in content layout
    GameObject* obj_ptr = static_cast<GameObject*>(object_ptr);
    obj_ptr->world_ptr->unpickObject();
    obj_ptr->pick(); //Object is picked now
    //Add setActive() checkbox
    BoolCheckboxArea* isActive = new BoolCheckboxArea;
    isActive->setLabel("Active ");
    isActive->go_property = nullptr;
    isActive->bool_ptr = &obj_ptr->active;
    addPropertyArea(isActive);
    //Next add all property areas
    unsigned int props_num = static_cast<unsigned int>(obj_ptr->props_num);
    //iterate over props to show them all
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
    //Getting number of areas
    unsigned int areas_num = static_cast<unsigned int>(this->property_areas.size());
    //iterate over all areas
    for(unsigned int area_i = 0; area_i < areas_num; area_i ++){
        PropertyEditArea* pea_ptr = this->property_areas[area_i]; //Obtain pointer to area
        pea_ptr->writeNewValues(); //Update state on it.
        areas_num = static_cast<unsigned int>(this->property_areas.size()); //recount amount
    }

}

void InspectorWin::updateAreasChanges(){
    //Check, if some property requested to redraw all
    if(this->updateRequired == false){
        unsigned int areas_num = static_cast<unsigned int>(this->property_areas.size());
        for(unsigned int area_i = 0; area_i < areas_num; area_i ++){
            PropertyEditArea* pea_ptr = this->property_areas[area_i]; //Obtain pointer to area
            pea_ptr->updateValues(); //Update state on it.
        }
    }else{
        updateRequired = false; //Unset flag
        this->updateObjectProperties(); //Reload UI
    }
}

void InspectorWin::onPropertyChange(){
    this->onPropertyEdited();
}

void AddGoComponentDialog::onAddButtonPressed(){
    GameObject* object_ptr = static_cast<GameObject*>(this->g_object_ptr);
    object_ptr->addProperty(static_cast<PROPERTY_TYPE>(comp_type->text().toInt()));

    auto prop_ptr = object_ptr->getPropertyPtrByType(static_cast<PROPERTY_TYPE>(comp_type->text().toInt()));
    prop_ptr->onAddToObject();

    accept(); //Close dialog with true
}

ManageComponentDialog::ManageComponentDialog(InspectorWin* win, void* g_object_ptr, QWidget* parent) :
    QDialog (parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint) {

    ctx_menu = new PropertyCtxMenu(win, this);
    this->g_object_ptr = g_object_ptr;
    GameObject* obj_ptr = static_cast<GameObject*>(g_object_ptr); //cast pointer
    for(int prop_i = 0; prop_i < static_cast<int>(obj_ptr->props_num); prop_i ++){ //iterate over all properties
        GameObjectProperty* prop_ptr = obj_ptr->properties[prop_i]; //obtain property pointer
        new QListWidgetItem(getPropertyString(prop_ptr->type), &this->property_list);
    }

    this->close_btn.setText("Close");

    contentLayout.addWidget(&property_list, 0, 0);
    contentLayout.addWidget(&close_btn, 1, 1);

    connect(&close_btn, SIGNAL(clicked()), this, SLOT(reject()));
    connect(&property_list, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onPropertyDoubleClick()));

    setLayout(&contentLayout);
    this->setWindowTitle("Manage properties");
}

ManageComponentDialog::~ManageComponentDialog(){

}

void ManageComponentDialog::onPropertyDoubleClick(){
    this->ctx_menu->show(QCursor::pos());
}

void ManageComponentDialog::deleteProperty(){
    GameObject* obj_ptr = static_cast<GameObject*>(g_object_ptr); //cast pointer

    QListWidgetItem* item = property_list.currentItem(); //Get pressed item
    QString text = item->text(); //get text of pressed item
    int item_ind = 0; //iterator
    for(int i = 0; i < static_cast<int>(obj_ptr->props_num); i ++){ //Iterate over all properties in object
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
    EditWindow* editwin = static_cast<EditWindow*>(win->editwindow_ptr);

    editwin->ppaint_state.enabled = true;

    GameObject* obj_ptr = static_cast<GameObject*>(this->dialog->g_object_ptr); //cast pointer

    QListWidgetItem* item = dialog->property_list.currentItem(); //Get pressed item
    QString text = item->text(); //get text of pressed item
    for(int i = 0; i < static_cast<int>(obj_ptr->props_num); i ++){ //Iterate over all properties in object
        GameObjectProperty* prop_ptr = obj_ptr->properties[i];
        if(getPropertyString(prop_ptr->type).compare(text) == 0){
            editwin->ppaint_state.prop_ptr = obj_ptr->properties[i];
        }
    }
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

