#include "headers/LuaScript.h"
#include "headers/zsensdk.h"
#include "../World/headers/obj_properties.h"
#include "../World/headers/2dtileproperties.h"
#include "../Render/headers/zs-math.h"
#include <QString>
#include <iostream>

void ZSENSDK::Debug::Log(std::string text){
    std::cout << "SCRIPT: " << text << std::endl;
}

ZSVECTOR3 ZSENSDK::Math::vnormalize(ZSVECTOR3 vec){
    ZSVECTOR3 result = vec;
    vNormalize(&result);
    return result;
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
ZSENSDK::ZSENAudSourceProperty ZSENSDK::ZSENGmObject::audio(){
    ZSENAudSourceProperty result;
    result.prop_ptr = this->updPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_AUDSOURCE);
    return result;
}

ZSENSDK::ZSENTileProperty ZSENSDK::ZSENGmObject::tile(){
    ZSENTileProperty result;
    result.prop_ptr = this->updPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TILE);
    return result;
}

void ZSENSDK::ZSENGmObject::prikol(){
    static_cast<AudioSourceProperty*>(this->object_ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_AUDSOURCE))->audio_start();
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
//Property functions
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
void ZSENSDK::ZSENTransformProperty::setRotation(ZSVECTOR3 rot){
    this->prop_ptr->rotation = rot;
    this->prop_ptr->updateMat();
}
void ZSENSDK::ZSENTransformProperty::setScale(ZSVECTOR3 scale){
    this->prop_ptr->scale = scale;
    this->prop_ptr->updateMat();
}
//AudioSource functions
void ZSENSDK::ZSENAudSourceProperty::Play(){
    AudioSourceProperty* prop_ptr = static_cast<AudioSourceProperty*>(this->prop_ptr);
    prop_ptr->audio_start();
}
void ZSENSDK::ZSENAudSourceProperty::Stop(){
    AudioSourceProperty* prop_ptr = static_cast<AudioSourceProperty*>(this->prop_ptr);
    prop_ptr->audio_stop();
}
float ZSENSDK::ZSENAudSourceProperty::getGain(){
    AudioSourceProperty* prop_ptr = static_cast<AudioSourceProperty*>(this->prop_ptr);
    return prop_ptr->source.source_gain;
}
float ZSENSDK::ZSENAudSourceProperty::getPitch(){
    AudioSourceProperty* prop_ptr = static_cast<AudioSourceProperty*>(this->prop_ptr);
    return prop_ptr->source.source_pitch;
}
void ZSENSDK::ZSENAudSourceProperty::setGain(float gain){
    AudioSourceProperty* prop_ptr = static_cast<AudioSourceProperty*>(this->prop_ptr);
    prop_ptr->source.source_gain = gain; //Set new gain value
    prop_ptr->source.apply_settings(); //Apply sound settings
}
void ZSENSDK::ZSENAudSourceProperty::setPitch(float pitch){
    AudioSourceProperty* prop_ptr = static_cast<AudioSourceProperty*>(this->prop_ptr);
    prop_ptr->source.source_pitch = pitch; //Set new gain value
    prop_ptr->source.apply_settings(); //Apply sound settings
}
//TileProperty functions
void ZSENSDK::ZSENTileProperty::playAnim(){
    TileProperty* prop_ptr = static_cast<TileProperty*>(this->prop_ptr);
}

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
            .endClass()
            .addFunction("length", &length)
            .addFunction("distance", &getDistance)
            .addFunction("normalize", &ZSENSDK::Math::vnormalize);


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
        .addFunction("audio", &ZSENSDK::ZSENGmObject::audio)
        .addFunction("tile", &ZSENSDK::ZSENGmObject::tile)
        .addFunction("prikol", &ZSENSDK::ZSENGmObject::prikol)

        .endClass()
        .endNamespace();

luabridge::getGlobalNamespace(state)
        .beginNamespace("engine")
        .beginClass <ZSEN_World>("World")

        .addFunction("findObject", &ZSENSDK::ZSEN_World::getObjectSDK)
        .addFunction("removeObject", &ZSENSDK::ZSEN_World::removeObject)

        .endClass()
        .endNamespace();

luabridge::getGlobalNamespace(state)
        .beginNamespace("engine")

        .beginClass <ZSENObjectProperty>("ObjectProperty")
        .endClass()


        .deriveClass <ZSENTransformProperty, ZSENObjectProperty>("Transform")
        .addFunction("getPosition", &ZSENSDK::ZSENTransformProperty::getPosition)
        .addFunction("getScale", &ZSENSDK::ZSENTransformProperty::getScale)
        .addFunction("getRotation", &ZSENSDK::ZSENTransformProperty::getRotation)
        .addFunction("setPosition", &ZSENSDK::ZSENTransformProperty::setPosition)
        .addFunction("setScale", &ZSENSDK::ZSENTransformProperty::setScale)
        .addFunction("setRotation", &ZSENSDK::ZSENTransformProperty::setRotation)
        .endClass()


        .deriveClass <ZSENAudSourceProperty, ZSENObjectProperty>("AudioSource")
        .addFunction("Play", &ZSENSDK::ZSENAudSourceProperty::Play)
        .addFunction("Stop", &ZSENSDK::ZSENAudSourceProperty::Stop)
        .addFunction("getGain", &ZSENSDK::ZSENAudSourceProperty::getGain)
        .addFunction("getPitch", &ZSENSDK::ZSENAudSourceProperty::getPitch)
        .addFunction("setGain", &ZSENSDK::ZSENAudSourceProperty::setGain)
        .addFunction("setPitch", &ZSENSDK::ZSENAudSourceProperty::setPitch)
        .endClass()

        .deriveClass <ZSENTileProperty, ZSENObjectProperty>("Tile2D")
        .endClass()

        .endNamespace();



}
