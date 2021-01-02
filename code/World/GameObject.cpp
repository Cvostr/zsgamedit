#include "headers/World.h"
#include "headers/Misc.h"
#include <world/ObjectsComponents/ZPScriptComponent.hpp>

void Engine::GameObject::recoverFromSnapshot(Engine::GameObjectSnapshot* snapshot) {
    this->clearAll();
    //Copy object class content
    snapshot->reserved_obj.copyTo(this);
    //iterate over all properties in snapshot
    for (unsigned int i = 0; i < static_cast<unsigned int>(snapshot->props_num); i++) {
        //Pointer to property in snapshot
        Engine::IGameObjectComponent* prop_ptr = snapshot->properties[i];
        //Pointer to new allocated property
        Engine::IGameObjectComponent* new_prop_ptr = Engine::allocProperty(prop_ptr->type);
        //Copy pointer in snapshot to new pointer
        prop_ptr->copyTo(new_prop_ptr);
        this->mComponents[props_num] = new_prop_ptr;
        new_prop_ptr->go_link = this->getLinkToThisObject();
        props_num += 1;

        if (prop_ptr->type == PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL) { //If it is label, we have to do extra stuff
            Engine::LabelProperty* label_p = static_cast<Engine::LabelProperty*>(new_prop_ptr);
        }
    }
    //recover scripts
    for (unsigned int i = 0; i < static_cast<unsigned int>(snapshot->scripts_num); i++) {
        //Pointer to property in snapshot
        Engine::ZPScriptComponent* script_ptr = snapshot->mScripts[i];
        //Pointer to new allocated property
        Engine::ZPScriptComponent* new_script_ptr = static_cast<Engine::ZPScriptComponent*>
            (Engine::allocProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT));
        //Copy pointer in snapshot to new pointer
        script_ptr->copyTo(new_script_ptr);
        this->mScripts[scripts_num] = new_script_ptr;
        new_script_ptr->go_link = this->getLinkToThisObject();
        scripts_num += 1;
    }

    if (this->hasParent) { //if object was parented
        snapshot->parent_link.updLinkPtr()->mChildren.push_back(this->getLinkToThisObject());
        this->mParent = snapshot->parent_link;
    }
    //Also recover children
    for (unsigned int i = 0; i < snapshot->children.size(); i++) {
        Engine::GameObjectLink link = snapshot->children[i];
        link.updLinkPtr()->recoverFromSnapshot(&snapshot->children_snapshots[i]);
    }
    GO_W_I::updateGameObjectItem(this);
}

void Engine::GameObject::pick(){
    mWorld->picked_objs_ids.push_back(array_index);
    for(unsigned int chil_i = 0; chil_i < mChildren.size(); chil_i++){
        Engine::GameObject* ChildPtr = mChildren[chil_i].updLinkPtr();
        if (ChildPtr == nullptr)
            continue;
        ChildPtr->pick(); //child and his children are picked now
    }
}

void Engine::GameObject::saveProperties(ZsStream* stream) {
    for (unsigned int prop_i = 0; prop_i < props_num; prop_i++) {
        Engine::IGameObjectComponent* property_ptr = this->mComponents[prop_i];
        *stream << "\nG_PROPERTY ";
        stream->writeBinaryValue(&property_ptr->type);
        stream->writeBinaryValue(&property_ptr->active);
        *stream << " ";

        saveProperty(property_ptr, stream);
    }
    for (unsigned int script_i = 0; script_i < this->scripts_num; script_i++) {
        Engine::ZPScriptComponent* script = mScripts[script_i];
        saveProperty(script, stream);
        *stream << '\n';
    }
}

void Engine::GameObject::saveProperty(IGameObjectComponent* prop, ZsStream* stream) {
    auto ptr = prop;
    ptr->savePropertyToStream(stream, this);
}