#include "headers/World.h"
#include "headers/obj_properties.h"
#include "headers/2dtileproperties.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include "../Render/headers/zs-mesh.h"
#include "../ProjEd/headers/InspEditAreas.h"

GameObjectProperty::GameObjectProperty(){
    type = GO_PROPERTY_TYPE_NONE;
    active = false; //Inactive by default
}

GameObjectProperty::~GameObjectProperty(){

}

void GameObjectProperty::copyTo(GameObjectProperty* dest){

}

void GameObjectProperty::onObjectDeleted(){

}

void GameObjectProperty::onAddToObject(){

}

void GameObjectProperty::onUpdate(float deltaTime){

}

QString getPropertyString(int type){
    switch (type) {
        case GO_PROPERTY_TYPE_TRANSFORM:{ //If type is transfrom
            return QString("Transform");
        }
        case GO_PROPERTY_TYPE_LABEL:{
            return QString("Label");
        }
        case GO_PROPERTY_TYPE_MESH:{
            return QString("Mesh");
        }
        case GO_PROPERTY_TYPE_LIGHTSOURCE:{
            return QString("Light");
        }
        case GO_PROPERTY_TYPE_SCRIPTGROUP:{
            return QString("Script Group");
        }
        case GO_PROPERTY_TYPE_AUDSOURCE:{
            return QString("Audio Source");
        }
        case GO_PROPERTY_TYPE_TILE_GROUP:{
            return QString("Tile Group");
        }
        case GO_PROPERTY_TYPE_TILE:{
            return QString("Tile");
        }
    }
    return QString("NONE");
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
        case GO_PROPERTY_TYPE_LIGHTSOURCE:{
            LightsourceProperty* ptr = new LightsourceProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_SCRIPTGROUP:{
            ScriptGroupProperty* ptr = new ScriptGroupProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_AUDSOURCE:{
            AudioSourceProperty* ptr = new AudioSourceProperty;
            _ptr = static_cast<AudioSourceProperty*>(ptr);
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

}

void GameObjectProperty::onValueChanged(){

}
//Inherited constructors
TransformProperty::TransformProperty(){
    type = GO_PROPERTY_TYPE_TRANSFORM; //Type of property is transform
    active = true; //property is active

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

    mesh_ptr = nullptr; //set it to 0x0 to check later
    this->resource_relpath = "@none";
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
    //If at least one coordinate changed
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
        ZSMATRIX4x4 rotation_mat1 = getIdentity();
        getAbsoluteRotationMatrix(rotation_mat1);

        ZSMATRIX4x4 rotation_mat = getRotationMat(this->rotation);
        //S * R * T
        this->transform_mat = scale_mat * rotation_mat * rotation_mat1 * translation_mat;
    }
}

void TransformProperty::getAbsoluteRotationMatrix(ZSMATRIX4x4& m){
    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property

    if(ptr == nullptr) return;

    if(ptr->hasParent == true){
        GameObject* parent_p = ptr->parent.ptr;
        TransformProperty* property = static_cast<TransformProperty*>(parent_p->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));

        ZSMATRIX4x4 rotation_mat1 = getRotationMat(property->rotation, ptr->getTransformProperty()->translation);
        m = rotation_mat1 * m;
        property->getAbsoluteRotationMatrix(m);
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
    if(resource_relpath.length() < 1) return;

    if(resource_relpath.compare("@plane") == false){
        this->mesh_ptr = ZSPIRE::getPlaneMesh2D();
    }else if(resource_relpath.compare("@isotile") == false){
        this->mesh_ptr = ZSPIRE::getIsoTileMesh2D();
    }else if(resource_relpath.compare("@cube") == false){
        this->mesh_ptr = ZSPIRE::getCubeMesh3D();
    }
    else //If it isn't built in mesh
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
    AreaRadioGroup* group = new AreaRadioGroup; //allocate button layout
    group->value_ptr = &this->light_type;
    group->go_property = static_cast<void*>(this);

    QRadioButton* directional_radio = new QRadioButton; //allocate first radio
    directional_radio->setText("Directional");
    QRadioButton* point_radio = new QRadioButton;
    point_radio->setText("Point");

    //add created radio buttons
    group->addRadioButton(directional_radio);
    group->addRadioButton(point_radio);
    inspector->registerUiObject(group);
    inspector->getContentLayout()->addLayout(group->btn_layout);

    FloatPropertyArea* intensity_area = new FloatPropertyArea;
    intensity_area->setLabel("Intensity"); //Its label
    intensity_area->value = &this->intensity;
    intensity_area->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(intensity_area);

    FloatPropertyArea* range_area = new FloatPropertyArea;
    range_area->setLabel("Range"); //Its label
    range_area->value = &this->range;
    range_area->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(range_area);

    ColorDialogArea* lcolor = new ColorDialogArea;
    lcolor->setLabel("Light color");
    lcolor->color = &this->color;
    lcolor->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(lcolor);
}
void LightsourceProperty::onValueChanged(){
    ZSVECTOR3* rot_vec_ptr = &transform->rotation;
    this->direction = _getDirection(rot_vec_ptr->X, rot_vec_ptr->Y, rot_vec_ptr->Z);
    if(deffered_shader_ptr != nullptr && isSent){
        deffered_shader_ptr->Use(); //use shader to make uniforms work
        //Send light uniforms
        deffered_shader_ptr->sendLight(this->id, static_cast<void*>(this));
    }
}
void LightsourceProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't transform

    LightsourceProperty* _dest = static_cast<LightsourceProperty*>(dest);
    _dest->color = color;
    _dest->intensity = intensity;
    _dest->range = range;
    _dest->light_type = light_type;
}

void LightsourceProperty::updTransformPtr(){
    if(transform == nullptr){
        transform = this->go_link.updLinkPtr()->getTransformProperty();
        this->last_pos = transform->_last_translation; //Store old value
    }
}

void LightsourceProperty::onObjectDeleted(){
    deffered_shader_ptr->unsetLight(this->id);
}

LightsourceProperty::LightsourceProperty(){
    type = GO_PROPERTY_TYPE_LIGHTSOURCE;
    active = true;
    light_type = LIGHTSOURCE_TYPE_DIRECTIONAL; //base type is directional

    intensity = 1.0f; //base light instensity is 1
    range = 10.0f;
    transform = nullptr;
    isSent = false; //isn't sent by default
}

AudioSourceProperty::AudioSourceProperty(){
    type = GO_PROPERTY_TYPE_AUDSOURCE;

    buffer_ptr = nullptr;

    source.Init();
}

void AudioSourceProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    PickResourceArea* area = new PickResourceArea;
    area->setLabel("Sound");
    area->go_property = static_cast<void*>(this);
    area->rel_path = &resource_relpath;
    area->resource_type = RESOURCE_TYPE_AUDIO; //It should load meshes only
    inspector->addPropertyArea(area);

    FloatPropertyArea* gain_area = new FloatPropertyArea;
    gain_area->setLabel("Gain"); //Its label
    gain_area->value = &this->source.source_gain;
    gain_area->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(gain_area);

    FloatPropertyArea* pitch_area = new FloatPropertyArea;
    pitch_area->setLabel("Pitch"); //Its label
    pitch_area->value = &this->source.source_pitch;
    pitch_area->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(pitch_area);
}

void AudioSourceProperty::onValueChanged(){
    updateAudioPtr();

    this->source.apply_settings();
}

void AudioSourceProperty::updateAudioPtr(){
    this->buffer_ptr = world_ptr->getSoundPtrByName(resource_relpath);

    this->source.setAlBuffer(this->buffer_ptr);
}

void AudioSourceProperty::onUpdate(float deltaTime){
    TransformProperty* transform = go_link.updLinkPtr()->getTransformProperty();

    if(transform->_last_translation != this->last_pos){
        this->source.setPosition(transform->translation);
        this->last_pos = transform->translation;
    }
}

void AudioSourceProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't transform

    AudioSourceProperty* _dest = static_cast<AudioSourceProperty*>(dest);

    _dest->source.Init();
    _dest->resource_relpath = this->resource_relpath;
    _dest->source.source_gain = this->source.source_gain;
    _dest->source.source_pitch = this->source.source_pitch;
    _dest->source.setPosition(this->source.source_pos);
    _dest->buffer_ptr = this->buffer_ptr;
    _dest->source.setAlBuffer(this->buffer_ptr);
}

void AudioSourceProperty::audio_start(){
    this->source.play();
}

void AudioSourceProperty::audio_stop(){
    this->source.stop();
}

void AudioSourceProperty::onObjectDeleted(){
    this->source.stop(); //Stop at first
    this->source.Destroy();
}

void GameObject::saveProperties(std::ofstream* stream){
    unsigned int props_num = static_cast<unsigned int>(this->props_num);

    for(unsigned int prop_i = 0; prop_i < props_num; prop_i ++){
        GameObjectProperty* property_ptr = static_cast<GameObjectProperty*>(this->properties[prop_i]);
        //*stream << "\nG_PROPERTY " << property_ptr->type << " "; //Writing property header
        *stream << "\nG_PROPERTY ";
        stream->write(reinterpret_cast<char*>(&property_ptr->type), sizeof(int));
        *stream << " ";

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
        case GO_PROPERTY_TYPE_LIGHTSOURCE:{
            LightsourceProperty* ptr = static_cast<LightsourceProperty*>(property_ptr);
            ZSLIGHTSOURCE_TYPE type = ptr->light_type;
            float intensity = ptr->intensity;
            float range = ptr->range;

            float color_r = ptr->color.r;
            float color_g = ptr->color.g;
            float color_b = ptr->color.b;

            stream->write(reinterpret_cast<char*>(&type), sizeof(ZSLIGHTSOURCE_TYPE));
            stream->write(reinterpret_cast<char*>(&intensity), sizeof(float));
            stream->write(reinterpret_cast<char*>(&range), sizeof(float));

            stream->write(reinterpret_cast<char*>(&color_r), sizeof(float));
            stream->write(reinterpret_cast<char*>(&color_g), sizeof(float));
            stream->write(reinterpret_cast<char*>(&color_b), sizeof(float));

            break;
        }
        case GO_PROPERTY_TYPE_AUDSOURCE:{
            AudioSourceProperty* ptr = static_cast<AudioSourceProperty*>(property_ptr);
            if(ptr->resource_relpath.isEmpty()) //check if object has no texture
                *stream << "@none";
            else
                *stream << ptr->resource_relpath.toStdString() << "\n";

            stream->write(reinterpret_cast<char*>(&ptr->source.source_gain), sizeof(float));
            stream->write(reinterpret_cast<char*>(&ptr->source.source_pitch), sizeof(float));

            break;
        }
        case GO_PROPERTY_TYPE_SCRIPTGROUP:{
            ScriptGroupProperty* ptr = static_cast<ScriptGroupProperty*>(property_ptr);
            int script_num = static_cast<int>(ptr->scr_num);
            //write amount of scripts
            stream->write(reinterpret_cast<char*>(&script_num), sizeof(int));
            *stream << "\n"; //write divider
            for(unsigned int script_w_i = 0; script_w_i < script_num; script_w_i ++){
                 *stream << ptr->path_names[script_w_i].toStdString() << "\n";
            }

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
            if(ptr->diffuse_relpath.isEmpty()) //check if object has no texture
                *stream << "@none";
            else
                *stream << ptr->diffuse_relpath.toStdString() << "\n";

            //Animation stuff
            stream->write(reinterpret_cast<char*>(&ptr->anim_property.isAnimated), sizeof(bool));
            if(ptr->anim_property.isAnimated){ //if animated, then write animation properties
                stream->write(reinterpret_cast<char*>(&ptr->anim_property.framesX), sizeof(int));
                stream->write(reinterpret_cast<char*>(&ptr->anim_property.framesY), sizeof(int));
            }
            break;
        }
        }
    }
}

void GameObject::loadProperty(std::ifstream* world_stream){
    int type;
    //*world_stream >> type;
    world_stream->seekg(1, std::ofstream::cur); //Skip space
    world_stream->read(reinterpret_cast<char*>(&type), sizeof(int));
    // world_stream->seekg(1, std::ofstream::cur); //Skip space
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
    case GO_PROPERTY_TYPE_LIGHTSOURCE:{
        LightsourceProperty* ptr = static_cast<LightsourceProperty*>(prop_ptr);
        world_stream->seekg(1, std::ofstream::cur);

        world_stream->read(reinterpret_cast<char*>(&ptr->light_type), sizeof(ZSLIGHTSOURCE_TYPE));
        world_stream->read(reinterpret_cast<char*>(&ptr->intensity), sizeof(float));
        world_stream->read(reinterpret_cast<char*>(&ptr->range), sizeof(float));

        float cl_r;
        float cl_g;
        float cl_b;

        world_stream->read(reinterpret_cast<char*>(&cl_r), sizeof(float));
        world_stream->read(reinterpret_cast<char*>(&cl_g), sizeof(float));
        world_stream->read(reinterpret_cast<char*>(&cl_b), sizeof(float));
        ptr->color = ZSRGBCOLOR(cl_r, cl_g, cl_b);

        break;
    }
    case GO_PROPERTY_TYPE_SCRIPTGROUP:{
        ScriptGroupProperty* ptr = static_cast<ScriptGroupProperty*>(prop_ptr);
        world_stream->seekg(1, std::ofstream::cur);
        //Read scripts number
        world_stream->read(reinterpret_cast<char*>(&ptr->scr_num), sizeof(int));
        world_stream->seekg(1, std::ofstream::cur);
        //resize arrays
        ptr->path_names.resize(ptr->scr_num);
        ptr->scripts_attached.resize(ptr->scr_num);
        //iterate over all scripts and read their path
        Project* project_ptr = static_cast<Project*>(this->world_ptr->proj_ptr);
        for(unsigned int script_w_i = 0; script_w_i < ptr->scr_num; script_w_i ++){
            std::string scr_path;
            *world_stream >> scr_path;
            ptr->path_names[script_w_i] = QString::fromStdString(scr_path);

            ptr->scripts_attached[script_w_i].fpath = project_ptr->root_path + "/" + ptr->path_names[script_w_i];
        }

        //ptr->onValueChanged();

        break;
    }
    case GO_PROPERTY_TYPE_AUDSOURCE:{
        std::string rel_path;
        *world_stream >> rel_path;
        AudioSourceProperty* lptr = static_cast<AudioSourceProperty*>(prop_ptr);
        if(rel_path.compare("@none") != 0){
            lptr->resource_relpath = QString::fromStdString(rel_path); //Write loaded mesh relative path
            lptr->updateAudioPtr(); //Pointer will now point to mesh resource
        }
        world_stream->seekg(1, std::ofstream::cur);
        //Load settings
        world_stream->read(reinterpret_cast<char*>(&lptr->source.source_gain), sizeof(float));
        world_stream->read(reinterpret_cast<char*>(&lptr->source.source_pitch), sizeof(float));
        lptr->source.apply_settings(); //Apply settings to openal

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

        t_ptr->isCreated = static_cast<bool>(isCreated);

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
        world_stream->seekg(1, std::ofstream::cur);
        world_stream->read(reinterpret_cast<char*>(&lptr->anim_property.isAnimated), sizeof(bool));
        if(lptr->anim_property.isAnimated){ //if animated, then write animation properties
            world_stream->read(reinterpret_cast<char*>(&lptr->anim_property.framesX), sizeof(int));
            world_stream->read(reinterpret_cast<char*>(&lptr->anim_property.framesY), sizeof(int));
        }

        break;
        }
    }
}

void ScriptGroupProperty::onValueChanged(){
    Project* project_ptr = static_cast<Project*>(this->world_ptr->proj_ptr);

    if(static_cast<int>(path_names.size()) != this->scr_num){
        path_names.resize(scr_num);
    }

    if(static_cast<int>(scripts_attached.size()) != this->scr_num){ //if size changed
        this->scripts_attached.resize(this->scr_num);
        //Iterate over all scripts and use absolute path
        for(unsigned int script_i = 0; script_i < scr_num; script_i ++){
            //Set absolute path to script object
            scripts_attached[script_i].fpath = project_ptr->root_path + "/" + path_names[script_i];
        }
        //Update inspector interface
        insp_win->updateObjectProperties();
    }
}

void ScriptGroupProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    this->insp_win = inspector; //store inspector window

    IntPropertyArea* scriptnum_area = new IntPropertyArea;
    scriptnum_area->setLabel("Scripts"); //Its label
    scriptnum_area->value = &this->scr_num;
    scriptnum_area->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(scriptnum_area);

    for(unsigned int script_i = 0; script_i < scr_num; script_i ++){
        PickResourceArea* area = new PickResourceArea;
        area->setLabel("Lua Script");
        area->go_property = static_cast<void*>(this);
        area->rel_path = &path_names[script_i];
        area->extension_mask = ".lua";
        area->resource_type = PICK_RES_TYPE_FILE; //It should load meshes only
        inspector->addPropertyArea(area);
    }
}
void ScriptGroupProperty::onUpdate(float deltaTime){
    for(unsigned int script_i = 0; script_i < this->scripts_attached.size(); script_i ++){
        ObjectScript* script_ptr = &this->scripts_attached[script_i]; //Obtain pointer to script
        script_ptr->_callDraw(); //Run onDraw() function in script
    }
}

void ScriptGroupProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't transform

    ScriptGroupProperty* _dest = static_cast<ScriptGroupProperty*>(dest);
    _dest->scr_num = this->scr_num;

    //resize data vectors
    _dest->scripts_attached.resize(scr_num);
    _dest->path_names.resize(scr_num);
    //Copy data
    for(unsigned int script_i = 0; script_i < scr_num; script_i ++){
        _dest->scripts_attached[script_i] = this->scripts_attached[script_i];
        _dest->path_names[script_i] = this->path_names[script_i];
    }
}

void ScriptGroupProperty::wakeUp(){
    for(unsigned int script_i = 0; script_i < scr_num; script_i ++){
        this->scripts_attached[script_i].link = this->go_link;

        this->scripts_attached[script_i]._InitScript();
        this->scripts_attached[script_i]._callStart();
    }
}

ScriptGroupProperty::ScriptGroupProperty(){
    type = GO_PROPERTY_TYPE_SCRIPTGROUP;

    scr_num = 0;
    this->scripts_attached.resize(this->scr_num);
}
