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
#include <SDL2/SDL.h>

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

    void playAnim();
    void stopAnim();
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
    ZSENTileProperty tile();

    void prikol();
};

class ZSEN_World{
public:
    World* world_ptr;

    ZSENGmObject getObjectSDK(std::string name);
    void removeObject(ZSENGmObject obj);
    void setCamera(ZSPIRE::Camera cam);
};
namespace Debug{
    void Log(std::string text);
}

namespace Math{
    ZSVECTOR3 vnormalize(ZSVECTOR3 vec);
}

namespace Input {
    void addPressedKeyToQueue(int keycode);
    void addHeldKeyToQueue(int keycode);
    void removeHeldKeyFromQueue(int keycode);

    void clearPressedKeys();
    bool isKeyPressed(int keycode);
    bool isKeyHold(int keycode);
}

namespace keycodes {
    static int kq = SDLK_q;
    static int kw = SDLK_w;
    static int ke = SDLK_e;
    static int kr = SDLK_r;
    static int kt = SDLK_t;
    static int ky = SDLK_y;
    static int ku = SDLK_u;
    static int ki = SDLK_i;
    static int ko = SDLK_o;
    static int kp = SDLK_p;
    static int ka = SDLK_a;
    static int ks = SDLK_s;
    static int kd = SDLK_d;
    static int kf = SDLK_f;
    static int kg = SDLK_g;
    static int kh = SDLK_h;
    static int kj = SDLK_j;
    static int kk = SDLK_k;
    static int kl = SDLK_l;
    static int kz = SDLK_z;
    static int kx = SDLK_x;
    static int kc = SDLK_c;
    static int kv = SDLK_v;
    static int kb = SDLK_b;
    static int kn = SDLK_n;
    static int km = SDLK_m;

    static int kspace = SDLK_SPACE;
    static int kenter = SDLK_RETURN;
    static int ktab = SDLK_TAB;
    static int kshift = SDLK_LSHIFT;
    static int kctrl = SDLK_LCTRL;
    static int kescape = SDLK_ESCAPE;
}


void bindSDK(lua_State* state);

}



#endif // ZSENSDK_H
