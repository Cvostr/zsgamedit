#include "headers/World.h"
#include "headers/Misc.h"
#include "headers/obj_properties.h"

GameObject::GameObject(){
    item_ptr = new QTreeWidgetItem; //Allocate tree widget item
    genRandomString(&this->str_id, 15); //Generate random string ID
    isPicked = false;
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

void GameObject::clearAll(bool clearQtWigt){
    unsigned int props_num = static_cast<unsigned int>(this->props_num);

    for(unsigned int prop_i = 0; prop_i < props_num; prop_i ++){ //Walk through all children an remove them
        //Obtain pointer to property
        Engine::GameObjectProperty* prop_ptr = properties[prop_i];
        prop_ptr->onObjectDeleted(); //Call on object deletion
        delete prop_ptr; //Destroy property
    }
    this->props_num = 0; //Set property counter to zero
    children.clear();
    if(item_ptr != nullptr && clearQtWigt == true){ //if Qt tree widget item not cleared
        delete item_ptr; //Destroy Qt tree widget item to remove object from tree
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
    this->clearAll(false);

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

        ((GameObject*)parent.updLinkPtr())->item_ptr->addChild(this->item_ptr);
    }else{
        static_cast<World*>(this->world_ptr)->obj_widget_ptr->addTopLevelItem(this->item_ptr);
    }


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

}

int GameObject::getAliveChildrenAmount(){
    int result = 0;
    for(unsigned int chi_i = 0; chi_i < children.size(); chi_i ++){ //Now iterate over all children
        Engine::GameObjectLink* child_ptr = &children[chi_i];
        if(!child_ptr->isEmpty()) //if it points to something
        result += 1;
    }
    return result;
}

void GameObject::pick(){
    this->isPicked = true;
    unsigned int children_am = static_cast<unsigned int>(this->children.size());
    for(unsigned int chil_i = 0; chil_i < children_am; chil_i++){
        ((GameObject*)children[chil_i].ptr)->pick(); //child and his children are picked now
    }
}

void GameObject::DrawMesh(RenderPipeline* pipeline){
    Engine::MeshProperty* mesh_prop = static_cast<Engine::MeshProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
    Engine::TerrainProperty* terrain_prop = getPropertyPtr<Engine::TerrainProperty>();
    //Draw default mesh
    if(mesh_prop != nullptr) mesh_prop->mesh_ptr->Draw();
    if(terrain_prop != nullptr) terrain_prop->DrawMesh(pipeline);
}
