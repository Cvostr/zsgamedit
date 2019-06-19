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

void ZSENSDK::Debug::Log(std::string text){
    std::cout << "SCRIPT: " << text << std::endl;
}


ZSVECTOR3 ZSENSDK::Math::vnormalize(ZSVECTOR3 vec){
    ZSVECTOR3 result = vec;
    vNormalize(&result);
    return result;
}

ZSVECTOR3 ZSENSDK::Math::vadd(ZSVECTOR3 v1, ZSVECTOR3 v2){
    return v1 + v2;
}

GameObject* ZSENSDK::ZSENGmObject::updPtr(){
    this->object_ptr = world_ptr->getObjectByStringId(this->str_id);
    return object_ptr;
}

std::string ZSENSDK::ZSENGmObject::getLabel(){
    return this->updPtr()->label->toStdString();
}
void ZSENSDK::ZSENGmObject::setActive(bool active){
    this->updPtr()->setActive(active);
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
    this->prop_ptr->setTranslation(pos);
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
void ZSENSDK::ZSENAudSourceProperty::setAudioFile(std::string aud){
    AudioSourceProperty* prop_ptr = static_cast<AudioSourceProperty*>(this->prop_ptr);
    //Assign new audio path
    prop_ptr->resource_relpath = QString::fromStdString(aud);
    //update audio buffer pointer
    prop_ptr->updateAudioPtr();
}
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
void ZSENSDK::ZSENTileProperty::setDiffuseTexture(std::string texture){
    TileProperty* prop_ptr = static_cast<TileProperty*>(this->prop_ptr);
    //Assign new texture path
    prop_ptr->diffuse_relpath = QString::fromStdString(texture);
    //Update texture pointer
    prop_ptr->updTexturePtr();
}
void ZSENSDK::ZSENTileProperty::playAnim(){
    TileProperty* prop_ptr = static_cast<TileProperty*>(this->prop_ptr);
    prop_ptr->anim_state.playing = true; //Set boolean to playing
}
void ZSENSDK::ZSENTileProperty::stopAnim(){
    TileProperty* prop_ptr = static_cast<TileProperty*>(this->prop_ptr);
    prop_ptr->anim_state.playing = false; //Set boolean to playing
}

void ZSENSDK::bindSDK(lua_State* state){
    luabridge::getGlobalNamespace(state)
            .beginNamespace("window")
            .addFunction("setWindowSize", &ZSENSDK::Window::setWindowSize)
            .endNamespace();

    luabridge::getGlobalNamespace(state)
            .beginNamespace("debug")
            .addFunction("Log", &ZSENSDK::Debug::Log)
            .endNamespace();

    luabridge::getGlobalNamespace(state)
            .beginNamespace("input")
            .addFunction("isKeyPressed", &ZSENSDK::Input::isKeyPressed)
            .addFunction("isKeyHold", &ZSENSDK::Input::isKeyHold)
            //Add mouse state class
            .beginClass <Input::MouseState>("MouseState")
            .addData("cursorX", &Input::MouseState::mouseX)
            .addData("cursorY", &Input::MouseState::mouseY)
            .addData("relX", &Input::MouseState::mouseRelX)
            .addData("relY", &Input::MouseState::mouseRelY)
            .addData("isLButtonDown", &Input::MouseState::isLButtonDown)
            .addData("isRButtonDown", &Input::MouseState::isRButtonDown)
            .endClass()

            .addFunction("getMouseState", &ZSENSDK::Input::getMouseState)

            .endNamespace();

    luabridge::getGlobalNamespace(state).beginClass <ZSVECTOR3>("Vec3")
            .addData("x", &ZSVECTOR3::X)
            .addData("y", &ZSVECTOR3::Y)
            .addData("z", &ZSVECTOR3::Z)
            .addConstructor <void(*) (float, float, float)>()
            .endClass();

    luabridge::getGlobalNamespace(state)
        .addFunction("length", &length)
        .addFunction("distance", &getDistance)
        .addFunction("normalize", &ZSENSDK::Math::vnormalize)
        .addFunction("v_add", &ZSENSDK::Math::vadd);

    luabridge::getGlobalNamespace(state).beginClass <ZSVIEWPORT>("CmViewport")
            .addData("startX", &ZSVIEWPORT::startX)
            .addData("startY", &ZSVIEWPORT::startY)
            .addData("endX", &ZSVIEWPORT::endX)
            .addData("endY", &ZSVIEWPORT::endY)
            .addConstructor <void(*) (unsigned int, unsigned int, unsigned int, unsigned int)>()
            .endClass();

    luabridge::getGlobalNamespace(state).beginClass <ZSPIRE::Camera>("Camera")

            .addFunction("setPosition", &ZSPIRE::Camera::setPosition)
            .addFunction("setFront", &ZSPIRE::Camera::setFront)
            .addFunction("getPosition", &ZSPIRE::Camera::getCameraPosition)
            .addFunction("getFront", &ZSPIRE::Camera::getCameraFrontVec)

            .addFunction("setProjection", &ZSPIRE::Camera::setProjectionType)
            .addFunction("setZplanes", &ZSPIRE::Camera::setZplanes)

            .addFunction("setViewport", &ZSPIRE::Camera::setViewport)
            .addFunction("getViewport", &ZSPIRE::Camera::getViewport)

            .endClass();

    luabridge::getGlobalNamespace(state).beginClass <ZSRGBCOLOR>("RGBColor")
            .addData("r", &ZSRGBCOLOR::r)
            .addData("g", &ZSRGBCOLOR::g)
            .addData("b", &ZSRGBCOLOR::b)
            .addConstructor <void(*) (float, float, float, float)>()
            .endClass();

luabridge::getGlobalNamespace(state)
        .beginNamespace("engine")
        .beginClass <ZSENGmObject>("GameObject")

        .addFunction("getLabel", &ZSENSDK::ZSENGmObject::getLabel)
        .addFunction("setActive", &ZSENSDK::ZSENGmObject::setActive)

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
        .addFunction("setCamera", &ZSENSDK::ZSEN_World::setCamera)
        .addFunction("getCamera", &ZSENSDK::ZSEN_World::getCamera)

        .addFunction("loadSceneFromFile", &ZSENSDK::ZSEN_World::loadWorldFromFile)
        .addFunction("instantiate", &ZSENSDK::ZSEN_World::Instantiate)

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
        .addFunction("setAudioFile", &ZSENSDK::ZSENAudSourceProperty::setAudioFile)
        .addFunction("Play", &ZSENSDK::ZSENAudSourceProperty::Play)
        .addFunction("Stop", &ZSENSDK::ZSENAudSourceProperty::Stop)
        .addFunction("getGain", &ZSENSDK::ZSENAudSourceProperty::getGain)
        .addFunction("getPitch", &ZSENSDK::ZSENAudSourceProperty::getPitch)
        .addFunction("setGain", &ZSENSDK::ZSENAudSourceProperty::setGain)
        .addFunction("setPitch", &ZSENSDK::ZSENAudSourceProperty::setPitch)
        .endClass()

        .deriveClass <ZSENTileProperty, ZSENObjectProperty>("Tile2D")
        .addFunction("playAnim", &ZSENSDK::ZSENTileProperty::playAnim)
        .addFunction("setDiffuseTexture", &ZSENSDK::ZSENTileProperty::setDiffuseTexture)
        .addFunction("stopAnim", &ZSENSDK::ZSENTileProperty::stopAnim)
        .endClass()

        .endNamespace();
}

void ZSENSDK::Window::setWindowSize(int W, int H){
    _editor_win->setGameViewWindowSize(W, H);
}
