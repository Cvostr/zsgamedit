#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <list>
#include <QString>
#include <QTreeWidget>
#include <fstream>
#include "../../ProjEd/headers/InspectorWin.h"

#include "../../Render/headers/zs-pipeline.h"
#include "../../Render/headers/zs-mesh.h"
#include "../../Render/headers/zs-texture.h"
#include "../../Render/headers/zs-math.h"
#include "../../Render/headers/zs-shader.h"
#include "../../Render/headers/MatShaderProps.h"
#include "../../Misc/headers/oal_manager.h"

#include "../../Misc/headers/AssimpMeshLoader.h"
#include "../../Misc/headers/zs_types.h"

#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btBox2dShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMeshShape.h>
#include <LinearMath/btDefaultMotionState.h>

#define OBJ_PROPS_SIZE 11
#define MAX_OBJS 12000



enum PROPERTY_TYPE{
    GO_PROPERTY_TYPE_NONE,
    GO_PROPERTY_TYPE_TRANSFORM,
    GO_PROPERTY_TYPE_LABEL,
    GO_PROPERTY_TYPE_MESH,
    GO_PROPERTY_TYPE_LIGHTSOURCE,
    GO_PROPERTY_TYPE_AUDSOURCE,
    GO_PROPERTY_TYPE_MATERIAL,
    GO_PROPERTY_TYPE_SCRIPTGROUP,
    GO_PROPERTY_TYPE_COLLIDER,
    GO_PROPERTY_TYPE_RIGIDBODY,
    GO_PROPERTY_TYPE_SKYBOX,
    GO_PROPERTY_TYPE_SHADOWCASTER,
    GO_PROPERTY_TYPE_TERRAIN,
    GO_PROPERTY_TYPE_TILE_GROUP = 1000,
    GO_PROPERTY_TYPE_TILE = 1001
};

typedef struct PhysicalWorldSettings{
    ZSVECTOR3 gravity;

    PhysicalWorldSettings(){
        gravity = ZSVECTOR3(0, -10.0f, 0);
    }
}PhysicalWorldSettings;

class PhysicalWorld{
private:
    btDefaultCollisionConfiguration* collisionConfiguraton;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* broadphase;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* physic_world;

public:
    void stepSimulation(float stepSimulation);
    void addRidigbodyToWorld(btRigidBody* body);
    void removeRidigbodyFromWorld(btRigidBody* body);
    PhysicalWorld(PhysicalWorldSettings* settings);
};

class GameObject;
class World;
class GameObjectProperty;
class GameObjectSnapshot;

class GameObjectLink{
public:
    World* world_ptr;
    std::string obj_str_id;
    GameObject* ptr;

    GameObject* updLinkPtr(); //Updates pointer to object and returns it
    bool isEmpty(); //Check, if this link doesn't link to some object
    void crack(); //Make this link empty

    GameObjectLink();
};

class GameObjectProperty{
public:
    PROPERTY_TYPE type; //Describe TYPE of property
    bool active; //Is property working
    GameObjectLink go_link; //link to object, that holds this property
    World* world_ptr; //Sometimes may be useful

    void setActive(bool active);

    GameObjectProperty();
    virtual ~GameObjectProperty();

    virtual void addPropertyInterfaceToInspector(InspectorWin* inspector);
    virtual void onValueChanged();
    virtual void copyTo(GameObjectProperty* dest);
    //On object first add
    virtual void onAddToObject();
    virtual void onObjectDeleted();
    virtual void onUpdate(float deltaTime);
    virtual void onPreRender(RenderPipeline* pipeline);
    virtual void onRender(RenderPipeline* pipeline);
    virtual void onTrigger(GameObject* obj);
};

class LabelProperty : public GameObjectProperty {
public:
    QString label; //Label of gameobject
    bool isActiveToggle;
    QTreeWidgetItem* list_item_ptr;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onValueChanged();
    void copyTo(GameObjectProperty* dest);

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
    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onValueChanged();
    void getAbsoluteParentTransform(ZSVECTOR3& t, ZSVECTOR3& s, ZSVECTOR3& r);
    void copyTo(GameObjectProperty* dest);
    void onPreRender(RenderPipeline* pipeline);
    void getAbsoluteRotationMatrix(ZSMATRIX4x4& m);

    void setTranslation(ZSVECTOR3 new_translation);
    void setScale(ZSVECTOR3 new_scale);
    void setRotation(ZSVECTOR3 new_rotation);

    TransformProperty();
};


class GameObject{
public:
    //Index in objects vector
    int array_index;
    //Pointer to string label in property
    QString* label;
    //String, gameobject identified by
    std::string str_id;
    bool hasParent; //If object has a parent
    bool alive; //if object marked as removed
    bool isPicked; //if user selected this object to edit it
    bool active; //if true, object will be active in scene
    bool IsStatic;
    World* world_ptr; //pointer to world, when object placed
    GameObjectLink parent; //Link to object's parent

    unsigned int props_num; //Count of created props
    QTreeWidgetItem* item_ptr;
    GameObjectProperty* properties[OBJ_PROPS_SIZE]; //Vector to store pointers to all properties
    std::vector<GameObjectLink> children; //Vector to store links to children of object
    int getAliveChildrenAmount(); //Gets current amount of children objects (exclude removed chidren)
    void pick(); //Mark object and its children picked

    void setActive(bool active); //toggle gameobject active property
    void setLabel(std::string label);
    std::string getLabel();

    bool addProperty(PROPERTY_TYPE property); //Adds property with property ID
    bool addTransformProperty();
    bool addLabelProperty();

    template<typename T>
    T* getPropertyPtr(){
        unsigned int props = static_cast<unsigned int>(this->props_num);
        for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
            auto property_ptr = this->properties[prop_i];
            if(typeid ( *property_ptr) == typeid(T)){ //If object already has one
                return static_cast<T*>(property_ptr); //return it
            }
        }
        return nullptr;
    }

    //returns pointer to property by property type
    GameObjectProperty* getPropertyPtrByType(PROPERTY_TYPE property);
    GameObjectProperty* getPropertyPtrByTypeI(int property);
    LabelProperty* getLabelProperty();
    TransformProperty* getTransformProperty();
    GameObjectLink getLinkToThisObject();
    //remove deleted children from vector
    void trimChildrenArray();

    void addChildObject(GameObjectLink link);
    void removeChildObject(GameObjectLink link);
    //Remove property with type
    void removeProperty(int index);

    void saveProperties(std::ofstream* stream); //Writes properties content at end of stream
    void loadProperty(std::ifstream* world_stream); //Loads one property from stream
    void clearAll(bool clearQtWigt = true); //Release all associated memory with this object
    void copyTo(GameObject* dest);
    void processObject(RenderPipeline* pipeline); //On render pipeline wish to work with object
    void Draw(RenderPipeline* pipeline); //On render pipeline wish to draw the object
    void DrawMesh();
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
    //if user unchecked active checkbox
    void uncheckInactive();

    GameObject(); //Default constructor
    ~GameObject();
};

class GameObjectSnapshot{
public:
    GameObject reserved_obj; //class object
    std::vector<GameObjectLink> children; //Vector to store links to children of object
    std::vector<GameObjectSnapshot> children_snapshots;
    GameObjectProperty* properties[OBJ_PROPS_SIZE]; //pointers to properties of object

    GameObjectLink parent_link;

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

class World{
public:
    QTreeWidget* obj_widget_ptr;
    Project* proj_ptr; //Pointer to Project structure
    PhysicalWorldSettings phys_settngs;
    PhysicalWorld* physical_world;
    ZSPIRE::Camera world_camera;

    std::vector<GameObject> objects; //Vector, containing all gameobjects
    //std::list<ColliderProperty*> colliders; //Vector, containing all collider properties;

    GameObject* addObject(GameObject obj);
    GameObject* newObject(); //Add new object to world
    GameObject* getObjectByLabel(QString label);
    GameObject* getObjectByLabelStr(std::string label);
    GameObject* getObjectByStringId(std::string id);
    int getFreeObjectSpaceIndex();
    bool isObjectLabelUnique(QString label); //Get amount of objects with this label
    void removeObj(GameObjectLink link); //Remove object from world
    void removeObjPtr(GameObject* obj); //Remove object from world
    GameObject* dublicateObject(GameObject* original, bool parent = true);
    GameObject* Instantiate(GameObject* original);
    void trimObjectsList();
    void unpickObject();

    ZSPIRE::Mesh* getMeshPtrByRelPath(QString label); //look through all meshes in project ptr
    ZSPIRE::Texture* getTexturePtrByRelPath(QString label); //look through all meshes in project ptr
    SoundBuffer* getSoundPtrByName(QString label);
    Material* getMaterialPtrByName(QString label);

    void saveToFile(QString file, RenderSettings* settings_ptr);
    void openFromFile(QString file, QTreeWidget* w_ptr, RenderSettings* settings_ptr);
    void writeGameObject(GameObject* object_ptr, std::ofstream* world_stream);
    void loadGameObject(GameObject* object_ptr, std::ifstream* world_stream);

    void storeObjectToPrefab(GameObject* object_ptr, QString file);
    void writeObjectToPrefab(GameObject* object_ptr, std::ofstream* stream);
    void addObjectsFromPrefab(QString file);
    void addObjectsFromPrefabStr(std::string file);
    void processPrefabObject(GameObject* object_ptr, std::vector<GameObject>* objects_array);

    void addMeshGroup(std::string file_path);
    GameObject* addMeshNode(MeshNode* node);

    void clear();

    void putToShapshot(WorldSnapshot* snapshot);
    void recoverFromSnapshot(WorldSnapshot* snapshot);

    void getAvailableNumObjLabel(QString label, int* result);

   // void pushCollider(ColliderProperty* property);
   // void removeCollider(ColliderProperty* property);
    //bool isCollide(TransformProperty* prop);

    World();

};
GameObjectProperty* allocProperty(int type);
QString getPropertyString(int type);

#endif
