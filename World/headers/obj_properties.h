#ifndef OBJ_PROPERTIES_H
#define OBJ_PROPERTIES_H

#include "World.h"
#include "../../Scripting/headers/LuaScript.h"
#include <render/zs-materials.h>
#include "../../World/headers/terrain.h"

#include "world/go_properties.h"

enum LIGHTSOURCE_TYPE {
    LIGHTSOURCE_TYPE_NONE,
    LIGHTSOURCE_TYPE_DIRECTIONAL,
    LIGHTSOURCE_TYPE_POINT,
    LIGHTSOURCE_TYPE_SPOT
};

typedef uint8_t ZSLIGHTSOURCE_GL_ID;

class ScriptGroupProperty : public Engine::GameObjectProperty {
public:
    int scr_num; //to update amount via IntPropertyArea

    std::vector<ObjectScript> scripts_attached;
    std::vector<std::string> path_names;

    void onValueChanged();
    void addPropertyInterfaceToInspector();
    void shutdown();
    void onUpdate(float deltaTime); //calls update in scripts
    void copyTo(Engine::GameObjectProperty* dest);

    ObjectScript* getScriptByName(std::string name);

    ScriptGroupProperty();
};

class AudioSourceProperty : public Engine::GameObjectProperty{
private:
    bool isPlaySheduled;
public:
    std::string resource_relpath; //Relative path to resource
    Engine::AudioResource* buffer_ptr;
    Engine::SoundSource source;

    ZSVECTOR3 last_pos;

    void addPropertyInterfaceToInspector();
    void onValueChanged(); //Update soud buffer pointer and send source props
    void onUpdate(float deltaTime);
    void onObjectDeleted();
    void copyTo(Engine::GameObjectProperty* dest);

    void setAudioFile(std::string relpath);
    void updateAudioPtr();
    void audio_start();
    void audio_pause();
    void audio_stop();

    float getGain();
    float getPitch();
    void setGain(float gain);
    void setPitch(float pitch);

    AudioSourceProperty();
};

class MaterialProperty : public Engine::GameObjectProperty{
private:
    std::string group_label;

public:
    //Pointer to picked material
    Material* material_ptr;
    //Path to material fil
    std::string material_path;
    //Draw shadows on object
    bool receiveShadows;

    void setMaterial(Material* mat);
    void setMaterial(std::string path);
    void addPropertyInterfaceToInspector();
    void onValueChanged();
    void copyTo(Engine::GameObjectProperty* dest);
    void onRender(Engine::RenderPipeline* pipeline);

    MaterialProperty();
};

class LightsourceProperty : public Engine::GameObjectProperty{
private:
    LIGHTSOURCE_TYPE _last_light_type;
public:
    LIGHTSOURCE_TYPE light_type; //type of lightsource
    Engine::TransformProperty* transform; //pointer to object's transform

    ZSVECTOR3 direction; //direction for directional & spotlight in quats
    //To compare differences
    ZSVECTOR3 last_pos; //transform* last position
    ZSVECTOR3 last_rot; //transform* last rotation

    ZSRGBCOLOR color; //Color of light
    float intensity; //Light's intensity
    float range; //Light's range
    float spot_angle;

    ZSLIGHTSOURCE_GL_ID id; //glsl uniform index

    void addPropertyInterfaceToInspector();
    void onValueChanged(); //Update mesh pointer
    void copyTo(Engine::GameObjectProperty* dest);
    void updTransformPtr();
    void onObjectDeleted();
    void onPreRender(Engine::RenderPipeline* pipeline);

    LightsourceProperty();
};

class CharacterControllerProperty : public Engine::PhysicalProperty{
public:
    ZSVECTOR3 gravity;
    ZSVECTOR3 linearVel;
    ZSVECTOR3 angularVel;

    void addPropertyInterfaceToInspector();
    void copyTo(Engine::GameObjectProperty* dest);
    void onUpdate(float deltaTime);

    void setLinearVelocity(ZSVECTOR3 lvel);

    CharacterControllerProperty();
};


class SkyboxProperty : public Engine::GameObjectProperty{
public:
    void onPreRender(Engine::RenderPipeline* pipeline);
    void DrawSky(RenderPipeline* pipeline);
    SkyboxProperty();
};

class ShadowCasterProperty : public Engine::GameObjectProperty{
private:
    bool initialized;
    unsigned int shadowBuffer;
    unsigned int shadowDepthTexture;

    ZSMATRIX4x4 LightProjectionMat;
    ZSMATRIX4x4 LightViewMat;
public:
    int TextureWidth;
    int TextureHeight;
    float shadow_bias;
    float nearPlane;
    float farPlane;
    int projection_viewport;

    void addPropertyInterfaceToInspector();
    void onPreRender(Engine::RenderPipeline* pipeline);
    void copyTo(Engine::GameObjectProperty* dest);
    void onValueChanged();
    void init();
    void Draw(Engine::Camera* cam, RenderPipeline* pipeline);
    void setTexture();
    void setTextureSize();
    bool isRenderAvailable();
    ShadowCasterProperty();
};

class AnimationProperty : public Engine::GameObjectProperty {
private:
public:
    bool Playing;
    double start_sec;

    Engine::AnimationResource* anim_prop_ptr;
    std::string anim_label;

    void addPropertyInterfaceToInspector();
    void onPreRender(Engine::RenderPipeline* pipeline);
    void onValueChanged();
    void copyTo(Engine::GameObjectProperty* dest);

    void play();
    void stop();
    void setAnimation(std::string anim);

    void updateAnimationPtr();
    void updateNodeTransform(GameObject* obj, ZSMATRIX4x4 parent);


    AnimationProperty();
};

class TerrainProperty : public Engine::GameObjectProperty{

private:
    TerrainData data;

    char _last_edit_mode;
    char edit_mode;

    btRigidBody* rigidBody;
public:
    Engine::UniformBuffer* terrainUniformBuffer;
    Engine::UniformBuffer* transformBuffer;

    std::vector<HeightmapTexturePair> textures;
    std::vector<HeightmapGrass> grass;

    std::string file_label;
    int Width; //Width of terrain mesh
    int Length; //Height of terrain mesh
    int MaxHeight;
    float GrassDensity;
    bool castShadows;
    int textures_size;
    int grassType_size;

    int range; //Range of edit
    float editHeight; //Modifying height
    int textureid;
    int vegetableid;

    void addPropertyInterfaceToInspector();
    void onRender(Engine::RenderPipeline* pipeline);

    void DrawMesh(RenderPipeline* pipeline);
    void onValueChanged();
    void onAddToObject();
    void copyTo(Engine::GameObjectProperty* dest);
    void onUpdate(float deltaTime);
    TerrainProperty();
    void onMouseClick(int posX, int posY, int screenY, bool isLeftButtonHold, bool isCtrlHold);
    void onMouseMotion(int posX, int posY, int screenY, bool isLeftButtonHold, bool isCtrlHold);
    void getPickedVertexId(int posX, int posY, int screenY, unsigned char* data);

    void modifyTerrainVertex(unsigned char* gl_data, bool isCtrlHold);

    TerrainData* getTerrainData();

};

#endif // OBJ_PROPERTIES_H
