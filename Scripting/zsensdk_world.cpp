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

GameObject* ZSENSDK::ZSEN_World::getObjectSDK(std::string name){
    GameObject* obj_ptr = this->world_ptr->getObjectByLabel(QString::fromStdString(name));

    return obj_ptr;
}

void ZSENSDK::ZSEN_World::removeObject(GameObject* obj){
    world_ptr->removeObj(obj->getLinkToThisObject());
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
void ZSENSDK::ZSEN_World::Instantiate(GameObject* obj){
    GameObjectLink link = obj->getLinkToThisObject();
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
