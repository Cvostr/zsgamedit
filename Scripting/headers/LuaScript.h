#ifndef zs_luascript
#define zs_luascript

#include <string>
extern "C" {
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
}
#include <LuaBridge/LuaBridge.h>

class ObjectScript {

protected:
    std::string fpath; //path to file
    lua_State* L;
public:

    void _InitScript();


    void _callStart();
    void _callDraw();
    void callDrawUI();

};
#endif
