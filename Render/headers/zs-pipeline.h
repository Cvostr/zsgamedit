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

class G_BUFFER_GL{
protected:
    unsigned int depthBuffer;
    unsigned int gBuffer; //framebuffer
    unsigned int tDiffuse;
    unsigned int tNormal;
    unsigned int tPos;
public:
    G_BUFFER_GL();
    void create(int width, int height);
    void bindFramebuffer();
    void bindTextures();
};

class RenderPipeline{
private:
    G_BUFFER_GL gbuffer;

    ZSPIRE::Shader tile_shader; //Shader to draw tiles
    ZSPIRE::Shader diffuse3d_shader;
    ZSPIRE::Shader pick_shader; //Shader to draw & pick objects
    ZSPIRE::Shader obj_mark_shader; //Shader to draw mark of selected objects

    ZSPIRE::Shader deffered_light;
    std::vector<void*> lights_ptr;
public:
    bool depthTest;
    int current_state;
    void setup();
    bool InitGLEW();
    void render(SDL_Window* w, void* projectedit_ptr);
    unsigned int render_getpickedObj(void* projectedit_ptr, int mouseX, int mouseY);
    void updateShadersCameraInfo(ZSPIRE::Camera* cam_ptr);
    void addLight(void* light_ptr);
    ZSPIRE::Shader* processShaderOnObject(void* _obj);
    ZSPIRE::Camera* cam;
    void* win_ptr;

    RenderPipeline();
};


#endif // ZSPIPELINE_H
