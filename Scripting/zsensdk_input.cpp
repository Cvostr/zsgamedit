#include "headers/LuaScript.h"
#include "headers/zsensdk.h"
#include <SDL2/SDL.h>

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

void ZSENSDK::bindKeyCodesSDK(lua_State* state){

    luabridge::getGlobalNamespace(state).beginNamespace("input")
                .addVariable("KEY_Q", &keycodes::kq, false)
                .addVariable("KEY_W", &keycodes::kw, false)
                .addVariable("KEY_E", &keycodes::ke, false)
                .addVariable("KEY_R", &keycodes::kr, false)
                .addVariable("KEY_T", &keycodes::kt, false)
                .addVariable("KEY_Y", &keycodes::ky, false)
                .addVariable("KEY_U", &keycodes::ku, false)
                .addVariable("KEY_I", &keycodes::ki, false)
                .addVariable("KEY_O", &keycodes::ko, false)
                .addVariable("KEY_P", &keycodes::kp, false)
                .addVariable("KEY_A", &keycodes::ka, false)
                .addVariable("KEY_S", &keycodes::ks, false)
                .addVariable("KEY_D", &keycodes::kd, false)
                .addVariable("KEY_F", &keycodes::kf, false)
                .addVariable("KEY_G", &keycodes::kg, false)
                .addVariable("KEY_H", &keycodes::kh, false)
                .addVariable("KEY_J", &keycodes::kj, false)
                .addVariable("KEY_K", &keycodes::kk, false)
                .addVariable("KEY_L", &keycodes::kl, false)
                .addVariable("KEY_Z", &keycodes::kz, false)
                .addVariable("KEY_X", &keycodes::kx, false)
                .addVariable("KEY_C", &keycodes::kc, false)
                .addVariable("KEY_V", &keycodes::kv, false)
                .addVariable("KEY_B", &keycodes::kb, false)
                .addVariable("KEY_N", &keycodes::kn, false)
                .addVariable("KEY_M", &keycodes::km, false)
                .addVariable("KEY_SPACE", &keycodes::kspace, false)
                .addVariable("KEY_ESCAPE", &keycodes::kescape, false)
                .addVariable("KEY_TAB", &keycodes::ktab, false)
                .addVariable("KEY_CTRL", &keycodes::kctrl, false)
                .addVariable("KEY_ENTER", &keycodes::kenter, false)
                .addVariable("KEY_SHIFT", &keycodes::kshift, false)
                .endNamespace();
}
