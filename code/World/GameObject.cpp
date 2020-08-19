#include "headers/World.h"
#include "headers/Misc.h"
#include <world/go_properties.h>

void Engine::GameObject::recoverFromSnapshot(Engine::GameObjectSnapshot* snapshot) {
    this->clearAll();
    //Copy object class content
    snapshot->reserved_obj.copyTo(this);
    //iterate over all properties in snapshot
    for (unsigned int i = 0; i < static_cast<unsigned int>(snapshot->props_num); i++) {
        //Pointer to property in snapshot
        Engine::GameObjectProperty* prop_ptr = snapshot->properties[i];
        //Pointer to new allocated property
        Engine::GameObjectProperty* new_prop_ptr = Engine::allocProperty(prop_ptr->type);
        //Copy pointer in snapshot to new pointer
        prop_ptr->copyTo(new_prop_ptr);
        this->properties[props_num] = new_prop_ptr;
        new_prop_ptr->go_link = this->getLinkToThisObject();
        props_num += 1;

        if (prop_ptr->type == PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL) { //If it is label, we have to do extra stuff
            Engine::LabelProperty* label_p = static_cast<Engine::LabelProperty*>(new_prop_ptr);
            this->label_ptr = &label_p->label;
        }
    }
    //recover scripts
    for (unsigned int i = 0; i < static_cast<unsigned int>(snapshot->scripts_num); i++) {
        //Pointer to property in snapshot
        Engine::ZPScriptProperty* script_ptr = static_cast<Engine::ZPScriptProperty*>(snapshot->scripts[i]);
        //Pointer to new allocated property
        Engine::ZPScriptProperty* new_script_ptr = static_cast<Engine::ZPScriptProperty*>
            (Engine::allocProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT));
        //Copy pointer in snapshot to new pointer
        script_ptr->copyTo(new_script_ptr);
        this->scripts[scripts_num] = new_script_ptr;
        new_script_ptr->go_link = this->getLinkToThisObject();
        scripts_num += 1;
    }

    if (this->hasParent) { //if object was parented
        snapshot->parent_link.updLinkPtr()->children.push_back(this->getLinkToThisObject());
        this->parent = snapshot->parent_link;
    }
    //Also recover children
    for (unsigned int i = 0; i < snapshot->children.size(); i++) {
        Engine::GameObjectLink link = snapshot->children[i];
        static_cast<GameObject*>(parent.updLinkPtr())->recoverFromSnapshot(&snapshot->children_snapshots[i]);
    }
    GO_W_I::updateGameObjectItem(this);
}

void Engine::GameObject::pick(){
    ((World*)world_ptr)->picked_objs_ids.push_back(array_index);
    for(unsigned int chil_i = 0; chil_i < children.size(); chil_i++){
        (static_cast<GameObject*>(children[chil_i].updLinkPtr()))->pick(); //child and his children are picked now
    }
}
