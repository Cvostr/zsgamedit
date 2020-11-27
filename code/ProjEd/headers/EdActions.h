#ifndef EDACTIONS_H
#define EDACTIONS_H

#include <vector>
#include "../../World/headers/World.h"
#include "InspectorWin.h"

#define ACT_TYPE_NONE 0
#define ACT_TYPE_SNAPSHOT 1
#define ACT_TYPE_PROPERTY 2
#define ACT_TYPE_OBJECT 3

class EdAction{
public:
    int type; //type of activity to do

    virtual void clear();
    EdAction();
    virtual ~EdAction();
};

class EdSnapshotAction : public EdAction{
public:
    Engine::WorldSnapshot snapshot; //Class object to store snapshot

    void clear();
    EdSnapshotAction();
};

class EdPropertyAction : public EdAction{
public:
    Engine::IGameObjectComponent* container_ptr;

    Engine::GameObjectLink linkToObj;
    PROPERTY_TYPE prop_type;

    void clear();
    EdPropertyAction();
};

class EdObjectAction : public EdAction{
public:
    Engine::GameObjectSnapshot snapshot;
    Engine::GameObjectLink linkToObj;

    void clear();
    EdObjectAction();
};

class EdActions{
private:
    bool storeActions;
public:
    std::vector<EdAction*> action_list;
    World* world_ptr; //Pointer to world
    bool hasChangesUnsaved;
    int current_pos;
    int end_pos;

    void setStoreActions(bool store);

    void newSnapshotAction(Engine::World* world_ptr);
    void newPropertyAction(Engine::GameObjectLink link, PROPERTY_TYPE property_type);
    void newGameObjectAction(Engine::GameObjectLink link);

    void putNewAction(EdAction* action);

    void clear();

    void undo();
    void redo();

    EdActions();
};

#endif // EDACTIONS_H
