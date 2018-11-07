#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <QString>
#include <QTreeWidget>
#include <fstream>
#include "../../ProjEd/headers/InspectorWin.h"

#include "../../Render/headers/zs-mesh.h"
#include "../../Render/headers/zs-math.h"

#define GO_PROPERTY_TYPE_NONE 0
#define GO_PROPERTY_TYPE_TRANSFORM 1
#define GO_PROPERTY_TYPE_LABEL 2
#define GO_PROPERTY_TYPE_MESH 3

class GameObject;
class World;

class GameObjectLink{
public:
    World* world_ptr;
    std::string obj_str_id;
    GameObject* ptr;

    GameObject* updLinkPtr(); //Updates pointer to object and returns it
    bool isEmpty(); //Check, if this link doesn't link to some object
    void crack();

    GameObjectLink();
};

class GameObjectProperty{
public:
    int type; //Describe TYPE of property
    bool active; //Is property working
    QString type_label; //Label of type
    std::string object_str_id; //String ID of connected object
    World* world_ptr; //Sometimes may be useful

    GameObjectProperty();
    virtual ~GameObjectProperty();

    virtual void addPropertyInterfaceToInspector(InspectorWin* inspector);
    virtual void onValueChanged();
};

class LabelProperty : public GameObjectProperty {
public:
    QString label; //Label of gameobject
    QTreeWidgetItem* list_item_ptr;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onValueChanged();

    LabelProperty();
};

class TransformProperty : public GameObjectProperty {
public:
    ZSMATRIX4x4 transform_mat;

    ZSVECTOR3 translation;
    ZSVECTOR3 scale;
    ZSVECTOR3 rotation;

    void updateMat();
    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onValueChanged();

    TransformProperty();
};

class MeshProperty : public GameObjectProperty{
public:
    QString resource_relpath; //Relative path to resource
    ZSPIRE::Mesh* mesh_ptr; //Pointer to mesh

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void update(); //Updates pointer aaccording to resource_relpath
    MeshProperty();
};

class GameObject{
public:
    QString* label;
    std::string str_id; //String, gameobject identified by
    bool hasParent; //If object has a parent
    World* world_ptr; //pointer to world, when object placed
    GameObjectLink parent;

    std::vector<GameObjectProperty*> properties; //Vector to store pointers to all properties
    std::vector<GameObjectLink> children;
    int getAliveChildrenAmount(); //Gets current amount of children objects (exclude removed chidren)

    QTreeWidgetItem* item_ptr;

    bool addProperty(int property); //Adds property with property ID
    bool addTransformProperty();
    bool addLabelProperty();

    GameObjectProperty* getPropertyPtrByType(int property);
    LabelProperty* getLabelProperty();
    TransformProperty* getTransformProperty();
    GameObjectLink getLinkToThisObject();

    void addChildObject(GameObjectLink link);
    void removeChildObject(GameObjectLink link);

    void saveProperties(std::ofstream* stream); //Writes properties content at end of stream

    GameObject(); //Default constructor
};

class World{
protected:
    void getAvailableNumObjLabel(QString label, int* result);

public:
    void* proj_ptr;

    std::vector<GameObject> objects;

    GameObject* addObject(GameObject obj);
    GameObject* newObject();
    GameObject* getObjectByLabel(QString label);
    GameObject* getObjectByStringId(std::string id);
    GameObject** getUnparentedObjs();

    ZSPIRE::Mesh* getMeshPtrByRelPath(QString label); //look through all meshes in project ptr

    void saveToFile(QString file);
    void openFromFile(QString file, QTreeWidgetItem* root_item, QTreeWidget* w_ptr);
    void clear();

};


#endif
