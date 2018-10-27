#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <QString>
#include <QTreeWidget>
#include "../../ProjEd/headers/InspectorWin.h"

#include "../../Render/headers/zs-math.h"

#define GO_PROPERTY_TYPE_NONE 0
#define GO_PROPERTY_TYPE_TRANSFORM 1
#define GO_PROPERTY_TYPE_LABEL 2

class GameObject;

class GameObjectProperty{
public:
    int type; //Describe TYPE of property
    bool active; //Is property working
    QString type_label; //Label of type
    std::string object_str_id; //String ID of connected object
    //GameObject* gobject_ptr; //Pointer to host gameobject

    GameObjectProperty();
    virtual ~GameObjectProperty();

    virtual void addPropertyInterfaceToInspector(InspectorWin* inspector);
    virtual void onValueChanged();
};

class LabelProperty : public GameObjectProperty {
public:
    QString label; //Label of gameobject

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

class GameObject{
public:
    QString* label;
    std::string str_id; //String, gameobject identified by
    bool hasParent; //If object has a parent

    std::vector<GameObjectProperty*> properties; //Vector to store pointers to all properties

    QTreeWidgetItem* item_ptr;

    bool addProperty(int property); //Adds property with property ID
    bool addTransformProperty();
    bool addLabelProperty();

    GameObjectProperty* getPropertyPtrByType(int property);
    LabelProperty* getLabelProperty();
    TransformProperty* getTransformProperty();

    GameObject(); //Default constructor
};

class World{
protected:
    void getAvailableNumObjLabel(QString label, int* result);

public:
    std::vector<GameObject> objects;

    GameObject* addObject(GameObject obj);
    GameObject* newObject();
    GameObject* getObjectByLabel(QString label);
    GameObject* getObjectByStringId();

    void saveToFile(QString file);
    void openFromFile(QString file);

};


#endif
