
#include "headers/LuaScript.h"
#include <iostream>

void ObjectScript::_InitScript() {
    L = luaL_newstate();
    int start_result = luaL_dofile(L, fpath.toStdString().c_str());

    if(start_result == 1){ //if error in script
        std::cout << "SCRIPT" << fpath.toStdString() << " error loading occured!" << std::endl;
        std::cout << "ERROR: " << lua_tostring(L, -1) << std::endl;
    }

    luaL_openlibs(L);
    lua_pcall(L, 0, 0, 0);

    //Bind DSDK to script
    ZSENSDK::bindSDK(L);
}

ZSENSDK::ZSENGmObject ObjectScript::getGameObjectSDK(){
    ZSENSDK::ZSENGmObject result;
    result.str_id = this->link.updLinkPtr()->str_id;
    result.world_ptr = this->link.world_ptr;
    result.updPtr();
    return result;
}

void ObjectScript::_DestroyScript(){
    lua_close(L);
}

void ObjectScript::_callStart() {
    ZSENSDK::ZSEN_World world;
    world.world_ptr = link.world_ptr;

    luabridge::LuaRef start = luabridge::getGlobal(L, "onStart");
    int result = 0;
    if (start.isFunction() == true) { //If function found
        try {
            result = start(getGameObjectSDK(), world); //Call script onStart()
        }
        catch (luabridge::LuaException e) {
           std::cout << "SCRIPT" << "Error occured in script (onStart) " << fpath.toStdString() << e.what() << std::endl;
        }
    }
    //Some error returned by script
    if(result == 1) std::cout << "SCRIPT" << "Script (onStart) function exited with 1" << fpath.toStdString() << std::endl;

}


void ObjectScript::_callDraw(float deltaTime) {

   luabridge::LuaRef frame = luabridge::getGlobal(L, "onFrame");
    if (frame.isFunction() == true) { //If function found
        try {
            frame(deltaTime);
        }
        catch (luabridge::LuaException e) {
            std::cout << "SCRIPT" << "Error occured in script (onFrame) " << fpath.toStdString() << e.what() << std::endl;
        }
    }
}
void ObjectScript::callDrawUI() {

    luabridge::LuaRef ui = luabridge::getGlobal(L, "onDrawUI");
    if (ui.isFunction() == true) { //If function found
        try {
            ui();
        }
        catch (luabridge::LuaException e) {
            std::cout << "SCRIPT" << "Error occured in script (onDrawUI) " << fpath.toStdString() << e.what() << std::endl;
        }
    }
}
