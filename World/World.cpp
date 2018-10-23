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
}

GameObject::GameObject(){
    this->hasParent = false; //No parent by default
    item_ptr = new QTreeWidgetItem; //Allocate tree widget item
    genRandomString(&this->str_id, 15); //Generate random string ID
}

bool GameObject::addTransformPropety(){
    unsigned int props = static_cast<unsigned int>(this->properties.size());
    for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
        GameObjectProperty* property = this->properties[prop_i];
        if(property->type == GO_PROPERTY_TYPE_TRANSFORM){
            return false;
        }
    }
    TransformProperty* property = new TransformProperty;
    this->properties.push_back(property);
    return true;
}

TransformProperty::TransformProperty(){
    type = GO_PROPERTY_TYPE_TRANSFORM; //Type of property is transform
    active = true; //property is active

    this->transform_mat = getIdentity(); //Result matrix is identity by default
    this->translation = ZSVECTOR3(0.0f, 0.0f, 0.0f); //Position is zero by default
    this->scale = ZSVECTOR3(1.0f, 1.0f, 1.0f); //Scale is 1 by default
}

void TransformProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    QHBoxLayout* pos_layout = new QHBoxLayout;

    QLineEdit* pos = new QLineEdit;
    pos->setFixedWidth(60);
    pos_layout->addWidget(pos);

    QLineEdit* scale = new QLineEdit;
    scale->setFixedWidth(60);
    pos_layout->addWidget(scale);

    inspector->getContentLayout()->addLayout(pos_layout);
}

void TransformProperty::updateMat(){
    //Calculate translation matrix
    ZSMATRIX4x4 translation_mat = getTranslationMat(this->translation);
    //Calculate scale matrix
    ZSMATRIX4x4 scale_mat = getScaleMat(scale);
    ZSMATRIX4x4 rotation_mat;
    //S * R * T
    this->transform_mat = scale_mat * translation_mat;
}

GameObject* World::addObject(GameObject obj){
    this->objects.push_back(obj);
    return &objects[objects.size() - 1];
}

GameObject* World::newObject(){
    GameObject obj; //Creating base gameobject
    int add_num = 0; //Declaration of addititonal integer
    getAvailableNumObjLabel("GameObject ", &add_num);
    obj.label = "GameObject " + QString::number(add_num); //Assigning label to object
    obj.item_ptr->setText(0, obj.label);
    return this->addObject(obj); //Return pointer to new object
}

GameObject* World::getObjectByLabel(QString label){
    unsigned int objs_num = static_cast<unsigned int>(this->objects.size());
    for(unsigned int obj_it = 0; obj_it < objs_num; obj_it ++){ //Iterate over all objs in scene
        GameObject* obj_ptr = &this->objects[obj_it]; //Get pointer to checking object
        if(obj_ptr->label.compare(label) == 0) //if labels are same
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
         if(obj_ptr->label.compare(tocheck_str) == 0) //If label on object is same
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
