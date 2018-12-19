#ifndef EDACTIONS_H
#define EDACTIONS_H

#include <vector>
#include "../../World/headers/World.h"

#define ACT_TYPE_SNAPSHOT 1
#define ACT_TYPE_DWORD 2

class EdAction{
    bool created;
    int type; //type of activity to do
};

class EdSnapshotAction : public EdAction{
    WorldSnapshot snapshot;
};

class EdActions{
public:
    std::vector<EdAction*> action_list;

    int current_pos;

    EdActions();
};

#endif // EDACTIONS_H
