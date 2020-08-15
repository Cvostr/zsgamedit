#include "headers/World.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include "headers/Misc.h"
#include <QLineEdit>
#include <world/go_properties.h>
#include <iostream>

extern RenderPipeline* renderer;
extern Material* default3dmat;
extern Project* project_ptr;
extern ZSGAME_DATA* game_data;

World::World(){
    obj_widget_ptr = nullptr;
}

GameObject* World::updateLink(Engine::GameObjectLink* link){
    link->ptr = getGameObjectByStrId( link->obj_str_id);
    return static_cast<GameObject*>(link->ptr);
}

int World::getFreeObjectSpaceIndex(){
    unsigned int index_to_push = static_cast<unsigned int>(objects.size()); //Set free index to objects amount
    unsigned int objects_num = static_cast<unsigned int>(this->objects.size());
    for(unsigned int objs_i = 0; objs_i < objects_num; objs_i ++){
        if(objects[objs_i]->alive == false){ //if object deleted
            index_to_push = objs_i; //set free index to index of deleted object
        }
    }

    if(index_to_push == objects.size()){ //if all indeces are busy
        return static_cast<int>(objects.size());
    }else{ //if vector has an empty space
        return static_cast<int>(index_to_push);
    }
}

GameObject* World::addObject(GameObject obj){
    //Allocate new Object
    GameObject* newobj = new GameObject;
    //Assign object's data to new object
    *newobj = obj;
    //Try to find, if there is a free space in array for new object
    int index_to_push = -1;
    unsigned int objects_num = static_cast<unsigned int>(this->objects.size());
    for(unsigned int objs_i = 0; objs_i < objects_num; objs_i ++){
        if(objects[objs_i]->alive == false){
            index_to_push = static_cast<int>(objs_i);
        }
    }

    GameObject* ptr = nullptr;
    if(index_to_push == -1){ //if all indeces are busy
        this->objects.push_back(newobj); //Push object to vector's end
        ptr = static_cast<GameObject*>(objects[objects.size() - 1]);
        ptr->array_index = static_cast<int>(objects.size() - 1);
    }else{ //if vector has an empty space
        objects[static_cast<unsigned int>(index_to_push)] = newobj;
        ptr = static_cast<GameObject*>(objects[static_cast<unsigned int>(index_to_push)]);
        ptr->array_index = index_to_push;
    }
    ptr->world_ptr = this;

    return ptr;
}

GameObject* World::dublicateObject(GameObject* original, bool parent){
    GameObject _new_obj;//Create an empty
    GameObject* new_obj = addObject(_new_obj);

    //Copying properties data
    for(unsigned int prop_i = 0; prop_i < original->props_num; prop_i ++){
        //Get pointer to original property
        auto prop_ptr = original->properties[prop_i];
        //register new property in new object
        new_obj->addProperty(prop_ptr->type);
        //Get created property
        auto new_prop = new_obj->getPropertyPtrByType(prop_ptr->type);
        //start property copying
        prop_ptr->copyTo(new_prop);
    }

    if(original->hasParent){ //if original has parent
        Engine::TransformProperty* transform = new_obj->getPropertyPtr<Engine::TransformProperty>();
        ZSVECTOR3 p_translation = ZSVECTOR3(0,0,0);
        ZSVECTOR3 p_scale = ZSVECTOR3(1,1,1);
        ZSVECTOR3 p_rotation = ZSVECTOR3(0,0,0);
        original->parent.ptr->getTransformProperty()->getAbsoluteParentTransform(p_translation, p_scale, p_rotation);
        transform->translation = transform->translation + p_translation;
        transform->scale = transform->scale * p_scale;
        transform->rotation = transform->rotation + p_rotation;
        if(parent == true)
            original->parent.ptr->addChildObject(new_obj->getLinkToThisObject());
    }
    //Set new name for object
    LabelProperty* label_prop = new_obj->getPropertyPtr<LabelProperty>(); //Obtain pointer to label property
    std::string to_paste;
    genRandomString(&to_paste, 3);
    label_prop->label = label_prop->label + "_" + to_paste;
    label_prop->list_item_ptr = new_obj->item_ptr; //Setting to label new qt item
    new_obj->label_ptr = &label_prop->label;
    new_obj->item_ptr->setText(0, QString::fromStdString(label_prop->label));
    //Dublicate chilldren object
    unsigned int children_amount = static_cast<unsigned int>(original->children.size());
    //Iterate over all children
    for(unsigned int child_i = 0; child_i < children_amount; child_i ++){
        //Get pointer to original child object
        Engine::GameObjectLink link = original->children[child_i];
        //create new child obect by dublication of original
        GameObject* new_child = dublicateObject((GameObject*)link.ptr, false);
        //parenting
        new_obj->addChildObject(new_child->getLinkToThisObject());
        //UI parenting
        new_obj->item_ptr->addChild(new_child->item_ptr);
    }

    return new_obj;
}

GameObject* World::Instantiate(GameObject* original){
    Engine::GameObjectLink link = original->getLinkToThisObject();
    GameObject* result = this->dublicateObject((GameObject*)link.ptr);

    if(result->hasParent){ //if object parented
        ((GameObject*)result->parent.ptr)->item_ptr->addChild(result->item_ptr);
    }else{
        obj_widget_ptr->addTopLevelItem(result->item_ptr);
    }
    return result;
}

GameObject* World::newObject(){
    GameObject obj; //Creating base gameobject
    int add_num = 0; //Declaration of addititonal integer
    getAvailableNumObjLabel("GameObject_", &add_num);

    obj.world_ptr = this;
    obj.addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL);
    obj.label_ptr = &obj.getLabelProperty()->label;
    *obj.label_ptr = "GameObject_" + std::to_string(add_num); //Assigning label to object
    obj.item_ptr->setText(0, QString::fromStdString(*obj.label_ptr));

    obj.addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM);
    return this->addObject(obj); //Return pointer to new object
}

void World::getAvailableNumObjLabel(std::string label, int* result){
     unsigned int objs_num = static_cast<unsigned int>(this->objects.size());
     std::string tocheck_str = label + std::to_string(*result); //Calculating compare string
     bool hasEqualName = false; //true if we already have this obj
     for(unsigned int obj_it = 0; obj_it < objs_num; obj_it ++){ //Iterate over all objs in scene
         Engine::GameObject* obj_ptr = this->objects[obj_it]; //Get pointer to checking object
         if(obj_ptr->label_ptr == nullptr || !obj_ptr->alive) continue;
         if(obj_ptr->label_ptr->compare(tocheck_str) == 0) //If label on object is same
             hasEqualName = true; //Then we founded equal name
     }
     if(hasEqualName == true){
         *result += 1;
         getAvailableNumObjLabel(label, result);
     }
}

bool World::isObjectLabelUnique(std::string label){
    unsigned int objs_num = static_cast<unsigned int>(this->objects.size());
    int ret_amount = 0;
    for(unsigned int obj_it = 0; obj_it < objs_num; obj_it ++){ //Iterate over all objs in scene
        Engine::GameObject* obj_ptr = this->objects[obj_it]; //Get pointer to checking object
        //if object was destroyed
        if(!obj_ptr->alive) continue;
        if(obj_ptr->label_ptr->compare(label) == 0){
            ret_amount += 1;
            if(ret_amount > 1) return false;
        }
    }
    return true;
}

void World::unpickObject(){
    picked_objs_ids.clear();
}

bool World::isPicked(GameObject* obj) {
    std::vector<int>::iterator iter;
    for (iter = picked_objs_ids.begin(); iter < picked_objs_ids.end(); iter++)
        if (*iter == obj->array_index)
            return true;
    return false;
}
void World::writeGameObject(GameObject* object_ptr, std::ofstream* world_stream){
    if(object_ptr->alive == true){
        *world_stream << "\nG_OBJECT " << object_ptr->str_id << " ";
        //Write main flags
        world_stream->write(reinterpret_cast<char*>(&object_ptr->active), sizeof(bool));
        world_stream->write(reinterpret_cast<char*>(&object_ptr->IsStatic), sizeof(bool));
        //If object has at least one child object
        if(object_ptr->children.size() > 0){ 
            int children_num = object_ptr->getAliveChildrenAmount();
            //Write children header
            *world_stream << "\nG_CHI ";
            //Write amount of children i object
            world_stream->write(reinterpret_cast<char*>(&children_num), sizeof(int));
            //get children amount
            unsigned int children_am = static_cast<unsigned int>(object_ptr->children.size());
            for(unsigned int chi_i = 0; chi_i < children_am; chi_i ++){ //iterate over all children
                Engine::GameObjectLink* link_ptr = &object_ptr->children[chi_i]; //Gettin pointer to child
                if(!link_ptr->isEmpty()){ //If this link isn't broken (after child removal)
                    *world_stream << link_ptr->obj_str_id << " "; //Writing child's string id
                }
            }
        }
        object_ptr->saveProperties(world_stream); //save object's properties
        *world_stream << "\nG_END"; //End writing object
    }
}

void World::saveToFile(std::string file){
    Engine::RenderSettings* settings_ptr = renderer->getRenderSettings();

    std::ofstream world_stream;
    world_stream.open(file, std::ofstream::binary);
    int version = 1;
    int obj_num = static_cast<int>(this->objects.size());
    world_stream << "ZSP_SCENE ";
    world_stream.write(reinterpret_cast<char*>(&version), sizeof(int));//Writing version
    world_stream.write(reinterpret_cast<char*>(&obj_num), sizeof(int)); //Writing objects amount
    //Writing render settings
    world_stream << "\nRENDER_SETTINGS_AMB_COLOR\n";
    world_stream.write(reinterpret_cast<char*>(&settings_ptr->ambient_light_color.r), sizeof(int)); //Writing R component of amb color
    world_stream.write(reinterpret_cast<char*>(&settings_ptr->ambient_light_color.g), sizeof(int)); //Writing G component of amb color
    world_stream.write(reinterpret_cast<char*>(&settings_ptr->ambient_light_color.b), sizeof(int)); //Writing B component of amb color
    world_stream << "\n_END\n";

    //Iterate over all objects and write them
    for(unsigned int obj_i = 0; obj_i < static_cast<unsigned int>(obj_num); obj_i ++){ //Iterate over all game objects
        GameObject* object_ptr = static_cast<GameObject*>(this->objects[obj_i]);
        //Write GameObject
        writeGameObject(object_ptr, &world_stream);
    }
    //Close stream
    world_stream.close();
}

void World::loadGameObjectFromMemory(GameObject* object_ptr, const char* bytes, unsigned int left_bytes) {
    std::string prefix;
    unsigned int iter = 1;
    object_ptr->world_ptr = this; //Assign pointer to world
    object_ptr->str_id.clear(); //Clear old string id
    while (bytes[iter] != ' ') {
        object_ptr->str_id += bytes[iter];
        iter++;
    }
    iter++;
    //Read ACTIVE and STATIC flags
    memcpy(&object_ptr->active, bytes + iter, sizeof(bool));
    iter += sizeof(bool);
    memcpy(&object_ptr->IsStatic, bytes + iter, sizeof(bool));
    iter += sizeof(bool);
    //Then do the same sh*t, iterate until "G_END" came up
    while (iter < left_bytes) {
        prefix.clear();
        //Read prefix
        while (bytes[iter] == ' ' || bytes[iter] == '\n') {
            iter++;
        }
        while (bytes[iter] != ' ' && bytes[iter] != '\n' && iter < left_bytes) {
            if (bytes[iter] != '\0')
                prefix += bytes[iter];
            iter++;
        }

        if (prefix.compare("G_END") == 0) { //If end reached
            break; //Then end this infinity loop
        }
        if (prefix.compare("G_CHI") == 0) { //Ops, it is chidren header
            unsigned int amount = 0;
            iter++;
            //Read amount of children of object
            memcpy(&amount, bytes + iter, sizeof(int));
            iter += sizeof(int);
            //Iterate over these children
            for (unsigned int ch_i = 0; ch_i < amount; ch_i++) { //Iterate over all written children to file
                std::string child_str_id;
                //Reading child string id
                while (bytes[iter] == ' ' || bytes[iter] == '\n') {
                    iter++;
                }
                while (bytes[iter] != ' ' && bytes[iter] != '\n') {
                    child_str_id += bytes[iter];
                    iter++;
                }
                //Create link for object
                Engine::GameObjectLink link;
                link.world_ptr = this; //Setting world pointer
                link.obj_str_id = child_str_id; //Setting string ID
                object_ptr->children.push_back(link); //Adding to object
            }
        }
        if (prefix.compare("G_PROPERTY") == 0) { //We found an property, zaeb*s'
            //Call function to load that property
            PROPERTY_TYPE type;
            iter++; //Skip space
            memcpy(&type, bytes + iter, sizeof(int));
            iter += sizeof(int);
            //Spawn new property with readed type
            object_ptr->addProperty(type);
            auto prop_ptr = object_ptr->getPropertyPtrByType(type); //get created property
            //Read ACTIVE flag
            memcpy(&prop_ptr->active, bytes + iter, sizeof(bool));
            iter += sizeof(bool);
            //Load property
            prop_ptr->loadPropertyFromMemory(bytes + iter, object_ptr);
        }
    }
}

void World::loadFromMemory(const char* bytes, unsigned int size, QTreeWidget* w_ptr) {
    Engine::RenderSettings* settings_ptr = renderer->getRenderSettings();
    this->obj_widget_ptr = w_ptr;

    unsigned int iter = 0;
    std::string test_header;
    //Read header
    while (bytes[iter] != ' ') {
        test_header += bytes[iter];
        iter++;
    }

    if (test_header.compare("ZSP_SCENE") != 0) //If it isn't zspire scene
        return; //Go out, we have nothing to do
    iter++;
    int version = 0; //define version on world file
    int objs_num = 0; //define number of objects
    memcpy(&version, bytes + iter, sizeof(int));
    iter += 4;
    memcpy(&objs_num, bytes + iter, sizeof(int));
    iter += 5;

    while (iter < size) { //until file is over
        std::string prefix;
        //read prefix
        while (bytes[iter] == ' ' || bytes[iter] == '\n') {
            iter++;
        }
        while (bytes[iter] != ' ' && bytes[iter] != '\n') {
            prefix += bytes[iter];
            iter++;
        }

        if (prefix.compare("RENDER_SETTINGS_AMB_COLOR") == 0) { //if it is render setting of ambient light color
            iter++;
            memcpy(&settings_ptr->ambient_light_color.r, bytes + iter, sizeof(int));
            iter += 4;
            memcpy(&settings_ptr->ambient_light_color.g, bytes + iter, sizeof(int));
            iter += 4;
            memcpy(&settings_ptr->ambient_light_color.b, bytes + iter, sizeof(int));
            iter += 4;
        }

        if (prefix.compare("G_OBJECT") == 0) { //if it is game object
            GameObject obj;
            //Call function to load object
            loadGameObjectFromMemory(&obj, bytes + iter, size - iter);
            //Add object to scene
            this->addObject(obj);
        }
    }
    //Now iterate over all objects and set depencies
    for (unsigned int obj_i = 0; obj_i < this->objects.size(); obj_i++) {
        Engine::GameObject* obj_ptr = this->objects[obj_i];
        for (unsigned int chi_i = 0; chi_i < obj_ptr->children.size(); chi_i++) { //Now iterate over all children
            Engine::GameObjectLink* child_ptr = &obj_ptr->children[chi_i];
            Engine::GameObject* child_go_ptr = child_ptr->updLinkPtr();
            child_go_ptr->parent = obj_ptr->getLinkToThisObject();
            child_go_ptr->hasParent = true;
        }
    }
    //Now add all objects to inspector tree
    for (unsigned int obj_i = 0; obj_i < this->objects.size(); obj_i++) {
        GameObject* obj_ptr = (GameObject*)this->objects[obj_i];
        if (obj_ptr->parent.isEmpty()) { //If object has no parent
            w_ptr->addTopLevelItem(obj_ptr->item_ptr);
        }
        else { //It has a parent
            GameObject* parent_ptr = (GameObject*)obj_ptr->parent.ptr; //Get parent pointer
            parent_ptr->item_ptr->addChild(obj_ptr->item_ptr); //Connect Qt Tree Items
        }
    }
}

void World::openFromFile(std::string file, QTreeWidget* w_ptr){
    Engine::RenderSettings* settings_ptr = renderer->getRenderSettings();
    this->obj_widget_ptr = w_ptr;

    //HACK to fix bug with crash when removing qt widget
    setLabelPropertyDeleteWidget(false);
    //Clear all objects
    clear(); 

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
    

    //Disable HACK
    setLabelPropertyDeleteWidget(true);
}

void World::processPrefabObject(GameObject* object_ptr, std::vector<GameObject>* objects_array){
    unsigned int props_amount = object_ptr->props_num;
    //iterate over all props and update gameobject links
    for(unsigned int prop_i = 0; prop_i < props_amount; prop_i ++){
        Engine::GameObjectProperty* prop_ptr = object_ptr->properties[prop_i];
        prop_ptr->go_link.obj_str_id = object_ptr->str_id; //set new string id

        prop_ptr->go_link.updLinkPtr();
    }
    //Obtain amount of objects
    unsigned int children_amount = static_cast<unsigned int>(object_ptr->children.size());
    //Iterate over all objects
    for(unsigned int chi_i = 0; chi_i < children_amount; chi_i ++){
        Engine::GameObjectLink link = object_ptr->children[chi_i];
        //find object with same string name as in link
        for(unsigned int obj_i = 0; obj_i < objects_array->size(); obj_i ++){
            GameObject* _object_ptr = &objects_array->at(obj_i);
            if(_object_ptr->str_id.compare(link.obj_str_id) == 0){ //we found object
                genRandomString(&_object_ptr->str_id, 15); //generate new string ID
                object_ptr->children[chi_i].obj_str_id = _object_ptr->str_id; //update string id in children array
                _object_ptr->parent.obj_str_id = object_ptr->str_id; //update string ID of parent link
                _object_ptr->hasParent = true;

                processPrefabObject(_object_ptr, objects_array);
            }
        }
    }
}

void World::writeObjectToPrefab(GameObject* object_ptr, std::ofstream* stream){
    //Write an object
    writeGameObject(object_ptr, stream);
    //get children amount
    unsigned int children_am = static_cast<unsigned int>(object_ptr->children.size());
    //iterate over all children and write them
    for(unsigned int ch_i = 0; ch_i < children_am; ch_i ++){
        Engine::GameObjectLink link = object_ptr->children[ch_i];
        writeObjectToPrefab((GameObject*)link.updLinkPtr(), stream);
    }
}

void World::addObjectsFromPrefab(char* data, unsigned int size) {
    unsigned int iter = 0;
    std::string test_header;
    //Read header
    while (data[iter] != ' ' && data[iter] != '\n') {
        test_header += data[iter];
        iter++;
    }

    if (test_header.compare("ZSPIRE_PREFAB") != 0) //If it isn't zspire scene
        return; //Go out, we have nothing to do
    iter++;
    //array for objects
    std::vector<GameObject> mObjects;

    while (iter < size) { //until file is over
        std::string prefix;
        //read prefix
        while (data[iter] == ' ' || data[iter] == '\n') {
            iter++;
        }
        while (data[iter] != ' ' && data[iter] != '\n') {
            prefix += data[iter];
            iter++;
        }
        //if we found an object
        if (!prefix.compare("G_OBJECT")) {
            GameObject obj;
            this->loadGameObjectFromMemory(&obj, data + iter, size - iter);
            mObjects.push_back(obj);
        }
    }

    genRandomString(&mObjects[0].str_id, 15); //generate new string ID for first object
    processPrefabObject(&mObjects[0], &mObjects);
    //iterate over all objects and push them to world
    for (unsigned int obj_i = 0; obj_i < mObjects.size(); obj_i++) {
        std::string label = *mObjects[obj_i].label_ptr;
        int add_num = 0; //Declaration of addititonal integer
        getAvailableNumObjLabel(label, &add_num);

        *mObjects[obj_i].label_ptr += std::to_string(add_num); //Set new label to object
        mObjects[obj_i].item_ptr->setText(0, QString::fromStdString(*mObjects[obj_i].label_ptr)); //Set text on widget

        this->addObject(mObjects[obj_i]);
    }
    //Add first object to top of tree
    this->obj_widget_ptr->addTopLevelItem(((GameObject*)this->getGameObjectByStrId(mObjects[0].str_id))->item_ptr);

    for (unsigned int obj_i = 1; obj_i < mObjects.size(); obj_i++) {
        Engine::GameObject* object_ptr = this->getGameObjectByStrId(mObjects[obj_i].str_id);
        object_ptr->parent.world_ptr = this;
        ((GameObject*)object_ptr->parent.updLinkPtr())->item_ptr->addChild(((GameObject*)object_ptr)->item_ptr);
    }
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

    GameObject* rootobj = addMeshNode(node);
    this->obj_widget_ptr->addTopLevelItem(rootobj->item_ptr);
    //Add animation property to root object for correct skinning support
    rootobj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_ANIMATION);
    //Set rootNode to meshProperty on all children objects
    rootobj->setMeshSkinningRootNodeRecursively(rootobj);
}

GameObject* World::addMeshNode(ZS3M::SceneNode* node){
    GameObject obj; //Creating base gameobject
    int add_num = 0; //Declaration of addititonal integer
    getAvailableNumObjLabel(node->node_label, &add_num);
    //Setting base variables
    obj.world_ptr = this;
    obj.addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL);
    //Add common base property Transform
    obj.addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM);
    //Add node property to support skinning
    obj.addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_NODE);

    obj.label_ptr = &obj.getLabelProperty()->label;
    *obj.label_ptr = node->node_label + std::to_string(add_num); //Assigning label to object
    obj.item_ptr->setText(0, QString::fromStdString(*obj.label_ptr));

    Engine::TransformProperty* transform_prop = obj.getTransformProperty();
    transform_prop->setScale(node->node_scaling);
    transform_prop->setTranslation(node->node_translation);
    transform_prop->updateMatrix();

    Engine::NodeProperty* node_prop = static_cast<Engine::NodeProperty*>(obj.getPropertyPtrByType(PROPERTY_TYPE::GO_PROPERTY_TYPE_NODE));
    node_prop->node_label = node->node_label;
    //Set original node transform
    node_prop->transform_mat = node->node_transform;

    //Add node to world
    GameObject* node_object = this->addObject(obj);
    //Iterate over all children nodes
    for(unsigned int node_i = 0; node_i < node->children.size(); node_i ++){
        ZS3M::SceneNode* ptr = node->children[node_i];
        //Create new node object
        GameObject* newobj = addMeshNode(ptr);
        node_object->addChildObject(newobj->getLinkToThisObject());
        node_object->item_ptr->addChild(newobj->item_ptr);

    }

    //Iterate over all meshes, that inside of node
    for(unsigned int mesh_i = 0; mesh_i < node->mesh_names.size(); mesh_i ++){
        std::string mesh_label = node->mesh_names[mesh_i];
        Engine::MeshResource* mesh_ptr = game_data->resources->getMeshByLabel(mesh_label);

        GameObject* mesh_obj = nullptr;
        //if found mesh with name as node, create mesh inside the node
        if(mesh_label.compare(node->node_label) == false){
            mesh_obj = node_object;
        }else {
            //We should create new mesh
            mesh_obj = new GameObject;

            int add_num = 0; //Declaration of addititonal integer
            getAvailableNumObjLabel(mesh_label, &add_num);

            mesh_obj->world_ptr = this;
            mesh_obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL);
            mesh_obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM);

            mesh_obj->label_ptr = &mesh_obj->getLabelProperty()->label;
            *mesh_obj->label_ptr = mesh_label + std::to_string(add_num); //Assigning label to object
            mesh_obj->item_ptr->setText(0, QString::fromStdString(*mesh_obj->label_ptr));
            //Add to world object and parent it
            mesh_obj = this->addObject(*mesh_obj);
            node_object->addChildObject(mesh_obj->getLinkToThisObject(), false);
            node_object->item_ptr->addChild(mesh_obj->item_ptr);
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

void World::storeObjectToPrefab(GameObject* object_ptr, QString file){
    //open stream
    std::ofstream prefab_stream;
    prefab_stream.open(file.toStdString(), std::ofstream::binary);
    //Writing prefab header
    prefab_stream << "ZSPIRE_PREFAB\n";
    //Call first object writing
    writeObjectToPrefab(object_ptr, &prefab_stream);
}

void World::putToShapshot(WorldSnapshot* snapshot){
    //iterate over all objects in scene
    for(unsigned int objs_num = 0; objs_num < this->objects.size(); objs_num ++){
        //Obtain pointer to object
        GameObject* obj_ptr = (GameObject*)this->objects[objs_num];
        if(obj_ptr->alive == false) continue;
        //Iterate over all properties in object and copy them into snapshot
        for(unsigned int prop_i = 0; prop_i < obj_ptr->props_num; prop_i ++){
            auto prop_ptr = obj_ptr->properties[prop_i];
            auto new_prop = _allocProperty(prop_ptr->type);
            new_prop->go_link = prop_ptr->go_link;
            prop_ptr->copyTo(new_prop);
            snapshot->props.push_back(new_prop);
        }
        //Iterate over all scripts in objects and copy them into snapshot
        for (unsigned int script_i = 0; script_i < obj_ptr->scripts_num; script_i++) {
            Engine::ZPScriptProperty* script_ptr = static_cast<Engine::ZPScriptProperty*>
                (obj_ptr->scripts[script_i]);
            Engine::ZPScriptProperty* script_prop = static_cast<Engine::ZPScriptProperty*>
                (_allocProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT));
            script_prop->go_link = script_ptr->go_link;
            script_ptr->copyTo(script_prop);
            snapshot->scripts.push_back(script_prop);
        }
        //Decalare new object
        GameObject newobj;
        //Copy object data
        obj_ptr->copyTo(&newobj);
        snapshot->objects.push_back(newobj);
    }
}

void World::recoverFromSnapshot(WorldSnapshot* snapshot){
    this->clear(); //clear world container first
    obj_widget_ptr->clear(); //clear objects tree
    //iterate over all objects in snapshot
    for(unsigned int objs_num = 0; objs_num < snapshot->objects.size(); objs_num ++){
        GameObject* obj_ptr = &snapshot->objects[objs_num];
        //Create new object
        GameObject newobj;
        obj_ptr->copyTo(&newobj); //Copy object's settings
        this->addObject(newobj); //add object and store pointer to it's new place
    }
    //iterate over all properties in object in snapshot
    for(unsigned int prop_i = 0; prop_i < snapshot->props.size(); prop_i ++){
        auto prop_ptr = snapshot->props[prop_i];
        Engine::GameObjectLink link = prop_ptr->go_link; //Define a link to created object
        link.world_ptr = this; //Set an new world pointer
        GameObject* obj_ptr = updateLink(&link); //Calculate pointer to new object
        obj_ptr->addProperty(prop_ptr->type); //Add new property to created object
        auto new_prop = obj_ptr->getPropertyPtrByType(prop_ptr->type);
        prop_ptr->copyTo(new_prop);
        if(prop_ptr->type == PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL){ //If it is label, we have to do extra stuff
            LabelProperty* label_p = static_cast<LabelProperty*>(new_prop);
            obj_ptr->label_ptr = &label_p->label;
            obj_ptr->item_ptr->setText(0, QString::fromStdString(label_p->label)); //set text to qt widget
            label_p->list_item_ptr = obj_ptr->item_ptr; //send item to LabelProperty
        }
    }
    //Copy all scripts
    for (unsigned int script_i = 0; script_i < snapshot->scripts.size(); script_i++) {
        Engine::ZPScriptProperty* script_ptr = static_cast<Engine::ZPScriptProperty*>
            (snapshot->scripts[script_i]);
        Engine::GameObjectLink link = script_ptr->go_link; //Define a link to created object
        link.world_ptr = this; //Set an new world pointer
        GameObject* obj_ptr = updateLink(&link); //Calculate pointer to new object
        obj_ptr->addScript(); //Add new property to created object
        Engine::ZPScriptProperty* new_script = static_cast<Engine::ZPScriptProperty*>
            (obj_ptr->scripts[script_i]);
        script_ptr->copyTo(new_script);
    }
    //iterate over all objects
    for(unsigned int objs_num = 0; objs_num < snapshot->objects.size(); objs_num ++){
        GameObject* obj_ptr = (GameObject*)objects[objs_num];
        if(!obj_ptr->hasParent) { //if object is unparented
            obj_widget_ptr->addTopLevelItem(obj_ptr->item_ptr); //add to top of widget
            continue;
        }
        GameObject* parent_p = updateLink(&obj_ptr->parent);
        parent_p->children.push_back(obj_ptr->getLinkToThisObject());
        parent_p->item_ptr->addChild(obj_ptr->item_ptr);
    }
}

WorldSnapshot::WorldSnapshot(){

}
void WorldSnapshot::clear(){
    this->objects.clear(); //clear all object
    //iterate over all properties
    for(unsigned int prop_it = 0; prop_it < props.size(); prop_it ++){
       delete props[prop_it];
    }
    props.clear();
}
