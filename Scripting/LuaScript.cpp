#include "headers/zsensdk.h"
#include <Scripting/zsensdk.h>
#include <Scripting/LuaScript.h>
#include <iostream>

#define SCRIPT_LOG std::cout << "SCRIPT "

void Engine::ObjectScript::__InitScript() {
    L = luaL_newstate();

    luaL_openlibs(L);

    created = true;

    //Bind DSDK to script
    EZSENSDK::bindSDKBaseMath(L);
    EZSENSDK::bindSDKProperties(L);
    ZSENSDK::bindSDK(L);
    EZSENSDK::bindKeyCodesSDK(L);

    int start_result = luaL_dostring(L, this->script_content.c_str());

    if(start_result == 1){ //if error in script
        SCRIPT_LOG << name << " error loading occured!" << std::endl;
        std::cout << "ERROR: " << lua_tostring(L, -1) << std::endl;
    }

    lua_pcall(L, 0, 0, 0);
}
