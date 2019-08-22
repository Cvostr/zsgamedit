#ifndef ZSENSDK_H
#define ZSENSDK_H

#include <string>
#include "../../World/headers/World.h"

extern "C" {
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>
}
#include <LuaBridge/LuaBridge.h>

#include "LuaScript.h"

namespace ZSENSDK {

namespace Engine {
    void loadWorldFromFile(std::string file);
}

namespace Debug{
    void Log(std::string text);
}

namespace Math{
    ZSVECTOR3 vnormalize(ZSVECTOR3 vec);
    ZSVECTOR3 vadd(ZSVECTOR3 v1, ZSVECTOR3 v2);
    ZSVECTOR3 vmul(ZSVECTOR3 v1, float m);
}

namespace Window {
    void setWindowSize(int W, int H);
    void setWindowMode(unsigned int mode);
}

namespace Input {
    void addPressedKeyToQueue(int keycode);
    void addHeldKeyToQueue(int keycode);
    void removeHeldKeyFromQueue(int keycode);
    void clearMouseState();

    void clearPressedKeys();
    bool isKeyPressed(int keycode);
    bool isKeyHold(int keycode);

    struct MouseState{
        int mouseX;
        int mouseRelX;

        int mouseY;
        int mouseRelY;

        bool isLButtonDown;
        bool isRButtonDown;
        bool isMidBtnDown;
    };

    MouseState* getMouseStatePtr();
    MouseState getMouseState();
}

void bindSDK(lua_State* state);
void bindKeyCodesSDK(lua_State* state);

}



#endif // ZSENSDK_H
