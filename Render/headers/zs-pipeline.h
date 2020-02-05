#ifndef ZSPIPELINE_H_1
#define ZSPIPELINE_H_1
#define GLEW_STATIC
#include <GL/glew.h>
#include <render/zs-shader.h>
#include <render/zs-mesh.h>
#include <render/zs-uniform-buffer.h>
#include <world/zs-camera.h>
#include <render/zs-texture.h>
#include "GizmosRenderer.h"
#include "../../Misc/headers/EditorManager.h"
#include <QMainWindow>
#include <SDL2/SDL.h>

#include "render/zs-pipeline.h"

enum PIPELINE_STATE {
    PIPELINE_STATE_DEFAULT,
    PIPELINE_STATE_PICKING,
    PIPELINE_STATE_MARKED,
    PIPELINE_STATE_SHADOWDEPTH
};


class RenderPipeline : public Engine::RenderPipeline{
private:
    GizmosRenderer* gizmos;

    Engine::Shader* pick_shader; //Shader to draw & pick objects
    Engine::Shader* obj_mark_shader; //Shader to draw mark of selected objects


public:
    void setLightsToBuffer();

    Engine::UniformBuffer* editorUniformBuffer;

    PIPELINE_STATE current_state;
    void setup(int bufWidth, int bufHeight);
    bool InitGLEW();
    void initGizmos(int projectPespective);

    void render(SDL_Window* w, void* projectedit_ptr);
    void render2D(void* projectedit_ptr);
    void render3D(void* projectedit_ptr, Engine::Camera* cam);
    void renderDepth(void* world_ptr);
    unsigned int render_getpickedObj(void* projectedit_ptr, int mouseX, int mouseY);
    ZSRGBCOLOR getColorOfPickedTransformControl(int mouseX, int mouseY, void* projectedit_ptr);

    Engine::Camera* cam;
    void* win_ptr;

    GizmosRenderer* getGizmosRenderer();
    //override virtual function from EngineComponentManager
    void init();

    Engine::Shader* getPickingShader();
    Engine::Shader* getShadowmapShader();
    Engine::Shader* getUiShader();

    RenderPipeline();
    ~RenderPipeline();
};


#endif // ZSPIPELINE_H
