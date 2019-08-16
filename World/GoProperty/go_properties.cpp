#include "../headers/World.h"
#include "../headers/obj_properties.h"
#include "../headers/2dtileproperties.h"
#include "../../ProjEd/headers/ProjectEdit.h"
#include "../../Render/headers/zs-mesh.h"
#include "../../ProjEd/headers/InspEditAreas.h"

extern InspectorWin* _inspector_win;
//selected terrain
static TerrainProperty* current_terrain_prop;

GameObjectProperty::GameObjectProperty(){
    type = GO_PROPERTY_TYPE_NONE;
    active = true; //Inactive by default
}

GameObjectProperty::~GameObjectProperty(){

}
void GameObjectProperty::setActive(bool active){
    this->active = active;
}
void GameObjectProperty::copyTo(GameObjectProperty* dest){
    dest->active = this->active;
    dest->world_ptr = this->world_ptr;
}

void GameObjectProperty::onObjectDeleted(){

}

void GameObjectProperty::onAddToObject(){

}

void GameObjectProperty::onUpdate(float deltaTime){

}

void GameObjectProperty::onPreRender(RenderPipeline* pipeline){

}

void GameObjectProperty::onRender(RenderPipeline* pipeline){

}
void GameObjectProperty::onTrigger(GameObject* obj){

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
        case GO_PROPERTY_TYPE_MATERIAL:{
            return QString("Material");
        }
        case GO_PROPERTY_TYPE_COLLIDER:{
            return QString("Collider");
        }
        case GO_PROPERTY_TYPE_RIGIDBODY:{
            return QString("Rigidbody");
        }
        case GO_PROPERTY_TYPE_SHADOWCASTER:{
            return QString("Shadow Caster");
        }
        case GO_PROPERTY_TYPE_TILE_GROUP:{
            return QString("Tile Group");
        }
        case GO_PROPERTY_TYPE_TILE:{
            return QString("Tile");
        }
        case GO_PROPERTY_TYPE_SKYBOX:{
            return QString("Skybox");
        }
        case GO_PROPERTY_TYPE_TERRAIN:{
            return QString("Terrain");
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
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_MATERIAL:{
            MaterialProperty* ptr = new MaterialProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_COLLIDER:{
            ColliderProperty* ptr = new ColliderProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_RIGIDBODY:{
            RigidbodyProperty* ptr = new RigidbodyProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_SKYBOX:{
            SkyboxProperty* ptr = new SkyboxProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_SHADOWCASTER:{
            ShadowCasterProperty* ptr = new ShadowCasterProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_TERRAIN:{
            TerrainProperty* ptr = new TerrainProperty;
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

    castShadows = true;
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

    ColliderProperty* coll = this->go_link.updLinkPtr()->getPropertyPtr<ColliderProperty>();
    RigidbodyProperty* rigid = this->go_link.updLinkPtr()->getPropertyPtr<RigidbodyProperty>();

    PhysicalProperty* phys = nullptr;

    if(coll == nullptr)
        phys = rigid;
    else {
        phys = coll;
    }

    if(go_link.updLinkPtr()->isRigidbody()){
        if(!phys->created) return;
        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3( btScalar(_last_translation.X),
                                                    btScalar(_last_translation.Y),
                                                    btScalar(_last_translation.Z)));

        startTransform.setRotation(btQuaternion(_last_rotation.X, _last_rotation.Y, _last_rotation.Z));


        phys->rigidBody->setWorldTransform(startTransform);
        phys->rigidBody->getMotionState()->setWorldTransform(startTransform);
        phys->rigidBody->activate(true);

        phys->shape->setLocalScaling(btVector3(btScalar(_last_scale.X),
                                               btScalar(_last_scale.Y),
                                               btScalar(_last_scale.Z)));

    }
}

void TransformProperty::onPreRender(RenderPipeline* pipeline){
    updateMat();
}

void TransformProperty::setTranslation(ZSVECTOR3 new_translation){
    this->translation = new_translation;
    onValueChanged();
}

void TransformProperty::setScale(ZSVECTOR3 new_scale){
    this->scale = new_scale;
    onValueChanged();
}
void TransformProperty::setRotation(ZSVECTOR3 new_rotation){
    this->rotation = new_rotation;
    onValueChanged();
}

void TransformProperty::updateMat(){
    //Variables to store
    ZSVECTOR3 p_translation = ZSVECTOR3(0,0,0);
    ZSVECTOR3 p_scale = ZSVECTOR3(1,1,1);
    ZSVECTOR3 p_rotation = ZSVECTOR3(0,0,0);

    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property
    if(ptr != nullptr){ //if object exist
        if(ptr->hasParent){ //if object dependent
            //Get parent's transform property
            TransformProperty* property = static_cast<TransformProperty*>(ptr->parent.updLinkPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
            //Calculate parent transform offset
            property->getAbsoluteParentTransform(p_translation, p_scale, p_rotation);
        }
    }
    //If at least one coordinate changed
    if(this->translation + p_translation == this->_last_translation
            && this->scale * p_scale == this->_last_scale
            && this->rotation + p_rotation == this->_last_rotation) {
        return; //No changes, nothing to be done
    }else{
        //Store last pos to work with collisions


        //rewrite last values
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
    //Do base things
    GameObjectProperty::copyTo(dest);

    //cast pointer and send data
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

   /* BoolCheckboxArea* isActive = new BoolCheckboxArea;
    isActive->setLabel("Active ");
    isActive->go_property = static_cast<void*>(this);
    isActive->bool_ptr = &this->isActiveToggle;
    inspector->addPropertyArea(isActive);
*/
}

void LabelProperty::onValueChanged(){
    World* world_ptr = this->world_ptr; //Obtain pointer to world object
    //lets chack if object already exist in world
    if(!world_ptr->isObjectLabelUnique(this->label)){
        //If object already exist
        int label_add = 0;
        world_ptr->getAvailableNumObjLabel(this->label, &label_add);
        label = label + "_" + QString::number(label_add);
    }

    this->list_item_ptr->setText(0, this->label);
}

void LabelProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't label

    //Do base things
    GameObjectProperty::copyTo(dest);

    LabelProperty* _dest = static_cast<LabelProperty*>(dest);
    _dest->label = label;
    _dest->isActiveToggle = isActiveToggle;
}

//Mesh property functions
void MeshProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    PickResourceArea* area = new PickResourceArea;
    area->setLabel("Mesh");
    area->go_property = static_cast<void*>(this);
    area->rel_path = &resource_relpath;
    area->resource_type = RESOURCE_TYPE_MESH; //It should load meshes only
    inspector->addPropertyArea(area);

    BoolCheckboxArea* IsCastShadows = new BoolCheckboxArea;
    IsCastShadows->setLabel("Cast shadows ");
    IsCastShadows->go_property = static_cast<void*>(this);
    IsCastShadows->bool_ptr = &this->castShadows;
    inspector->addPropertyArea(IsCastShadows);
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
    if(dest->type != this->type) return; //if it isn't mesh, then exit

    //Do base things
    GameObjectProperty::copyTo(dest);

    MeshProperty* _dest = static_cast<MeshProperty*>(dest);
    _dest->resource_relpath = resource_relpath;
    _dest->mesh_ptr = mesh_ptr;
}

void LightsourceProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    AreaRadioGroup* group = new AreaRadioGroup; //allocate button layout
    group->value_ptr = reinterpret_cast<uint8_t*>(&this->light_type);
    group->go_property = static_cast<void*>(this);

    QRadioButton* directional_radio = new QRadioButton; //allocate first radio
    directional_radio->setText("Directional");
    QRadioButton* point_radio = new QRadioButton;
    point_radio->setText("Point");
    QRadioButton* spot_radio = new QRadioButton;
    spot_radio->setText("Spot");

    //add created radio buttons
    group->addRadioButton(directional_radio);
    group->addRadioButton(point_radio);
    group->addRadioButton(spot_radio);
    inspector->registerUiObject(group);
    inspector->getContentLayout()->addLayout(group->btn_layout);

    FloatPropertyArea* intensity_area = new FloatPropertyArea;
    intensity_area->setLabel("Intensity"); //Its intensity
    intensity_area->value = &this->intensity;
    intensity_area->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(intensity_area);
    if(this->light_type > LIGHTSOURCE_TYPE_DIRECTIONAL){

        FloatPropertyArea* range_area = new FloatPropertyArea;
        range_area->setLabel("Range"); //Its range
        range_area->value = &this->range;
        range_area->go_property = static_cast<void*>(this);
        inspector->addPropertyArea(range_area);
        if(this->light_type == LIGHTSOURCE_TYPE_SPOT){
            FloatPropertyArea* spotangle_area = new FloatPropertyArea;
            spotangle_area->setLabel("Spot Angle"); //Its range
            spotangle_area->value = &this->spot_angle;
            spotangle_area->go_property = static_cast<void*>(this);
            inspector->addPropertyArea(spotangle_area);
        }
    }

    ColorDialogArea* lcolor = new ColorDialogArea;
    lcolor->setLabel("Light color");
    lcolor->color = &this->color;
    lcolor->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(lcolor);
}
void LightsourceProperty::onValueChanged(){
    if(this->_last_light_type != this->light_type){
        //We need to change UI
        _inspector_win->updateRequired = true;
        this->_last_light_type = this->light_type;
    }

    ZSVECTOR3* rot_vec_ptr = &transform->rotation;
    this->direction = _getDirection(rot_vec_ptr->X, rot_vec_ptr->Y, rot_vec_ptr->Z);

}
void LightsourceProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't Lightsource, then exit

    //Do base things
    GameObjectProperty::copyTo(dest);

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
}

void LightsourceProperty::onPreRender(RenderPipeline* pipeline){
    TransformProperty* transform_prop = go_link.updLinkPtr()->getTransformProperty();

    updTransformPtr();

    pipeline->addLight(static_cast<void*>(this)); //put light pointer to vector

    //check, if light transformation changed
    if((this->last_pos != transform_prop->_last_translation || this->last_rot != transform_prop->rotation)){
        this->onValueChanged();
        //store new transform values
        this->last_pos = transform_prop->_last_translation;
        this->last_rot = transform_prop->rotation;
    }
}

LightsourceProperty::LightsourceProperty(){
    type = GO_PROPERTY_TYPE_LIGHTSOURCE;
    active = true;
    light_type = LIGHTSOURCE_TYPE_DIRECTIONAL; //base type is directional

    intensity = 1.0f; //base light instensity is 1
    range = 10.0f;
    spot_angle = 12.5f;
    transform = nullptr;
}

AudioSourceProperty::AudioSourceProperty(){
    type = GO_PROPERTY_TYPE_AUDSOURCE;

    buffer_ptr = nullptr;
    this->resource_relpath = "@none";

    this->source.source_gain = 1.0f;
    this->source.source_pitch = 1.0f;

    source.Init();
}

void AudioSourceProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    PickResourceArea* area = new PickResourceArea;
    area->setLabel("Sound");
    area->go_property = static_cast<void*>(this);
    area->rel_path = &resource_relpath;
    area->resource_type = RESOURCE_TYPE_AUDIO; //It should load meshes only
    inspector->addPropertyArea(area);

    BoolCheckboxArea* isLooped = new BoolCheckboxArea;
    isLooped->setLabel("Loop ");
    isLooped->go_property = static_cast<void*>(this);
    isLooped->bool_ptr = &this->source.looped;
    inspector->addPropertyArea(isLooped);

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

    if(buffer_ptr == nullptr) return;

    this->source.setAlBuffer(this->buffer_ptr);
}

void AudioSourceProperty::onUpdate(float deltaTime){
    TransformProperty* transform = go_link.updLinkPtr()->getTransformProperty();

    //if(transform->translation != this->last_pos){
    this->source.setPosition(transform->translation);
      //  this->last_pos = transform->translation;
    //}
}

void AudioSourceProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't audiosource then exit

    AudioSourceProperty* _dest = static_cast<AudioSourceProperty*>(dest);

    //Do base things
    GameObjectProperty::copyTo(dest);

    _dest->source.Init();
    _dest->resource_relpath = this->resource_relpath;
    _dest->source.source_gain = this->source.source_gain;
    _dest->source.source_pitch = this->source.source_pitch;
    _dest->source.setPosition(this->source.source_pos);
    _dest->buffer_ptr = this->buffer_ptr;
    //_dest->source.setAlBuffer(this->buffer_ptr);
}
void AudioSourceProperty::setAudioFile(std::string relpath){
    this->resource_relpath = QString::fromStdString(relpath);
    this->updateAudioPtr();
}
void AudioSourceProperty::audio_start(){
    this->source.play();
}

void AudioSourceProperty::audio_stop(){
    this->source.stop();
}

void AudioSourceProperty::audio_pause(){
    this->source.pause();
}

float AudioSourceProperty::getGain(){
    return source.source_gain;
}
float AudioSourceProperty::getPitch(){
    return source.source_pitch;
}
void AudioSourceProperty::setGain(float gain){
    source.source_gain = gain;
    source.apply_settings();
}
void AudioSourceProperty::setPitch(float pitch){
    source.source_pitch = pitch;
    source.apply_settings();
}

void AudioSourceProperty::onObjectDeleted(){
    this->source.stop(); //Stop at first
    this->source.Destroy();
}

MaterialProperty::MaterialProperty(){
    type = GO_PROPERTY_TYPE_MATERIAL;

    receiveShadows = true;
    this->material_ptr = nullptr;
}

void MaterialProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    //Add area to pick material file
    PickResourceArea* area = new PickResourceArea;
    area->setLabel("Material");
    area->go_property = static_cast<void*>(this);
    area->rel_path = &material_path;
    area->resource_type = RESOURCE_TYPE_MATERIAL; //It should load meshes only
    inspector->addPropertyArea(area);
    //No material, exiting
    if(material_ptr == nullptr) return;

    if(material_ptr->group_ptr->acceptShadows){
        BoolCheckboxArea* receiveShdws = new BoolCheckboxArea;
        receiveShdws->setLabel("Receive Shadows ");
        receiveShdws->go_property = static_cast<void*>(this);
        receiveShdws->bool_ptr = &this->receiveShadows;
        inspector->addPropertyArea(receiveShdws);
    }
    if(material_ptr == nullptr) return;

    ComboBoxArea* mt_shader_group_area = new ComboBoxArea;
    mt_shader_group_area->setLabel("Shader Group");
    mt_shader_group_area->go_property = static_cast<void*>(this);
    mt_shader_group_area->result_string = &this->group_label;

    for(unsigned int i = 0; i < MtShProps::getMaterialShaderPropertyAmount(); i ++){
        MtShaderPropertiesGroup* ptr = MtShProps::getMtShaderPropertiesGroupByIndex(i);
        mt_shader_group_area->widget.addItem(ptr->groupCaption);
    }
    inspector->addPropertyArea(mt_shader_group_area);

    //if material isn't set up, exiting
    if(material_ptr->group_ptr == nullptr) return;
    //If set up, iterating over all items
    for(unsigned int prop_i = 0; prop_i < material_ptr->group_ptr->properties.size(); prop_i ++){
        MaterialShaderProperty* prop_ptr = material_ptr->group_ptr->properties[prop_i];
        MaterialShaderPropertyConf* conf_ptr = this->material_ptr->confs[prop_i];
        switch(prop_ptr->type){
            case MATSHPROP_TYPE_NONE:{
                break;
            }
            case MATSHPROP_TYPE_TEXTURE:{
                //Cast pointer
                TextureMaterialShaderProperty* texture_p = static_cast<TextureMaterialShaderProperty*>(prop_ptr);
                TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);

                PickResourceArea* area = new PickResourceArea;
                area->setLabel(texture_p->prop_caption);
                area->go_property = static_cast<void*>(this);
                area->rel_path = &texture_conf->path;
                area->isShowNoneItem = true;
                area->resource_type = RESOURCE_TYPE_TEXTURE; //It should load textures only
                inspector->addPropertyArea(area);

                break;
            }
            case MATSHPROP_TYPE_FLOAT:{
                //Cast pointer
                FloatMaterialShaderProperty* float_p = static_cast<FloatMaterialShaderProperty*>(prop_ptr);
                FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);

                FloatPropertyArea* float_area = new FloatPropertyArea;
                float_area->setLabel(float_p->prop_caption); //Its label
                float_area->value = &float_conf->value;
                float_area->go_property = static_cast<void*>(this);
                inspector->addPropertyArea(float_area);

                break;
            }
            case MATSHPROP_TYPE_FVEC3:{
                //Cast pointer
                Float3MaterialShaderProperty* float3_p = static_cast<Float3MaterialShaderProperty*>(prop_ptr);
                Float3MtShPropConf* float3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);

                Float3PropertyArea* float3_area = new Float3PropertyArea;
                float3_area->setLabel(float3_p->prop_caption); //Its label
                float3_area->vector = &float3_conf->value;
                float3_area->go_property = static_cast<void*>(this);
                inspector->addPropertyArea(float3_area);

                break;
            }
            case MATSHPROP_TYPE_INTEGER:{
                //Cast pointer
                IntegerMaterialShaderProperty* integer_p = static_cast<IntegerMaterialShaderProperty*>(prop_ptr);
                IntegerMtShPropConf* integer_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);

                IntPropertyArea* integer_area = new IntPropertyArea;
                integer_area->setLabel(integer_p->prop_caption); //Its label
                integer_area->value = &integer_conf->value;
                integer_area->go_property = static_cast<void*>(this);
                inspector->addPropertyArea(integer_area);

                break;
            }
            case MATSHPROP_TYPE_COLOR:{
                //Cast pointer
                ColorMaterialShaderProperty* color_p = static_cast<ColorMaterialShaderProperty*>(prop_ptr);
                ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);

                ColorDialogArea* color_area = new ColorDialogArea;
                color_area->setLabel(color_p->prop_caption); //Its label
                color_area->color = &color_conf->color;
                color_area->go_property = static_cast<void*>(this);
                inspector->addPropertyArea(color_area);

                break;
            }
            case MATSHPROP_TYPE_TEXTURE3:{
                //Cast pointer
                Texture3MaterialShaderProperty* texture_p = static_cast<Texture3MaterialShaderProperty*>(prop_ptr);
                Texture3MtShPropConf* texture_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);

                IntPropertyArea* integer_area = new IntPropertyArea;
                integer_area->setLabel("Textures count"); //Its label
                integer_area->value = &texture_conf->texture_count;
                integer_area->go_property = static_cast<void*>(this);
                inspector->addPropertyArea(integer_area);

                for(int i = 0; i < texture_conf->texture_count; i ++){
                    PickResourceArea* area = new PickResourceArea;
                    area->setLabel(texture_p->prop_caption);
                    area->go_property = static_cast<void*>(this);
                    area->rel_path = &texture_conf->texture_str[i];
                    area->isShowNoneItem = true;
                    area->resource_type = RESOURCE_TYPE_TEXTURE; //It should load textures only
                    inspector->addPropertyArea(area);
                }

                break;
            }
        }
    }
}
void MaterialProperty::onValueChanged(){
    Material* newmat_ptr = go_link.world_ptr->getMaterialPtrByName(material_path);

    //Check, if material file has changed
    if(newmat_ptr != this->material_ptr){
        this->material_ptr = newmat_ptr;
        //this->group_ptr = newmat_ptr->group_ptr;
        this->group_label = material_ptr->group_ptr->groupCaption;

        //update window
        _inspector_win->updateRequired = true;
    }

    if(material_ptr == nullptr) return;
    if(material_ptr->group_ptr == nullptr) { //if material has no MaterialShaderPropertyGroup
        //if user specified group first time
        if(MtShProps::getMtShaderPropertyGroupByLabel(this->group_label) != nullptr){
            //then apply that group
            material_ptr->setPropertyGroup(MtShProps::getMtShaderPropertyGroupByLabel(this->group_label));

        }else { //user haven't specified
            return; //go out
        }
    }else{ //Material already had group, check, if user decided to change it
        if(MtShProps::getMtShaderPropertyGroupByLabel(this->group_label) != this->material_ptr->group_ptr){
            //Apply changing
            this->material_ptr->setPropertyGroup(MtShProps::getMtShaderPropertyGroupByLabel(this->group_label));
           // this->group_label = material_ptr->group_ptr->groupCaption;
            _inspector_win->updateRequired = true;
        }
    }

    for(unsigned int prop_i = 0; prop_i < material_ptr->group_ptr->properties.size(); prop_i ++){
        MaterialShaderProperty* prop_ptr = material_ptr->group_ptr->properties[prop_i];
        MaterialShaderPropertyConf* conf_ptr = this->material_ptr->confs[prop_i];
        switch(prop_ptr->type){
            case MATSHPROP_TYPE_TEXTURE:{
                //Cast pointer
                TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);

                texture_conf->texture = go_link.world_ptr->getTexturePtrByRelPath(texture_conf->path);

                break;
            }
            case MATSHPROP_TYPE_TEXTURE3:{
                //Cast pointer
                Texture3MtShPropConf* texture_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);

                if(texture_conf->texture_count > 6)
                    texture_conf->texture_count = 6;
                texture_conf->rel_path = this->world_ptr->proj_ptr->root_path;
                texture_conf->texture3D->created = false;
                _inspector_win->updateRequired = true;

                break;
            }
        }
    }
    //save changes to material file
    material_ptr->saveToFile();
}

void MaterialProperty::copyTo(GameObjectProperty* dest){
    //MaterialShaderProperty
    if(dest->type != GO_PROPERTY_TYPE_MATERIAL) return;

    //Do base things
    GameObjectProperty::copyTo(dest);

    MaterialProperty* mat_prop = static_cast<MaterialProperty*>(dest);
    mat_prop->material_path = this->material_path;
    mat_prop->material_ptr = this->material_ptr;
    mat_prop->group_label = this->group_label;
}

void MaterialProperty::onAddToObject(){

}
/*
void ColliderProperty::onAddToObject(){
    this->go_link.world_ptr->pushCollider(this);
} //will register in world
void ColliderProperty::onObjectDeleted(){
    this->go_link.world_ptr->removeCollider(this);
} //unregister in world
*/
void ColliderProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    addColliderRadio(inspector);

    //isTrigger checkbox
    BoolCheckboxArea* istrigger = new BoolCheckboxArea;
    istrigger->setLabel("IsTrigger ");
    istrigger->go_property = static_cast<void*>(this);
    istrigger->bool_ptr = &this->isTrigger;
    inspector->addPropertyArea(istrigger);
}

void ColliderProperty::onUpdate(float deltaTime){
    if(!created)
        init();
}

void ColliderProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != GO_PROPERTY_TYPE_COLLIDER) return;

    ColliderProperty* coll_prop = static_cast<ColliderProperty*>(dest);
    coll_prop->isTrigger = this->isTrigger;
    coll_prop->coll_type = this->coll_type;
}

TransformProperty* ColliderProperty::getTransformProperty(){
    return go_link.updLinkPtr()->getTransformProperty();
}

ColliderProperty::ColliderProperty(){
    type = GO_PROPERTY_TYPE_COLLIDER;

    isTrigger = false;
    coll_type = COLLIDER_TYPE_CUBE;
    created = false;
    mass = 0.0f; //collider is static
}

RigidbodyProperty::RigidbodyProperty(){

    mass = 1.0f;
    created = false;
    type = GO_PROPERTY_TYPE_RIGIDBODY;
    coll_type = COLLIDER_TYPE_CUBE;

    gravity = ZSVECTOR3(0.f, -10.f, 0.f);
    linearVel = ZSVECTOR3(0.f, 0.f, 0.f);
}

bool GameObject::isRigidbody(){
    if(getPropertyPtrByType(GO_PROPERTY_TYPE_RIGIDBODY) != nullptr || getPropertyPtrByType(GO_PROPERTY_TYPE_COLLIDER) != nullptr)
        return true;

    return false;
}

void RigidbodyProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    PhysicalProperty::addColliderRadio(inspector);
    PhysicalProperty::addMassField(inspector);

    Float3PropertyArea* gravityE = new Float3PropertyArea; //New property area
    gravityE->setLabel("Gravity"); //Its label
    gravityE->vector = &this->gravity; //Ptr to our vector
    gravityE->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(gravityE);

    Float3PropertyArea* linearE = new Float3PropertyArea; //New property area
    linearE->setLabel("Linear"); //Its label
    linearE->vector = &this->linearVel; //Ptr to our vector
    linearE->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(linearE);
}

void RigidbodyProperty::onUpdate(float deltaTime){
    if(!created){
        //if uninitialized
        init();

        this->rigidBody->setGravity(btVector3(gravity.X, gravity.Y, gravity.Z));
        this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));
    }
    else{
        TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<TransformProperty>();
        btVector3 current_pos = rigidBody->getCenterOfMassPosition();
        btQuaternion current_rot = rigidBody->getWorldTransform().getRotation();
        //get current position
        float curX = current_pos.getX();
        float curY = current_pos.getY();
        float curZ = current_pos.getZ();

        float rotX = 0, rotY = 0, rotZ = 0;
        //Convert quaternion to euler
        current_rot.getEulerZYX(rotZ, rotY, rotX);
        //Convert radians to degrees
        rotX = rotX / ZS_PI * 180.0f;
        rotY = rotY / ZS_PI * 180.0f;
        rotZ = rotZ / ZS_PI * 180.0f;

        //if(transform->translation != ZSVECTOR3(curX, curY, curZ))
            transform->translation = ZSVECTOR3(curX, curY, curZ);
        //if(transform->_last_rotation != ZSVECTOR3(rotX, rotY, rotZ))
            transform->rotation = ZSVECTOR3(rotX, rotY, rotZ);
    }
}

void RigidbodyProperty::onValueChanged(){
    if(!created) return;
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        shape->calculateLocalInertia(mass, localInertia);
    //SET mass to bullet rigidbody
    this->rigidBody->setMassProps(mass, localInertia);
    this->rigidBody->setGravity(btVector3(gravity.X, gravity.Y, gravity.Z));
    this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));

    delete shape;
    updateCollisionShape();
    this->rigidBody->setCollisionShape(shape);
}

void RigidbodyProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != GO_PROPERTY_TYPE_RIGIDBODY) return;

    //Do base things
    GameObjectProperty::copyTo(dest);

    RigidbodyProperty* rigi_prop = static_cast<RigidbodyProperty*>(dest);
    rigi_prop->mass = this->mass;
    rigi_prop->gravity = this->gravity;
    rigi_prop->linearVel = this->linearVel;
    rigi_prop->coll_type = this->coll_type;
    rigi_prop->angularVel = this->angularVel;
}

void ScriptGroupProperty::onValueChanged(){
    Project* project_ptr = static_cast<Project*>(this->world_ptr->proj_ptr);
    //if size changed
    if(static_cast<int>(path_names.size()) != this->scr_num){
        path_names.resize(static_cast<unsigned int>(scr_num));
        //Update inspector interface
        _inspector_win->updateRequired = true;
    }

    if(static_cast<int>(scripts_attached.size()) != this->scr_num){ //if size changed
        this->scripts_attached.resize(static_cast<unsigned int>(this->scr_num));
        //Iterate over all scripts and use absolute path

    }
    for(unsigned int script_i = 0; script_i < static_cast<unsigned int>(scr_num); script_i ++){
        //Set absolute path to script object
        scripts_attached[script_i].fpath = project_ptr->root_path + "/" + path_names[script_i];
        scripts_attached[script_i].name = path_names[script_i].toStdString();
    }

}

void ScriptGroupProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){

    IntPropertyArea* scriptnum_area = new IntPropertyArea;
    scriptnum_area->setLabel("Scripts"); //Its label
    scriptnum_area->value = &this->scr_num;
    scriptnum_area->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(scriptnum_area);

    for(int script_i = 0; script_i < scr_num; script_i ++){
        PickResourceArea* area = new PickResourceArea;
        area->setLabel("Lua Script");
        area->go_property = static_cast<void*>(this);
        area->rel_path = &path_names[static_cast<unsigned int>(script_i)];
        area->extension_mask = ".lua";
        area->resource_type = RESOURCE_TYPE_FILE; //It should load meshes only
        inspector->addPropertyArea(area);
    }
}
void ScriptGroupProperty::onUpdate(float deltaTime){
    for(unsigned int script_i = 0; script_i < this->scripts_attached.size(); script_i ++){
        ObjectScript* script_ptr = &this->scripts_attached[script_i]; //Obtain pointer to script

        if(!script_ptr->created){
            script_ptr->_InitScript();
            script_ptr->_callStart(this->go_link.updLinkPtr(), go_link.world_ptr);
        }

        script_ptr->_callDraw(deltaTime); //Run onDraw() function in script
    }
}

void ScriptGroupProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't script group

    //Do base things
    GameObjectProperty::copyTo(dest);

    ScriptGroupProperty* _dest = static_cast<ScriptGroupProperty*>(dest);
    _dest->scr_num = this->scr_num;

    //resize data vectors
    _dest->scripts_attached.resize(static_cast<unsigned int>(scr_num));
    _dest->path_names.resize(static_cast<unsigned int>(scr_num));
    //Copy data
    for(unsigned int script_i = 0; script_i < static_cast<unsigned int>(scr_num); script_i ++){
        _dest->scripts_attached[script_i] = this->scripts_attached[script_i];
        _dest->path_names[script_i] = this->path_names[script_i];
    }
}

void ScriptGroupProperty::wakeUp(){

}

void ScriptGroupProperty::shutdown(){
    for(unsigned int script_i = 0; script_i < static_cast<unsigned int>(scr_num); script_i ++){
        this->scripts_attached[script_i]._DestroyScript();
    }
}

ObjectScript* ScriptGroupProperty::getScriptByName(std::string name){
    for(unsigned int script_i = 0; script_i < static_cast<unsigned int>(scr_num); script_i ++){
        if(!name.compare(scripts_attached[script_i].name))
            return &scripts_attached[script_i];
    }
    return nullptr;
}
ScriptGroupProperty::ScriptGroupProperty(){
    type = GO_PROPERTY_TYPE_SCRIPTGROUP;

    scr_num = 0;
    this->scripts_attached.resize(static_cast<unsigned int>(this->scr_num));
}


SkyboxProperty::SkyboxProperty(){
    type = GO_PROPERTY_TYPE_SKYBOX;
}

ShadowCasterProperty::ShadowCasterProperty(){
    type = GO_PROPERTY_TYPE_SHADOWCASTER;

    initialized = false;

    TextureWidth = 2048;
    TextureHeight = 2048;

    shadow_bias = 0.005f;

    nearPlane = 1.0f;
    farPlane = 75.0f;

    projection_viewport = 20;
}

void ShadowCasterProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){

    IntPropertyArea* textureW = new IntPropertyArea; //New property area
    textureW->setLabel("Texture Width"); //Its label
    textureW->value = &this->TextureWidth; //Ptr to our vector
    textureW->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    inspector->addPropertyArea(textureW);

    IntPropertyArea* textureH = new IntPropertyArea; //New property area
    textureH->setLabel("Texture Height"); //Its label
    textureH->value = &this->TextureHeight; //Ptr to our vector
    textureH->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    inspector->addPropertyArea(textureH);

    FloatPropertyArea* bias = new FloatPropertyArea; //New property area
    bias->setLabel("Shadow bias"); //Its label
    bias->value = &this->shadow_bias; //Ptr to our vector
    bias->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    inspector->addPropertyArea(bias);

    FloatPropertyArea* _nearPlane = new FloatPropertyArea; //New property area
    _nearPlane->setLabel("Near plane"); //Its label
    _nearPlane->value = &this->nearPlane; //Ptr to our vector
    _nearPlane->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    inspector->addPropertyArea(_nearPlane);
    FloatPropertyArea* _farPlane = new FloatPropertyArea; //New property area
    _farPlane->setLabel("Far plane"); //Its label
    _farPlane->value = &this->farPlane; //Ptr to our vector
    _farPlane->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    inspector->addPropertyArea(_farPlane);

    IntPropertyArea* _viewport = new IntPropertyArea; //New property area
    _viewport->setLabel("Shadow viewport"); //Its label
    _viewport->value = &this->projection_viewport; //Ptr to our vector
    _viewport->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    inspector->addPropertyArea(_viewport);
}

void ShadowCasterProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't script group

    //Do base things
    GameObjectProperty::copyTo(dest);

    ShadowCasterProperty* _dest = static_cast<ShadowCasterProperty*>(dest);
    _dest->farPlane = this->farPlane;
    _dest->nearPlane = this->nearPlane;
    _dest->shadow_bias = this->shadow_bias;
    _dest->TextureWidth = this->TextureWidth;
    _dest->TextureHeight = this->TextureHeight;
    _dest->projection_viewport = this->projection_viewport;
}

TerrainProperty::TerrainProperty(){
    type = GO_PROPERTY_TYPE_TERRAIN;

    this->Width = 500;
    this->Length = 500;
    this->MaxHeight = 500;
    castShadows = true;

    this->range = 15;
    this->editHeight = 10;
    this->textureid = 0;

    edit_mode = 1;

    hasChanged = false;
}

void onClearTerrain(){
    current_terrain_prop->getTerrainData()->alloc(current_terrain_prop->Width, current_terrain_prop->Length);
    current_terrain_prop->getTerrainData()->generateGLMesh();
}

void TerrainProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    current_terrain_prop = this;

    IntPropertyArea* HWidth = new IntPropertyArea; //New property area
    HWidth->setLabel("Heightmap Width"); //Its label
    HWidth->value = &this->Width; //Ptr to our vector
    HWidth->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    inspector->addPropertyArea(HWidth);

    IntPropertyArea* HLength = new IntPropertyArea; //New property area
    HLength->setLabel("Heightmap Length"); //Its label
    HLength->value = &this->Length; //Ptr to our vector
    HLength->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    inspector->addPropertyArea(HLength);

    IntPropertyArea* MHeight = new IntPropertyArea; //New property area
    MHeight->setLabel("Max Height"); //Its label
    MHeight->value = &this->MaxHeight; //Ptr to our vector
    MHeight->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    inspector->addPropertyArea(MHeight);

    BoolCheckboxArea* castShdws = new BoolCheckboxArea;
    castShdws->setLabel("Cast Shadows ");
    castShdws->go_property = static_cast<void*>(this);
    castShdws->bool_ptr = &this->castShadows;
    inspector->addPropertyArea(castShdws);

    //Add button to add objects
    AreaButton* clear_btn = new AreaButton;
    clear_btn->onPressFuncPtr = &onClearTerrain;
    clear_btn->button->setText("Clear"); //Setting text to qt button
    inspector->getContentLayout()->addWidget(clear_btn->button);
    clear_btn->insp_ptr = inspector; //Setting inspector pointer
    inspector->registerUiObject(clear_btn);

    AreaRadioGroup* group = new AreaRadioGroup; //allocate button layout
    group->value_ptr = reinterpret_cast<uint8_t*>(&this->edit_mode);
    group->go_property = static_cast<void*>(this);

    QRadioButton* directional_radio = new QRadioButton; //allocate first radio
    directional_radio->setText("Map");
    QRadioButton* point_radio = new QRadioButton;
    point_radio->setText("Texture");

    group->addRadioButton(directional_radio);
    group->addRadioButton(point_radio);
    inspector->registerUiObject(group);
    inspector->getContentLayout()->addLayout(group->btn_layout);
    if(edit_mode == 1){
        IntPropertyArea* EditRange = new IntPropertyArea; //New property area
        EditRange->setLabel("brush range"); //Its label
        EditRange->value = &this->range; //Ptr to our vector
        EditRange->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
        inspector->addPropertyArea(EditRange);

        FloatPropertyArea* EditHeight = new FloatPropertyArea; //New property area
        EditHeight->setLabel("brush height"); //Its label
        EditHeight->value = &this->editHeight; //Ptr to our vector
        EditHeight->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
        inspector->addPropertyArea(EditHeight);
    }
    if(edit_mode == 2){
        IntPropertyArea* EditRange = new IntPropertyArea; //New property area
        EditRange->setLabel("brush range"); //Its label
        EditRange->value = &this->range; //Ptr to our vector
        EditRange->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
        inspector->addPropertyArea(EditRange);

        IntPropertyArea* EditTex = new IntPropertyArea; //New property area
        EditTex->setLabel("Texture"); //Its label
        EditTex->value = &this->textureid; //Ptr to our vector
        EditTex->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
        inspector->addPropertyArea(EditTex);
    }

}

void TerrainProperty::onRender(RenderPipeline* pipeline){
    if(hasChanged){
        this->data.generateGLMesh();
        hasChanged = false;
    }

    MaterialProperty* mat = this->go_link.updLinkPtr()->getPropertyPtr<MaterialProperty>();
    if(mat == nullptr) return;
    //Apply material shader
    mat->onRender(pipeline);
}

void TerrainProperty::DrawMesh(){
    data.Draw();
}

void TerrainProperty::onValueChanged(){
    if(_last_edit_mode != edit_mode){
        _last_edit_mode = edit_mode;
        _inspector_win->updateRequired = true;
    }
}

void TerrainProperty::onAddToObject(){
    //relative path to terrain file
    this->file_label = *this->go_link.updLinkPtr()->label + ".terrain";
    //Allocate terrain
    data.alloc(this->Width, this->Length);
    //Generate opengl mesh to draw
    data.generateGLMesh();
    //absolute path to terrain file
    std::string fpath = this->go_link.world_ptr->proj_ptr->root_path.toStdString() + "/" + this->file_label.toStdString();
    //Create and save file
    data.saveToFile(fpath.c_str());
}

void TerrainProperty::onMouseClick(int posX, int posY, int screenY, bool isLeftButtonHold, bool isCtrlHold){
    if(isLeftButtonHold){
        unsigned char _data[4];
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //get pointer to material property
        MaterialProperty* mat = this->go_link.updLinkPtr()->getPropertyPtr<MaterialProperty>();
        if(mat == nullptr || mat->material_ptr == nullptr) return;
        //Apply material shader
        mat->material_ptr->group_ptr->render_shader->Use();
        mat->material_ptr->group_ptr->render_shader->setGLuniformInt("isPicking", 1);
        data.Draw();
        //read picked pixel
        glReadPixels(posX, screenY - posY, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, _data);
        mat->material_ptr->group_ptr->render_shader->setGLuniformInt("isPicking", 0);
        //find picked texel
        for(int i = 0; i < Width; i ++){
            for(int y = 0; y < Length; y ++){
                if(i == _data[0] * 2 && y == _data[2] * 2){
                    int mul = 1;
                    if(isCtrlHold)
                        mul *= -1;
                    //apply change
                    if(edit_mode == 1)
                        this->data.modifyHeight(y, i, editHeight, range, mul);
                    else
                        this->data.modifyTexture(i, y, range, static_cast<unsigned char>(textureid));
                    hasChanged = true;
                    return;
                }
            }
        }
    }
}

void TerrainProperty::onMouseMotion(int posX, int posY, int relX, int relY, int screenY, bool isLeftButtonHold, bool isCtrlHold){
    if(isLeftButtonHold){
        unsigned char _data[4];
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        MaterialProperty* mat = this->go_link.updLinkPtr()->getPropertyPtr<MaterialProperty>();
        if(mat == nullptr || mat->material_ptr == nullptr) return;
        //Apply material shader
        mat->material_ptr->group_ptr->render_shader->Use();
        mat->material_ptr->group_ptr->render_shader->setGLuniformInt("isPicking", 1);
        data.Draw();
        //read picked pixel
        glReadPixels(posX, screenY - posY, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, _data);
        mat->material_ptr->group_ptr->render_shader->setGLuniformInt("isPicking", 0);
        //find picked texel
        for(int i = 0; i < Width; i ++){
            for(int y = 0; y < Length; y ++){
                if(i == _data[0] * 2 && y == _data[2] * 2){
                    int mul = 1;
                    if(isCtrlHold)
                        mul *= -1;
                    //apply change
                    if(edit_mode == 1){
                       // this->data.modifyHeight(y, i, editHeight, range, mul);
                    }else
                        this->data.modifyTexture(i, y, range, static_cast<unsigned char>(textureid));
                    hasChanged = true;
                    return;
                }
            }
        }
    }
}

TerrainData* TerrainProperty::getTerrainData(){
    return &data;
}
