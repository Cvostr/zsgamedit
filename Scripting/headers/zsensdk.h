#ifndef ZSENSDK__H
#define ZSENSDK__H

#include <string>
#include "../../World/headers/World.h"

extern "C" {
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>
}
#include <LuaBridge/LuaBridge.h>

#include <Scripting/LuaScript.h>

namespace ZSENSDK {

namespace _Engine {
    void loadWorldFromFile(std::string file);
}

namespace Window {
    void setWindowSize(int W, int H);
    void setWindowMode(unsigned int mode);
}

void bindSDK(lua_State* state);

}



#endif // ZSENSDK_H
