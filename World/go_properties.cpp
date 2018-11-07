#include "headers/World.h"
#include "../ProjEd/headers/ProjectEdit.h"

GameObjectProperty::GameObjectProperty(){
    type = GO_PROPERTY_TYPE_NONE;
    active = false; //Inactive by default
}

GameObjectProperty::~GameObjectProperty(){

}
//Cast inheritance calls
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
//Inherited constructors
TransformProperty::TransformProperty(){
    type = GO_PROPERTY_TYPE_TRANSFORM; //Type of property is transform
    active = true; //property is active
    type_label = "Transform";

    this->transform_mat = getIdentity(); //Result matrix is identity by default
    this->translation = ZSVECTOR3(0.0f, 0.0f, 0.0f); //Position is zero by default
    this->scale = ZSVECTOR3(1.0f, 1.0f, 1.0f); //Scale is 1 by default
    this->rotation = ZSVECTOR3(0.0f, 0.0f, 0.0f); //Rotation is 0 by default
}

LabelProperty::LabelProperty(){
    type = GO_PROPERTY_TYPE_LABEL; //its an label
    active = true;
}

MeshProperty::MeshProperty(){
    type = GO_PROPERTY_TYPE_MESH;
    active = true;
}
//Transform property functions
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
//Label property functions
void LabelProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    StringPropertyArea* area = new StringPropertyArea;
    area->setLabel("Label");
    area->value_ptr = &this->label;
    area->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(area);
}

void LabelProperty::onValueChanged(){
    this->list_item_ptr->setText(0, this->label);
}
//Mesh property functions
void MeshProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    PickResourceArea* area = new PickResourceArea;
    area->setLabel("Mesh");
    area->go_property = static_cast<void*>(this);
    area->resource_type = RESOURCE_TYPE_MESH; //It should load meshes only
    inspector->addPropertyArea(area);
}
void MeshProperty::updateMeshPtr(){

}
