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

class ZSENObjectProperty{
public:
    int type;
};

class ZSENTransformProperty : public ZSENObjectProperty{
public:
    TransformProperty* prop_ptr;

    ZSVECTOR3 getPosition();
    ZSVECTOR3 getScale();
    ZSVECTOR3 getRotation();

    void setPosition(ZSVECTOR3 pos);
    void setRotation(ZSVECTOR3 rot);
    void setScale(ZSVECTOR3 scale);
};

class ZSENTileProperty : public ZSENObjectProperty{
public:
    void* prop_ptr;

    void setDiffuseTexture(std::string texture);

    void playAnim();
    void stopAnim();
};

class ZSENAudSourceProperty : public ZSENObjectProperty{
public:
    void* prop_ptr;

    void setAudioFile(std::string aud);

    float getGain();
    float getPitch();

    void setGain(float gain);
    void setPitch(float pitch);

    void Play();
    void Stop();

};

class ZSENGmObject{
public:
    std::string str_id;
    World* world_ptr;
    GameObject* object_ptr;

    GameObject* updPtr();

    std::string getLabel();

    ZSENTransformProperty transform();
    ZSENAudSourceProperty audio();
    ZSENTileProperty tile();

    void prikol();
};

class ZSEN_World{
public:
    World* world_ptr;

    ZSENGmObject getObjectSDK(std::string name);
    void removeObject(ZSENGmObject obj);
    void setCamera(ZSPIRE::Camera cam);
    ZSPIRE::Camera getCamera();
    void loadWorldFromFile(std::string file);
};
namespace Debug{
    void Log(std::string text);
}

namespace Math{
    ZSVECTOR3 vnormalize(ZSVECTOR3 vec);
    ZSVECTOR3 vadd(ZSVECTOR3 v1, ZSVECTOR3 v2);
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
    };

    MouseState* getMouseStatePtr();
    MouseState getMouseState();
}

void bindSDK(lua_State* state);

}



#endif // ZSENSDK_H