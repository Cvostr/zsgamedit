#ifndef ZSPIPELINE_H
#define ZSPIPELINE_H
#define GLEW_STATIC
#include <GL/glew.h>
#include "zs-shader.h"
#include "zs-mesh.h"
#include "zs-texture.h"
#include "GizmosRenderer.h"
#include "../../Misc/headers/EditorManager.h"
#include <QMainWindow>
#include <SDL2/SDL.h>

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

    ZSPIRE::Shader tile_shader; //Shader to draw tiles
    ZSPIRE::Shader diffuse3d_shader;
    ZSPIRE::Shader pick_shader; //Shader to draw & pick objects
    ZSPIRE::Shader obj_mark_shader; //Shader to draw mark of selected objects
    ZSPIRE::Shader ui_shader;
    ZSPIRE::Shader deffered_light;
    ZSPIRE::Shader skybox;
    ZSPIRE::Shader shadowMap;
    ZSPIRE::Shader heightmap;

    std::vector<void*> lights_ptr;

    RenderSettings render_settings;
public:
    unsigned int camBuffer;
    unsigned int lightsBuffer;
    unsigned int shadowBuffer;
    unsigned int terrainUniformBuffer;
    unsigned int skinningUniformBuffer;
    unsigned int tileMaterialUniformBuffer;
    unsigned int skyboxTransformUniformBuffer;

    bool depthTest; //if enabled, GL will do depth testing
    bool cullFaces; //if enabled, GL will cull faces
    PIPELINE_STATE current_state;
    void setup(int bufWidth, int bufHeight);
    bool InitGLEW();
    void initGizmos(int projectPespective);

    void render(SDL_Window* w, void* projectedit_ptr);
    void render2D(void* projectedit_ptr);
    void render3D(void* projectedit_ptr);
    void renderDepth(void* world_ptr);
    unsigned int render_getpickedObj(void* projectedit_ptr, int mouseX, int mouseY);
    ZSRGBCOLOR getColorOfPickedTransformControl(ZSVECTOR3 translation, int mouseX, int mouseY);

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

    ZSPIRE::Shader* getTileShader();
    ZSPIRE::Shader* getPickingShader();
    ZSPIRE::Shader* getShadowmapShader();
    ZSPIRE::Shader* getUiShader();

    RenderPipeline();
    ~RenderPipeline();
};


#endif // ZSPIPELINE_H
