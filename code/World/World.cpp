#include "headers/World.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include "headers/Misc.h"
#include <QLineEdit>
#include <iostream>

#include <world/ObjectsComponents/MaterialComponent.hpp>
#include <world/ObjectsComponents/NodeComponent.hpp>
#include <world/ObjectsComponents/MeshComponent.hpp>
#include <world/ObjectsComponents/ZPScriptComponent.hpp>

extern RenderPipelineEditor* renderer;
extern Material* default3dmat;
extern Project* project_ptr;
extern ZSGAME_DATA* game_data;

World::World(){
    obj_widget_ptr = nullptr;
    GO_W_I::reserve(MAX_OBJS);
}

Engine::GameObject* World::updateLink(Engine::GameObjectLink* link){
    link->ptr = getGameObjectByStrId( link->obj_str_id);
    return static_cast<Engine::GameObject*>(link->ptr);
}

Engine::GameObject* World::dublicateObject(Engine::GameObject* original, bool parent){
    Engine::GameObject* new_obj = Engine::World::dublicateObject(original, parent);
    Engine::LabelProperty* label_prop = new_obj->getPropertyPtr<Engine::LabelProperty>(); //Obtain pointer to label property

    for (size_t child_i = 0; child_i < new_obj->mChildren.size(); child_i++) {
        Engine::GameObject* new_child = new_obj->mChildren[child_i].updLinkPtr();
        //UI parenting
        GO_W_I::getItem(new_obj)->addChild(GO_W_I::getItem(new_child));
    }
    GO_W_I::updateGameObjectItem(new_obj);

    return new_obj;
}

Engine::GameObject* World::Instantiate(Engine::GameObject* original){
    Engine::GameObjectLink link = original->getLinkToThisObject();
    Engine::GameObject* result = this->dublicateObject(link.ptr);

    if(result->hasParent){ //if object parented
        GO_W_I::getItem(result->mParent.ptr->array_index)->addChild(GO_W_I::getItem(result->array_index));
    }else{
        obj_widget_ptr->addTopLevelItem(GO_W_I::getItem(result->array_index));
    }
    return result;
}

void World::unpickObject(){
    picked_objs_ids.clear();
}

bool World::isPicked(Engine::GameObject* obj) {
    std::vector<int>::iterator iter;
    for (iter = picked_objs_ids.begin(); iter < picked_objs_ids.end(); iter++)
        if (*iter == obj->array_index)
            return true;
    return false;
}
void World::writeGameObject(Engine::GameObject* object_ptr, ZsStream* world_stream){
    if(object_ptr->mAlive == true){
        *world_stream << "\nG_OBJECT " << object_ptr->str_id << '\0';
        //Write main flags
        world_stream->writeBinaryValue(&object_ptr->mActive);
        world_stream->writeBinaryValue(&object_ptr->IsStatic);
        //If object has at least one child object
        if(object_ptr->mChildren.size() > 0){
            int children_num = object_ptr->getAliveChildrenAmount();
            //Write children header
            *world_stream << "\nG_CHI ";
            //Write amount of children i object
            world_stream->writeBinaryValue(&children_num);
            //get children amount
            size_t children_am = object_ptr->mChildren.size();
            for(size_t chi_i = 0; chi_i < children_am; chi_i ++){ //iterate over all children
                Engine::GameObjectLink* link_ptr = &object_ptr->mChildren[chi_i]; //Gettin pointer to child
                if(!link_ptr->isEmpty()){ //If this link isn't broken (after child removal)
                    world_stream->writeString(link_ptr->obj_str_id); //Writing child's string id
                }
            }
        }
        object_ptr->saveProperties(world_stream); //save object's properties
        *world_stream << "\nG_END"; //End writing object
    }
}

void World::saveToFile(std::string file){
    Engine::RenderSettings* settings_ptr = renderer->getRenderSettings();

    ZsStream world_stream;
    world_stream.open(file, std::ofstream::binary);
    int version = 1;
    int obj_num = static_cast<int>(this->objects.size());
    world_stream << "ZSP_SCENE ";
    world_stream.writeBinaryValue(&version);//Writing version
    world_stream.writeBinaryValue(&obj_num); //Writing objects amount
    //Writing render settings
    world_stream << "\nRENDER_SETTINGS_AMB_COLOR\n";
    world_stream.writeBinaryValue(&settings_ptr->ambient_light_color.r); //Writing R component of amb color
    world_stream.writeBinaryValue(&settings_ptr->ambient_light_color.g); //Writing G component of amb color
    world_stream.writeBinaryValue(&settings_ptr->ambient_light_color.b); //Writing B component of amb color
    world_stream << "\n_END\n";

    //Iterate over all objects and write them
    for(unsigned int obj_i = 0; obj_i < static_cast<unsigned int>(obj_num); obj_i ++){ //Iterate over all game objects
        Engine::GameObject* object_ptr = (this->objects[obj_i]);
        //Write GameObject
        writeGameObject(object_ptr, &world_stream);
    }
    //Close stream
    world_stream.close();
}

void World::loadFromMemory(const char* bytes, unsigned int size, QTreeWidget* w_ptr) {
    Engine::RenderSettings* settings_ptr = renderer->getRenderSettings();
    this->obj_widget_ptr = w_ptr;
    //Perform loading by engine function
    Engine::World::loadFromMemory(bytes, size, settings_ptr);
    //make parentings
    GO_W_I::recreateAll(MAX_OBJS);
    GO_W_I::makeParentings(this, this->obj_widget_ptr);
}

void World::openFromFile(std::string file, QTreeWidget* w_ptr){
    Engine::RenderSettings* settings_ptr = renderer->getRenderSettings();

    std::ifstream world_stream;
    world_stream.open(file, std::ifstream::binary | std::ifstream::ate); //Opening to read binary data
    //Check, if file is open
    if (!world_stream.is_open()) {
        game_data->out_manager->spawnRuntimeError(RuntimeErrorType::RE_TYPE_SCENE_OPEN_ERROR);
    }

    unsigned int size = static_cast<unsigned int>(world_stream.tellg());
    //Check for file size
    if (size == 0) {
        game_data->out_manager->spawnRuntimeError(RuntimeErrorType::RE_TYPE_SCENE_OPEN_ERROR);
    }

    world_stream.seekg(0);
    char* data = new char[size];
    world_stream.read(data, size);
    //Process World
    loadFromMemory(data, size, w_ptr);
    //Free allocated data
    delete[] data;
}

void World::writeObjectToPrefab(Engine::GameObject* object_ptr, ZsStream* stream){
    //Write an object
    writeGameObject(object_ptr, stream);
    //get children amount
    size_t children_am = object_ptr->mChildren.size();
    //iterate over all children and write them
    for(unsigned int ch_i = 0; ch_i < children_am; ch_i ++){
        Engine::GameObjectLink link = object_ptr->mChildren[ch_i];
        writeObjectToPrefab(link.updLinkPtr(), stream);
    }
}

void World::addObjectsFromPrefab(char* data, unsigned int size) {
    Engine::GameObject* object = Engine::World::addObjectsFromPrefab(data, size);
    GO_W_I::updateGameObjectItem(object);
}

void World::addObjectsFromPrefab(std::string file){
    std::ifstream prefab_stream;
    //opening prefab stream
    prefab_stream.open(file, std::ifstream::binary | std::ifstream::ate);

    if (!prefab_stream.is_open()) {
        std::cout << "Error opening PREFAB file " << file;
        return;
    }

    unsigned int size = prefab_stream.tellg();
    char* data = new char[size];
    prefab_stream.seekg(0);
    prefab_stream.read(data, size);
    //Load prefab from bytes
    addObjectsFromPrefab(data, size);
    //free readed bytes
    delete[] data;
}

void World::addMeshGroup(std::string file_path){
    ZS3M::SceneNode* node = nullptr;
    QString fpath_qt = QString::fromStdString(file_path);
    fpath_qt = fpath_qt.toLower();

    if(fpath_qt.endsWith(".fbx") || fpath_qt.endsWith(".dae")){
        node = new ZS3M::SceneNode;
        Engine::loadNodeTree(file_path, node);
    }else if(fpath_qt.endsWith(".zs3m")){
        std::string absolute_path = file_path;

        std::cout << absolute_path << std::endl;

        ZS3M::ImportedSceneFile isf;
        isf.loadFromFile(absolute_path);

        node = isf.rootNode;
    }else {
        return;
    }

    Engine::GameObject* rootobj = addMeshNode(node);
    this->obj_widget_ptr->addTopLevelItem(GO_W_I::getItem(rootobj->array_index));
    //Add animation property to root object for correct skinning support
    rootobj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_ANIMATION);
    //Set rootNode to meshProperty on all children objects
    rootobj->setMeshSkinningRootNodeRecursively(rootobj);
}

Engine::GameObject* World::addMeshNode(ZS3M::SceneNode* node){
    Engine::GameObject obj; //Creating base gameobject
    int add_num = 0; //Declaration of addititonal integer
    getAvailableNumObjLabel(node->node_label, &add_num);
    //Setting base variables
    obj.mWorld = this;
    obj.addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL);
    //Add common base property Transform
    obj.addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM);
    //Add node property to support skinning
    obj.addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_NODE);
    //Set label of new object
    obj.setLabel(node->node_label + std::to_string(add_num)); //Assigning label to object

    Engine::TransformProperty* transform_prop = obj.getTransformProperty();
    transform_prop->setScale(node->node_scaling);
    transform_prop->setTranslation(node->node_translation);
    transform_prop->updateMatrix();

    Engine::NodeProperty* node_prop = static_cast<Engine::NodeProperty*>(obj.getPropertyPtrByType(PROPERTY_TYPE::GO_PROPERTY_TYPE_NODE));
    node_prop->node_label = node->node_label;
    //Set original node transform
    node_prop->transform_mat = node->node_transform;

    //Add node to world
    Engine::GameObject* node_object = this->addObject(obj);
    //Iterate over all children nodes
    for(unsigned int node_i = 0; node_i < node->children.size(); node_i ++){
        ZS3M::SceneNode* ptr = node->children[node_i];
        //Create new node object
        Engine::GameObject* newobj = addMeshNode(ptr);
        node_object->addChildObject(newobj->getLinkToThisObject());
        GO_W_I::getItem(node_object->array_index)->addChild(GO_W_I::getItem(newobj->array_index));
    }

    //Iterate over all meshes, that inside of node
    for(unsigned int mesh_i = 0; mesh_i < node->mesh_names.size(); mesh_i ++){
        std::string mesh_label = node->mesh_names[mesh_i];
        Engine::MeshResource* mesh_ptr = game_data->resources->getMeshByLabel(mesh_label);

        Engine::GameObject* mesh_obj = nullptr;
        //if found mesh with name as node, create mesh inside the node
        if(mesh_label.compare(node->node_label) == false){
            mesh_obj = node_object;
        }else {
            //We should create new mesh
            mesh_obj = new Engine::GameObject;

            int add_num = 0; //Declaration of addititonal integer
            getAvailableNumObjLabel(mesh_label, &add_num);

            mesh_obj->mWorld = this;
            mesh_obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL);
            mesh_obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM);
            mesh_obj->setLabel(mesh_label + std::to_string(add_num)); //Assigning label to object

            //Add to world object and parent it
            mesh_obj = this->addObject(*mesh_obj);
            node_object->addChildObject(mesh_obj->getLinkToThisObject(), false);
            GO_W_I::getItem(node_object)->addChild(GO_W_I::getItem(mesh_obj));
        }

        mesh_obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH); //add mesh property
        mesh_obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL); //add material property

        //configure mesh
        Engine::MeshProperty* mesh_prop_ptr = static_cast<Engine::MeshProperty*>(mesh_obj->getPropertyPtrByType(PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH));
        mesh_prop_ptr->resource_relpath = mesh_label;

        mesh_prop_ptr->updateMeshPtr();
        //configure material
        Engine::MaterialProperty* mat_prop_ptr = static_cast<Engine::MaterialProperty*>(mesh_obj->getPropertyPtrByType(PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL));
        mat_prop_ptr->setMaterial("@default");

    }

    return node_object; //Return pointer to new object
}

void World::storeObjectToPrefab(Engine::GameObject* object_ptr, QString file){
    //open stream
    ZsStream prefab_stream;
    prefab_stream.open(file.toStdString(), std::ofstream::binary);
    //Writing prefab header
    prefab_stream << "ZSPIRE_PREFAB\n";
    //Call first object writing
    writeObjectToPrefab(object_ptr, &prefab_stream);
}


void World::recoverFromSnapshot(Engine::WorldSnapshot* snapshot){
    this->clear(); //clear world container first
    //iterate over all objects in snapshot
    for(unsigned int objs_num = 0; objs_num < snapshot->objects.size(); objs_num ++){
        Engine::GameObject* obj_ptr = &snapshot->objects[objs_num];
        //Create new object
        Engine::GameObject newobj;
        obj_ptr->copyTo(&newobj); //Copy object's settings
        this->addObject(newobj); //add object and store pointer to it's new place
    }
    //iterate over all properties in object in snapshot
    for(unsigned int prop_i = 0; prop_i < snapshot->props.size(); prop_i ++){
        auto prop_ptr = snapshot->props[prop_i];
        Engine::GameObjectLink link = prop_ptr->go_link; //Define a link to created object
        link.world_ptr = this; //Set an new world pointer
        Engine::GameObject* obj_ptr = updateLink(&link); //Calculate pointer to new object
        obj_ptr->addProperty(prop_ptr->type); //Add new property to created object
        auto new_prop = obj_ptr->getPropertyPtrByType(prop_ptr->type);
        prop_ptr->copyTo(new_prop);
    }
    //Copy all scripts
    for (unsigned int script_i = 0; script_i < snapshot->scripts.size(); script_i++) {
        Engine::ZPScriptComponent* script_ptr = static_cast<Engine::ZPScriptComponent*>
            (snapshot->scripts[script_i]);
        Engine::GameObjectLink link = script_ptr->go_link; //Define a link to created object
        link.world_ptr = this; //Set an new world pointer
        Engine::GameObject* obj_ptr = updateLink(&link); //Calculate pointer to new object
        obj_ptr->addScript(); //Add new property to created object
        Engine::ZPScriptComponent* new_script = obj_ptr->mScripts[obj_ptr->scripts_num - 1]; ///PIZDEC SUKA NAXUI TUT OSHIBKA BLYAT !!!!
        script_ptr->copyTo(new_script);
    }
    //iterate over all objects
    for(unsigned int objs_num = 0; objs_num < snapshot->objects.size(); objs_num ++){
        Engine::GameObject* obj_ptr = objects[objs_num];
        if(!obj_ptr->hasParent)  //if object is unparented
            continue;
        
        Engine::GameObject* parent_p = updateLink(&obj_ptr->mParent);
        parent_p->mChildren.push_back(obj_ptr->getLinkToThisObject());
    }
    GO_W_I::recreateAll(MAX_OBJS);
    //make parentings
    GO_W_I::makeParentings(this, this->obj_widget_ptr);
}