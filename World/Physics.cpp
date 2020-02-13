#include <world/Physics.h>
#include "headers/World.h"
#include "headers/obj_properties.h"
#include "ProjEd/headers/InspEditAreas.h"

extern InspectorWin* _inspector_win;

void Engine::PhysicalProperty::addColliderRadio(){
    AreaRadioGroup* group = new AreaRadioGroup; //allocate button layout
    group->value_ptr = reinterpret_cast<uint8_t*>(&this->coll_type);
    group->go_property = static_cast<void*>(this);

    QRadioButton* box_radio = new QRadioButton; //allocate first radio
    box_radio->setText("Box");
    QRadioButton* cube_radio = new QRadioButton;
    cube_radio->setText("Cube");
    QRadioButton* sphere_radio = new QRadioButton;
    sphere_radio->setText("Sphere");
    QRadioButton* convex_radio = new QRadioButton;
    convex_radio->setText("Convex Hull");
    QRadioButton* mesh_radio = new QRadioButton;
    mesh_radio->setText("Mesh");
    //add created radio buttons
    group->addRadioButton(box_radio);
    group->addRadioButton(cube_radio);
    group->addRadioButton(sphere_radio);
    group->addRadioButton(convex_radio);
    group->addRadioButton(mesh_radio);
    //Register in Inspector
    _inspector_win->registerUiObject(group);
    _inspector_win->getContentLayout()->addLayout(group->btn_layout);
}
void Engine::PhysicalProperty::addMassField(){
    FloatPropertyArea* mass_area = new FloatPropertyArea;
    mass_area->setLabel("Mass"); //Its label
    mass_area->value = &this->mass;
    mass_area->go_property = static_cast<void*>(this);
    _inspector_win->addPropertyArea(mass_area);
}

void Engine::PhysicalProperty::addCustomSizeField(){
    BoolCheckboxArea* cust = new BoolCheckboxArea;
    cust->setLabel("Custom size ");
    cust->go_property = static_cast<void*>(this);
    cust->bool_ptr = &this->isCustomPhysicalSize;
    _inspector_win->addPropertyArea(cust);

    if(isCustomPhysicalSize){
        Float3PropertyArea* cust = new Float3PropertyArea; //New property area
        cust->setLabel("Scale"); //Its label
        cust->vector = &this->cust_size; //Ptr to our vector
        cust->go_property = static_cast<void*>(this);
        _inspector_win->addPropertyArea(cust);

    }
}

