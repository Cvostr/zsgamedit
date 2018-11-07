#include "headers/World.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include "headers/Misc.h"
#include <QLineEdit>



GameObjectLink::GameObjectLink(){
    ptr = nullptr;
    world_ptr = nullptr;
}

GameObject* GameObjectLink::updLinkPtr(){
    if(world_ptr == nullptr) //If world not defined, exiting
        return nullptr;

    return world_ptr->getObjectByStringId(this->obj_str_id);
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
    item_ptr = new QTreeWidgetItem; //Allocate tree widget item
    genRandomString(&this->str_id, 15); //Generate random string ID
}

bool GameObject::addProperty(int property){
    unsigned int props = static_cast<unsigned int>(this->properties.size());
    for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
        GameObjectProperty* property_ptr = this->properties[prop_i];
        if(property_ptr->type == property){ //If object already has one
            return false; //Exit function
        }
    }
    GameObjectProperty* _ptr = nullptr;
    switch (property) {
        case GO_PROPERTY_TYPE_TRANSFORM:{ //If type is transfrom
            _ptr = static_cast<GameObjectProperty*>(new TransformProperty); //Allocation of transform in heap
            break;
        }
        case GO_PROPERTY_TYPE_LABEL:{
            LabelProperty* ptr = new LabelProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            ptr->list_item_ptr = this->item_ptr;
            break;
        }
    case GO_PROPERTY_TYPE_MESH:{
        MeshProperty* ptr = new MeshProperty;
        _ptr = static_cast<GameObjectProperty*>(ptr);
        break;
    }
    }
    _ptr->object_str_id = this->str_id; //Connect to gameobject via string id
    _ptr->world_ptr = this->world_ptr; //Assign pointer to world
    this->properties.push_back(_ptr); //Store poroperty in gameobject
    return true;
}

bool GameObject::addTransformProperty(){
    return addProperty(GO_PROPERTY_TYPE_TRANSFORM);
}

bool GameObject::addLabelProperty(){
    return addProperty(GO_PROPERTY_TYPE_LABEL);
}

GameObjectProperty* GameObject::getPropertyPtrByType(int property){
    unsigned int props = static_cast<unsigned int>(this->properties.size());
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
    link.world_ptr = this->world_ptr; //Placing world pointer

    link.ptr = world_ptr->getObjectByStringId(link.obj_str_id);
    //link.updLinkPtr();
    return link;
}

void GameObject::saveProperties(std::ofstream* stream){
    unsigned int props_num = static_cast<unsigned int>(this->properties.size());

    for(unsigned int prop_i = 0; prop_i < props_num; prop_i ++){
        GameObjectProperty* property_ptr = static_cast<GameObjectProperty*>(this->properties[prop_i]);
        *stream << "\nG_PROPERTY " << property_ptr->type << " "; //Writing property header

        switch(property_ptr->type){
        case GO_PROPERTY_TYPE_TRANSFORM:{
            TransformProperty* ptr = static_cast<TransformProperty*>(property_ptr);
            float posX = ptr->translation.X;
            float posY = ptr->translation.Y;
            float posZ = ptr->translation.Z;

            float scaleX = ptr->scale.X;
            float scaleY = ptr->scale.Y;
            float scaleZ = ptr->scale.Z;

            float rotX = ptr->rotation.X;
            float rotY = ptr->rotation.Y;
            float rotZ = ptr->rotation.Z;

            stream->write(reinterpret_cast<char*>(&posX), sizeof(float));//Writing position X
            stream->write(reinterpret_cast<char*>(&posY), sizeof(float)); //Writing position Y
            stream->write(reinterpret_cast<char*>(&posZ), sizeof(float)); //Writing position Z

            stream->write(reinterpret_cast<char*>(&scaleX), sizeof(float));//Writing scale X
            stream->write(reinterpret_cast<char*>(&scaleY), sizeof(float)); //Writing scale Y
            stream->write(reinterpret_cast<char*>(&scaleZ), sizeof(float)); //Writing scale Z

            stream->write(reinterpret_cast<char*>(&rotX), sizeof(float));//Writing rotation X
            stream->write(reinterpret_cast<char*>(&rotY), sizeof(float)); //Writing rotation Y
            stream->write(reinterpret_cast<char*>(&rotZ), sizeof(float)); //Writing rotation Z
            break;
        }
        case GO_PROPERTY_TYPE_LABEL:{
            LabelProperty* ptr = static_cast<LabelProperty*>(property_ptr);
            *stream << ptr->label.toStdString();
            break;
        }
        }
    }
}

LabelProperty* GameObject::getLabelProperty(){
    return static_cast<LabelProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_LABEL));
}
TransformProperty* GameObject::getTransformProperty(){
    return static_cast<TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
}

void GameObject::addChildObject(GameObjectLink link){
    link.updLinkPtr(); //Calculating object pointer
    link.ptr->hasParent = true; //Object now has a parent (if it has't before)
    link.ptr->parent = this->getLinkToThisObject(); //Assigning pointer to new parent
    this->children.push_back(link);
}
void GameObject::removeChildObject(GameObjectLink link){
    unsigned int children_am = static_cast<unsigned int>(children.size());
    for(unsigned int i = 0; i < children_am; i++){
        GameObject* ptr = children[i].updLinkPtr();
        if(link.obj_str_id.compare(ptr->str_id) == 0){
            children[i].crack(); //Make link broken
        }
    }
}

GameObject* World::addObject(GameObject obj){
    this->objects.push_back(obj);
    GameObject* ptr = &objects[objects.size() - 1];
    ptr->world_ptr = this;
    return ptr;
}

GameObject* World::newObject(){
    GameObject obj; //Creating base gameobject
    int add_num = 0; //Declaration of addititonal integer
    getAvailableNumObjLabel("GameObject_", &add_num);

    obj.addLabelProperty();
    obj.label = &obj.getLabelProperty()->label;
    *obj.label = "GameObject_" + QString::number(add_num); //Assigning label to object
    obj.item_ptr->setText(0, *obj.label);

    obj.addProperty(GO_PROPERTY_TYPE_TRANSFORM);
    return this->addObject(obj); //Return pointer to new object
}

GameObject* World::getObjectByLabel(QString label){
    unsigned int objs_num = static_cast<unsigned int>(this->objects.size());
    for(unsigned int obj_it = 0; obj_it < objs_num; obj_it ++){ //Iterate over all objs in scene
        GameObject* obj_ptr = &this->objects[obj_it]; //Get pointer to checking object
        if(obj_ptr->label->compare(label) == 0) //if labels are same
            return obj_ptr; //Return founded object
    }
    return nullptr; //if we haven't found one
}

GameObject* World::getObjectByStringId(std::string id){
    unsigned int objs_num = static_cast<unsigned int>(this->objects.size());
    for(unsigned int obj_it = 0; obj_it < objs_num; obj_it ++){ //Iterate over all objs in scene
        GameObject* obj_ptr = &this->objects[obj_it]; //Get pointer to checking object
        if(obj_ptr->str_id.compare(id) == 0) //if labels are same
            return obj_ptr; //Return founded object
    }
    return nullptr; //if we haven't found one
}

void World::getAvailableNumObjLabel(QString label, int* result){
     unsigned int objs_num = static_cast<unsigned int>(this->objects.size());
     QString tocheck_str = label + QString::number(*result); //Calculating compare string
     bool hasEqualName = false; //true if we already have this obj
     for(unsigned int obj_it = 0; obj_it < objs_num; obj_it ++){ //Iterate over all objs in scene
         GameObject* obj_ptr = &this->objects[obj_it]; //Get pointer to checking object
         if(obj_ptr->label->compare(tocheck_str) == 0) //If label on object is same
             hasEqualName = true; //Then we founded equal name
     }
     if(hasEqualName == true){
         *result += 1;
         getAvailableNumObjLabel(label, result);
     }
}

void World::saveToFile(QString file){
    std::string fpath = file.toStdString();

    std::ofstream world_stream;
    world_stream.open(fpath.c_str(), std::ofstream::binary);
    int version = 1;
    int obj_num = static_cast<int>(this->objects.size());
    world_stream << "ZSP_SCENE ";
    world_stream.write(reinterpret_cast<char*>(&version), sizeof(int));//Writing version
    world_stream.write(reinterpret_cast<char*>(&obj_num), sizeof(int)); //Writing objects amount

    for(unsigned int obj_i = 0; obj_i < static_cast<unsigned int>(obj_num); obj_i ++){ //Iterate over all game objects
        GameObject* object_ptr = static_cast<GameObject*>(&this->objects[obj_i]);
        world_stream << "\nG_OBJECT " << object_ptr->str_id; //Start object's header
        if(object_ptr->children.size() > 0){ //If object has at least one child object
            world_stream << "\nG_CHI " << object_ptr->getAliveChildrenAmount() << " "; //Start children header
            unsigned int children_am = static_cast<unsigned int>(object_ptr->children.size());
            for(unsigned int chi_i = 0; chi_i < children_am; chi_i ++){ //iterate over all children
                GameObjectLink* link_ptr = &object_ptr->children[chi_i]; //Gettin pointer to child
                if(!link_ptr->isEmpty()){ //If this link isn't broken (after child removal)
                    world_stream << link_ptr->obj_str_id << " "; //Writing child's string id
                }
                }
        }
        object_ptr->saveProperties(&world_stream); //save object's properties
        world_stream << "\nG_END"; //End writing object
    }

    world_stream.close();

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

void World::openFromFile(QString file, QTreeWidgetItem* root_item, QTreeWidget* w_ptr){
    clear(); //Clear all objects
    std::string fpath = file.toStdString();

    std::ifstream world_stream;
    world_stream.open(fpath.c_str(), std::ofstream::binary); //Opening to read binary data

    std::string test_header;
    world_stream >> test_header; //Read header
    if(test_header.compare("ZSP_SCENE") != 0) //If it isn't zspire scene
        return; //Go out, we have nothing to do
    world_stream.seekg(1, std::ofstream::cur);
    int version = 0;
    int objs_num = 0;
    world_stream.read(reinterpret_cast<char*>(&version), sizeof(int)); //reading version
    world_stream.read(reinterpret_cast<char*>(&objs_num), sizeof(int)); //reading objects count

    while(!world_stream.eof()){ //While file not finished reading
        std::string prefix;
        world_stream >> prefix; //Read prefix
        if(prefix.compare("G_OBJECT") == 0){ //if it is game object
            GameObject object; //firstly, define an object
            world_stream >> object.str_id;
            //Then do the same sh*t, iterate until "G_END" came up
            while(true){
                std::string _prefix;
                world_stream >> prefix; //Read prefix
                if(prefix.compare("G_END") == 0){ //If end reached
                    break; //Then end this infinity loop
                }
                if(prefix.compare("G_CHI") == 0) { //Ops, it is chidren header
                    unsigned int amount;
                    world_stream >> amount; //Reading children amount

                    for(unsigned int ch_i = 0; ch_i < amount; ch_i ++){ //Iterate over all written children to file
                        std::string child_str_id;
                        world_stream >> child_str_id; //Reading child string id

                        GameObjectLink link;
                        link.world_ptr = this; //Setting world pointer
                        link.obj_str_id = child_str_id; //Setting string ID
                        object.children.push_back(link); //Adding to object
                        //object.addChildObject(link);
                    }
                }
                if(prefix.compare("G_PROPERTY") == 0){ //We found an property, zaeb*s'
                    int type;
                    world_stream >> type;
                    object.addProperty(type);
                    GameObjectProperty* prop_ptr = object.getPropertyPtrByType(type); //get created property
                    //since more than 1 properties same type can't be on one gameobject
                    switch(type){
                        case GO_PROPERTY_TYPE_LABEL :{
                            std::string label;
                            world_stream >> label;
                            LabelProperty* lptr = static_cast<LabelProperty*>(prop_ptr);
                            object.label = &lptr->label;
                            lptr->label = QString::fromStdString(label); //Write loaded string
                            lptr->list_item_ptr->setText(0, lptr->label); //Set text on widget
                            break;
                        }
                        case GO_PROPERTY_TYPE_TRANSFORM :{
                            world_stream.seekg(1, std::ofstream::cur); //Skip space
                            TransformProperty* lptr = static_cast<TransformProperty*>(prop_ptr);
                            world_stream.read(reinterpret_cast<char*>(&lptr->translation.X), sizeof(float));
                            world_stream.read(reinterpret_cast<char*>(&lptr->translation.Y), sizeof(float));
                            world_stream.read(reinterpret_cast<char*>(&lptr->translation.Z), sizeof(float));

                            world_stream.read(reinterpret_cast<char*>(&lptr->scale.X), sizeof(float));
                            world_stream.read(reinterpret_cast<char*>(&lptr->scale.Y), sizeof(float));
                            world_stream.read(reinterpret_cast<char*>(&lptr->scale.Z), sizeof(float));

                            world_stream.read(reinterpret_cast<char*>(&lptr->rotation.X), sizeof(float));
                            world_stream.read(reinterpret_cast<char*>(&lptr->rotation.Y), sizeof(float));
                            world_stream.read(reinterpret_cast<char*>(&lptr->rotation.Z), sizeof(float));

                            lptr->updateMat(); //After everything is loaded, update matrices

                        break;
                    }
                    }
                }
            }
            //this->objects.push_back(object);
            this->addObject(object);
        }
    }
    //We finished reading file
    //Now iterate over all objects in world and set dependencies
    for(unsigned int obj_i = 0; obj_i < this->objects.size(); obj_i ++){
        GameObject* obj_ptr = &this->objects[obj_i];
        for(unsigned int chi_i = 0; chi_i < obj_ptr->children.size(); chi_i ++){ //Now iterate over all children
            GameObjectLink* child_ptr = &obj_ptr->children[chi_i];
            GameObject* child_go_ptr = child_ptr->updLinkPtr();
            child_go_ptr->parent = obj_ptr->getLinkToThisObject();
            child_go_ptr->hasParent = true;
        }
    }

    for(unsigned int obj_i = 0; obj_i < this->objects.size(); obj_i ++){
        GameObject* obj_ptr = &this->objects[obj_i];
        if(obj_ptr->parent.isEmpty()){ //If object has no parent
            //root_item->addChild(obj_ptr->item_ptr);
            w_ptr->addTopLevelItem(obj_ptr->item_ptr);
        }else{ //It has a parent
            GameObject* parent_ptr = obj_ptr->parent.ptr; //Get parent pointer
            parent_ptr->item_ptr->addChild(obj_ptr->item_ptr); //Connect Qt Tree Items
        }
    }
}

void World::clear(){
     objects.resize(0);
}

GameObject** World::getUnparentedObjs(){
    GameObject** result_arr = new GameObject*[5];
    int amount_of_uprted = 0;
    unsigned int objs_num = static_cast<unsigned int>(this->objects.size());
    for(unsigned int obj_it = 0; obj_it < objs_num; obj_it ++){ //Iterate over all objs in scene
        GameObject* obj_ptr = &this->objects[obj_it]; //Get pointer to checking object
        if(obj_ptr->hasParent == false){
            result_arr[amount_of_uprted] = obj_ptr;
            amount_of_uprted += 1;
        }
    }
    return result_arr; //if we haven't found one
}

ZSPIRE::Mesh* World::getMeshPtrByRelPath(QString label){
    Project* proj_ptr = static_cast<Project*>(this->proj_ptr); //Convert void pointer to Project*
    unsigned int resources_num = static_cast<unsigned int>(proj_ptr->resources.size()); //Receive resource amount in project

    for(unsigned int r_it = 0; r_it < resources_num; r_it ++){ //Iteerate over all resources in project
        Resource* r_ptr = &proj_ptr->resources[r_it]; //Obtain pointer to resource
        //If resource is mesh and has same name as in argument
        if(r_ptr->type == RESOURCE_TYPE_MESH && r_ptr->rel_path.compare(label) == 0){
            return static_cast<ZSPIRE::Mesh*>(r_ptr->class_ptr);
        }
    }
    return nullptr;
}
