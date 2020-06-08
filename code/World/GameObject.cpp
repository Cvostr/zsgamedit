#include "headers/World.h"
#include "headers/Misc.h"
#include <world/go_properties.h>

GameObject::GameObject(){
    item_ptr = new QTreeWidgetItem; //Allocate tree widget item
}

GameObject::~GameObject(){
}

bool GameObject::addProperty(PROPERTY_TYPE property){
    unsigned int props = static_cast<unsigned int>(this->props_num);
    for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
        Engine::GameObjectProperty* property_ptr = this->properties[prop_i];
        if(property_ptr->type == property){ //If object already has one
            return false; //Exit function
        }
    }
    Engine::GameObjectProperty* _ptr = _allocProperty(property);
    if(property == GO_PROPERTY_TYPE_LABEL){
        LabelProperty* ptr = static_cast<LabelProperty*>(_ptr);
        ptr->list_item_ptr = this->item_ptr;
    }

    _ptr->go_link = this->getLinkToThisObject();
    _ptr->go_link.updLinkPtr();
    _ptr->world_ptr = static_cast<World*>(this->world_ptr); //Assign pointer to world
    this->properties[props_num] = _ptr; //Store property in gameobject
    this->props_num += 1;
    return true;
}

void GameObject::setMeshSkinningRootNodeRecursively(GameObject* rootNode){
    Engine::MeshProperty* mesh = getPropertyPtr<Engine::MeshProperty>();
    if(mesh)
        mesh->skinning_root_node = rootNode;

    for(unsigned int ch_i = 0; ch_i < children.size(); ch_i ++){
        GameObject* obj_ptr = (GameObject*)children[ch_i].updLinkPtr();
        obj_ptr->setMeshSkinningRootNodeRecursively(rootNode);
    }
}

void GameObject::putToSnapshot(GameObjectSnapshot* snapshot){
    snapshot->props_num = 0;

    snapshot->parent_link = this->parent;
    snapshot->obj_array_ind = this->array_index;

    this->copyTo(&snapshot->reserved_obj);
    //Copy all properties
    for(unsigned int i = 0; i < this->props_num; i ++){
        Engine::GameObjectProperty* prop_ptr = this->properties[i];
        Engine::GameObjectProperty* new_prop_ptr = _allocProperty(prop_ptr->type);
        prop_ptr->copyTo(new_prop_ptr);
        snapshot->properties[snapshot->props_num] = new_prop_ptr;
        snapshot->props_num += 1;
    }
    snapshot->children_snapshots.resize(this->children.size());
    //Copy all children links
    for(unsigned int i = 0; i < this->children.size(); i ++){
        snapshot->children.push_back(this->children[i]);

        ((GameObject*)children[i].ptr)->putToSnapshot(&snapshot->children_snapshots[i]);
    }
}
void GameObject::recoverFromSnapshot(GameObjectSnapshot* snapshot){
    this->clearAll();
    //if restored object is dead, then hide it in object list
    if(snapshot->reserved_obj.alive == false){
        delete this->item_ptr;
        item_ptr = nullptr;
    }

    if(snapshot->reserved_obj.alive == true){
        this->item_ptr = new QTreeWidgetItem;
    }

    //Copy object class content
    snapshot->reserved_obj.copyTo(this);
    //iterate over all properties in snapshot
    for(unsigned int i = 0; i < static_cast<unsigned int>(snapshot->props_num); i ++){
        //Pointer to property in snapshot
        Engine::GameObjectProperty* prop_ptr = snapshot->properties[i];
        //Pointer to new allocated property
        Engine::GameObjectProperty* new_prop_ptr = _allocProperty(prop_ptr->type);
        //Copy pointer in snapshot to new pointer
        prop_ptr->copyTo(new_prop_ptr);
        this->properties[props_num] = new_prop_ptr;
        new_prop_ptr->go_link = this->getLinkToThisObject();
        props_num += 1;

        if(prop_ptr->type == GO_PROPERTY_TYPE_LABEL){ //If it is label, we have to do extra stuff
            LabelProperty* label_p = static_cast<LabelProperty*>(new_prop_ptr);
            this->label_ptr = &label_p->label;
            this->item_ptr->setText(0, QString::fromStdString(*this->label_ptr));
            label_p->list_item_ptr = this->item_ptr;
        }
    }

    if(this->hasParent){ //if object was parented
        snapshot->parent_link.updLinkPtr()->children.push_back(this->getLinkToThisObject());
        this->parent = snapshot->parent_link;
        //Show visual parenting in object list
        ((GameObject*)parent.updLinkPtr())->item_ptr->addChild(this->item_ptr);
    }else{
        //Object has no parents, show it in top
        static_cast<World*>(this->world_ptr)->obj_widget_ptr->addTopLevelItem(this->item_ptr);
    }
    //Also recover children
    for(unsigned int i = 0; i < snapshot->children.size(); i ++){
        Engine::GameObjectLink link = snapshot->children[i];
        ((GameObject*)link.updLinkPtr())->recoverFromSnapshot(&snapshot->children_snapshots[i]);
    }
}

void GameObjectSnapshot::clear(){
    this->children.clear();

    for(unsigned int prop = 0; prop < static_cast<unsigned int>(this->props_num); prop ++){
        auto prop_ptr = this->properties[prop];
        delete prop_ptr;
        prop_ptr = nullptr;
    }
    props_num = 0;

    for(unsigned int child = 0; child < this->children.size(); child ++){
        children_snapshots[child].clear();
    }
    children_snapshots.clear(); //Free snapshot vector
    this->children.clear(); //Free link vector
}

GameObjectSnapshot::GameObjectSnapshot(){
    props_num = 0;
    obj_array_ind = 0;
}

void GameObject::pick(){
    ((World*)world_ptr)->picked_objs_ids.push_back(array_index);
    for(unsigned int chil_i = 0; chil_i < children.size(); chil_i++){
        ((GameObject*)children[chil_i].updLinkPtr())->pick(); //child and his children are picked now
    }
}
