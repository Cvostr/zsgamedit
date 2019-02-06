
#include "headers/LuaScript.h"
#include <iostream>

void ObjectScript::_InitScript() {
    L = luaL_newstate();
    luaL_dofile(L, fpath.toStdString().c_str());
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
    delete L; //Release this
    L = 0x0; //Mark as deleted
}

void ObjectScript::_callStart() {

    luabridge::LuaRef start = luabridge::getGlobal(L, "onStart");
    if (start.isFunction() == true) { //If function found
        try {
            int result = start(getGameObjectSDK());
        }
        catch (luabridge::LuaException e) {
           std::cout << "SCRIPT" << "Error occured in script (onStart) " << fpath.toStdString() << e.what() << std::endl;
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
