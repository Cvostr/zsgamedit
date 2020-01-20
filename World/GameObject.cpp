#include "headers/World.h"
#include "headers/Misc.h"
#include "headers/obj_properties.h"

GameObjectLink::GameObjectLink(){
    ptr = nullptr;
    world_ptr = nullptr;
}

GameObject* GameObjectLink::updLinkPtr(){
    if(world_ptr == nullptr) //If world not defined, exiting
        return nullptr;
    if(this->ptr != nullptr) //if pointer already calculated
        if(this->obj_str_id.compare(ptr->str_id) == 0)
            return ptr;

    this->ptr = world_ptr->getObjectByStringId(this->obj_str_id);
    return ptr;
}

bool GameObjectLink::isEmpty(){
    if (this->world_ptr == nullptr) return true;
    return false;
}

void GameObjectLink::crack(){
    this->world_ptr = nullptr; //It will now pass isEmpty() check
}

GameObject::GameObject(){
    this->hasParent = false; //No parent by default
    this->world_ptr = nullptr;
    item_ptr = new QTreeWidgetItem; //Allocate tree widget item
    genRandomString(&this->str_id, 15); //Generate random string ID
    //render_type = GO_RENDER_TYPE_NONE; //No render by default
    alive = true; //Object exist by default
    IsStatic = false; //Object is dynamic by default
    isPicked = false;
    active = true;
    props_num = 0;
    label = nullptr;

    for(unsigned int prop_i = 0; prop_i < OBJ_PROPS_SIZE; prop_i ++){ //iterate over all property pointers and clear them
        properties[prop_i] = nullptr;
    }
}

GameObject::~GameObject(){
}

bool GameObject::addProperty(PROPERTY_TYPE property){
    unsigned int props = static_cast<unsigned int>(this->props_num);
    for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
        GameObjectProperty* property_ptr = this->properties[prop_i];
        if(property_ptr->type == property){ //If object already has one
            return false; //Exit function
        }
    }
    GameObjectProperty* _ptr = allocProperty(property);
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

bool GameObject::addTransformProperty(){
    return addProperty(GO_PROPERTY_TYPE_TRANSFORM);
}

bool GameObject::addLabelProperty(){
    return addProperty(GO_PROPERTY_TYPE_LABEL);
}

GameObjectProperty* GameObject::getPropertyPtrByType(PROPERTY_TYPE property){
    unsigned int props = static_cast<unsigned int>(this->props_num);
    for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
        GameObjectProperty* property_ptr = this->properties[prop_i];
        if(property_ptr->type == property){ //If object already has one
            return property_ptr; //return it
        }
    }
    return nullptr;
}

GameObjectProperty* GameObject::getPropertyPtrByTypeI(int property){
    unsigned int props = static_cast<unsigned int>(this->props_num);
    for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
        GameObjectProperty* property_ptr = this->properties[prop_i];
        if(property_ptr->type == property){ //If object already has one
            return property_ptr; //return it
        }
    }
    return nullptr;
}

GameObjectLink GameObject::getLinkToThisObject(){
    GameObjectLink link; //Definition of result link
    link.obj_str_id = this->str_id; //Placing string id
    link.world_ptr = static_cast<World*>(this->world_ptr); //Placing world pointer

    link.ptr = static_cast<World*>(world_ptr)->getObjectByStringId(link.obj_str_id);
    return link;
}

void GameObject::uncheckInactive(){
    //for(unsigned int i = 0; i < this->ch)
}

LabelProperty* GameObject::getLabelProperty(){
    return static_cast<LabelProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_LABEL));
}
TransformProperty* GameObject::getTransformProperty(){
    return static_cast<TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
}

void GameObject::addChildObject(GameObjectLink link, bool updTransform){
    GameObjectLink _link = link;
    _link.updLinkPtr(); //Calculating object pointer
    _link.ptr->hasParent = true; //Object now has a parent (if it has't before)
    _link.ptr->parent.obj_str_id = this->getLinkToThisObject().obj_str_id; //Assigning pointer to new parent
    _link.ptr->parent.world_ptr = this->getLinkToThisObject().world_ptr;
    _link.ptr->parent.updLinkPtr();

    //Updating child's transform
    //Now check, if it is possible
    TransformProperty* Pobj_transform = static_cast<TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    TransformProperty* Cobj_transform = static_cast<TransformProperty*>(link.ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));

    if(updTransform && Pobj_transform != nullptr && Cobj_transform != nullptr){ //If both objects have transform property

        ZSVECTOR3 p_translation = ZSVECTOR3(0,0,0);
        ZSVECTOR3 p_scale = ZSVECTOR3(1,1,1);
        ZSVECTOR3 p_rotation = ZSVECTOR3(0,0,0);
        Pobj_transform->getAbsoluteParentTransform(p_translation, p_scale, p_rotation); //Collecting transforms
        //Change transform
        Cobj_transform->translation = Cobj_transform->translation - p_translation;
        Cobj_transform->scale = Cobj_transform->scale / p_scale;
        Cobj_transform->rotation = Cobj_transform->rotation - p_rotation;

        Cobj_transform->updateMat(); //Update transform matrix
    }

    this->children.push_back(_link);
}
void GameObject::removeChildObject(GameObjectLink link){
    unsigned int children_am = static_cast<unsigned int>(children.size()); //get children amount
    for(unsigned int i = 0; i < children_am; i++){ //Iterate over all children in object
        GameObjectLink* link_ptr = &children[i];
        if(link.obj_str_id.compare(link_ptr->obj_str_id) == 0){ //if str_id in requested link compares to iteratable link
            GameObject* ptr = children[i].updLinkPtr();
            children[i].crack(); //Make link broken

            //Updating child's transform
            //Now check, if it is possible
            TransformProperty* Pobj_transform = static_cast<TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
            TransformProperty* Cobj_transform = static_cast<TransformProperty*>(ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));

            if(Pobj_transform != nullptr && Cobj_transform != nullptr){ //If both objects have mesh property

                ZSVECTOR3 p_translation = ZSVECTOR3(0,0,0);
                ZSVECTOR3 p_scale = ZSVECTOR3(1,1,1);
                ZSVECTOR3 p_rotation = ZSVECTOR3(0,0,0);
                Pobj_transform->getAbsoluteParentTransform(p_translation, p_scale, p_rotation);

                Cobj_transform->translation = Cobj_transform->translation + p_translation;

                Cobj_transform->scale = Cobj_transform->scale * p_scale;

                Cobj_transform->rotation = Cobj_transform->rotation + Pobj_transform->rotation;

                Cobj_transform->updateMat(); //Update transform matrix
            }
        }
    }
    trimChildrenArray(); //Remove broken link from vector
}

GameObject* GameObject::getChildObjectWithNodeLabel(std::string label){
    //This function works recursively
    //Iterate over all children in current object
    for (unsigned int i = 0; i < this->children.size(); i ++) {
        GameObjectLink* l = &this->children[i];
        NodeProperty* node_p = l->updLinkPtr()->getPropertyPtr<NodeProperty>();
        //if node's name match
        if(!node_p->node_label.compare(label))
            //Then return object with this node
            return l->updLinkPtr();
        //call function from this child
        GameObject* obj_ch = l->updLinkPtr()->getChildObjectWithNodeLabel(label);
        if(obj_ch != nullptr) return obj_ch;

    }
    return nullptr;
}

void GameObject::setMeshSkinningRootNodeRecursively(GameObject* rootNode){
    MeshProperty* mesh = getPropertyPtr<MeshProperty>();
    if(mesh)
        mesh->skinning_root_node = rootNode;

    for(unsigned int ch_i = 0; ch_i < children.size(); ch_i ++){
        GameObject* obj_ptr = children[ch_i].updLinkPtr();
        obj_ptr->setMeshSkinningRootNodeRecursively(rootNode);
    }
}

void GameObject::trimChildrenArray(){
    for (unsigned int i = 0; i < children.size(); i ++) { //Iterating over all objects
        if(children[i].isEmpty() == true){ //If object marked as deleted
            for (unsigned int obj_i = i + 1; obj_i < children.size(); obj_i ++) { //Iterate over all next chidren
                children[obj_i - 1] = children[obj_i]; //Move it to previous place
            }
            children.resize(children.size() - 1); //Reduce vector length
        }
    }
}

void GameObject::clearAll(bool clearQtWigt){
    unsigned int props_num = static_cast<unsigned int>(this->props_num);

    for(unsigned int prop_i = 0; prop_i < props_num; prop_i ++){ //Walk through all children an remove them
        //Obtain pointer to property
        GameObjectProperty* prop_ptr = properties[prop_i];
        prop_ptr->onObjectDeleted(); //Call on object deletion
        delete prop_ptr; //Destroy property
    }
    this->props_num = 0; //Set property counter to zero
    children.clear();
    if(item_ptr != nullptr && clearQtWigt == true){ //if Qt tree widget item not cleared
        delete item_ptr; //Destroy Qt tree widget item to remove object from tree
    }
}

void GameObject::removeProperty(int index){
    GameObjectProperty* prop_ptr = static_cast<GameObjectProperty*>(this->properties[index]);
    prop_ptr->onObjectDeleted();
    this->properties[index] = nullptr; //set as deleted

    for(unsigned int i = static_cast<unsigned int>(index); i < props_num - 1; i ++){
        properties[i] = properties[i + 1];
    }

    props_num -= 1;
}

void GameObject::onUpdate(int deltaTime){
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        if(!properties[i]->active) continue; //if property is inactive, then skip it
        properties[i]->onUpdate(deltaTime); //and call onUpdate on each property
    }
}

void GameObject::onTrigger(GameObject* obj){
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        properties[i]->onTrigger(obj); //and call onUpdate on each property
    }
}

void GameObject::onPreRender(RenderPipeline* pipeline){
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        if(!properties[i]->active) continue; //if property is inactive, then skip it
        properties[i]->onPreRender(pipeline); //and call onUpdate on each property
    }
}

void GameObject::onRender(RenderPipeline* pipeline){
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        if(!properties[i]->active) continue; //if property is inactive, then skip it
        properties[i]->onRender(pipeline); //and call onUpdate on each property
    }
}


void GameObject::putToSnapshot(GameObjectSnapshot* snapshot){
    snapshot->props_num = 0;

    snapshot->parent_link = this->parent;
    snapshot->obj_array_ind = this->array_index;

    this->copyTo(&snapshot->reserved_obj);
    //Copy all properties
    for(unsigned int i = 0; i < this->props_num; i ++){
        GameObjectProperty* prop_ptr = this->properties[i];
        GameObjectProperty* new_prop_ptr = allocProperty(prop_ptr->type);
        prop_ptr->copyTo(new_prop_ptr);
        snapshot->properties[snapshot->props_num] = new_prop_ptr;
        snapshot->props_num += 1;
    }
    snapshot->children_snapshots.resize(this->children.size());
    //Copy all children links
    for(unsigned int i = 0; i < this->children.size(); i ++){
        snapshot->children.push_back(this->children[i]);

        children[i].ptr->putToSnapshot(&snapshot->children_snapshots[i]);
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
        GameObjectProperty* prop_ptr = snapshot->properties[i];
        //Pointer to new allocated property
        GameObjectProperty* new_prop_ptr = allocProperty(prop_ptr->type);
        //Copy pointer in snapshot to new pointer
        prop_ptr->copyTo(new_prop_ptr);
        this->properties[props_num] = new_prop_ptr;
        new_prop_ptr->go_link = this->getLinkToThisObject();
        props_num += 1;

        if(prop_ptr->type == GO_PROPERTY_TYPE_LABEL){ //If it is label, we have to do extra stuff
            LabelProperty* label_p = static_cast<LabelProperty*>(new_prop_ptr);
            this->label = &label_p->label;
            this->item_ptr->setText(0, *this->label);
            label_p->list_item_ptr = this->item_ptr;
        }
    }


    if(this->hasParent){ //if object was parented
        snapshot->parent_link.updLinkPtr()->children.push_back(this->getLinkToThisObject());
        this->parent = snapshot->parent_link;

        parent.updLinkPtr()->item_ptr->addChild(this->item_ptr);
    }else{
        static_cast<World*>(this->world_ptr)->obj_widget_ptr->addTopLevelItem(this->item_ptr);
    }


    for(unsigned int i = 0; i < snapshot->children.size(); i ++){
        GameObjectLink link = snapshot->children[i];
        link.updLinkPtr()->recoverFromSnapshot(&snapshot->children_snapshots[i]);
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
        GameObjectLink* child_ptr = &children[chi_i];
        if(!child_ptr->isEmpty()) //if it points to something
        result += 1;
    }
    return result;
}

void GameObject::pick(){
    this->isPicked = true;
    unsigned int children_am = static_cast<unsigned int>(this->children.size());
    for(unsigned int chil_i = 0; chil_i < children_am; chil_i++){
        children[chil_i].ptr->pick(); //child and his children are picked now
    }
}

void GameObject::setActive(bool active){
    this->active = active;
}

void GameObject::setLabel(std::string label){
    this->getLabelProperty()->label = QString::fromStdString(label);
    this->getLabelProperty()->onValueChanged();
}

std::string GameObject::getLabel(){
    return this->label->toStdString();
}

void GameObject::copyTo(GameObject* dest){
    dest->array_index = this->array_index;
    dest->alive = this->alive;
    dest->active = this->active;
    dest->hasParent = this->hasParent;
    dest->parent = this->parent;
    dest->str_id = this->str_id;
}

bool GameObject::hasMesh(){
    MeshProperty* mesh_prop = static_cast<MeshProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
    if(mesh_prop != nullptr){
        if(!mesh_prop->active) return false;
        if(mesh_prop->mesh_ptr != nullptr) return true;
    }
    return false;
}

bool GameObject::hasTerrain(){
    TerrainProperty* terrain = getPropertyPtr<TerrainProperty>();
    if(terrain != nullptr){
        if(!terrain->active) return false;
        return true;
    }
    return false;
}

void GameObject::DrawMesh(RenderPipeline* pipeline){
    MeshProperty* mesh_prop = static_cast<MeshProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
    TerrainProperty* terrain_prop = getPropertyPtr<TerrainProperty>();
    //Draw default mesh
    if(mesh_prop != nullptr) mesh_prop->mesh_ptr->Draw();
    if(terrain_prop != nullptr) terrain_prop->DrawMesh(pipeline);
}
