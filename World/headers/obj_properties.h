#ifndef OBJ_PROPERTIES_H
#define OBJ_PROPERTIES_H

#include "World.h"
#include "../../Scripting/headers/LuaScript.h"
#include <render/zs-materials.h>
#include "../../World/headers/terrain.h"

#include "world/go_properties.h"

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
