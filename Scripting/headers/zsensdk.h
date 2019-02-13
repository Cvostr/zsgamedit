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

class ZSENAudSourceProperty : public ZSENObjectProperty{
public:
    void* prop_ptr;

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

    ZSENTransformProperty transform();
    ZSENAudSourceProperty audio();

    void prikol();
};

class ZSEN_World{
public:
    World* world_ptr;

    ZSENGmObject getObjectSDK(std::string name);
    void removeObject(ZSENGmObject obj);
};
namespace Debug{
    void Log(std::string text);
}

void bindSDK(lua_State* state);

}



#endif // ZSENSDK_H
