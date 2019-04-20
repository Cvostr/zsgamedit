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

#define OBJ_PROPS_SIZE 11
#define MAX_OBJS 12000

enum COLLIDER_TYPE {COLLIDER_TYPE_NONE, COLLIDER_TYPE_BOX, COLLIDER_TYPE_CUBE};

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
    GO_PROPERTY_TYPE_TILE_GROUP = 1000,
    GO_PROPERTY_TYPE_TILE = 1001
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

    GameObjectProperty();
    virtual ~GameObjectProperty();

    virtual void addPropertyInterfaceToInspector(InspectorWin* inspector);
    virtual void onValueChanged();
    virtual void copyTo(GameObjectProperty* dest);
    virtual void onAddToObject();
    virtual void onObjectDeleted();
    virtual void onUpdate(float deltaTime);
    virtual void onPreRender(RenderPipeline* pipeline);
};

class LabelProperty : public GameObjectProperty {
public:
    QString label; //Label of gameobject
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

    TransformProperty();
};

class ColliderProperty : public GameObjectProperty{
public:
    void onAddToObject(); //will register in world
    void onObjectDeleted(); //unregister in world
    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void copyTo(GameObjectProperty* dest);

    TransformProperty* getTransformProperty();

    bool isTrigger;
    COLLIDER_TYPE coll_type;

    ColliderProperty();
};

class GameObject{
public:
    int array_index; //Index in objects vector
    QString* label; //Pointer to string label in property
    std::string str_id; //String, gameobject identified by
    bool hasParent; //If object has a parent
    bool alive; //if object marked as removed
    bool isPicked; //if user selected this object to edit it
    bool active;
    World* world_ptr; //pointer to world, when object placed
    GameObjectLink parent; //Link to object's parent
    GO_RENDER_TYPE render_type; //Render mode of this object

    unsigned int props_num; //Count of created props
    QTreeWidgetItem* item_ptr;
    GameObjectProperty* properties[OBJ_PROPS_SIZE]; //Vector to store pointers to all properties
    std::vector<GameObjectLink> children; //Vector to store links to children of object
    int getAliveChildrenAmount(); //Gets current amount of children objects (exclude removed chidren)
    void pick(); //Mark object and its children picked

    void setActive(bool active);

    bool addProperty(PROPERTY_TYPE property); //Adds property with property ID
    bool addTransformProperty();
    bool addLabelProperty();

    GameObjectProperty* getPropertyPtrByType(int property);
    LabelProperty* getLabelProperty();
    TransformProperty* getTransformProperty();
    GameObjectLink getLinkToThisObject();
    void trimChildrenArray(); //remove deleted children from vector

    void addChildObject(GameObjectLink link);
    void removeChildObject(GameObjectLink link);

    void removeProperty(int index); //Remove property with type

    void saveProperties(std::ofstream* stream); //Writes properties content at end of stream
    void loadProperty(std::ifstream* world_stream); //Loads one property from stream
    void clearAll(bool clearQtWigt = true); //Release all associated memory with this object
    void copyTo(GameObject* dest);
    void Draw(RenderPipeline* pipeline); //On render pipeline wish to draw the object
    void onUpdate(int deltaTime); //calls onUpdate on all properties
    void onPreRender(RenderPipeline* pipeline); //calls onPreRender on all properties
    void onTrigger(GameObject* obj);
    bool isRigidbody(); //true, if object has rigidbody component

    void putToSnapshot(GameObjectSnapshot* snapshot);
    void recoverFromSnapshot(GameObjectSnapshot* snapshot);

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
    void* proj_ptr; //Pointer to Project structure
    ZSPIRE::Camera world_camera;

    std::vector<GameObject> objects; //Vector, containing all gameobjects
    std::list<ColliderProperty*> colliders; //Vector, containing all collider properties;

    GameObject* addObject(GameObject obj);
    GameObject* newObject(); //Add new object to world
    GameObject* getObjectByLabel(QString label);
    GameObject* getObjectByStringId(std::string id);
    int getFreeObjectSpaceIndex();
    bool isObjectLabelUnique(QString label); //Get amount of objects with this label
    void removeObj(GameObjectLink link); //Remove object from world
    GameObject* dublicateObject(GameObject* original, bool parent = true);
    void trimObjectsList();
    void unpickObject();

    ZSPIRE::Mesh* getMeshPtrByRelPath(QString label); //look through all meshes in project ptr
    ZSPIRE::Texture* getTexturePtrByRelPath(QString label); //look through all meshes in project ptr
    SoundBuffer* getSoundPtrByName(QString label);
    Material* getMaterialPtrByName(QString label);

    void saveToFile(QString file, RenderSettings* settings_ptr);
    void openFromFile(QString file, QTreeWidget* w_ptr, RenderSettings* settings_ptr);
    void clear();

    void putToShapshot(WorldSnapshot* snapshot);
    void recoverFromSnapshot(WorldSnapshot* snapshot);

    void getAvailableNumObjLabel(QString label, int* result);

    void pushCollider(ColliderProperty* property);
    void removeCollider(ColliderProperty* property);
    bool isCollide(TransformProperty* prop);

    World();

};
GameObjectProperty* allocProperty(int type);
QString getPropertyString(int type);

#endif
