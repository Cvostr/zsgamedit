#ifndef ZSENSDK_H
#define ZSENSDK_H

#include <string>
#include "../../World/headers/World.h"
#include "../../World/headers/obj_properties.h"
#include "../../World/headers/2dtileproperties.h"

extern "C" {
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>
}
#include <LuaBridge/LuaBridge.h>

#include "LuaScript.h"

namespace ZSENSDK {

class ZSENGmObject{
public:
    std::string str_id;
    World* world_ptr;
    GameObject* object_ptr;

    GameObject* updPtr();

    std::string getLabel();
    void setLabel(std::string label);
    void setActive(bool active);

    TransformProperty* transform();
    AudioSourceProperty* audio();
    TileProperty* tile();
    LightsourceProperty* light();
    ScriptGroupProperty* script();

};

class ZSEN_World{
public:
    World* world_ptr;

    ZSENGmObject getObjectSDK(std::string name);
    void removeObject(ZSENGmObject obj);
    void setCamera(ZSPIRE::Camera cam);
    ZSPIRE::Camera getCamera();
    void loadWorldFromFile(std::string file);
    void addPrefab(std::string prefab);
    void Instantiate(ZSENGmObject obj);
};
namespace Debug{
    void Log(std::string text);
}

namespace Math{
    ZSVECTOR3 vnormalize(ZSVECTOR3 vec);
    ZSVECTOR3 vadd(ZSVECTOR3 v1, ZSVECTOR3 v2);
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
    };

    MouseState* getMouseStatePtr();
    MouseState getMouseState();
}

void bindSDK(lua_State* state);
void bindKeyCodesSDK(lua_State* state);

}



#endif // ZSENSDK_H
