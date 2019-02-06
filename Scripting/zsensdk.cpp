#include "headers/LuaScript.h"
#include "headers/zsensdk.h"
#include "../World/headers/obj_properties.h"
#include "../Render/headers/zs-math.h"
#include <QString>
#include <iostream>

void ZSENSDK::Debug::Log(std::string text){
    std::cout << "SCRIPT: " << text << std::endl;
}

GameObject* ZSENSDK::ZSENGmObject::updPtr(){
    this->object_ptr = world_ptr->getObjectByStringId(this->str_id);
    return object_ptr;
}

ZSENSDK::ZSENTransformProperty ZSENSDK::ZSENGmObject::transform(){
    ZSENTransformProperty result;
    result.prop_ptr = static_cast<TransformProperty*>(this->updPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    return result;
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

ZSVECTOR3 ZSENSDK::ZSENTransformProperty::getPosition(){
    return this->prop_ptr->translation;
}
ZSVECTOR3 ZSENSDK::ZSENTransformProperty::getScale(){
    return this->prop_ptr->scale;
}
ZSVECTOR3 ZSENSDK::ZSENTransformProperty::getRotation(){
    return this->prop_ptr->rotation;
}

void ZSENSDK::ZSENTransformProperty::setPosition(ZSVECTOR3 pos){
    this->prop_ptr->translation = pos;
    this->prop_ptr->updateMat();
}
void ZSENSDK::ZSENTransformProperty::setRotation(ZSVECTOR3 rot){}
void ZSENSDK::ZSENTransformProperty::setScale(ZSVECTOR3 scale){}

void ZSENSDK::bindSDK(lua_State* state){
    luabridge::getGlobalNamespace(state)
            .beginNamespace("debug")
            .addFunction("Log", &ZSENSDK::Debug::Log)
            .endNamespace();

    luabridge::getGlobalNamespace(state).beginClass <ZSVECTOR3>("Vec3")
            .addData("x", &ZSVECTOR3::X)
            .addData("y", &ZSVECTOR3::Y)
            .addData("z", &ZSVECTOR3::Z)
            .addConstructor <void(*) (float, float, float)>()
            .endClass();

    luabridge::getGlobalNamespace(state).beginClass <ZSRGBCOLOR>("RGBColor")
            .addData("r", &ZSRGBCOLOR::r)
            .addData("g", &ZSRGBCOLOR::g)
            .addData("b", &ZSRGBCOLOR::b)
            //.addConstructor <void(*) (float, float, float)>()
            .endClass();

luabridge::getGlobalNamespace(state)
        .beginNamespace("engine")
        .beginClass <ZSENGmObject>("GameObject")

        .addFunction("transform", &ZSENSDK::ZSENGmObject::transform)

        .endClass()
        .endNamespace();

luabridge::getGlobalNamespace(state)
        .beginNamespace("engine")
        .beginClass <ZSEN_World>("World")

        .addFunction("findObject", &ZSENSDK::ZSEN_World::getObjectSDK)
        .addFunction("removeObject", &ZSENSDK::ZSEN_World::removeObject)

        .endClass()
        .endNamespace();



}
