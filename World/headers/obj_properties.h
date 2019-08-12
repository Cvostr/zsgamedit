#ifndef OBJ_PROPERTIES_H
#define OBJ_PROPERTIES_H

#include "World.h"
#include "../../Scripting/headers/LuaScript.h"
#include "../../Render/headers/MatShaderProps.h"
#include "../../World/headers/terrain.h"

enum LIGHTSOURCE_TYPE {
    LIGHTSOURCE_TYPE_NONE,
    LIGHTSOURCE_TYPE_DIRECTIONAL,
    LIGHTSOURCE_TYPE_POINT
};

typedef uint8_t ZSLIGHTSOURCE_GL_ID;

class ScriptGroupProperty : public GameObjectProperty {
public:
    int scr_num; //to update amount via IntPropertyArea

    std::vector<ObjectScript> scripts_attached;
    std::vector<QString> path_names;

    void onValueChanged();
    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void wakeUp(); //on scene startup
    void shutdown();
    void onUpdate(float deltaTime); //calls update in scripts
    void copyTo(GameObjectProperty* dest);

    ObjectScript* getScriptByName(std::string name);

    ScriptGroupProperty();
};

class AudioSourceProperty : public GameObjectProperty{
public:
    QString resource_relpath; //Relative path to resource
    SoundBuffer* buffer_ptr;
    SoundSource source;

    ZSVECTOR3 last_pos;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onValueChanged(); //Update soud buffer pointer and send source props
    void onUpdate(float deltaTime);
    void onObjectDeleted();
    void copyTo(GameObjectProperty* dest);

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

class MaterialProperty : public GameObjectProperty{
private:
    QString group_label;

public:
    //Pointer to picked material
    Material* material_ptr;
    //Path to material fil
    QString material_path;

    bool receiveShadows;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onValueChanged();
    void copyTo(GameObjectProperty* dest);
    void onAddToObject(); //will update render flag
    void onRender(RenderPipeline* pipeline);

    MaterialProperty();
};


class MeshProperty : public GameObjectProperty{
public:
    QString resource_relpath; //Relative path to resource
    ZSPIRE::Mesh* mesh_ptr; //Pointer to mesh
    bool castShadows;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void updateMeshPtr(); //Updates pointer according to resource_relpath
    void onValueChanged(); //Update mesh pointer
    void copyTo(GameObjectProperty* dest);
    MeshProperty();
};


class LightsourceProperty : public GameObjectProperty{
public:
    LIGHTSOURCE_TYPE light_type; //type of lightsource
    TransformProperty* transform; //pointer to object's transform

    ZSVECTOR3 direction; //direction for directional & spotlight in quats
    //To compare differences
    ZSVECTOR3 last_pos; //transform* last position
    ZSVECTOR3 last_rot; //transform* last rotation

    ZSRGBCOLOR color; //Color of light
    float intensity; //Light's intensity
    float range; //Light's range

    ZSLIGHTSOURCE_GL_ID id; //glsl uniform index

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onValueChanged(); //Update mesh pointer
    void copyTo(GameObjectProperty* dest);
    void updTransformPtr();
    void onObjectDeleted();
    void onPreRender(RenderPipeline* pipeline);

    LightsourceProperty();
};

class RigidbodyProperty : public GameObjectProperty{
private:
    bool created;
    btRigidBody* rigidBody;
    btCollisionShape* shape;

    void init();
public:

    float mass;
    bool hasGravity;
    COLLIDER_TYPE coll_type;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onUpdate(float deltaTime);
    void copyTo(GameObjectProperty* dest);

    RigidbodyProperty();
};

class SkyboxProperty : public GameObjectProperty{
public:
    void onPreRender(RenderPipeline* pipeline);
    void DrawSky(RenderPipeline* pipeline);
    SkyboxProperty();
};

class ShadowCasterProperty : public GameObjectProperty{
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

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onPreRender(RenderPipeline* pipeline);
    void copyTo(GameObjectProperty* dest);
    void init();
    void Draw(ZSPIRE::Camera* cam, RenderPipeline* pipeline);
    void sendData(ZSPIRE::Shader* shader);
    ShadowCasterProperty();
};

class TerrainProperty : public GameObjectProperty{
private:
    TerrainData data;
    bool hasChanged;
    char edit_mode;
public:
    QString file_label;
    int Width;
    int Length;
    int MaxHeight;

    int range;
    float editHeight;
    int textureid;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onPreRender(RenderPipeline* pipeline);
    void onValueChanged();
    void onAddToObject();
    TerrainProperty();
    void onMouseClick(int posX, int posY, int screenY, bool isLeftButtonHold, bool isCtrlHold);
    void onMouseMotion(int posX, int posY, int relX, int relY, int screenY, bool isLeftButtonHold, bool isCtrlHold);

    TerrainData* getTerrainData();

};

#endif // OBJ_PROPERTIES_H
