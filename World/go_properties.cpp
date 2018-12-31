#include "headers/World.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include "../Render/headers/zs-mesh.h"

GameObjectProperty::GameObjectProperty(){
    type = GO_PROPERTY_TYPE_NONE;
    active = false; //Inactive by default
    //size = sizeof(GameObjectProperty);
    //data_start = 0x0;
}

GameObjectProperty::~GameObjectProperty(){

}

void GameObjectProperty::copyTo(GameObjectProperty* dest){

}

GameObjectProperty* allocProperty(int type){
    GameObjectProperty* _ptr = nullptr;
    switch (type) {
        case GO_PROPERTY_TYPE_TRANSFORM:{ //If type is transfrom
            _ptr = static_cast<GameObjectProperty*>(new TransformProperty); //Allocation of transform in heap
            break;
        }
        case GO_PROPERTY_TYPE_LABEL:{
            LabelProperty* ptr = new LabelProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_MESH:{
            MeshProperty* ptr = new MeshProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_TILE_GROUP:{
            TileGroupProperty* ptr = new TileGroupProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_TILE:{
            TileProperty* ptr = new TileProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
    }
    return _ptr;
}

//Cast inheritance calls
void GameObjectProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
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
    //size = sizeof(TransformProperty);
    //data_start = &_last_translation;

    this->transform_mat = getIdentity(); //Result matrix is identity by default
    this->translation = ZSVECTOR3(0.0f, 0.0f, 0.0f); //Position is zero by default
    this->scale = ZSVECTOR3(1.0f, 1.0f, 1.0f); //Scale is 1 by default
    this->rotation = ZSVECTOR3(0.0f, 0.0f, 0.0f); //Rotation is 0 by default
}

LabelProperty::LabelProperty(){
    type = GO_PROPERTY_TYPE_LABEL; //its an label
    active = true;
    //size = sizeof(LabelProperty);
    //data_start = &label;
}

MeshProperty::MeshProperty(){
    type = GO_PROPERTY_TYPE_MESH;
    active = true;
    //size = sizeof(MeshProperty);
    //data_start = &resource_relpath;
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
    //Variables to store
    ZSVECTOR3 p_translation = ZSVECTOR3(0,0,0);
    ZSVECTOR3 p_scale = ZSVECTOR3(1,1,1);
    ZSVECTOR3 p_rotation = ZSVECTOR3(0,0,0);

    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property
    if(ptr != nullptr){ //if object exist
        if(ptr->hasParent){ //if object dependent

            TransformProperty* property = static_cast<TransformProperty*>(ptr->parent.updLinkPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
            property->getAbsoluteParentTransform(p_translation, p_scale, p_rotation);
        }
    }

    if(this->translation + p_translation == this->_last_translation
            && this->scale * p_scale == this->_last_scale
            && this->rotation + p_rotation == this->_last_rotation) {
        return;
    }else{

        this->_last_translation = this->translation + p_translation;
        this->_last_scale = this->scale * p_scale;
        this->_last_rotation = this->rotation + p_rotation;

        //Calculate translation matrix
        ZSMATRIX4x4 translation_mat = getTranslationMat(_last_translation);
        //Calculate scale matrix
        ZSMATRIX4x4 scale_mat = getScaleMat(_last_scale);
        //Calculate rotation matrix
        ZSMATRIX4x4 rotation_mat = getRotationMat(_last_rotation);
        //S * R * T
        this->transform_mat = scale_mat * rotation_mat * translation_mat;
    }
}


void TransformProperty::getAbsoluteParentTransform(ZSVECTOR3& t, ZSVECTOR3& s, ZSVECTOR3& r){
    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property

    t = t + this->translation;
    s = s * this->scale;
    r = r + this->rotation;

    if(ptr->hasParent){
        GameObject* parent_p = ptr->parent.ptr;
        TransformProperty* property = static_cast<TransformProperty*>(parent_p->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
        property->getAbsoluteParentTransform(t, s, r);
    }
}

void TransformProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't transform

    TransformProperty* _dest = static_cast<TransformProperty*>(dest);
    _dest->translation = translation;
    _dest->scale = scale;
    _dest->rotation = rotation;
    _dest->transform_mat = transform_mat;
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

void LabelProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't transform

    LabelProperty* _dest = static_cast<LabelProperty*>(dest);
    _dest->label = label;
}

//Mesh property functions
void MeshProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    PickResourceArea* area = new PickResourceArea;
    area->setLabel("Mesh");
    area->go_property = static_cast<void*>(this);
    area->rel_path = &resource_relpath;
    area->resource_type = RESOURCE_TYPE_MESH; //It should load meshes only
    inspector->addPropertyArea(area);
}
void MeshProperty::updateMeshPtr(){
    if(resource_relpath.compare("@plane") == false){
        this->mesh_ptr = ZSPIRE::getPlaneMesh2D();
    }else //If it isn't built in mesh
    {
       this->mesh_ptr = world_ptr->getMeshPtrByRelPath(resource_relpath);
    }
}

void MeshProperty::onValueChanged(){
    updateMeshPtr();
}

void MeshProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't transform

    MeshProperty* _dest = static_cast<MeshProperty*>(dest);
    _dest->resource_relpath = resource_relpath;
    _dest->mesh_ptr = mesh_ptr;
}

void LightsourceProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    AreaRadioGroup* group = new AreaRadioGroup;

    QRadioButton* directional_radio;
    QRadioButton* point_radio;
}
void LightsourceProperty::onValueChanged(){

}
void LightsourceProperty::copyTo(GameObjectProperty* dest){

}
LightsourceProperty::LightsourceProperty(){
    type = GO_PROPERTY_TYPE_LIGHTSOURCE;
    active = true;
    light_type = LIGHTSOURCE_TYPE_DIRECTIONAL; //base type is directional

    intensity = 1.0f; //base light instensity is 1
}

void GameObject::saveProperties(std::ofstream* stream){
    unsigned int props_num = static_cast<unsigned int>(this->props_num);

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
        case GO_PROPERTY_TYPE_MESH:{
            MeshProperty* ptr = static_cast<MeshProperty*>(property_ptr);
            *stream << ptr->resource_relpath.toStdString();
            break;
        }
        case GO_PROPERTY_TYPE_TILE_GROUP:{
            TileGroupProperty* ptr = static_cast<TileGroupProperty*>(property_ptr);
            int isCreated = static_cast<int>(ptr->isCreated);
            int geometryWidth = ptr->geometry.tileWidth;
            int geometryHeight = ptr->geometry.tileHeight;
            int amountX = ptr->tiles_amount_X;
            int amountY = ptr->tiles_amount_Y;

            stream->write(reinterpret_cast<char*>(&isCreated), sizeof(int));
            stream->write(reinterpret_cast<char*>(&geometryWidth), sizeof(int));
            stream->write(reinterpret_cast<char*>(&geometryHeight), sizeof(int));
            stream->write(reinterpret_cast<char*>(&amountX), sizeof(int));
            stream->write(reinterpret_cast<char*>(&amountY), sizeof(int));
            break;
        }
        case GO_PROPERTY_TYPE_TILE:{
            TileProperty* ptr = static_cast<TileProperty*>(property_ptr);
            if(ptr->diffuse_relpath.isEmpty())
                *stream << "@none";
            else
                *stream << ptr->diffuse_relpath.toStdString();
            break;
        }
        }
    }
}

void GameObject::loadProperty(std::ifstream* world_stream){
    int type;
    *world_stream >> type;
    addProperty(type);
    GameObjectProperty* prop_ptr = getPropertyPtrByType(type); //get created property
    //since more than 1 properties same type can't be on one gameobject
    switch(type){
        case GO_PROPERTY_TYPE_LABEL :{
            std::string label;
            *world_stream >> label;
            LabelProperty* lptr = static_cast<LabelProperty*>(prop_ptr);
            this->label = &lptr->label; //Making GameObjects's pointer to string in label property
            lptr->label = QString::fromStdString(label); //Write loaded string
            lptr->list_item_ptr->setText(0, lptr->label); //Set text on widget
            break;
        }
        case GO_PROPERTY_TYPE_TRANSFORM :{
            world_stream->seekg(1, std::ofstream::cur); //Skip space
            TransformProperty* t_ptr = static_cast<TransformProperty*>(prop_ptr);
            world_stream->read(reinterpret_cast<char*>(&t_ptr->translation.X), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->translation.Y), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->translation.Z), sizeof(float));

            world_stream->read(reinterpret_cast<char*>(&t_ptr->scale.X), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->scale.Y), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->scale.Z), sizeof(float));

            world_stream->read(reinterpret_cast<char*>(&t_ptr->rotation.X), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->rotation.Y), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->rotation.Z), sizeof(float));

            t_ptr->updateMat(); //After everything is loaded, update matrices

        break;
    }
    case GO_PROPERTY_TYPE_MESH :{
        std::string rel_path;
        *world_stream >> rel_path;
        MeshProperty* lptr = static_cast<MeshProperty*>(prop_ptr);
        lptr->resource_relpath = QString::fromStdString(rel_path); //Write loaded mesh relative path
        lptr->updateMeshPtr(); //Pointer will now point to mesh resource

        break;
    }
    case GO_PROPERTY_TYPE_TILE_GROUP :{
        world_stream->seekg(1, std::ofstream::cur); //Skip space
        TileGroupProperty* t_ptr = static_cast<TileGroupProperty*>(prop_ptr);
        int isCreated = 0;
        world_stream->read(reinterpret_cast<char*>(&isCreated), sizeof(int));
        world_stream->read(reinterpret_cast<char*>(&t_ptr->geometry.tileWidth), sizeof(int));
        world_stream->read(reinterpret_cast<char*>(&t_ptr->geometry.tileHeight), sizeof(int));

        world_stream->read(reinterpret_cast<char*>(&t_ptr->tiles_amount_X), sizeof(int));
        world_stream->read(reinterpret_cast<char*>(&t_ptr->tiles_amount_Y), sizeof(int));

        t_ptr->isCreated = static_cast<bool>(isCreated  );

    break;
    }
    case GO_PROPERTY_TYPE_TILE:{
        std::string rel_path;
        *world_stream >> rel_path;
        TileProperty* lptr = static_cast<TileProperty*>(prop_ptr);
        if(rel_path.compare("@none") != 0){
            lptr->diffuse_relpath = QString::fromStdString(rel_path); //Write loaded mesh relative path
            lptr->updTexturePtr(); //Pointer will now point to mesh resource
        }
        break;
    }
    }
}
