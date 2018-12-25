#include "headers/EdActions.h"

EdActions::EdActions(){
    this->current_pos = 0;
    this->end_pos = 0;
    action_list.resize(0);
}

EdAction::EdAction(){
    type = ACT_TYPE_NONE;
}

void EdAction::clear(){

}

EdSnapshotAction::EdSnapshotAction(){
    this->type = ACT_TYPE_SNAPSHOT;
}

EdPropertyAction::EdPropertyAction(){
    this->type = ACT_TYPE_PROPERTY;
    this->container_ptr = nullptr;
}

void EdSnapshotAction::clear(){
    this->snapshot.clear();
}

void EdPropertyAction::clear(){

}

void EdActions::newSnapshotAction(World* world_ptr){
    EdSnapshotAction* new_action = new EdSnapshotAction;
    world_ptr->putToShapshot(&new_action->snapshot);

    if(this->current_pos < this->end_pos){
        this->action_list[current_pos] = new_action;

        current_pos += 1;
    }else{
        this->action_list.push_back(new_action);
        current_pos += 1;
        end_pos += 1;
    }
}

void EdActions::newPropertyAction(GameObjectLink link, int property_type){
    EdPropertyAction* new_action = new EdPropertyAction; //Allocate memory for action class
    new_action->linkToObj = link; //Store link to object
    new_action->prop_type = property_type; //Sore property type
    new_action->container_ptr = allocProperty(property_type); //Allocate property

    if(this->current_pos < this->end_pos){
        this->action_list[current_pos] = new_action;

        current_pos += 1;
    }else{
        this->action_list.push_back(new_action);
        current_pos += 1;
        end_pos += 1;
    }
}

void EdActions::undo(){
    if(current_pos == 0) return; //if no actions done
    int act_type = this->action_list[current_pos - 1]->type; //getting action type

    if(act_type == ACT_TYPE_SNAPSHOT){ //if this action is snapshot
        EdSnapshotAction* snapshot = static_cast<EdSnapshotAction*>(this->action_list[current_pos - 1]);

        world_ptr->recoverFromSnapshot(&snapshot->snapshot);

        current_pos -= 1;
    }
}

void EdActions::redo(){

}

void EdActions::clear(){
    for(unsigned int action_it = 0; action_it < this->action_list.size(); action_it ++){
        this->action_list[action_it]->clear();
    }
    current_pos = 0;
    end_pos = 0;
    action_list.clear();
}
