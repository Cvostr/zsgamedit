#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <QString>
#include <QTreeWidget>
#include <fstream>
#include "../../ProjEd/headers/InspectorWin.h"

#include "../../Render/headers/zs-mesh.h"
#include "../../Render/headers/zs-texture.h"
#include "../../Render/headers/zs-math.h"
#include "../../Render/headers/zs-shader.h"
#include "../../Render/headers/zs-pipeline.h"

#define GO_PROPERTY_TYPE_NONE 0
#define GO_PROPERTY_TYPE_TRANSFORM 1
#define GO_PROPERTY_TYPE_LABEL 2
#define GO_PROPERTY_TYPE_MESH 3


class GameObject;
class World;
class GameObjectProperty;

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
    int type; //Describe TYPE of property
    bool active; //Is property working
    GameObjectLink go_link;
    World* world_ptr; //Sometimes may be useful
    int size; //Size of object in bytes

    GameObjectProperty();
    virtual ~GameObjectProperty();

    virtual void addPropertyInterfaceToInspector(InspectorWin* inspector);
    virtual void onValueChanged();
};

#include "2dtileproperties.h" //Include that to define 2dTile game elements


class LabelProperty : public GameObjectProperty {
public:
    QString label; //Label of gameobject
    QTreeWidgetItem* list_item_ptr;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onValueChanged();

    LabelProperty();
};

class TransformProperty : public GameObjectProperty {
private:
    ZSVECTOR3 _last_translation;
    ZSVECTOR3 _last_scale;
    ZSVECTOR3 _last_rotation;
public:
    ZSMATRIX4x4 transform_mat;

    ZSVECTOR3 translation;
    ZSVECTOR3 scale;
    ZSVECTOR3 rotation;

    void updateMat();
    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onValueChanged();
    void getAbsoluteParentTransform(ZSVECTOR3& t, ZSVECTOR3& s, ZSVECTOR3& r);

    TransformProperty();
};

class MeshProperty : public GameObjectProperty{
public:
    QString resource_relpath; //Relative path to resource
    ZSPIRE::Mesh* mesh_ptr; //Pointer to mesh

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void updateMeshPtr(); //Updates pointer according to resource_relpath
    void onValueChanged(); //Update mesh pointer
    MeshProperty();
};

class GameObject{
public:
    int array_index; //Index in objects vector
    QString* label; //Pointer to string label in property
    std::string str_id; //String, gameobject identified by
    bool hasParent; //If object has a parent
    bool alive; //if object marked s removed
    bool isPicked;
    World* world_ptr; //pointer to world, when object placed
    GameObjectLink parent; //Link to object's parent
    int render_type;

    int props_num;
    GameObjectProperty* properties[10]; //Vector to store pointers to all properties
    std::vector<GameObjectLink> children; //Vector to store links to children of object
    int getAliveChildrenAmount(); //Gets current amount of children objects (exclude removed chidren)

    QTreeWidgetItem* item_ptr;

    bool addProperty(int property); //Adds property with property ID
    bool addTransformProperty();
    bool addLabelProperty();

    GameObjectProperty* getPropertyPtrByType(int property);
    LabelProperty* getLabelProperty();
    TransformProperty* getTransformProperty();
    GameObjectLink getLinkToThisObject();
    void trimChildrenArray();

    void addChildObject(GameObjectLink link);
    void removeChildObject(GameObjectLink link);

    void saveProperties(std::ofstream* stream); //Writes properties content at end of stream
    void loadProperty(std::ifstream* world_stream); //Loads one property from stream
    void clearAll(bool clearQtWigt = true);
    void Draw(RenderPipeline* pipeline);

    GameObject(); //Default constructor
    ~GameObject();
};

class World{
protected:
    void getAvailableNumObjLabel(QString label, int* result);

public:
    void* proj_ptr; //Pointer to Project structure

    std::vector<GameObject> objects; //Vector, containing all gameobjects

    GameObject* addObject(GameObject obj);
    GameObject* newObject(); //Add new object to world
    GameObject* getObjectByLabel(QString label);
    GameObject* getObjectByStringId(std::string id);
    GameObject** getUnparentedObjs();
    void removeObj(GameObjectLink link); //Remove object from world
    GameObject* dublicateObject(GameObject* original, bool parent = true);
    void trimObjectsList();
    void unpickObject();

    ZSPIRE::Mesh* getMeshPtrByRelPath(QString label); //look through all meshes in project ptr
    ZSPIRE::Texture* getTexturePtrByRelPath(QString label); //look through all meshes in project ptr

    void saveToFile(QString file);
    void openFromFile(QString file, QTreeWidget* w_ptr);
    void clear();

    World();

};


#endif
