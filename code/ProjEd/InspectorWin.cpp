#include "headers/InspectorWin.h"
#include "headers/InspEditAreas.h"
#include "headers/ProjectEdit.h"
#include "ui_inspector_win.h"

#include <world/tile_properties.h>
#include "../World/headers/World.h"
#include <QDoubleValidator>
#include <QObject>
#include <thread>

extern EditWindow* _editor_win;

InspectorWin::InspectorWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::InspectorWin),
    ThumbnailUpdateRequired(false),
    updateRequired(false),
    gameobject_ptr(nullptr),

    paintTileButton(nullptr),
    addObjComponentBtn(nullptr),
    line(nullptr)
{
    ui->setupUi(this);

    this->ui->propertySpace->setMargin(0);
    this->ui->propertySpace->setSpacing(5);
    this->ui->propertySpace->setContentsMargins(5,1,2,0);
    this->ui->propertySpace->setAlignment(Qt::AlignTop);

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
    AddGoComponentDialog* dialog = new AddGoComponentDialog(gameobject_ptr); //Creating dialog instance
    dialog->exec(); //show dialog
    updateObjectProperties(); //update prop interface
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
    this->property_areas.clear(); //No areas in list
    this->additional_objects.clear(); //No objects in list

    //remove buttons
    if(paintTileButton != nullptr){
        delete paintTileButton;
        paintTileButton = nullptr;
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

    if (_editor_win->project.perspective == PERSP_2D && gameobject_ptr->getPropertyPtr<Engine::TileProperty>() != nullptr) {
        paintTileButton = new QPushButton;
        paintTileButton->setText("Paint Tile");
        ui->propertySpace->addWidget(paintTileButton);
    }

    connect(addObjComponentBtn, SIGNAL(clicked()), this, SLOT(onAddComponentBtnPressed()));
    connect(paintTileButton, SIGNAL(clicked()), this, SLOT(onPaintTileClicked()));
}

void InspectorWin::onPaintTileClicked() {
    _editor_win->ppaint_state.enabled = true;
    _editor_win->ppaint_state.prop_ptr = gameobject_ptr->getPropertyPtr<Engine::TileProperty>();
}

void InspectorWin::ShowObjectProperties(Engine::GameObject* object_ptr){
    clearContentLayout(); //Clears everything in content layout
    //unpick old object
    _editor_win->world.unpickObject();
    //Object is picked now
    object_ptr->pick();
    //Add setActive() checkbox
    BoolCheckboxArea* isActive = new BoolCheckboxArea;
    isActive->setLabel("Active ");
    isActive->go_property = nullptr;
    isActive->pResultBool = &object_ptr->mActive;
    addPropertyArea(isActive);
    //Next add all property areas
    unsigned int props_num = static_cast<unsigned int>(object_ptr->props_num);
    unsigned int scripts_num = static_cast<unsigned int>(object_ptr->scripts_num);
    //iterate over props to show them all
    for(unsigned int prop_it = 0; prop_it < props_num; prop_it ++){ //iterate over all properties and send them to inspector
        Engine::IGameObjectComponent* property_ptr = object_ptr->mComponents[prop_it]; //Obtain pointer to object property
        //Add property to inspector
        addPropertyInterfaceToInspector(property_ptr);
    }
    for (unsigned int script_it = 0; script_it < scripts_num; script_it++) { //iterate over all scripts and send them to inspector
        Engine::ZPScriptProperty* script_ptr = object_ptr->mScripts[script_it]; //Obtain pointer to object script
        //Add script to inspector
        addPropertyInterfaceToInspector((Engine::IGameObjectComponent*)script_ptr);
    }
    gameobject_ptr = object_ptr;
    addPropButtons(); //add buttons
    
}

void InspectorWin::addPropertyInterfaceToInspector(Engine::IGameObjectComponent* property_ptr) {
    AreaPropertyTitle* prop_title = new AreaPropertyTitle(property_ptr);

    getContentLayout()->addLayout(&prop_title->layout);
    this->registerUiObject(prop_title);

    property_ptr->addPropertyInterfaceToInspector(); //Add its interface to inspector
}

void InspectorWin::updateObjectProperties(){
    if(gameobject_ptr != nullptr) //if gameobject_ptr is not nullptr
        ShowObjectProperties(gameobject_ptr); //Show properties
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
        //get edit areas amount
        size_t areas_num = this->property_areas.size();
        for(size_t area_i = 0; area_i < areas_num; area_i ++){
            PropertyEditArea* pea_ptr = this->property_areas[area_i]; //Obtain pointer to area
            pea_ptr->updateValues(); //Update state on it.
        }
    }else{
        updateRequired = false; //Unset flag
        this->updateObjectProperties(); //Reload UI
    }
    this->ThumbnailUpdateRequired = false;
}

void InspectorWin::onPropertyChange(){
    this->onPropertyEdited();
}

AddGoComponentDialog::AddGoComponentDialog(Engine::GameObject* game_object_ptr, QWidget* parent)
    : QDialog (parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint){
    g_object_ptr = game_object_ptr;
    //Allocating buttons
    this->add_btn = new QPushButton;
    this->close_btn = new QPushButton;
    this->comp_type = new QLineEdit;
    //Allocation of main layout
    contentLayout = new QGridLayout;
    //Add list widget to grid layout
    contentLayout->addWidget(&property_list, 0, 0);
    //Iterate over all base properties and add them to list
    Engine::GameObject* obj = static_cast<Engine::GameObject*>(g_object_ptr);
    for(int i = 1; i <= 16; i ++){
        //Check, if property already exist
        if(obj->getPropertyPtrByTypeI(i) == nullptr)
            QListWidgetItem* item = new QListWidgetItem(getPropertyString((PROPERTY_TYPE)i), &this->property_list, i);
    }
    new QListWidgetItem(getPropertyString((PROPERTY_TYPE)1000), &this->property_list, 1000);
    new QListWidgetItem(getPropertyString((PROPERTY_TYPE)1001), &this->property_list, 1001);
    //Add button
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
    QListWidgetItem* item = this->property_list.currentItem();
    PROPERTY_TYPE newtype = static_cast<PROPERTY_TYPE>(item->type());
    //Obtain pointer to object
    Engine::GameObject* object_ptr = static_cast<Engine::GameObject*>(this->g_object_ptr);
    if (newtype != PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT) {
        object_ptr->addProperty(newtype);
        auto prop_ptr = object_ptr->getPropertyPtrByType(newtype);
        prop_ptr->onAddToObject();
    }
    else
        object_ptr->addScript();

    accept(); //Close dialog with true
    return;
}
AddGoComponentDialog::~AddGoComponentDialog(){
    delete this->comp_type;
    delete this->add_btn;
    delete this->close_btn;
}

