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
#define GO_RENDER_TYPE_MATERIAL 2

#define PIPELINE_STATE_DEFAULT 0
#define PIPELINE_STATE_PICKING 1
#define PIPELINE_STATE_MARKED 2

class G_BUFFER_GL{
protected:
    unsigned int depthBuffer;
    unsigned int gBuffer; //framebuffer
    unsigned int tDiffuse; //To store RGB diffuse Color A - shininess
    unsigned int tNormal; //To store normal coordinate
    unsigned int tPos; //To store position coordinate
    unsigned int tTransparent; //To store color with alpha
public:
    G_BUFFER_GL();
    void create(int width, int height);
    void bindFramebuffer();
    void bindTextures();
    void Destroy();
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
    float deltaTime;
    bool depthTest; //if enabled, GL will do depth testing
    bool cullFaces; //if enabled, GL will cull faces
    int current_state;
    void setup(int bufWidth, int bufHeight);
    bool InitGLEW();
    void render(SDL_Window* w, void* projectedit_ptr);
    unsigned int render_getpickedObj(void* projectedit_ptr, int mouseX, int mouseY);
    void updateShadersCameraInfo(ZSPIRE::Camera* cam_ptr);
    void addLight(void* light_ptr);
    void removeLights();
    ZSPIRE::Shader* processShaderOnObject(void* _obj);
    ZSPIRE::Camera* cam;
    void* win_ptr;

    RenderPipeline();
    ~RenderPipeline();
};


#endif // ZSPIPELINE_H
