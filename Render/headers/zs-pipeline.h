#ifndef ZSPIPELINE_H
#define ZSPIPELINE_H
#define GLEW_STATIC
#include <GL/glew.h>
#include <render/zs-shader.h>
#include <render/zs-mesh.h>
#include <render/zs-uniform-buffer.h>
#include "../../World/headers/zs-camera.h"
#include "zs-texture.h"
#include "GizmosRenderer.h"
#include "../../Misc/headers/EditorManager.h"
#include <QMainWindow>
#include <SDL2/SDL.h>

#define MAX_LIGHTS_AMOUNT 150

enum PIPELINE_STATE {
    PIPELINE_STATE_DEFAULT,
    PIPELINE_STATE_PICKING,
    PIPELINE_STATE_MARKED,
    PIPELINE_STATE_SHADOWDEPTH
};

struct RenderSettings {
    ZSRGBCOLOR ambient_light_color;
    void* skybox_ptr;

    void* shadowcaster_ptr;

    void defaults();

    RenderSettings(){
        defaults();
    }
};

class G_BUFFER_GL{
protected:
    unsigned int depthBuffer;
    unsigned int gBuffer; //framebuffer
    unsigned int tDiffuse; //To store RGB diffuse Color A - shininess
    unsigned int tNormal; //To store normal coordinate
    unsigned int tPos; //To store position coordinate
    unsigned int tTransparent; //To store color with alpha
    unsigned int tMasks;
public:
    bool created;
    G_BUFFER_GL();
    void create(int width, int height);
    void bindFramebuffer();
    void bindTextures();
    void Destroy();
};

class RenderPipeline : public EditorComponentManager{
private:
    GizmosRenderer* gizmos;

    G_BUFFER_GL gbuffer;

    Engine::Shader* tile_shader; //Shader to draw tiles

    Engine::Shader* pick_shader; //Shader to draw & pick objects
    Engine::Shader* obj_mark_shader; //Shader to draw mark of selected objects
    Engine::Shader* ui_shader;
    Engine::Shader* deffered_light;
    Engine::Shader* skybox;
    Engine::Shader* shadowMap;
    Engine::Shader* heightmap;

    std::vector<void*> lights_ptr;

    RenderSettings render_settings;
public:
    Engine::Shader* diffuse3d_shader;

    Engine::UniformBuffer* transformBuffer;
    Engine::UniformBuffer* lightsBuffer;
    Engine::UniformBuffer* shadowBuffer;
    Engine::UniformBuffer* terrainUniformBuffer;
    Engine::UniformBuffer* skinningUniformBuffer;
    Engine::UniformBuffer* tileMaterialUniformBuffer;
    Engine::UniformBuffer* skyboxTransformUniformBuffer;
    Engine::UniformBuffer* uiUniformBuffer;
    Engine::UniformBuffer* editorUniformBuffer;

    bool depthTest; //if enabled, GL will do depth testing
    bool cullFaces; //if enabled, GL will cull faces
    PIPELINE_STATE current_state;
    void setup(int bufWidth, int bufHeight);
    bool InitGLEW();
    void initGizmos(int projectPespective);

    void render(SDL_Window* w, void* projectedit_ptr);
    void render2D(void* projectedit_ptr);
    void render3D(void* projectedit_ptr, ZSPIRE::Camera* cam);
    void renderDepth(void* world_ptr);
    unsigned int render_getpickedObj(void* projectedit_ptr, int mouseX, int mouseY);
    ZSRGBCOLOR getColorOfPickedTransformControl(ZSVECTOR3 translation, int mouseX, int mouseY, void* projectedit_ptr);

    void updateShadersCameraInfo(ZSPIRE::Camera* cam_ptr);
    void addLight(void* light_ptr);
    void removeLights();
    void setLightsToBuffer();
    ZSPIRE::Camera* cam;
    void* win_ptr;

    RenderSettings* getRenderSettings();
    GizmosRenderer* getGizmosRenderer();

    void renderSprite(unsigned int texture_id, int X, int Y, int scaleX, int scaleY);
    void renderSprite(ZSPIRE::Texture* texture_sprite, int X, int Y, int scaleX, int scaleY);

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
