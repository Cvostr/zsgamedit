#include "headers/World.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include "headers/Misc.h"
#include <QLineEdit>
#include "headers/obj_properties.h"

extern RenderPipeline* renderer;
extern Material* default3dmat;

World::World(){
    objects.reserve(MAX_OBJS);
    proj_ptr = nullptr;
}

int World::getFreeObjectSpaceIndex(){
    unsigned int index_to_push = static_cast<unsigned int>(objects.size()); //Set free index to objects amount
    unsigned int objects_num = static_cast<unsigned int>(this->objects.size());
    for(unsigned int objs_i = 0; objs_i < objects_num; objs_i ++){
        if(objects[objs_i].alive == false){ //if object deleted
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

    int index_to_push = -1;
    unsigned int objects_num = static_cast<unsigned int>(this->objects.size());
    for(unsigned int objs_i = 0; objs_i < objects_num; objs_i ++){
        if(objects[objs_i].alive == false){
            index_to_push = static_cast<int>(objs_i);
        }
    }

    GameObject* ptr = nullptr;
    if(index_to_push == -1){ //if all indeces are busy
        this->objects.push_back(obj); //Push object to vector's end
        ptr = &objects[objects.size() - 1];
        ptr->array_index = static_cast<int>(objects.size() - 1);
    }else{ //if vector has an empty space
        objects[static_cast<unsigned int>(index_to_push)] = obj;
        ptr = &objects[static_cast<unsigned int>(index_to_push)];
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
        TransformProperty* transform = new_obj->getTransformProperty();
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
    LabelProperty* label_prop = new_obj->getLabelProperty(); //Obtain pointer to label property
    std::string to_paste;
    genRandomString(&to_paste, 3);
    label_prop->label = label_prop->label + "_" + QString::fromStdString(to_paste);
    label_prop->list_item_ptr = new_obj->item_ptr; //Setting to label new qt item
    new_obj->label = &label_prop->label;
    new_obj->item_ptr->setText(0, label_prop->label);
    //Dublicate chilldren object
    unsigned int children_amount = static_cast<unsigned int>(original->children.size());
    //Iterate over all children
    for(unsigned int child_i = 0; child_i < children_amount; child_i ++){
        //Get pointer to original child object
        GameObjectLink link = original->children[child_i];
        //create new child obect by dublication of original
        GameObject* new_child = dublicateObject(link.ptr, false);
        //parenting
        new_obj->addChildObject(new_child->getLinkToThisObject());
        //UI parenting
        new_obj->item_ptr->addChild(new_child->item_ptr);
    }

    return new_obj;
}

GameObject* World::Instantiate(GameObject* original){
    GameObjectLink link = original->getLinkToThisObject();
    GameObject* result = this->dublicateObject(link.ptr);

    if(result->hasParent){ //if object parented
        result->parent.ptr->item_ptr->addChild(result->item_ptr);
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
        if(!obj_ptr->alive) continue;
        if(obj_ptr->label->compare(label) == 0) //if labels are same
            return obj_ptr; //Return founded object
    }
    return nullptr; //if we haven't found one
}

GameObject* World::getObjectByLabelStr(std::string label){
    return getObjectByLabel(QString::fromStdString(label));
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
         if(obj_ptr->label == nullptr || !obj_ptr->alive) continue;
         if(obj_ptr->label->compare(tocheck_str) == 0) //If label on object is same
             hasEqualName = true; //Then we founded equal name
     }
     if(hasEqualName == true){
         *result += 1;
         getAvailableNumObjLabel(label, result);
     }
}

bool World::isObjectLabelUnique(QString label){
    unsigned int objs_num = static_cast<unsigned int>(this->objects.size());
    int ret_amount = 0;
    for(unsigned int obj_it = 0; obj_it < objs_num; obj_it ++){ //Iterate over all objs in scene
        GameObject* obj_ptr = &this->objects[obj_it]; //Get pointer to checking object
        //if object was destroyed
        if(!obj_ptr->alive) continue;
        if(obj_ptr->label->compare(label) == 0){
            ret_amount += 1;
            if(ret_amount > 1) return false;
        }
    }
    return true;
}

void World::removeObj(GameObjectLink link){
    GameObjectLink l = link;
    l.updLinkPtr();
    l.ptr->alive = false; //Mark object as dead

    unsigned int children_num = static_cast<unsigned int>(l.ptr->children.size());

    for(unsigned int ch_i = 0; ch_i < children_num; ch_i ++){ //Walk through all children an remove them
        GameObjectLink link = l.ptr->children[0]; //Remove first of children because of trim
        removeObj(link);
    }
    //Remove all content in heap, related to object class object
    l.ptr->clearAll();
    if(l.ptr->hasParent == true){ //If object parented by other obj
        GameObject* parent = l.ptr->parent.updLinkPtr(); //Receive pointer to object's parent

        unsigned int children_am = static_cast<unsigned int>(parent->children.size()); //get children amount
        for(unsigned int i = 0; i < children_am; i++){ //Iterate over all children in object
            GameObjectLink* link_ptr = &parent->children[i];
            if(l.obj_str_id.compare(link_ptr->obj_str_id) == 0){ //if str_id in requested link compares to iteratable link
                parent->children[i].crack(); //Make link broken
            }
        }
        parent->trimChildrenArray(); //Remove cracked link from vector
    }
}

void World::removeObjPtr(GameObject* obj){
    removeObj(obj->getLinkToThisObject());
}
void World::trimObjectsList(){
    for (unsigned int i = 0; i < objects.size(); i ++) { //Iterating over all objects
        if(objects[i].alive == false){ //If object marked as deleted
            for (unsigned int obj_i = i + 1; obj_i < objects.size(); obj_i ++) { //Iterate over all next chidren
                objects[obj_i - 1] = objects[obj_i]; //Move it to previous place

            }
            objects.resize(objects.size() - 1); //remove last position
        }
    }
}

void World::unpickObject(){
    for (unsigned int i = 0; i < objects.size(); i ++) { //Iterating over all objects
        if(objects[i].isPicked == true){ //If object marked as deleted
            objects[i].isPicked = false; //Mark object as unpicked
        }
    }
}

void World::writeGameObject(GameObject* object_ptr, std::ofstream* world_stream){
    if(object_ptr->alive == true){
        *world_stream << "\nG_OBJECT " << object_ptr->str_id << " ";

        world_stream->write(reinterpret_cast<char*>(&object_ptr->active), sizeof(bool));
        world_stream->write(reinterpret_cast<char*>(&object_ptr->IsStatic), sizeof(bool));

        if(object_ptr->children.size() > 0){ //If object has at least one child object
            int children_num = object_ptr->getAliveChildrenAmount();
            //Write children header
            *world_stream << "\nG_CHI ";
            //Write amount of children i object
            world_stream->write(reinterpret_cast<char*>(&children_num), sizeof(int));

            unsigned int children_am = static_cast<unsigned int>(object_ptr->children.size());
            for(unsigned int chi_i = 0; chi_i < children_am; chi_i ++){ //iterate over all children
                GameObjectLink* link_ptr = &object_ptr->children[chi_i]; //Gettin pointer to child
                if(!link_ptr->isEmpty()){ //If this link isn't broken (after child removal)
                    *world_stream << link_ptr->obj_str_id << " "; //Writing child's string id
                }
            }
        }
        object_ptr->saveProperties(world_stream); //save object's properties
        *world_stream << "\nG_END"; //End writing object
    }
}

void World::loadGameObject(GameObject* object_ptr, std::ifstream* world_stream){
    std::string prefix;

    object_ptr->world_ptr = this; //Writing pointer to world
    *world_stream >> object_ptr->str_id;

    world_stream->seekg(1, std::ofstream::cur);

    world_stream->read(reinterpret_cast<char*>(&object_ptr->active), sizeof(bool));
    world_stream->read(reinterpret_cast<char*>(&object_ptr->IsStatic), sizeof(bool));

    //Then do the same sh*t, iterate until "G_END" came up
    while(true){
        *world_stream >> prefix; //Read prefix
        if(prefix.compare("G_END") == 0){ //If end reached
            break; //Then end this infinity loop
        }
        if(prefix.compare("G_CHI") == 0) { //Ops, it is chidren header
            unsigned int amount;
            //world_stream >> amount; //Reading children amount
            world_stream->seekg(1, std::ofstream::cur);
            world_stream->read(reinterpret_cast<char*>(&amount), sizeof(int));

            for(unsigned int ch_i = 0; ch_i < amount; ch_i ++){ //Iterate over all written children to file
                std::string child_str_id;
                *world_stream >> child_str_id; //Reading child string id

                GameObjectLink link;
                link.world_ptr = this; //Setting world pointer
                link.obj_str_id = child_str_id; //Setting string ID
                object_ptr->children.push_back(link); //Adding to object
            }
        }
        if(prefix.compare("G_PROPERTY") == 0){ //We found an property, zaeb*s'
            object_ptr->loadProperty(world_stream);
        }
    }
}

void World::saveToFile(QString file){
    RenderSettings* settings_ptr = renderer->getRenderSettings();

    std::string fpath = file.toStdString();

    std::ofstream world_stream;
    world_stream.open(fpath.c_str(), std::ofstream::binary);
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
        GameObject* object_ptr = static_cast<GameObject*>(&this->objects[obj_i]);
        //Write GameObject
        writeGameObject(object_ptr, &world_stream);
    }

    //world_stream << "\nRENDER_SETTINGS_SKYOBJ\n";
    //SkyboxProperty* skyb = static_cast<SkyboxProperty*>(settings_ptr->skybox_ptr);
    //world_stream << skyb->go_link.updLinkPtr()->str_id;
    //world_stream << "\n_END\n";

    world_stream.close();

}


void World::openFromFile(QString file, QTreeWidget* w_ptr){
    RenderSettings* settings_ptr = renderer->getRenderSettings();
    this->obj_widget_ptr = w_ptr;

    clear(); //Clear all objects
    std::string fpath = file.toStdString(); //Turn QString to std::string

    std::ifstream world_stream;
    world_stream.open(fpath.c_str(), std::ifstream::binary); //Opening to read binary data

    std::string test_header;
    world_stream >> test_header; //Read header
    if(test_header.compare("ZSP_SCENE") != 0) //If it isn't zspire scene
        return; //Go out, we have nothing to do
    world_stream.seekg(1, std::ifstream::cur);
    int version = 0;
    int objs_num = 0;
    world_stream.read(reinterpret_cast<char*>(&version), sizeof(int)); //reading version
    world_stream.read(reinterpret_cast<char*>(&objs_num), sizeof(int)); //reading objects count

    while(!world_stream.eof()){ //While file not finished reading
        std::string prefix;
        world_stream >> prefix; //Read prefix

        if(prefix.compare("RENDER_SETTINGS_AMB_COLOR") == 0){ //if it is render setting of ambient light color
            world_stream.seekg(1, std::ifstream::cur);
            world_stream.read(reinterpret_cast<char*>(&settings_ptr->ambient_light_color.r), sizeof(int)); //Writing R component of amb color
            world_stream.read(reinterpret_cast<char*>(&settings_ptr->ambient_light_color.g), sizeof(int)); //Writing G component of amb color
            world_stream.read(reinterpret_cast<char*>(&settings_ptr->ambient_light_color.b), sizeof(int)); //Writing B component of amb color
        }

        if(prefix.compare("RENDER_SETTINGS_SKYOBJ") == 0){ //if it is render setting of skybox object
            std::string obj_label;
            world_stream >> obj_label;
            //find object
            GameObject* obj_ptr = this->getObjectByStringId(obj_label);
            settings_ptr->skybox_ptr = obj_ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_SKYBOX);
        }

        if(prefix.compare("G_OBJECT") == 0){ //if it is game object
            GameObject object; //firstly, define an object

            World::loadGameObject(&object, &world_stream);

            this->addObject(object); //Add object to world
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
    //Now add all objects to inspector tree
    for(unsigned int obj_i = 0; obj_i < this->objects.size(); obj_i ++){
        GameObject* obj_ptr = &this->objects[obj_i];
        if(obj_ptr->parent.isEmpty()){ //If object has no parent
            w_ptr->addTopLevelItem(obj_ptr->item_ptr);
        }else{ //It has a parent
            GameObject* parent_ptr = obj_ptr->parent.ptr; //Get parent pointer
            parent_ptr->item_ptr->addChild(obj_ptr->item_ptr); //Connect Qt Tree Items
        }
    }
}

void World::processPrefabObject(GameObject* object_ptr, std::vector<GameObject>* objects_array){
    unsigned int props_amount = object_ptr->props_num;
    //iterate over all props and update gameobject links
    for(unsigned int prop_i = 0; prop_i < props_amount; prop_i ++){
        GameObjectProperty* prop_ptr = object_ptr->properties[prop_i];
        prop_ptr->go_link.obj_str_id = object_ptr->str_id; //set new string id
        prop_ptr->go_link.updLinkPtr();
    }

    unsigned int children_amount = static_cast<unsigned int>(object_ptr->children.size());

    for(unsigned int chi_i = 0; chi_i < children_amount; chi_i ++){
        GameObjectLink link = object_ptr->children[chi_i];
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
        GameObjectLink link = object_ptr->children[ch_i];
        writeObjectToPrefab(link.updLinkPtr(), stream);
    }
}

void World::addObjectsFromPrefab(QString file){
    std::ifstream prefab_stream;
    //opening prefab stream
    prefab_stream.open(file.toStdString(), std::ifstream::binary);

    std::string header;
    //reading headers
    prefab_stream >> header;
    //testing header
    if(header.compare("ZSPIRE_PREFAB")) return;

    std::string prefix;

    std::vector<GameObject> mObjects;

    while(!prefab_stream.eof()){
        prefab_stream >> prefix;

        if(!prefix.compare("G_OBJECT")){
            GameObject obj;
            this->loadGameObject(&obj, &prefab_stream);
            mObjects.push_back(obj);
        }
    }

    genRandomString(&mObjects[0].str_id, 15); //generate new string ID for first object
    processPrefabObject(&mObjects[0], &mObjects);
    //iterate over all objects and push them to world
    for(unsigned int obj_i = 0; obj_i < mObjects.size(); obj_i ++){
        QString label = *mObjects[obj_i].label;
        int add_num = 0; //Declaration of addititonal integer
        getAvailableNumObjLabel(label, &add_num);

        *mObjects[obj_i].label += QString::number(add_num); //Set new label to object
        mObjects[obj_i].item_ptr->setText(0, *mObjects[obj_i].label); //Set text on widget

        this->addObject(mObjects[obj_i]);
    }
    //Add first object to top of tree
    this->obj_widget_ptr->addTopLevelItem(this->getObjectByStringId(mObjects[0].str_id)->item_ptr);

    for(unsigned int obj_i = 1; obj_i < mObjects.size(); obj_i ++){
        GameObject* object_ptr = this->getObjectByStringId(mObjects[obj_i].str_id);
        object_ptr->parent.world_ptr = this;
        object_ptr->parent.updLinkPtr()->item_ptr->addChild(object_ptr->item_ptr);
    }
}

void World::addObjectsFromPrefabStr(std::string file){
    addObjectsFromPrefab(QString::fromStdString(file));
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
    rootobj->addProperty(GO_PROPERTY_TYPE_ANIMATION);
    //Set rootNode to meshProperty on all children objects
    rootobj->setMeshSkinningRootNodeRecursively(rootobj);
}

GameObject* World::addMeshNode(ZS3M::SceneNode* node){
    GameObject obj; //Creating base gameobject
    int add_num = 0; //Declaration of addititonal integer
    getAvailableNumObjLabel(QString::fromStdString(node->node_label), &add_num);
    //Setting base variables
    obj.world_ptr = this;
    obj.addLabelProperty();
    //Add common base property Transform
    obj.addProperty(GO_PROPERTY_TYPE_TRANSFORM);
    //Add node property to support skinning
    obj.addProperty(GO_PROPERTY_TYPE_NODE);

    obj.label = &obj.getLabelProperty()->label;
    *obj.label = QString::fromStdString(node->node_label) + QString::number(add_num); //Assigning label to object
    obj.item_ptr->setText(0, *obj.label);

    TransformProperty* transform_prop = static_cast<TransformProperty*>(obj.getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    transform_prop->setScale(node->node_scaling);
    transform_prop->setTranslation(node->node_translation);
    transform_prop->updateMat();

    NodeProperty* node_prop = static_cast<NodeProperty*>(obj.getPropertyPtrByType(GO_PROPERTY_TYPE_NODE));
    node_prop->node_label = QString::fromStdString(node->node_label);
    //Set original node transform
    node_prop->transform_mat = node->node_transform;

    if(node->hasBone){
        node_prop->hasBone = true;
    }

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
        ZSPIRE::Mesh* mesh_ptr = this->getMeshPtrByRelPath(QString::fromStdString(mesh_label));

        GameObject* mesh_obj = nullptr;
        //if found mesh with name as node, create mesh inside the node
        if(mesh_label.compare(node->node_label) == false){
            mesh_obj = node_object;
        }else {
            //We should create new mesh
            mesh_obj = new GameObject;

            int add_num = 0; //Declaration of addititonal integer
            getAvailableNumObjLabel(QString::fromStdString(mesh_label), &add_num);

            mesh_obj->world_ptr = this;
            mesh_obj->addLabelProperty();
            mesh_obj->addProperty(GO_PROPERTY_TYPE_TRANSFORM);

            mesh_obj->label = &mesh_obj->getLabelProperty()->label;
            *mesh_obj->label = QString::fromStdString(mesh_label) + QString::number(add_num); //Assigning label to object
            mesh_obj->item_ptr->setText(0, *mesh_obj->label);
            //Add to world object and parent it
            mesh_obj = this->addObject(*mesh_obj);
            node_object->addChildObject(mesh_obj->getLinkToThisObject(), false);
            node_object->item_ptr->addChild(mesh_obj->item_ptr);
        }

        mesh_obj->addProperty(GO_PROPERTY_TYPE_MESH); //add mesh property
        mesh_obj->addProperty(GO_PROPERTY_TYPE_MATERIAL); //add material property

        //configure mesh
        MeshProperty* mesh_prop_ptr = static_cast<MeshProperty*>(mesh_obj->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
        mesh_prop_ptr->resource_relpath = QString::fromStdString(mesh_label);

        mesh_prop_ptr->updateMeshPtr();
        //configure material
        MaterialProperty* mat_prop_ptr = static_cast<MaterialProperty*>(mesh_obj->getPropertyPtrByType(GO_PROPERTY_TYPE_MATERIAL));
        mat_prop_ptr->setMaterial("@none");

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

void World::clear(){
    //iterate over all objects and purge them all
    for(unsigned int objs_i = 0; objs_i < objects.size(); objs_i ++){
        GameObject* obj_ptr = &objects[objs_i];
        obj_ptr->alive = false;
        obj_ptr->clearAll(false);
    }
    objects.clear();
}

void World::putToShapshot(WorldSnapshot* snapshot){
    //iterate over all objects in scene
    for(unsigned int objs_num = 0; objs_num < this->objects.size(); objs_num ++){
        //Obtain pointer to object
        GameObject* obj_ptr = &this->objects[objs_num];
        if(obj_ptr->alive == false) continue;
        //Iterate over all properties in object and copy them into snapshot
        for(unsigned int prop_i = 0; prop_i < obj_ptr->props_num; prop_i ++){
            auto prop_ptr = obj_ptr->properties[prop_i];
            auto new_prop = allocProperty(prop_ptr->type);
            new_prop->go_link = prop_ptr->go_link;
            prop_ptr->copyTo(new_prop);
            snapshot->props.push_back(new_prop);
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
    GameObject* newobj_ptr = nullptr;

    for(unsigned int objs_num = 0; objs_num < snapshot->objects.size(); objs_num ++){
        GameObject* obj_ptr = &snapshot->objects[objs_num];
        //Create new object
        GameObject newobj;
        obj_ptr->copyTo(&newobj); //Copy object's settings
        newobj_ptr = this->addObject(newobj); //add object and store pointer to it's new place
    }
    //iterate over all properties in object in snapshot
    for(unsigned int prop_i = 0; prop_i < snapshot->props.size(); prop_i ++){
        auto prop_ptr = snapshot->props[prop_i];
        GameObjectLink link = prop_ptr->go_link; //Define a link to created object
        link.world_ptr = this; //Set an new world pointer
        GameObject* obj_ptr = link.updLinkPtr(); //Calculate pointer to new object
        obj_ptr->addProperty(prop_ptr->type); //Add new property to created object
        auto new_prop = obj_ptr->getPropertyPtrByType(prop_ptr->type);
        prop_ptr->copyTo(new_prop);
        if(prop_ptr->type == GO_PROPERTY_TYPE_LABEL){ //If it is label, we have to do extra stuff
            LabelProperty* label_p = static_cast<LabelProperty*>(new_prop);
            obj_ptr->label = &label_p->label;
            obj_ptr->item_ptr->setText(0, *obj_ptr->label); //set text to qt widget
            label_p->list_item_ptr = obj_ptr->item_ptr; //send item to LabelProperty
        }

    }
    //iterate over all objects
    for(unsigned int objs_num = 0; objs_num < snapshot->objects.size(); objs_num ++){
        GameObject* obj_ptr = &objects[objs_num];
        if(!obj_ptr->hasParent) { //if object is unparented
            obj_widget_ptr->addTopLevelItem(obj_ptr->item_ptr); //add to top of widget
            continue;
        }
        GameObject* parent_p = obj_ptr->parent.updLinkPtr();
        parent_p->children.push_back(obj_ptr->getLinkToThisObject());
        parent_p->item_ptr->addChild(obj_ptr->item_ptr);
    }
}

ZSPIRE::Mesh* World::getMeshPtrByRelPath(QString label){
    Project* proj_ptr = static_cast<Project*>(this->proj_ptr); //Convert void pointer to Project*
    unsigned int resources_num = static_cast<unsigned int>(proj_ptr->resources.size()); //Receive resource amount in project

    for(unsigned int r_it = 0; r_it < resources_num; r_it ++){ //Iteerate over all resources in project
        Resource* r_ptr = &proj_ptr->resources[r_it]; //Obtain pointer to resource
        //If resource is mesh and has same name as in argument
        if(r_ptr->type == RESOURCE_TYPE_MESH && r_ptr->resource_label.compare(label.toStdString()) == 0){
            return static_cast<ZSPIRE::Mesh*>(r_ptr->class_ptr);
        }
    }
    return nullptr;
}

ZSPIRE::Animation* World::getAnimationPtrByRelPath(QString label){
    Project* proj_ptr = static_cast<Project*>(this->proj_ptr); //Convert void pointer to Project*
    unsigned int resources_num = static_cast<unsigned int>(proj_ptr->resources.size()); //Receive resource amount in project

    for(unsigned int r_it = 0; r_it < resources_num; r_it ++){ //Iteerate over all resources in project
        Resource* r_ptr = &proj_ptr->resources[r_it]; //Obtain pointer to resource
        //If resource is mesh and has same name as in argument
        if(r_ptr->type == RESOURCE_TYPE_ANIMATION && r_ptr->resource_label.compare(label.toStdString()) == 0){
            return static_cast<ZSPIRE::Animation*>(r_ptr->class_ptr);
        }
    }
    return nullptr;
}


ZSPIRE::Texture* World::getTexturePtrByRelPath(QString label){
    Project* proj_ptr = static_cast<Project*>(this->proj_ptr); //Convert void pointer to Project*
    unsigned int resources_num = static_cast<unsigned int>(proj_ptr->resources.size()); //Receive resource amount in project

    for(unsigned int r_it = 0; r_it < resources_num; r_it ++){ //Iteerate over all resources in project
        Resource* r_ptr = &proj_ptr->resources[r_it]; //Obtain pointer to resource
        //If resource is mesh and has same name as in argument
        if(r_ptr->type == RESOURCE_TYPE_TEXTURE && r_ptr->rel_path.compare(label) == 0){
            return static_cast<ZSPIRE::Texture*>(r_ptr->class_ptr);
        }
    }
    return nullptr;
}

SoundBuffer* World::getSoundPtrByName(QString label){
    Project* proj_ptr = static_cast<Project*>(this->proj_ptr); //Convert void pointer to Project*
    unsigned int resources_num = static_cast<unsigned int>(proj_ptr->resources.size()); //Receive resource amount in project

    for(unsigned int r_it = 0; r_it < resources_num; r_it ++){ //Iteerate over all resources in project
        Resource* r_ptr = &proj_ptr->resources[r_it]; //Obtain pointer to resource
        //If resource is mesh and has same name as in argument
        if(r_ptr->type == RESOURCE_TYPE_AUDIO && r_ptr->rel_path.compare(label) == 0){
            return static_cast<SoundBuffer*>(r_ptr->class_ptr);
        }
    }
    return nullptr;
}

Material* World::getMaterialPtrByName(QString label){
    Project* proj_ptr = static_cast<Project*>(this->proj_ptr); //Convert void pointer to Project*
    unsigned int resources_num = static_cast<unsigned int>(proj_ptr->resources.size()); //Receive resource amount in project

    for(unsigned int r_it = 0; r_it < resources_num; r_it ++){ //Iteerate over all resources in project
        Resource* r_ptr = &proj_ptr->resources[r_it]; //Obtain pointer to resource
        //If resource is mesh and has same name as in argument
        if(r_ptr->type == RESOURCE_TYPE_MATERIAL && r_ptr->rel_path.compare(label) == 0){
            return static_cast<Material*>(r_ptr->class_ptr);
        }
    }
    //if path is incorrect, return default material
    return default3dmat;
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
