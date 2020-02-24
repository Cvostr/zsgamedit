#include <Scripting/LuaScript.h>
#include "headers/zsensdk.h"
#include <Scripting/zsensdk.h>
#include "../World/headers/World.h"
#include <render/zs-materials.h>
#include "../World/headers/terrain.h"
#include "world/go_properties.h"
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
        .beginNamespace("engine")

        .addFunction("loadWorld", &_Engine::loadWorldFromFile)

        .addVariable("PROPERTY_SCRIPT", &prop_script)
        .addVariable("PROPERTY_TRANSFORM", &prop_transform)
        .addVariable("PROPERTY_MESH", &prop_mesh)
        .addVariable("PROPERTY_AUDIOSOURCE", &prop_audio)
        .addVariable("PROPERTY_LIGHTSOURCE", &prop_light)
        .addVariable("PROPERTY_TILE", &prop_tile)

/*
        .beginClass <Engine::GameObject>("GameObject")
        .addFunction("getLabel", &Engine::GameObject::getLabel)
        .addFunction("setLabel", &Engine::GameObject::setLabel)
        .addFunction("setActive", &Engine::GameObject::setActive)
        .addData("active", &Engine::GameObject::active, false)
        .addData("propsNum", &Engine::GameObject::props_num, false)
        .addFunction("getProperty", &Engine::GameObject::getPropertyPtrByTypeI)
        .addFunction("removeProperty", &Engine::GameObject::removeProperty)

        .addFunction("transform", &Engine::GameObject::getPropertyPtr<Engine::TransformProperty>)
        .addFunction("mesh", &Engine::GameObject::getPropertyPtr<Engine::MeshProperty>)
        .addFunction("audio", &Engine::GameObject::getPropertyPtr<Engine::AudioSourceProperty>)
        .addFunction("light", &Engine::GameObject::getPropertyPtr<Engine::LightsourceProperty>)
        .addFunction("tile", &Engine::GameObject::getPropertyPtr<Engine::TileProperty>)
        .addFunction("script", &Engine::GameObject::getPropertyPtr<ScriptGroupProperty>)
        .addFunction("rigidbody", &Engine::GameObject::getPropertyPtr<Engine::RigidbodyProperty>)
        .addFunction("character", &Engine::GameObject::getPropertyPtr<Engine::CharacterControllerProperty>)
        .addFunction("animation", &Engine::GameObject::getPropertyPtr<Engine::AnimationProperty>)
        .endClass()

        .beginClass <Engine::World>("World")
        .addFunction("findObject", &Engine::World::getGameObjectByLabel)
        //.addFunction("instantiate", &World::Instantiate)
        //.addFunction("addFromPrefab", &World::addObjectsFromPrefab)
        //.addFunction("removeObject", &World::removeObjPtr)
        .addData("camera", &Engine::World::world_camera, true)
        .endClass()*/

        .deriveClass <Engine::ScriptGroupProperty, Engine::GameObjectProperty>("ScriptGroup")
        .addFunction("getScript", &Engine::ScriptGroupProperty::getScriptByName)
        .endClass()

        .endNamespace();
}

void ZSENSDK::Window::setWindowSize(int W, int H){
    _editor_win->setGameViewWindowSize(W, H);
}
void ZSENSDK::Window::setWindowMode(unsigned int mode){
    _editor_win->setGameViewWindowMode(mode);
}
