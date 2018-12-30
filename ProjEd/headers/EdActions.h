#ifndef EDACTIONS_H
#define EDACTIONS_H

#include <vector>
#include "../../World/headers/World.h"
#include "InspectorWin.h"

#define ACT_TYPE_NONE 0
#define ACT_TYPE_SNAPSHOT 1
#define ACT_TYPE_PROPERTY 2

class EdAction{
public:
    int type; //type of activity to do

    virtual void clear();
    EdAction();
};

class EdSnapshotAction : public EdAction{
public:
    WorldSnapshot snapshot; //Class object to store snapshot

    void clear();
    EdSnapshotAction();
};

class EdPropertyAction : public EdAction{
public:
    GameObjectProperty* container_ptr;

    GameObjectLink linkToObj;
    int prop_type;

    void clear();
    EdPropertyAction();
};

class EdActions{
public:
    std::vector<EdAction*> action_list;
    World* world_ptr; //Pointer to world
    InspectorWin* insp_win; //Pointer to inspector window

    int current_pos;
    int end_pos;

    void newSnapshotAction(World* world_ptr);
    void newPropertyAction(GameObjectLink link, int property_type);
    void clear();

    void undo();
    void redo();

    EdActions();
};

#endif // EDACTIONS_H
