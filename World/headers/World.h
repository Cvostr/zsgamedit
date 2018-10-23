#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <QString>
#include <QTreeWidget>
#include "../../ProjEd/headers/InspectorWin.h"

#include "../../Render/headers/zs-math.h"

#define GO_PROPERTY_TYPE_NONE 0
#define GO_PROPERTY_TYPE_TRANSFORM 1

class GameObjectProperty{
public:
    int type; //Describe TYPE of property
    bool active; //Is property working

    GameObjectProperty();
    virtual ~GameObjectProperty();

    virtual void addPropertyInterfaceToInspector(InspectorWin* inspector);
};

class TransformProperty : public GameObjectProperty {
public:
    ZSMATRIX4x4 transform_mat;

    ZSVECTOR3 translation;
    ZSVECTOR3 scale;
    ZSVECTOR3 rotation;

    void updateMat();
    void addPropertyInterfaceToInspector(InspectorWin* inspector);

    TransformProperty();
};

class GameObject{
public:
    QString label;
    std::string str_id; //String, gameobject identified by
    bool hasParent; //If object has a parent

    std::vector<GameObjectProperty*> properties; //Vector to store pointers to all properties

    QTreeWidgetItem* item_ptr;

    bool addTransformPropety();

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

    void saveToFile(QString file);
    void openFromFile(QString file);

};


#endif
