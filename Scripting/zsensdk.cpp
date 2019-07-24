#include "headers/LuaScript.h"
#include "headers/zsensdk.h"
#include "../World/headers/obj_properties.h"
#include "../World/headers/2dtileproperties.h"
#include "../Render/headers/zs-math.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include <QString>
#include <iostream>
#include <SDL2/SDL.h>

unsigned int mode_fullscreen = SDL_WINDOW_FULLSCREEN;
unsigned int mode_borderless = SDL_WINDOW_FULLSCREEN_DESKTOP;
unsigned int mode_windowed = 0;

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

TransformProperty* ZSENSDK::ZSENGmObject::transform(){
    return static_cast<TransformProperty*>(this->updPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
}
AudioSourceProperty* ZSENSDK::ZSENGmObject::audio(){
    return static_cast<AudioSourceProperty*>(this->updPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_AUDSOURCE));
}
TileProperty* ZSENSDK::ZSENGmObject::tile(){
    return static_cast<TileProperty*>(this->updPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TILE));
}
LightsourceProperty* ZSENSDK::ZSENGmObject::light(){
    return static_cast<LightsourceProperty*>(this->updPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_LIGHTSOURCE));
}
ScriptGroupProperty* ZSENSDK::ZSENGmObject::script(){
    return static_cast<ScriptGroupProperty*>(this->updPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_SCRIPTGROUP));
}

void ZSENSDK::bindSDK(lua_State* state){
    luabridge::getGlobalNamespace(state)
        .beginNamespace("window")
        .addVariable("MODE_WINDOWED", &mode_windowed, false)
        .addVariable("MODE_FULLSCREEN", &mode_fullscreen, false)
        .addVariable("MODE_BORDERLESS", &mode_borderless, false)
        .addFunction("setWindowSize", &ZSENSDK::Window::setWindowSize)
        .addFunction("setWindowMode", &ZSENSDK::Window::setWindowMode)
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
        .addFunction("v_add", &ZSENSDK::Math::vadd)
        .addFunction("v_cross", &vCross);

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
        .addFunction("setLabel", &ZSENSDK::ZSENGmObject::setLabel)
        .addFunction("setActive", &ZSENSDK::ZSENGmObject::setActive)

        .addFunction("transform", &ZSENSDK::ZSENGmObject::transform)
        .addFunction("audio", &ZSENSDK::ZSENGmObject::audio)
        .addFunction("light", &ZSENSDK::ZSENGmObject::light)
        .addFunction("tile", &ZSENSDK::ZSENGmObject::tile)
        .addFunction("script", &ZSENSDK::ZSENGmObject::script)

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
        .addFunction("addFromPrefab", &ZSENSDK::ZSEN_World::addPrefab)

        .endClass()
        .endNamespace();

    luabridge::getGlobalNamespace(state)
        .beginNamespace("engine")

        .beginClass <ObjectScript>("Script")
        .addFunction("onStart", &ObjectScript::_callStart)
        .addFunction("onFrame", &ObjectScript::_callDraw)
        .addFunction("func", &ObjectScript::func)
        .addFunction("funcA", &ObjectScript::_func)
        .endClass()

        .beginClass <GameObjectProperty>("ObjectProperty")
        .addFunction("setActive", &GameObjectProperty::setActive)
        .endClass()

        .deriveClass <LightsourceProperty, GameObjectProperty>("LightSource")
        .addData("intensity", &LightsourceProperty::intensity)
        .addData("range", &LightsourceProperty::range)
        .addData("color", &LightsourceProperty::color)

        .endClass()

        .deriveClass <TransformProperty, GameObjectProperty>("Transform")
        .addData("translation", &TransformProperty::translation, false)
        .addData("scale", &TransformProperty::scale, false)
        .addData("rotation", &TransformProperty::rotation, false)
        .addFunction("setPosition", &TransformProperty::setTranslation)
        .addFunction("setScale", &TransformProperty::setScale)
        .addFunction("setRotation", &TransformProperty::setRotation)
        .endClass()


        .deriveClass <AudioSourceProperty, GameObjectProperty>("AudioSource")
        .addFunction("setAudioFile", &AudioSourceProperty::setAudioFile)
        .addFunction("Play", &AudioSourceProperty::audio_start)
        .addFunction("Stop", &AudioSourceProperty::audio_stop)
        .addFunction("Pause", &AudioSourceProperty::audio_pause)
        .addFunction("getGain", &AudioSourceProperty::getGain)
        .addFunction("getPitch", &AudioSourceProperty::getPitch)
        .addFunction("setGain", &AudioSourceProperty::setGain)
        .addFunction("setPitch", &AudioSourceProperty::setPitch)
        .endClass()

        .deriveClass <TileProperty, GameObjectProperty>("Tile2D")
        .addFunction("playAnim", &TileProperty::playAnim)
        .addFunction("setDiffuseTexture", &TileProperty::setDiffuseTexture)
        .addFunction("stopAnim", &TileProperty::stopAnim)
        .endClass()

        .deriveClass <ScriptGroupProperty, GameObjectProperty>("ScriptGroup")
        .addFunction("getScript", &ScriptGroupProperty::getScriptByName)
        .endClass()

        .endNamespace();
}

void ZSENSDK::Window::setWindowSize(int W, int H){
    _editor_win->setGameViewWindowSize(W, H);
}
void ZSENSDK::Window::setWindowMode(unsigned int mode){
    _editor_win->setGameViewWindowMode(mode);
}
