#include "headers/LuaScript.h"
#include "headers/zsensdk.h"
#include "../World/headers/obj_properties.h"
#include "../World/headers/2dtileproperties.h"
#include <render/zs-math.h>
#include "../ProjEd/headers/ProjectEdit.h"
#include <QString>
#include <iostream>
#include <functional>
#include <SDL2/SDL.h>
#include <input/zs-input.h>
#include <world/zs-camera.h>

static unsigned int mode_fullscreen = SDL_WINDOW_FULLSCREEN;
static unsigned int mode_borderless = SDL_WINDOW_FULLSCREEN_DESKTOP;
static unsigned int mode_windowed = 0;

static unsigned int prop_transform = GO_PROPERTY_TYPE_TRANSFORM;
static unsigned int prop_mesh = GO_PROPERTY_TYPE_MESH;
static unsigned int prop_audio = GO_PROPERTY_TYPE_AUDSOURCE;
static unsigned int prop_light = GO_PROPERTY_TYPE_LIGHTSOURCE;
static unsigned int prop_script = GO_PROPERTY_TYPE_SCRIPTGROUP;
static unsigned int prop_tile = GO_PROPERTY_TYPE_TILE;

extern EditWindow* _editor_win;
extern Project* project_ptr;

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

ZSVECTOR3 ZSENSDK::Math::vmul(ZSVECTOR3 v1, float m){
    return v1 * m;
}

void ZSENSDK::_Engine::loadWorldFromFile(std::string file){
    QString load = QString::fromStdString(project_ptr->root_path) + "/" + QString::fromStdString(file);

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
        .addFunction("isKeyPressed", &Input::isKeyPressed)
        .addFunction("isKeyHold", &Input::isKeyHold)
        .addFunction("getMouseState", &Input::getMouseState)
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
        .addFunction("sum", &ZSENSDK::Math::vadd)
        .addFunction("mul", &ZSENSDK::Math::vmul)
        .addFunction("v_cross", &vCross);

     luabridge::getGlobalNamespace(state).beginClass <Engine::ZSVIEWPORT>("CmViewport")
        .addData("startX", &Engine::ZSVIEWPORT::startX)
        .addData("startY", &Engine::ZSVIEWPORT::startY)
        .addData("endX", &Engine::ZSVIEWPORT::endX)
        .addData("endY", &Engine::ZSVIEWPORT::endY)
        .addConstructor <void(*) (unsigned int, unsigned int, unsigned int, unsigned int)>()
        .endClass();

    luabridge::getGlobalNamespace(state).beginClass <Engine::Camera>("Camera")
            .addFunction("setPosition", &Engine::Camera::setPosition)
            .addFunction("setFront", &Engine::Camera::setFront)
            .addData("pos", &Engine::Camera::camera_pos, false)
            .addData("front", &Engine::Camera::camera_front, false)
            .addData("up", &Engine::Camera::camera_up, false)
            .addFunction("setProjection", &Engine::Camera::setProjectionType)
            .addFunction("setZplanes", &Engine::Camera::setZplanes)
            .addFunction("setViewport", &Engine::Camera::setViewport)
            .addData("viewport", &Engine::Camera::viewport, false)
            .addData("Fov", &Engine::Camera::FOV, false)
            .addData("nearZ", &Engine::Camera::nearZ, false)
            .addData("farZ", &Engine::Camera::farZ, false)
        .endClass();

    luabridge::getGlobalNamespace(state).beginClass <ZSRGBCOLOR>("RGBColor")
        .addData("r", &ZSRGBCOLOR::r)
        .addData("g", &ZSRGBCOLOR::g)
        .addData("b", &ZSRGBCOLOR::b)
        .addConstructor <void(*) (float, float, float, float)>()
        .endClass();

    luabridge::getGlobalNamespace(state)
        .beginNamespace("engine")

        .addFunction("loadWorld", &_Engine::loadWorldFromFile)

        .addVariable("PROPERTY_SCRIPT", &prop_script)
        .addVariable("PROPERTY_TRANSFORM", &prop_transform)
        .addVariable("PROPERTY_MESH", &prop_mesh)
        .addVariable("PROPERTY_AUDIOSOURCE", &prop_audio)
        .addVariable("PROPERTY_LIGHTSOURCE", &prop_light)
        .addVariable("PROPERTY_TILE", &prop_tile)

        .beginClass <Engine::GameObjectProperty>("ObjectProperty")
        .addFunction("setActive", &Engine::GameObjectProperty::setActive)
        .addData("active", &Engine::GameObjectProperty::active, false)
        .addData("type", &Engine::GameObjectProperty::type, false)
        .endClass()


        .beginClass <Engine::GameObject>("GameObject")
        .addFunction("getLabel", &Engine::GameObject::getLabel)
        .addFunction("setLabel", &Engine::GameObject::setLabel)
        .addFunction("setActive", &Engine::GameObject::setActive)
        .addData("active", &Engine::GameObject::active, false)
        .addData("propsNum", &Engine::GameObject::props_num, false)
        .addFunction("getProperty", &Engine::GameObject::getPropertyPtrByTypeI)
        //.addFunction("addProperty", &GameObject::addProperty)
        .addFunction("removeProperty", &Engine::GameObject::removeProperty)

        .addFunction("transform", &Engine::GameObject::getPropertyPtr<Engine::TransformProperty>)
        .addFunction("mesh", &Engine::GameObject::getPropertyPtr<Engine::MeshProperty>)
        .addFunction("audio", &Engine::GameObject::getPropertyPtr<AudioSourceProperty>)
        .addFunction("light", &Engine::GameObject::getPropertyPtr<LightsourceProperty>)
        .addFunction("tile", &Engine::GameObject::getPropertyPtr<TileProperty>)
        .addFunction("script", &Engine::GameObject::getPropertyPtr<ScriptGroupProperty>)
        .addFunction("rigidbody", &Engine::GameObject::getPropertyPtr<Engine::RigidbodyProperty>)
        .addFunction("character", &Engine::GameObject::getPropertyPtr<CharacterControllerProperty>)
        .addFunction("animation", &Engine::GameObject::getPropertyPtr<AnimationProperty>)
        .endClass()

        .beginClass <Engine::World>("World")
        .addFunction("findObject", &Engine::World::getObjectByLabel)
        //.addFunction("instantiate", &World::Instantiate)
        //.addFunction("addFromPrefab", &World::addObjectsFromPrefab)
        //.addFunction("removeObject", &World::removeObjPtr)
        .addData("camera", &Engine::World::world_camera, true)
        .endClass()

         //Usual script
        .beginClass <ObjectScript>("Script")
        .addFunction("onStart", &ObjectScript::_callStart)
        .addFunction("onFrame", &ObjectScript::_callDraw)
        .addFunction("func", &ObjectScript::func)
        .addFunction("funcA", &ObjectScript::_func)
        .endClass()


        .deriveClass <LightsourceProperty, Engine::GameObjectProperty>("LightSource")
        .addData("intensity", &LightsourceProperty::intensity)
        .addData("range", &LightsourceProperty::range)
        .addData("color", &LightsourceProperty::color)
        .addData("spot_angle", &LightsourceProperty::spot_angle)
        .endClass()

        .deriveClass <Engine::TransformProperty, Engine::GameObjectProperty>("Transform")
        .addData("translation", &Engine::TransformProperty::translation, false)
        .addData("scale", &Engine::TransformProperty::scale, false)
        .addData("rotation", &Engine::TransformProperty::rotation, false)
        .addFunction("setPosition", &Engine::TransformProperty::setTranslation)
        .addFunction("setScale", &Engine::TransformProperty::setScale)
        .addFunction("setRotation", &Engine::TransformProperty::setRotation)
        .endClass()


        .deriveClass <AudioSourceProperty, Engine::GameObjectProperty>("AudioSource")
        .addFunction("setAudioFile", &AudioSourceProperty::setAudioFile)
        .addFunction("Play", &AudioSourceProperty::audio_start)
        .addFunction("Stop", &AudioSourceProperty::audio_stop)
        .addFunction("Pause", &AudioSourceProperty::audio_pause)
        .addFunction("getGain", &AudioSourceProperty::getGain)
        .addFunction("getPitch", &AudioSourceProperty::getPitch)
        .addFunction("setGain", &AudioSourceProperty::setGain)
        .addFunction("setPitch", &AudioSourceProperty::setPitch)
        .endClass()

        .deriveClass <Engine::PhysicalProperty, Engine::GameObjectProperty>("Physical")
        .addData("mass", &Engine::PhysicalProperty::mass, false)
        .endClass()

        .deriveClass <Engine::RigidbodyProperty, Engine::PhysicalProperty>("Rigidbody")
        .addData("gravity", &Engine::RigidbodyProperty::gravity, false)
        .addData("linearVelocity", &Engine::RigidbodyProperty::linearVel, false)
        .addFunction("setLinearVelocity", &Engine::RigidbodyProperty::setLinearVelocity)
        .endClass()

         .deriveClass <CharacterControllerProperty, Engine::PhysicalProperty>("CharacterController")
         .addData("gravity", &CharacterControllerProperty::gravity, false)
         .addData("linearVelocity", &CharacterControllerProperty::linearVel, false)
         .addFunction("setLinearVelocity", &CharacterControllerProperty::setLinearVelocity)
         .endClass()

        .deriveClass <TileProperty, Engine::GameObjectProperty>("Tile2D")
        .addFunction("playAnim", &TileProperty::playAnim)
        .addFunction("setDiffuseTexture", &TileProperty::setDiffuseTexture)
        .addFunction("stopAnim", &TileProperty::stopAnim)
        .endClass()

        .deriveClass <AnimationProperty, Engine::GameObjectProperty>("Animation")
        .addFunction("play", &AnimationProperty::play)
        .addFunction("stop", &AnimationProperty::stop)
        .addFunction("setAnimation", &AnimationProperty::setAnimation)
        .endClass()

        .deriveClass <ScriptGroupProperty, Engine::GameObjectProperty>("ScriptGroup")
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
