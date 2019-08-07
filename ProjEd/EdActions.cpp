#include "headers/EdActions.h"

EdActions::EdActions(){
    this->current_pos = 0; //Default pos is zero
    this->end_pos = 0; //Default tail pos is zero as well
    action_list.resize(0);
    storeActions = false;
    this->insp_win = nullptr;
    hasChangesUnsaved = false; //Defaultly, nothing to save
}

EdAction::EdAction(){
    type = ACT_TYPE_NONE;
}

EdAction::~EdAction(){

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

EdObjectAction::EdObjectAction(){
    this->type = ACT_TYPE_OBJECT;
}

void EdSnapshotAction::clear(){
    this->snapshot.clear();
}

void EdPropertyAction::clear(){
    delete container_ptr;
}

void EdObjectAction::clear(){
    this->snapshot.clear();
}

void EdActions::newSnapshotAction(World* world_ptr){
    EdSnapshotAction* new_action = new EdSnapshotAction;
    world_ptr->putToShapshot(&new_action->snapshot);

    putNewAction(new_action);
}

void EdActions::newPropertyAction(GameObjectLink link, PROPERTY_TYPE property_type){
    EdPropertyAction* new_action = new EdPropertyAction; //Allocate memory for action class
    new_action->linkToObj = link; //Store link to object
    new_action->linkToObj.updLinkPtr();
    new_action->prop_type = property_type; //Sore property type
    new_action->container_ptr = allocProperty(property_type); //Allocate property
    GameObjectProperty* origin_prop = link.updLinkPtr()->getPropertyPtrByType(property_type);
    origin_prop->copyTo(new_action->container_ptr);

    putNewAction(new_action);
}

void EdActions::newGameObjectAction(GameObjectLink link){
    EdObjectAction* new_action = new EdObjectAction;

    GameObject* obj_ptr = link.updLinkPtr();

    obj_ptr->putToSnapshot(&new_action->snapshot);
    new_action->linkToObj = link;
    new_action->linkToObj.updLinkPtr();

    putNewAction(new_action);
}

void EdActions::undo(){
    if(current_pos == 0) return; //if no actions done
    int act_type = this->action_list[current_pos - 1]->type; //getting action type

    if(act_type == ACT_TYPE_SNAPSHOT){ //if this action is snapshot
        EdSnapshotAction* snapshot = static_cast<EdSnapshotAction*>(this->action_list[current_pos - 1]);

        WorldSnapshot cur_state_snap; //Declare snapshot to store current state
        world_ptr->putToShapshot(&cur_state_snap); //Backup current state

        world_ptr->recoverFromSnapshot(&snapshot->snapshot); //Recover previous state

        snapshot->clear(); //Clear previous state
        snapshot->snapshot = cur_state_snap; //put previous state to current actions
    }

    if(act_type == ACT_TYPE_OBJECT){ //if this action is snapshot
        EdObjectAction* snapshot = static_cast<EdObjectAction*>(this->action_list[current_pos - 1]);

        GameObjectSnapshot cur_state_snap; //Declare snapshot to store current state
        snapshot->linkToObj.ptr->putToSnapshot(&cur_state_snap); //Backup current state

        int array_index = snapshot->snapshot.obj_array_ind;

        world_ptr->objects[array_index].recoverFromSnapshot(&snapshot->snapshot); //Recover previous state

        snapshot->clear(); //Clear previous state
        snapshot->snapshot = cur_state_snap; //put previous state to current actions
    }

    if(act_type == ACT_TYPE_PROPERTY){ //if this action is property
        EdPropertyAction* snapshot = static_cast<EdPropertyAction*>(this->action_list[current_pos - 1]);
        //Declare pointer to destination
        GameObjectProperty* dest = snapshot->linkToObj.updLinkPtr()->getPropertyPtrByType(snapshot->prop_type);
        //Backup current property data
        GameObjectProperty* cur_state_prop = allocProperty(snapshot->prop_type); //Allocate property for current state
        dest->copyTo(cur_state_prop); //Copy current property data to buffer

        //Make undo (copy saved data to property in object)
        snapshot->container_ptr->copyTo(dest);
        //Call onValueChanged() on recovered property
        dest->onValueChanged();
        this->insp_win->updateObjectProperties();

        snapshot->clear();
        snapshot->container_ptr = cur_state_prop;
    }
    current_pos -= 1; //Move to back
}

void EdActions::redo(){
    if(current_pos == end_pos) return; //if no actions next to this
    int act_type = this->action_list[current_pos]->type; //getting action type

    if(act_type == ACT_TYPE_SNAPSHOT){ //if this action is snapshot
        EdSnapshotAction* snapshot = static_cast<EdSnapshotAction*>(this->action_list[current_pos]);

        WorldSnapshot cur_state_snap;
        world_ptr->putToShapshot(&cur_state_snap);

        world_ptr->recoverFromSnapshot(&snapshot->snapshot);

        snapshot->snapshot = cur_state_snap;
    }

    if(act_type == ACT_TYPE_OBJECT){
        EdObjectAction* snapshot = static_cast<EdObjectAction*>(this->action_list[current_pos ]);

        GameObjectSnapshot cur_state_snap; //Declare snapshot to store current state
        snapshot->linkToObj.ptr->putToSnapshot(&cur_state_snap); //Backup current state

        int array_index = snapshot->snapshot.obj_array_ind;

        world_ptr->objects[array_index].recoverFromSnapshot(&snapshot->snapshot); //Recover previous state

        snapshot->clear(); //Clear previous state
        snapshot->snapshot = cur_state_snap; //put previous state to current actions
    }

    if(act_type == ACT_TYPE_PROPERTY){ //if this action is property
        EdPropertyAction* snapshot = static_cast<EdPropertyAction*>(this->action_list[current_pos]);
        //Declare pointer to destination
        GameObjectProperty* dest = snapshot->linkToObj.updLinkPtr()->getPropertyPtrByType(snapshot->prop_type);
        //Backup current property data
        GameObjectProperty* cur_state_prop = allocProperty(snapshot->prop_type); //Allocate property for current state
        dest->copyTo(cur_state_prop); //Copy current property data to buffer
        //Make undo

        snapshot->container_ptr->copyTo(dest);
        //Call onValueChanged() on recovered property
        dest->onValueChanged();
        this->insp_win->updateObjectProperties();

        snapshot->clear();
        snapshot->container_ptr = cur_state_prop;
    }
    current_pos += 1; //Move forward
}

void EdActions::putNewAction(EdAction* action){
    //Actions storing disabled
    if(!storeActions) {
        action->clear();
        return;
    }
    //We made some change
    hasChangesUnsaved = true;

    //if we have some positions left in vector
    if(this->current_pos < this->end_pos){
        this->action_list[static_cast<unsigned int>(current_pos)] = action;

        current_pos += 1;
    }else{ //Allocate new space in vector
        this->action_list.push_back(action);
        current_pos += 1;
        end_pos += 1;
    }
}

void EdActions::clear(){
    for(unsigned int action_it = 0; action_it < this->action_list.size(); action_it ++){
        this->action_list[action_it]->clear();
    }
    current_pos = 0;
    end_pos = 0;
    action_list.clear();
}

void EdActions::setStoreActions(bool store){
    this->storeActions = store;
}
