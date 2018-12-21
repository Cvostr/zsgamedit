#ifndef EDACTIONS_H
#define EDACTIONS_H

#include <vector>
#include "../../World/headers/World.h"

#define ACT_TYPE_NONE 0
#define ACT_TYPE_SNAPSHOT 1
#define ACT_TYPE_DWORD 2

class EdAction{
public:
    int type; //type of activity to do

    EdAction();
};

class EdSnapshotAction : public EdAction{
public:
    WorldSnapshot snapshot;

    EdSnapshotAction();
};

class EdActions{
public:
    std::vector<EdAction*> action_list;
    World* world_ptr;

    int current_pos;
    int end_pos;

    void newSnapshotAction(World* world_ptr);
    void clear();

    void undo();
    void redo();

    EdActions();
};

#endif // EDACTIONS_H
