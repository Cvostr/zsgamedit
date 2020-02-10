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

class GameObjectProperty : public Engine::GameObjectProperty{
public:
    bool isActive();

    GameObjectProperty();
    virtual ~GameObjectProperty();
};

class LabelProperty : public GameObjectProperty {
public:
    std::string label; //Label of gameobject
    QTreeWidgetItem* list_item_ptr;

    void addPropertyInterfaceToInspector();
    void onValueChanged();
    void copyTo(Engine::GameObjectProperty* dest);

    LabelProperty();
};

class TransformProperty : public GameObjectProperty {
public:
    ZSMATRIX4x4 transform_mat;

    ZSVECTOR3 translation;
    ZSVECTOR3 scale;
    ZSVECTOR3 rotation;
    //Absolute values
    ZSVECTOR3 _last_translation;
    ZSVECTOR3 _last_scale;
    ZSVECTOR3 _last_rotation;

    void updateMat();
    void addPropertyInterfaceToInspector();
    void onValueChanged();
    void getAbsoluteParentTransform(ZSVECTOR3& t, ZSVECTOR3& s, ZSVECTOR3& r);
    void onRender(Engine::RenderPipeline* pipeline);
    void copyTo(Engine::GameObjectProperty* dest);
    void onPreRender(Engine::RenderPipeline* pipeline);

    void getAbsoluteRotationMatrix(ZSMATRIX4x4& m);

    void setTranslation(ZSVECTOR3 new_translation);
    void setScale(ZSVECTOR3 new_scale);
    void setRotation(ZSVECTOR3 new_rotation);

    TransformProperty();
};


class GameObject : public Engine::GameObject{
public:
    bool isPicked; //if user selected this object to edit it
    QTreeWidgetItem* item_ptr;
    int getAliveChildrenAmount(); //Gets current amount of children objects (exclude removed chidren)
    void pick(); //Mark object and its children picked

    bool addProperty(PROPERTY_TYPE property); //Adds property with property ID

    //returns pointer to property by property type
    GameObjectProperty* getPropertyPtrByType(PROPERTY_TYPE property);
    GameObjectProperty* getPropertyPtrByTypeI(int property);
    Engine::GameObjectLink getLinkToThisObject();

    void addChildObject(Engine::GameObjectLink link, bool updTransform = true);
    void removeChildObject(Engine::GameObjectLink link);

    GameObject* getChildObjectWithNodeLabel(std::string label);
    void setMeshSkinningRootNodeRecursively(GameObject* rootNode);

    //Remove property with type
    void removeProperty(int index);

    void saveProperties(std::ofstream* stream); //Writes properties content at end of stream
    void loadProperty(std::ifstream* world_stream); //Loads one property from stream
    void clearAll(bool clearQtWigt = true); //Release all associated memory with this object
    void copyTo(GameObject* dest);
    void processObject(RenderPipeline* pipeline); //On render pipeline wish to work with object
    void Draw(RenderPipeline* pipeline); //On render pipeline wish to draw the object
    void DrawMesh(RenderPipeline* pipeline);
    bool hasMesh(); //Check if gameobject has mesh property and mesh inside
    bool hasTerrain(); //Check if gameobject has terrain inside
    void onUpdate(int deltaTime); //calls onUpdate on all properties
    //calls onPreRender on all properties
    void onPreRender(RenderPipeline* pipeline);
    void onRender(RenderPipeline* pipeline);
    void onTrigger(GameObject* obj);
     //true, if object has rigidbody component
    bool isRigidbody();

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
    GameObjectProperty* properties[OBJ_PROPS_SIZE]; //pointers to properties of object

    Engine::GameObjectLink parent_link;

    int props_num; //number of properties

    int obj_array_ind;

    void clear();
    GameObjectSnapshot();
};

class WorldSnapshot{
public:
    std::vector<GameObject> objects;
    std::vector<GameObjectProperty*> props;

    WorldSnapshot();
    void clear();
};

class World : public Engine::World{
public:
    QTreeWidget* obj_widget_ptr;
    Engine::Camera world_camera;

    std::vector<GameObject> objects; //Vector, containing all gameobjects

    GameObject* addObject(GameObject obj);
    GameObject* newObject(); //Add new object to world
    Engine::GameObject* getObjectByLabel(std::string label);
    GameObject* getObjectByStringId(std::string id);
    int getFreeObjectSpaceIndex();
    bool isObjectLabelUnique(std::string label); //Get amount of objects with this label
    void removeObj(Engine::GameObjectLink& link); //Remove object from world
    void removeObjPtr(GameObject* obj); //Remove object from world
    GameObject* dublicateObject(GameObject* original, bool parent = true);
    GameObject* Instantiate(GameObject* original);
    void trimObjectsList();
    void unpickObject();

    GameObject* updateLink(Engine::GameObjectLink* link);

    void saveToFile(std::string file);
    void openFromFile(std::string file, QTreeWidget* w_ptr);
    void writeGameObject(GameObject* object_ptr, std::ofstream* world_stream);
    void loadGameObject(GameObject* object_ptr, std::ifstream* world_stream);

    void storeObjectToPrefab(GameObject* object_ptr, QString file);
    void writeObjectToPrefab(GameObject* object_ptr, std::ofstream* stream);
    void addObjectsFromPrefab(std::string file);
    void processPrefabObject(GameObject* object_ptr, std::vector<GameObject>* objects_array);

    void addMeshGroup(std::string file_path);
    GameObject* addMeshNode(ZS3M::SceneNode* node);

    void clear();

    void putToShapshot(WorldSnapshot* snapshot);
    void recoverFromSnapshot(WorldSnapshot* snapshot);

    void getAvailableNumObjLabel(std::string label, int* result);

    World();

};
GameObjectProperty* _allocProperty(PROPERTY_TYPE type);
QString getPropertyString(int type);

#endif
