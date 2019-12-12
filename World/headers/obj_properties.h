#ifndef OBJ_PROPERTIES_H
#define OBJ_PROPERTIES_H

#include "World.h"
#include "../../Scripting/headers/LuaScript.h"
#include <render/zs-materials.h>
#include "../../World/headers/terrain.h"

enum LIGHTSOURCE_TYPE {
    LIGHTSOURCE_TYPE_NONE,
    LIGHTSOURCE_TYPE_DIRECTIONAL,
    LIGHTSOURCE_TYPE_POINT,
    LIGHTSOURCE_TYPE_SPOT
};

typedef uint8_t ZSLIGHTSOURCE_GL_ID;

class ScriptGroupProperty : public GameObjectProperty {
public:
    int scr_num; //to update amount via IntPropertyArea

    std::vector<ObjectScript> scripts_attached;
    std::vector<std::string> path_names;

    void onValueChanged();
    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void shutdown();
    void onUpdate(float deltaTime); //calls update in scripts
    void copyTo(GameObjectProperty* dest);

    ObjectScript* getScriptByName(std::string name);

    ScriptGroupProperty();
};

class AudioSourceProperty : public GameObjectProperty{
private:
    bool isPlaySheduled;
public:
    std::string resource_relpath; //Relative path to resource
    Engine::AudioResource* buffer_ptr;
    Engine::SoundSource source;

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
    std::string group_label;

public:
    //Pointer to picked material
    Material* material_ptr;
    //Path to material fil
    std::string material_path;

    bool receiveShadows;

    void setMaterial(Material* mat);
    void setMaterial(std::string path);
    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onValueChanged();
    void copyTo(GameObjectProperty* dest);
    void onAddToObject(); //will update render flag
    void onRender(RenderPipeline* pipeline);

    MaterialProperty();
};


class MeshProperty : public GameObjectProperty{
public:
    std::string resource_relpath; //Relative path to resource
    Engine::MeshResource* mesh_ptr; //Pointer to mesh
    bool castShadows;

    GameObject* skinning_root_node;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void updateMeshPtr(); //Updates pointer according to resource_relpath
    void onValueChanged(); //Update mesh    pointer
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
    void onValueChanged();
    void init();
    void Draw(Engine::Camera* cam, RenderPipeline* pipeline);
    void setTexture();
    void setTextureSize();
    ShadowCasterProperty();
};

class NodeProperty : public GameObjectProperty {
public:
    std::string node_label;

    ZSVECTOR3 translation;
    ZSVECTOR3 scale;
    ZSQUATERNION rotation;
    //Node transform from file
    ZSMATRIX4x4 transform_mat;
    //Caclulated node transform
    ZSMATRIX4x4 abs;

    //bool hasBone;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onPreRender(RenderPipeline* pipeline);
    void copyTo(GameObjectProperty* dest);

    NodeProperty();
};

class AnimationProperty : public GameObjectProperty {
private:
public:
    bool Playing;
    double start_sec;

    Engine::AnimationResource* anim_prop_ptr;
    std::string anim_label;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onPreRender(RenderPipeline* pipeline);
    void onValueChanged();
    void copyTo(GameObjectProperty* dest);

    void play();
    void stop();
    void setAnimation(std::string anim);

    void updateAnimationPtr();
    void updateNodeTransform(GameObject* obj, ZSMATRIX4x4 parent);


    AnimationProperty();
};

class TerrainProperty : public GameObjectProperty{

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

    QString file_label;
    int Width; //Width of terrain mesh
    int Length; //Height of terrain mesh
    int MaxHeight;
    bool castShadows;
    int textures_size;
    int grassType_size;

    int range; //Range of edit
    float editHeight; //Modifying height
    int textureid;
    int vegetableid;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onRender(RenderPipeline* pipeline);

    void DrawMesh();
    void onValueChanged();
    void onAddToObject();
    void copyTo(GameObjectProperty* dest);
    void onUpdate(float deltaTime);
    TerrainProperty();
    void onMouseClick(int posX, int posY, int screenX, int screenY, bool isLeftButtonHold, bool isCtrlHold);
    void onMouseMotion(int posX, int posY, int screenX, int screenY, bool isLeftButtonHold, bool isCtrlHold);
    void getPickedVertexId(int posX, int posY, int screenX, int screenY, unsigned char* data);

    void modifyTerrainVertex(unsigned char* gl_data, bool isCtrlHold);

    TerrainData* getTerrainData();

};

#endif // OBJ_PROPERTIES_H
