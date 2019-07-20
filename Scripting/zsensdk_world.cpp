#include "headers/LuaScript.h"
#include "headers/zsensdk.h"
#include "../World/headers/obj_properties.h"
#include "../World/headers/2dtileproperties.h"
#include "../Render/headers/zs-math.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include <QString>
#include <iostream>
#include <SDL2/SDL.h>

extern EditWindow* _editor_win;

GameObject* ZSENSDK::ZSENGmObject::updPtr(){
    this->object_ptr = world_ptr->getObjectByStringId(this->str_id);
    return object_ptr;
}

std::string ZSENSDK::ZSENGmObject::getLabel(){
    return this->updPtr()->label->toStdString();
}
void ZSENSDK::ZSENGmObject::setLabel(std::string label){
    this->updPtr()->setLabel(label);
}
void ZSENSDK::ZSENGmObject::setActive(bool active){
    this->updPtr()->setActive(active);
}



ZSENSDK::ZSENGmObject ZSENSDK::ZSEN_World::getObjectSDK(std::string name){
    ZSENGmObject result;

    GameObject* common_obj_ptr = this->world_ptr->getObjectByLabel(QString::fromStdString(name));
    result.str_id = common_obj_ptr->str_id;
    result.world_ptr = this->world_ptr;
    result.updPtr();

    return result;
}

void ZSENSDK::ZSEN_World::removeObject(ZSENGmObject obj){
    world_ptr->removeObj(obj.updPtr()->getLinkToThisObject());
}
void ZSENSDK::ZSEN_World::setCamera(ZSPIRE::Camera cam){
    cam.setViewport(world_ptr->world_camera.getViewport());
    world_ptr->world_camera = cam;
}
ZSPIRE::Camera ZSENSDK::ZSEN_World::getCamera(){
    return world_ptr->world_camera;
}
void ZSENSDK::ZSEN_World::loadWorldFromFile(std::string file){
    Project* proj_ptr = static_cast<Project*>(world_ptr->proj_ptr);

    QString load = proj_ptr->root_path + "/" + QString::fromStdString(file);

   _editor_win->sheduleWorldLoad(load);
}
void ZSENSDK::ZSEN_World::Instantiate(ZSENGmObject obj){
    GameObjectLink link = obj.updPtr()->getLinkToThisObject();
    GameObject* result = this->world_ptr->dublicateObject(link.ptr);

    if(result->hasParent){ //if object parented
        result->parent.ptr->item_ptr->addChild(result->item_ptr);
    }else{
        this->world_ptr->obj_widget_ptr->addTopLevelItem(result->item_ptr);
    }
}
void ZSENSDK::ZSEN_World::addPrefab(std::string prefab){
    this->world_ptr->addObjectsFromPrefab(QString::fromStdString(prefab));
}
