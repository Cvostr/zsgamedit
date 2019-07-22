#ifndef zs_luascript
#define zs_luascript

#include <QString>
extern "C" {
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>
}
#include <LuaBridge/LuaBridge.h>

#include "../../World/headers/World.h"


class ObjectScript {

private:
    lua_State* L;
public:
    GameObjectLink link;
    QString fpath; //path to file
    std::string name;

    void _InitScript();
    void _DestroyScript();

    void _callStart();
    void _callDraw(float deltaTime);
    void callDrawUI();
    void onTrigger();
    void func(std::string func_name);

};
#endif
