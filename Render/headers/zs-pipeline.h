#ifndef ZSPIPELINE_H
#define ZSPIPELINE_H
#define GLEW_STATIC
#include <GL/glew.h>
#include "zs-shader.h"
#include "zs-mesh.h"
#include "zs-texture.h"

#include <QMainWindow>
#include <SDL2/SDL.h>

class RenderPipeline{
public:
    ZSPIRE::Shader diffuse_shader;
    ZSPIRE::Shader pick_shader;
public:
    void setup();
    bool InitGLEW();
    void render(SDL_Window* w, void* projectedit_ptr);
    unsigned int render_getpickedObj(void* projectedit_ptr, int mouseX, int mouseY);
};


#endif // ZSPIPELINE_H
