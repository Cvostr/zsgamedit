#include "headers/LuaScript.h"
#include "headers/zsensdk.h"
#include "../World/headers/obj_properties.h"
#include "../World/headers/2dtileproperties.h"
#include "../Render/headers/zs-math.h"
#include <QString>
#include <iostream>

#define KEYS_QUEUE_SIZE 10
#define KEY_NONE -200

static int pressed_keys_queue[KEYS_QUEUE_SIZE];
static int pressed_keys_q_size = 0;
static int hold_keys_queue[KEYS_QUEUE_SIZE];
static int hold_keys_q_size = 0;

void ZSENSDK::Debug::Log(std::string text){
    std::cout << "SCRIPT: " << text << std::endl;
}

void ZSENSDK::Input::addPressedKeyToQueue(int keycode){
    if(pressed_keys_q_size > KEYS_QUEUE_SIZE) return;
    pressed_keys_queue[pressed_keys_q_size] = keycode;
    pressed_keys_q_size += 1;
}
void ZSENSDK::Input::addHeldKeyToQueue(int keycode){
    if(isKeyHold(keycode)) return;

    bool insertable = false;
    int insert_pos = 0;

    for(int iterator = 0; iterator < hold_keys_q_size; iterator ++){
        if(hold_keys_queue[iterator] == KEY_NONE){
            insertable = true;
            insert_pos = iterator;
        }
    }
    if(!insertable){
        if(hold_keys_q_size > KEYS_QUEUE_SIZE) return;
        hold_keys_queue[hold_keys_q_size] = keycode;
        hold_keys_q_size += 1;
    }else{
        hold_keys_queue[insert_pos] = keycode;
    }
}
void ZSENSDK::Input::removeHeldKeyFromQueue(int keycode){
    //int pos;
    for(int iterator = 0; iterator < hold_keys_q_size; iterator ++){
        if(hold_keys_queue[iterator] == keycode){
            hold_keys_queue[iterator] = KEY_NONE;
            //pos = iterator;
        }
    }
    /*for (unsigned int obj_i = pos + 1; obj_i < hold_keys_q_size; obj_i ++) { //Iterate over all next chidren
        hold_keys_queue[obj_i - 1] = hold_keys_queue[obj_i]; //Move it to previous place
    }

    hold_keys_q_size -= 1;*/
}
void ZSENSDK::Input::clearPressedKeys(){
    pressed_keys_q_size = 0;
}
bool ZSENSDK::Input::isKeyPressed(int keycode){
    for(int i = 0; i < pressed_keys_q_size; i ++){
        if(pressed_keys_queue[i] == keycode)
            return true;
    }
    return false;
}
bool ZSENSDK::Input::isKeyHold(int keycode){
    for(int i = 0; i < hold_keys_q_size; i ++){
        if(hold_keys_queue[i] == keycode)
            return true;
    }
    return false;
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
    prop_ptr->anim_state.playing = true; //Set boolean to playing
}
void ZSENSDK::ZSENTileProperty::stopAnim(){
    TileProperty* prop_ptr = static_cast<TileProperty*>(this->prop_ptr);
    prop_ptr->anim_state.playing = false; //Set boolean to playing
}

void ZSENSDK::bindSDK(lua_State* state){
    luabridge::getGlobalNamespace(state)
            .beginNamespace("debug")
            .addFunction("Log", &ZSENSDK::Debug::Log)
            .endNamespace();

    luabridge::getGlobalNamespace(state)
            .beginNamespace("input")
            .addFunction("isKeyPressed", &ZSENSDK::Input::isKeyPressed)
            .addFunction("isKeyHold", &ZSENSDK::Input::isKeyHold)
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
        .addFunction("playAnim", &ZSENSDK::ZSENTileProperty::playAnim)
        .addFunction("stopAnim", &ZSENSDK::ZSENTileProperty::stopAnim)
        .endClass()

        .endNamespace();

luabridge::getGlobalNamespace(state).beginNamespace("input")
            .addVariable("KEY_Q", &keycodes::kq, false)
            .addVariable("KEY_W", &keycodes::kw, false)
            .addVariable("KEY_E", &keycodes::ke, false)
            .addVariable("KEY_R", &keycodes::kr, false)
            .addVariable("KEY_T", &keycodes::kt, false)
            .addVariable("KEY_Y", &keycodes::ky, false)
            .addVariable("KEY_U", &keycodes::ku, false)
            .addVariable("KEY_I", &keycodes::ki, false)
            .addVariable("KEY_O", &keycodes::ko, false)
            .addVariable("KEY_P", &keycodes::kp, false)
            .addVariable("KEY_A", &keycodes::ka, false)
            .addVariable("KEY_S", &keycodes::ks, false)
            .addVariable("KEY_D", &keycodes::kd, false)
            .addVariable("KEY_F", &keycodes::kf, false)
            .addVariable("KEY_G", &keycodes::kg, false)
            .addVariable("KEY_H", &keycodes::kh, false)
            .addVariable("KEY_J", &keycodes::kj, false)
            .addVariable("KEY_K", &keycodes::kk, false)
            .addVariable("KEY_L", &keycodes::kl, false)
            .addVariable("KEY_Z", &keycodes::kz, false)
            .addVariable("KEY_X", &keycodes::kx, false)
            .addVariable("KEY_C", &keycodes::kc, false)
            .addVariable("KEY_V", &keycodes::kv, false)
            .addVariable("KEY_B", &keycodes::kb, false)
            .addVariable("KEY_N", &keycodes::kn, false)
            .addVariable("KEY_M", &keycodes::km, false)
            .addVariable("KEY_ESCAPE", &keycodes::kescape, false)
            .addVariable("KEY_TAB", &keycodes::ktab, false)
            .addVariable("KEY_CTRL", &keycodes::kctrl, false)
            .addVariable("KEY_ENTER", &keycodes::kenter, false)
            .addVariable("KEY_SHIFT", &keycodes::kshift, false)
            .endNamespace();
}
