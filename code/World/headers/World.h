#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <list>
#include <QString>
#include <QTreeWidget>
#include <fstream>
#include "../../ProjEd/headers/InspectorWin.h"

#include "../../Render/headers/zs-pipeline.h"
#include <render/zs-mesh.h>
#include <render/zs-math.h>
#include <render/zs-shader.h>
#include <render/zs-materials.h>
#include <misc/oal_manager.h>

#include "../../Misc/headers/AssimpMeshLoader.h"
#include "../../Misc/headers/zs_types.h"

#include <world/Physics.h>

#include <game.h>

#include "world/World.h"

class GameObject;
class World;
class GameObjectProperty;
class GameObjectSnapshot;

class ObjectPropertyLink{
public:
    Engine::GameObjectLink object;
    PROPERTY_TYPE prop_type;
    Engine::GameObjectProperty* ptr;

    Engine::GameObjectProperty* updLinkPtr();
    ObjectPropertyLink();
};

class LabelProperty : public Engine::GameObjectProperty {
public:
    std::string label; //Label of gameobject
    QTreeWidgetItem* list_item_ptr;

    void addPropertyInterfaceToInspector();
    void onValueChanged();
    void onObjectDeleted();
    void copyTo(Engine::GameObjectProperty* dest);
    void loadPropertyFromMemory(const char* data, Engine::GameObject* obj);

    LabelProperty();
    ~LabelProperty();
};

class GameObject : public Engine::GameObject{
public:
    QTreeWidgetItem* item_ptr;
    void pick(); //Mark object and its children picked

    bool addProperty(PROPERTY_TYPE property); //Adds property with property ID

    void setMeshSkinningRootNodeRecursively(GameObject* rootNode);

    void saveProperties(std::ofstream* stream); //Writes properties content at end of stream

    void putToSnapshot(GameObjectSnapshot* snapshot);
    void recoverFromSnapshot(GameObjectSnapshot* snapshot);

    GameObject(); //Default constructor
    ~GameObject();
};

class GameObjectSnapshot{
public:
    GameObject reserved_obj; //class object
    std::vector<Engine::GameObjectLink> children; //Vector to store links to children of object
    std::vector<GameObjectSnapshot> children_snapshots;
    Engine::GameObjectProperty* properties[OBJ_PROPS_SIZE]; //pointers to properties of object

    Engine::GameObjectLink parent_link;

    int props_num; //number of properties

    int obj_array_ind;

    void clear();
    GameObjectSnapshot();
};

class WorldSnapshot{
public:
    std::vector<GameObject> objects;
    std::vector<Engine::GameObjectProperty*> props;

    WorldSnapshot();
    void clear();
};

class World : public Engine::World{
public:
    //-----------------DATA FOR QT INTERFACE-----------
    QTreeWidget* obj_widget_ptr;
    std::vector<int> picked_objs_ids;
    bool isPicked(GameObject* obj);
    void unpickObject();
    //
    GameObject* addObject(GameObject obj);
    GameObject* newObject(); //Add new object to world
    int getFreeObjectSpaceIndex();
    bool isObjectLabelUnique(std::string label); //Get amount of objects with this label
    
    GameObject* dublicateObject(GameObject* original, bool parent = true);
    GameObject* Instantiate(GameObject* original);
    
    GameObject* updateLink(Engine::GameObjectLink* link);
    //Save loaded world to file
    void saveToFile(std::string file);
    //open world from file
    void openFromFile(std::string file, QTreeWidget* w_ptr);
    //Load world from bytes
    void loadFromMemory(const char* bytes, unsigned int size, QTreeWidget* w_ptr);
    //Write gameobject data to specified stream
    void writeGameObject(GameObject* object_ptr, std::ofstream* world_stream);

    void loadGameObjectFromMemory(GameObject* object_ptr, const char* bytes, unsigned int left_bytes);

    void storeObjectToPrefab(GameObject* object_ptr, QString file);
    void writeObjectToPrefab(GameObject* object_ptr, std::ofstream* stream);

    void addObjectsFromPrefab(std::string file);
    void addObjectsFromPrefab(char* data, unsigned int size);

    void processPrefabObject(GameObject* object_ptr, std::vector<GameObject>* objects_array);

    void addMeshGroup(std::string file_path);
    GameObject* addMeshNode(ZS3M::SceneNode* node);

    void putToShapshot(WorldSnapshot* snapshot);
    void recoverFromSnapshot(WorldSnapshot* snapshot);

    void getAvailableNumObjLabel(std::string label, int* result);

    World();

};

Engine::GameObjectProperty* _allocProperty(PROPERTY_TYPE type);
QString getPropertyString(PROPERTY_TYPE type);

#endif
