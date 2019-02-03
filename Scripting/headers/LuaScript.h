#ifndef zs_luascript
#define zs_luascript

#include <QString>
extern "C" {
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
}
#include <LuaBridge/LuaBridge.h>

class ObjectScript {

protected:
    lua_State* L;
public:
    QString fpath; //path to file

    void _InitScript();
    void _DestroyScript();


    void _callStart();
    void _callDraw();
    void callDrawUI();

};
#endif
