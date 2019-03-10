#ifndef OBJ_PROPERTIES_H
#define OBJ_PROPERTIES_H

#include "World.h"
#include "../../Scripting/headers/LuaScript.h"
#include "../../Render/headers/MatShaderProps.h"

#define GO_PROPERTY_TYPE_NONE 0
#define GO_PROPERTY_TYPE_TRANSFORM 1
#define GO_PROPERTY_TYPE_LABEL 2
#define GO_PROPERTY_TYPE_MESH 3
#define GO_PROPERTY_TYPE_LIGHTSOURCE 4
#define GO_PROPERTY_TYPE_AUDSOURCE 5
#define GO_PROPERTY_TYPE_MATERIAL 6
#define GO_PROPERTY_TYPE_SCRIPTGROUP 7

#define LIGHTSOURCE_TYPE_DIRECTIONAL 1
#define LIGHTSOURCE_TYPE_POINT 2

typedef uint8_t ZSLIGHTSOURCE_TYPE; //type to store lightsource type
typedef uint8_t ZSLIGHTSOURCE_GL_ID;

class ScriptGroupProperty : public GameObjectProperty {
public:
    int scr_num; //to update amount via IntPropertyArea

    InspectorWin* insp_win;

    std::vector<ObjectScript> scripts_attached;
    std::vector<QString> path_names;

    void onValueChanged();
    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void wakeUp(); //on scene startup
    void onUpdate(float deltaTime); //calls update in scripts
    void copyTo(GameObjectProperty* dest);

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

    void updateAudioPtr();
    void audio_start();
    void audio_stop();

    AudioSourceProperty();
};


class MaterialProperty : public GameObjectProperty{
public:
    MtShaderPropertiesGroup* group_ptr;

    std::vector<MaterialShaderPropertyConf*> property_confs;

    MaterialShaderPropertyConf* addPropertyConf(int type);
    void loadPropsFromGroup(MtShaderPropertiesGroup* group);
    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onValueChanged();
    void clear();

    MaterialProperty();
};


class MeshProperty : public GameObjectProperty{
public:
    QString resource_relpath; //Relative path to resource
    ZSPIRE::Mesh* mesh_ptr; //Pointer to mesh

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void updateMeshPtr(); //Updates pointer according to resource_relpath
    void onValueChanged(); //Update mesh pointer
    void copyTo(GameObjectProperty* dest);
    MeshProperty();
};


class LightsourceProperty : public GameObjectProperty{
public:
    ZSLIGHTSOURCE_TYPE light_type; //type of lightsource
    TransformProperty* transform; //pointer to object's transform

    ZSVECTOR3 direction; //direction for directional & spotlight in quats
    //To compare differences
    ZSVECTOR3 last_pos; //transform* last position
    ZSVECTOR3 last_rot; //transform* last rotation

    ZSRGBCOLOR color; //Color of light
    float intensity; //Light's intensity
    float range; //Light's range

    ZSLIGHTSOURCE_GL_ID id; //glsl uniform index
    bool isSent; //is glsl uniform sent to deffered shader

    ZSPIRE::Shader* deffered_shader_ptr;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onValueChanged(); //Update mesh pointer
    void copyTo(GameObjectProperty* dest);
    void updTransformPtr();
    void onObjectDeleted();

    LightsourceProperty();
};

#endif // OBJ_PROPERTIES_H
