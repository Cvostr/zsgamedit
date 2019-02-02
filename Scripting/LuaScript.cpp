#include "headers/LuaScript.h"
#include "headers/zsensdk.h"
#include <iostream>

void ObjectScript::_InitScript() {
    L = luaL_newstate();
    luaL_dofile(L, fpath.toStdString().c_str());
    luaL_openlibs(L);
    lua_pcall(L, 0, 0, 0);

    //Bind DSDK to script
    //bindStructures(L);
    //bindSDK(L);
}

void ObjectScript::_callStart() {

    luabridge::LuaRef start = luabridge::getGlobal(L, "onStart");
    if (start.isFunction() == true) { //If function found
        try {
            int result = start();
        }
        catch (luabridge::LuaException e) {
           std::cout << "SCRIPT" << "Error occured in script (onStart) " << fpath.toStdString() << e.what();
           // dlogger::Log(TYPE_SCRIPTERROR, "%s %s %m %i %k %s", "Error occured in script (onStart) ", script_path, obj_pos, e.what());
        }
    }

}


void ObjectScript::_callDraw() {

   luabridge::LuaRef frame = luabridge::getGlobal(L, "onFrame");
    if (frame.isFunction() == true) { //If function found
        try {
            int result = frame();
        }
        catch (luabridge::LuaException e) {

         //   dlogger::Log(TYPE_SCRIPTERROR, "%s %s %m %i %k %s", "Error occured in script (onFrame) ", script_path, obj_pos, e.what());
        }

    }

}
void ObjectScript::callDrawUI() {

    luabridge::LuaRef ui = luabridge::getGlobal(L, "onDrawUI");
    if (ui.isFunction() == true) { //If function found
        try {
            int result = ui();
        }
        catch (luabridge::LuaException e) {

          //  dlogger::Log(TYPE_SCRIPTERROR, "%s %s %m %i %k %s", "Error occured in script (onDrawUI) ", script_path, obj_pos, e.what());
        }

    }

}
