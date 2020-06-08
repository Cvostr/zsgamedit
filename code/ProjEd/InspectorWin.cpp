#include "headers/InspectorWin.h"
#include "headers/InspEditAreas.h"
#include "headers/ProjectEdit.h"
#include "ui_inspector_win.h"

#include "../World/headers/World.h"
#include <QDoubleValidator>
#include <QObject>
#include <thread>

extern EditWindow* _editor_win;

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
     gameobject_ptr = nullptr;

     QWidget* widget = new QWidget;
     widget->setLayout(getContentLayout());
     delete this->ui->scrollAreaWidgetContents;
     ui->scrollArea->setWidget(widget);
     updatePropertyStateLock = false;
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

    _editor_win->world.unpickObject();

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
        Engine::GameObjectProperty* property_ptr = obj_ptr->properties[prop_it]; //Obtain pointer to object property

        AreaPropertyTitle* prop_title = new AreaPropertyTitle;
        prop_title->prop_title.setText(getPropertyString(property_ptr->type));

        if(!property_ptr->active) //if property is disabled
            prop_title->prop_title.setStyleSheet("QLabel { color : gray; }"); //then set text color to gray

        getContentLayout()->addLayout(&prop_title->layout);
        this->registerUiObject(prop_title);

        property_ptr->addPropertyInterfaceToInspector(); //Add its interface to inspector
    }
    addPropButtons(); //add buttons
    gameobject_ptr = static_cast<void*>(obj_ptr);
}

void InspectorWin::updateObjectProperties(){
    if(gameobject_ptr != nullptr) //if gameobject_ptr is not nullptr
        ShowObjectProperties(gameobject_ptr); //Show properties
}

void InspectorWin::onPropertyEdited(){
    //updatePropertyStateLock = true;
    //Getting number of areas
    unsigned int areas_num = static_cast<unsigned int>(this->property_areas.size());
    //iterate over all areas
    for(unsigned int area_i = 0; area_i < areas_num; area_i ++){
        PropertyEditArea* pea_ptr = this->property_areas[area_i]; //Obtain pointer to area
        pea_ptr->writeNewValues(); //Update state on it.
        areas_num = static_cast<unsigned int>(this->property_areas.size()); //recount amount
    }
    //updatePropertyStateLock = false;
}

void InspectorWin::updateAreasChanges(){
    //Check, if some property requested to redraw all
    if(this->updateRequired == false){

        unsigned int areas_num = static_cast<unsigned int>(this->property_areas.size());
        for(unsigned int area_i = 0; area_i < areas_num; area_i ++){
           // if(this->updatePropertyStateLock == true)
             //   return;
            PropertyEditArea* pea_ptr = this->property_areas[area_i]; //Obtain pointer to area
            pea_ptr->updateValues(); //Update state on it.
        }
        //this->updatePropertyStateLock = false;
    }else{
        updateRequired = false; //Unset flag
        this->updateObjectProperties(); //Reload UI
    }
}

void InspectorWin::onPropertyChange(){
    this->onPropertyEdited();
}

ManageComponentDialog::ManageComponentDialog(InspectorWin* win, void* g_object_ptr, QWidget* parent) :
    QDialog (parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint) {

    ctx_menu = new PropertyCtxMenu(win, this);
    this->g_object_ptr = g_object_ptr;
    refresh_list();

    this->close_btn.setText("Close");

    contentLayout.addWidget(&property_list, 0, 0);
    contentLayout.addWidget(&close_btn, 1, 1);

    connect(&close_btn, SIGNAL(clicked()), this, SLOT(reject()));
    connect(&property_list, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onPropertyDoubleClick()));

    setLayout(&contentLayout);
    this->setWindowTitle("Manage properties");
}

void ManageComponentDialog::refresh_list(){
    this->property_list.clear();

    GameObject* obj_ptr = static_cast<GameObject*>(g_object_ptr); //cast pointer
    for(int prop_i = 0; prop_i < static_cast<int>(obj_ptr->props_num); prop_i ++){ //iterate over all properties
        Engine::GameObjectProperty* prop_ptr = obj_ptr->properties[prop_i]; //obtain property pointer
        QListWidgetItem* item = new QListWidgetItem(getPropertyString(prop_ptr->type), &this->property_list);
        if(!prop_ptr->active)
            item->setTextColor(QColor(Qt::gray));
    }
}

ManageComponentDialog::~ManageComponentDialog(){

}

void ManageComponentDialog::onPropertyDoubleClick(){
    this->ctx_menu->selected_property_index = this->property_list.currentIndex().row();
    this->ctx_menu->show(QCursor::pos());
}

void ManageComponentDialog::deleteProperty(){
    GameObject* obj_ptr = static_cast<GameObject*>(g_object_ptr); //cast pointer

    getActionManager()->newGameObjectAction(obj_ptr->getLinkToThisObject());

    QListWidgetItem* item = property_list.currentItem(); //Get pressed item
    QString text = item->text(); //get text of pressed item
    int item_ind = 0; //iterator
    for(int i = 0; i < static_cast<int>(obj_ptr->props_num); i ++){ //Iterate over all properties in object
        Engine::GameObjectProperty* prop_ptr = obj_ptr->properties[i];
        if(getPropertyString(prop_ptr->type).compare(text) == 0){
            item_ind = i;
        }
    }

    obj_ptr->removeProperty(item_ind);

    refresh_list();
    this->win->updateObjectProperties();
    //accept();
}

PropertyCtxMenu::PropertyCtxMenu(InspectorWin* win, ManageComponentDialog* dialog, QWidget* parent) : QObject(parent){
    //Allocate Qt stuff
    this->menu = new QMenu(win);
    this->win = win;
    this->dialog = dialog;

    this->action_delete = new QAction("Delete", win);
    this->action_paint_prop = new QAction("Paint", win);
    this->toggle_active = new QAction("Active", win);

    menu->addAction(toggle_active);
    menu->addAction(action_delete);
    menu->addAction(action_paint_prop);

    QObject::connect(this->action_delete, SIGNAL(triggered(bool)), this, SLOT(onDeleteClicked()));
    QObject::connect(this->action_paint_prop, SIGNAL(triggered(bool)), this, SLOT(onPaintClicked()));
    QObject::connect(this->toggle_active, SIGNAL(triggered(bool)), this, SLOT(onActiveToggleClicked()));
}
void PropertyCtxMenu::show(QPoint point){
    GameObject* obj_ptr = static_cast<GameObject*>(this->win->gameobject_ptr); //cast pointer

    Engine::GameObjectProperty* prop_ptr = obj_ptr->properties[this->selected_property_index];
    //No actions with label property
    if(prop_ptr->type == GO_PROPERTY_TYPE_LABEL) return;

    if(!prop_ptr->active)
        this->toggle_active->setText("Activate");
    else
        this->toggle_active->setText("Deactivate");

    this->menu->popup(point);


}
void PropertyCtxMenu::onDeleteClicked(){
    dialog->deleteProperty();
}
void PropertyCtxMenu::onPaintClicked(){
    _editor_win->ppaint_state.enabled = true;

    GameObject* obj_ptr = static_cast<GameObject*>(this->dialog->g_object_ptr); //cast pointer

    QListWidgetItem* item = dialog->property_list.currentItem(); //Get pressed item
    QString text = item->text(); //get text of pressed item
    for(int i = 0; i < static_cast<int>(obj_ptr->props_num); i ++){ //Iterate over all properties in object
        Engine::GameObjectProperty* prop_ptr = obj_ptr->properties[i];
        if(getPropertyString(prop_ptr->type).compare(text) == 0){
            _editor_win->ppaint_state.prop_ptr = obj_ptr->properties[i];
        }
    }
}

void PropertyCtxMenu::onActiveToggleClicked(){
    GameObject* obj_ptr = static_cast<GameObject*>(this->win->gameobject_ptr); //cast pointer
    //Calculate property pointer
    Engine::GameObjectProperty* prop_ptr = obj_ptr->properties[this->selected_property_index];

    //Make action
    getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);
    //Change state
    prop_ptr->active = !prop_ptr->active;

    this->win->updateObjectProperties();
    dialog->refresh_list();
}

AddGoComponentDialog::AddGoComponentDialog(QWidget* parent)
    : QDialog (parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint){
    //Allocating buttons
    this->add_btn = new QPushButton;
    this->close_btn = new QPushButton;

    this->comp_type = new QLineEdit;
    //Allocation of main layout
    contentLayout = new QGridLayout;

    contentLayout->addWidget(&property_list, 0, 0);
    //Iterate over all base properties and add them to list
    for(int i = 1; i <= 16; i ++){
        new QListWidgetItem(getPropertyString(i), &this->property_list);
    }
    new QListWidgetItem(getPropertyString(1000), &this->property_list);
    new QListWidgetItem(getPropertyString(1001), &this->property_list);

    //contentLayout->addWidget(comp_type, 0, 0);
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
void AddGoComponentDialog::onAddButtonPressed(){
    QString component_label = this->property_list.currentItem()->text();
    for(int i = 1; i <= 1001; i ++){
        if(getPropertyString(i).compare(component_label) == false){
            GameObject* object_ptr = static_cast<GameObject*>(this->g_object_ptr);
            object_ptr->addProperty(static_cast<PROPERTY_TYPE>(i));

            auto prop_ptr = object_ptr->getPropertyPtrByType(static_cast<PROPERTY_TYPE>(i));
            prop_ptr->onAddToObject();

            accept(); //Close dialog with true
            return;
        }
    }

}
AddGoComponentDialog::~AddGoComponentDialog(){
    delete this->comp_type;
    delete this->add_btn;
    delete this->close_btn;
}

