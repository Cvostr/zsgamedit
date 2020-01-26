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


class RenderPipeline : public EngineComponentManager{
private:
    GizmosRenderer* gizmos;

    Engine::G_BUFFER_GL gbuffer;

    Engine::Shader* tile_shader; //Shader to draw tiles
    Engine::Shader* pick_shader; //Shader to draw & pick objects
    Engine::Shader* obj_mark_shader; //Shader to draw mark of selected objects
    Engine::Shader* ui_shader;
    Engine::Shader* deffered_light;
    Engine::Shader* skybox;
    Engine::Shader* shadowMap;
    Engine::Shader* heightmap;

    std::vector<void*> lights_ptr;

    Engine::RenderSettings render_settings;
public:
    Engine::Shader* diffuse3d_shader;
    Engine::Shader* grass_shader;

    Engine::UniformBuffer* transformBuffer;
    Engine::UniformBuffer* lightsBuffer;
    Engine::UniformBuffer* shadowBuffer;
    Engine::UniformBuffer* terrainUniformBuffer;
    Engine::UniformBuffer* skinningUniformBuffer;
    Engine::UniformBuffer* tileMaterialUniformBuffer;
    Engine::UniformBuffer* skyboxTransformUniformBuffer;
    Engine::UniformBuffer* uiUniformBuffer;
    Engine::UniformBuffer* instancedTransformBuffer;
    Engine::UniformBuffer* editorUniformBuffer;

    bool depthTest; //if enabled, GL will do depth testing
    bool cullFaces; //if enabled, GL will cull faces
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

    void updateShadersCameraInfo(Engine::Camera* cam_ptr);
    void addLight(void* light_ptr);
    void removeLights();
    void setLightsToBuffer();
    Engine::Camera* cam;
    void* win_ptr;

    Engine::RenderSettings* getRenderSettings();
    GizmosRenderer* getGizmosRenderer();

    void renderSprite(Engine::Texture* texture_sprite, int X, int Y, int scaleX, int scaleY);

    void renderGlyph(unsigned int texture_id, int X, int Y, int scaleX, int scaleY, ZSRGBCOLOR color);
    //override virtual function from EngineComponentManager
    void init();
    void updateWindowSize(int W, int H);

    Engine::Shader* getTileShader();
    Engine::Shader* getPickingShader();
    Engine::Shader* getShadowmapShader();
    Engine::Shader* getUiShader();

    RenderPipeline();
    ~RenderPipeline();
};


#endif // ZSPIPELINE_H
