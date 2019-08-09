#include "headers/LuaScript.h"
#include "headers/zsensdk.h"
#include "../World/headers/obj_properties.h"
#include "../World/headers/2dtileproperties.h"
#include "../Render/headers/zs-math.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include <QString>
#include <iostream>
#include <functional>
#include <SDL2/SDL.h>

unsigned int mode_fullscreen = SDL_WINDOW_FULLSCREEN;
unsigned int mode_borderless = SDL_WINDOW_FULLSCREEN_DESKTOP;
unsigned int mode_windowed = 0;

unsigned int prop_transform = GO_PROPERTY_TYPE_TRANSFORM;
unsigned int prop_mesh = GO_PROPERTY_TYPE_MESH;
unsigned int prop_audio = GO_PROPERTY_TYPE_AUDSOURCE;
unsigned int prop_light = GO_PROPERTY_TYPE_LIGHTSOURCE;
unsigned int prop_script = GO_PROPERTY_TYPE_SCRIPTGROUP;
unsigned int prop_tile = GO_PROPERTY_TYPE_TILE;

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

void ZSENSDK::Engine::loadWorldFromFile(std::string file){
    Project* proj_ptr = static_cast<Project*>(_editor_win->world.proj_ptr);

    QString load = proj_ptr->root_path + "/" + QString::fromStdString(file);

   _editor_win->sheduleWorldLoad(load);
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
        .addFunction("getMouseState", &ZSENSDK::Input::getMouseState)
        //Add mouse state class
        .beginClass <Input::MouseState>("MouseState")
        .addData("cursorX", &Input::MouseState::mouseX)
        .addData("cursorY", &Input::MouseState::mouseY)
        .addData("relX", &Input::MouseState::mouseRelX)
        .addData("relY", &Input::MouseState::mouseRelY)
        .addData("isLButtonDown", &Input::MouseState::isLButtonDown)
        .addData("isRButtonDown", &Input::MouseState::isRButtonDown)
        .addData("isMidButtonDown", &Input::MouseState::isMidBtnDown)
        .endClass()

        .endNamespace();

    luabridge::getGlobalNamespace(state).beginClass <ZSVECTOR3>("Vec3")
        .addData("x", &ZSVECTOR3::X)
        .addData("y", &ZSVECTOR3::Y)
        .addData("z", &ZSVECTOR3::Z)
        .addConstructor <void(*) (float, float, float)>()
        .endClass();

    luabridge::getGlobalNamespace(state)
        .addFunction("length", &getLength)
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
            .addData("pos", &ZSPIRE::Camera::camera_pos, false)
            .addData("front", &ZSPIRE::Camera::camera_front, false)
            .addFunction("setProjection", &ZSPIRE::Camera::setProjectionType)
            .addFunction("setZplanes", &ZSPIRE::Camera::setZplanes)
            .addFunction("setViewport", &ZSPIRE::Camera::setViewport)
            .addData("viewport", &ZSPIRE::Camera::viewport, false)
            .addData("Fov", &ZSPIRE::Camera::FOV, false)
            .addData("nearZ", &ZSPIRE::Camera::nearZ, false)
            .addData("farZ", &ZSPIRE::Camera::farZ, false)
        .endClass();

    luabridge::getGlobalNamespace(state).beginClass <ZSRGBCOLOR>("RGBColor")
        .addData("r", &ZSRGBCOLOR::r)
        .addData("g", &ZSRGBCOLOR::g)
        .addData("b", &ZSRGBCOLOR::b)
        .addConstructor <void(*) (float, float, float, float)>()
        .endClass();

    luabridge::getGlobalNamespace(state)
        .beginNamespace("engine")

        .addFunction("loadWorld", &Engine::loadWorldFromFile)

        .addVariable("PROPERTY_SCRIPT", &prop_script)
        .addVariable("PROPERTY_TRANSFORM", &prop_transform)
        .addVariable("PROPERTY_MESH", &prop_mesh)
        .addVariable("PROPERTY_AUDIOSOURCE", &prop_audio)
        .addVariable("PROPERTY_LIGHTSOURCE", &prop_light)
        .addVariable("PROPERTY_TILE", &prop_tile)

        .beginClass <GameObjectProperty>("ObjectProperty")
        .addFunction("setActive", &GameObjectProperty::setActive)
        .addData("active", &GameObjectProperty::active, false)
        .addData("type", &GameObjectProperty::type, false)
        .endClass()


        .beginClass <GameObject>("GameObject")
        .addFunction("getLabel", &GameObject::getLabel)
        .addFunction("setLabel", &GameObject::setLabel)
        .addFunction("setActive", &GameObject::setActive)
        .addData("active", &GameObject::active, false)
        .addData("propsNum", &GameObject::props_num, false)
        .addFunction("getProperty", &GameObject::getPropertyPtrByTypeI)
        .addFunction("addProperty", &GameObject::addProperty)
        .addFunction("removeProperty", &GameObject::removeProperty)

        .addFunction("transform", &GameObject::getPropertyPtr<TransformProperty>)
        .addFunction("mesh", &GameObject::getPropertyPtr<MeshProperty>)
        .addFunction("audio", &GameObject::getPropertyPtr<AudioSourceProperty>)
        .addFunction("light", &GameObject::getPropertyPtr<LightsourceProperty>)
        .addFunction("tile", &GameObject::getPropertyPtr<TileProperty>)
        .addFunction("script", &GameObject::getPropertyPtr<ScriptGroupProperty>)
        .endClass()

        .beginClass <World>("World")
        .addFunction("findObject", &World::getObjectByLabelStr)
        .addFunction("instantiate", &World::Instantiate)
        .addFunction("addFromPrefab", &World::addObjectsFromPrefabStr)
        .addFunction("removeObject", &World::removeObjPtr)
        .addData("camera", &World::world_camera, true)
        .endClass()

         //Usual script
        .beginClass <ObjectScript>("Script")
        .addFunction("onStart", &ObjectScript::_callStart)
        .addFunction("onFrame", &ObjectScript::_callDraw)
        .addFunction("func", &ObjectScript::func)
        .addFunction("funcA", &ObjectScript::_func)
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
