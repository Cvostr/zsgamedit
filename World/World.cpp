#include "headers/World.h"
#include "headers/Misc.h"
#include <fstream>
#include <QLineEdit>

GameObjectProperty::GameObjectProperty(){
    type = GO_PROPERTY_TYPE_NONE;
    active = false; //Inactive by default
}

GameObjectProperty::~GameObjectProperty(){

}

void GameObjectProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
     //QTextEdit* pos = new QTextEdit;
    switch(this->type){
        case GO_PROPERTY_TYPE_TRANSFORM:{ //If it is transform
            TransformProperty* transfrom = static_cast<TransformProperty*>(this);
            transfrom->addPropertyInterfaceToInspector(inspector);
            break;
        }
        case GO_PROPERTY_TYPE_LABEL:{ //If it is label
            LabelProperty* label = static_cast<LabelProperty*>(this);
            label->addPropertyInterfaceToInspector(inspector);
            break;
        }
    }
}

void GameObjectProperty::onValueChanged(){
    switch(this->type){
        case GO_PROPERTY_TYPE_TRANSFORM:{ //If it is transform
            TransformProperty* transfrom = static_cast<TransformProperty*>(this);
            transfrom->onValueChanged();
            break;
        }
        case GO_PROPERTY_TYPE_LABEL:{ //If it is label
            LabelProperty* label = static_cast<LabelProperty*>(this);
            label->onValueChanged();
            break;
        }
    }
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
    GameObjectProperty* ptr;
    switch (property) {
        case GO_PROPERTY_TYPE_TRANSFORM:{ //If type is transfrom
            ptr = static_cast<GameObjectProperty*>(new TransformProperty); //Allocation of transform in heap
            break;
        }
        case GO_PROPERTY_TYPE_LABEL:{
            ptr = static_cast<GameObjectProperty*>(new LabelProperty);
            break;
        }
    }
    ptr->object_str_id = this->str_id; //Connect to gameobject via string id
    this->properties.push_back(ptr); //Store poroperty in gameobject
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

LabelProperty* GameObject::getLabelProperty(){
    return static_cast<LabelProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_LABEL));
}
TransformProperty* GameObject::getTransformProperty(){
    return static_cast<TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
}

TransformProperty::TransformProperty(){
    type = GO_PROPERTY_TYPE_TRANSFORM; //Type of property is transform
    active = true; //property is active
    type_label = "Transform";

    this->transform_mat = getIdentity(); //Result matrix is identity by default
    this->translation = ZSVECTOR3(0.0f, 0.0f, 0.0f); //Position is zero by default
    this->scale = ZSVECTOR3(1.0f, 1.0f, 1.0f); //Scale is 1 by default
    this->rotation = ZSVECTOR3(0.0f, 0.0f, 0.0f); //Rotation is 0 by default
}

void TransformProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){

    Float3PropertyArea* area_pos = new Float3PropertyArea; //New property area
    area_pos->setLabel("Position"); //Its label
    area_pos->vector = &this->translation; //Ptr to our vector
    area_pos->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    inspector->addPropertyArea(area_pos);

    Float3PropertyArea* area_scale = new Float3PropertyArea; //New property area
    area_scale->setLabel("Scale"); //Its label
    area_scale->vector = &this->scale; //Ptr to our vector
    area_scale->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(area_scale);

    Float3PropertyArea* area_rotation = new Float3PropertyArea; //New property area
    area_rotation->setLabel("Rotation"); //Its label
    area_rotation->vector = &this->rotation; //Ptr to our vector
    area_rotation->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(area_rotation);
}

void TransformProperty::onValueChanged(){
    updateMat();
}

void TransformProperty::updateMat(){
    //Calculate translation matrix
    ZSMATRIX4x4 translation_mat = getTranslationMat(this->translation);
    //Calculate scale matrix
    ZSMATRIX4x4 scale_mat = getScaleMat(scale);
    //Calculate rotation matrix
    ZSMATRIX4x4 rotation_mat = getRotationMat(rotation);
    //S * R * T
    this->transform_mat = scale_mat * rotation_mat * translation_mat;
}

LabelProperty::LabelProperty(){
    type = GO_PROPERTY_TYPE_LABEL; //its an label
    active = true;
}

void LabelProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    StringPropertyArea* area = new StringPropertyArea;
    area->setLabel("Label");
    area->value_ptr = &this->label;
    area->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(area);
}

void LabelProperty::onValueChanged(){
    this->gobject_ptr->item_ptr->setText(0, this->label);
}

GameObject* World::addObject(GameObject obj){
    this->objects.push_back(obj);
    return &objects[objects.size() - 1];
}

GameObject* World::newObject(){
    GameObject obj; //Creating base gameobject
    int add_num = 0; //Declaration of addititonal integer
    getAvailableNumObjLabel("GameObject ", &add_num);

    obj.addLabelProperty();
    obj.label = &obj.getLabelProperty()->label;
    *obj.label = "GameObject " + QString::number(add_num); //Assigning label to object
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
    world_stream << "ZSP_SCENE";
    world_stream.write((char*)&version, 4);//Writing version
    world_stream.write((char*)&obj_num, 4);
    world_stream.close();

}

void World::openFromFile(QString file){
    std::string fpath = file.toStdString();

    std::ifstream world_stream;
    world_stream.open(fpath.c_str(), std::ofstream::binary);
}
