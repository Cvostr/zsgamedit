#ifndef ZSENSDK_H
#define ZSENSDK_H

#include <string>
#include "../../World/headers/World.h"

extern "C" {
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
}
#include <LuaBridge/LuaBridge.h>

namespace ZSENSDK {



class ZSENGmObject{
private:
    GameObject* object_ptr;
public:
    std::string str_id;
    World* world_ptr;

    GameObject* updPtr();
};

class ZSEN_World{
public:
    World* world_ptr;

    ZSENGmObject getObjectSDK(std::string name);
};

void bindSDK(lua_State* state);

}



#endif // ZSENSDK_H
