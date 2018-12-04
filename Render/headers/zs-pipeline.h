#ifndef ZSPIPELINE_H
#define ZSPIPELINE_H
#define GLEW_STATIC
#include <GL/glew.h>
#include "zs-shader.h"
#include "zs-mesh.h"
#include "zs-texture.h"

#include <QMainWindow>
#include <SDL2/SDL.h>

#define GO_RENDER_TYPE_NONE 0
#define GO_RENDER_TYPE_TILE 1
#define GO_RENDER_TYPE_3D 2
#define GO_RENDER_TYPE_2D 3

#define PIPELINE_STATE_DEFAULT 0
#define PIPELINE_STATE_PICKING 1
#define PIPELINE_STATE_MARKED 2

class RenderPipeline{
private:
    ZSPIRE::Shader tile_shader; //Shader to draw tiles
    ZSPIRE::Shader diffuse3d_shader;
    ZSPIRE::Shader pick_shader; //Shader to draw & pick objects
    ZSPIRE::Shader obj_mark_shader; //Shader to draw mark of selected objects
public:
    int current_state;
    void setup();
    bool InitGLEW();
    void render(SDL_Window* w, void* projectedit_ptr);
    unsigned int render_getpickedObj(void* projectedit_ptr, int mouseX, int mouseY);
    void updateShadersCameraInfo(ZSPIRE::Camera* cam_ptr);
    ZSPIRE::Shader* processShaderOnObject(void* _obj);
    ZSPIRE::Camera* cam;

    RenderPipeline();
};


#endif // ZSPIPELINE_H
