#include "headers/EdActions.h"

EdActions::EdActions(){
    this->current_pos = 0;
    this->end_pos = 0;
    action_list.resize(0);
}

EdAction::EdAction(){
    type = ACT_TYPE_NONE;
}

EdSnapshotAction::EdSnapshotAction(){
    this->type = ACT_TYPE_SNAPSHOT;
}

void EdActions::newSnapshotAction(World* world_ptr){
    EdSnapshotAction* new_action = new EdSnapshotAction;
    world_ptr->putToShapshot(&new_action->snapshot);

    if(this->current_pos < this->end_pos){
        this->action_list[current_pos] = new_action;

        current_pos += 1;
        //end_pos += 1;
    }else{
        this->action_list.push_back(new_action);
        current_pos += 1;
        end_pos += 1;
    }
}

void EdActions::undo(){
    int act_type = this->action_list[current_pos - 1]->type;

    if(act_type == ACT_TYPE_SNAPSHOT){
        EdSnapshotAction* snapshot = static_cast<EdSnapshotAction*>(this->action_list[current_pos - 1]);

        world_ptr->recoverFromSnapshot(&snapshot->snapshot);

        current_pos -= 1;
    }
}

void EdActions::redo(){

}

void EdActions::clear(){

}
