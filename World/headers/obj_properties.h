#ifndef OBJ_PROPERTIES_H
#define OBJ_PROPERTIES_H

#include "World.h"
#include "../../Scripting/headers/LuaScript.h"
#include "../../Render/headers/MatShaderProps.h"
#include "../../World/headers/terrain.h"

enum LIGHTSOURCE_TYPE {
    LIGHTSOURCE_TYPE_NONE,
    LIGHTSOURCE_TYPE_DIRECTIONAL,
    LIGHTSOURCE_TYPE_POINT,
    LIGHTSOURCE_TYPE_SPOT
};

enum COLLIDER_TYPE {COLLIDER_TYPE_NONE,
                    COLLIDER_TYPE_BOX,
                    COLLIDER_TYPE_CUBE,
                    COLLIDER_TYPE_SPHERE};

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

    ZSMATRIX4x4 inverse;

    GameObject* skinning_root_node;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void updateMeshPtr(); //Updates pointer according to resource_relpath
    void onValueChanged(); //Update mesh pointer
    void copyTo(GameObjectProperty* dest);
    MeshProperty();
};


class LightsourceProperty : public GameObjectProperty{
private:
    LIGHTSOURCE_TYPE _last_light_type;
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
    float spot_angle;

    ZSLIGHTSOURCE_GL_ID id; //glsl uniform index

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onValueChanged(); //Update mesh pointer
    void copyTo(GameObjectProperty* dest);
    void updTransformPtr();
    void onObjectDeleted();
    void onPreRender(RenderPipeline* pipeline);

    LightsourceProperty();
};

class PhysicalProperty : public GameObjectProperty{
protected:

    void init();
    void updateCollisionShape();
public:
    bool created;
    btRigidBody* rigidBody;
    btCollisionShape* shape;
    COLLIDER_TYPE coll_type;
    float mass;
    void addColliderRadio(InspectorWin* inspector);
    void addMassField(InspectorWin* inspector);
    void onUpdate(float deltaTime);
    PhysicalProperty();
};

class ColliderProperty : public PhysicalProperty{
public:
    void onObjectDeleted(); //unregister in world
    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void copyTo(GameObjectProperty* dest);
    void onUpdate(float deltaTime);
    TransformProperty* getTransformProperty();

    bool isTrigger;

    ColliderProperty();
};

class CharacterControllerProperty : public PhysicalProperty{
public:
    ZSVECTOR3 gravity;
    ZSVECTOR3 linearVel;
    ZSVECTOR3 angularVel;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void copyTo(GameObjectProperty* dest);
    void onUpdate(float deltaTime);

    void setLinearVelocity(ZSVECTOR3 lvel);

    CharacterControllerProperty();
};

class RigidbodyProperty : public PhysicalProperty{
public:
    ZSVECTOR3 gravity;
    ZSVECTOR3 linearVel;
    ZSVECTOR3 angularVel;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onUpdate(float deltaTime);
    void copyTo(GameObjectProperty* dest);
    void onValueChanged();

    void setLinearVelocity(ZSVECTOR3 lvel);

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

class NodeProperty : public GameObjectProperty {
public:
    ZSVECTOR3 translation;
    ZSVECTOR3 rotation;

    ZSMATRIX4x4 local_transform_mat;
    ZSMATRIX4x4 transform_mat;
    ZSMATRIX4x4 bone_transform;

    ZSMATRIX4x4 abs;

    bool hasBone;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onPreRender(RenderPipeline* pipeline);
    void updateChildren();
    //void copyTo(GameObjectProperty* dest);

    NodeProperty();
};

class TerrainProperty : public GameObjectProperty{

private:
    TerrainData data;
    bool hasChanged;
    char _last_edit_mode;
    char edit_mode;
public:
    unsigned int terrainUniformBuffer;

    std::vector<HeightmapTexturePair> textures;

    QString file_label;
    int Width;
    int Length;
    int MaxHeight;
    bool castShadows;
    int textures_size;

    int range;
    float editHeight;
    int textureid;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onRender(RenderPipeline* pipeline);
    void DrawMesh();
    void onValueChanged();
    void onAddToObject();
    void copyTo(GameObjectProperty* dest);
    TerrainProperty();
    void onMouseClick(int posX, int posY, int screenY, bool isLeftButtonHold, bool isCtrlHold);
    void onMouseMotion(int posX, int posY, int relX, int relY, int screenY, bool isLeftButtonHold, bool isCtrlHold);
    void getPickedVertexId(int posX, int posY, int screenY, unsigned char* data);

    TerrainData* getTerrainData();

};

#endif // OBJ_PROPERTIES_H
