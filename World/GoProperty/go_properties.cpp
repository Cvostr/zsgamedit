#include "../headers/World.h"
#include "../headers/obj_properties.h"
#include "../headers/2dtileproperties.h"
#include "../../ProjEd/headers/ProjectEdit.h"
#include <render/zs-mesh.h>
#include "../../ProjEd/headers/InspEditAreas.h"
#include "../headers/Misc.h"

extern InspectorWin* _inspector_win;
extern EditWindow* _editor_win;
extern Project* project_ptr;
extern ZSGAME_DATA* game_data;
//selected terrain
static TerrainProperty* current_terrain_prop;
//Selected animation
static AnimationProperty* current_anim;

GameObjectProperty* ObjectPropertyLink::updLinkPtr(){
    ptr = this->object.updLinkPtr()->getPropertyPtrByType(this->prop_type);

    return ptr;
}
ObjectPropertyLink::ObjectPropertyLink(){

}

GameObjectProperty::GameObjectProperty(){
    type = GO_PROPERTY_TYPE_NONE;
    active = true; //Inactive by default
    world_ptr = nullptr; //World ponter is nullptr
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
    assert(pipeline);
}

void GameObjectProperty::onRender(RenderPipeline* pipeline){
    assert(pipeline);
}
void GameObjectProperty::onTrigger(GameObject* obj){
    assert(obj);
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
        case GO_PROPERTY_TYPE_NODE:{
            return QString("Node");
        }
        case GO_PROPERTY_TYPE_ANIMATION:{
            return QString("Animation");
        }
        case GO_PROPERTY_TYPE_CHARACTER_CONTROLLER:{
            return QString("Character Controller");
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
        case GO_PROPERTY_TYPE_NODE:{ //If type is transfrom
            _ptr = static_cast<GameObjectProperty*>(new NodeProperty); //Allocation of transform in heap
            break;
        }
        case GO_PROPERTY_TYPE_ANIMATION:{ //If type is transfrom
            _ptr = static_cast<GameObjectProperty*>(new AnimationProperty); //Allocation of transform in heap
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
        case GO_PROPERTY_TYPE_CHARACTER_CONTROLLER:{
            CharacterControllerProperty* ptr = new CharacterControllerProperty;
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
    assert(inspector);
    std::cout << "addPropertyInterfaceToInspector(" << inspector << ")";
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
    list_item_ptr = nullptr;
}

MeshProperty::MeshProperty(){
    type = GO_PROPERTY_TYPE_MESH;
    active = true;

    mesh_ptr = nullptr; //set it to 0x0 to check later
    this->resource_relpath = "@none";

    castShadows = true;

    this->skinning_root_node = nullptr;
    this->rootNodeStr = "@none";
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

    if(go_link.updLinkPtr() == nullptr) return;

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

        phys->shape->setLocalScaling(btVector3(btScalar(_last_scale.X),
                                               btScalar(_last_scale.Y),
                                               btScalar(_last_scale.Z)));

    }
}

void TransformProperty::onPreRender(RenderPipeline* pipeline){
    //assert(pipeline);
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

void TransformProperty::onRender(RenderPipeline* pipeline){
    //updateMat();
    //Send transform matrix to transform buffer
    pipeline->transformBuffer->bind();
    pipeline->transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &transform_mat);
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
    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_MESH);
    area->setLabel("Mesh");
    area->go_property = static_cast<void*>(this);
    area->rel_path_std = &resource_relpath;
    inspector->addPropertyArea(area);

    BoolCheckboxArea* IsCastShadows = new BoolCheckboxArea;
    IsCastShadows->setLabel("Cast shadows ");
    IsCastShadows->go_property = static_cast<void*>(this);
    IsCastShadows->bool_ptr = &this->castShadows;
    inspector->addPropertyArea(IsCastShadows);


    if(mesh_ptr){
        if(mesh_ptr->mesh_ptr->hasBones()){
            GameobjectPickArea* rtnode = new GameobjectPickArea;
            rtnode->gameobject_ptr_ptr = &this->skinning_root_node;
            rtnode->setLabel("Root Node");
            inspector->addPropertyArea(rtnode);
        }
    }
}
void MeshProperty::updateMeshPtr(){
    if(resource_relpath.length() < 1) return;
    this->mesh_ptr = game_data->resources->getMeshByLabel(resource_relpath);
}

void MeshProperty::onRender(RenderPipeline* pipeline){
    if(this->skinning_root_node == nullptr)
        skinning_root_node = world_ptr->getObjectByLabelStr(this->rootNodeStr);
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
    _dest->rootNodeStr = rootNodeStr;
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
    isPlaySheduled = false;

    this->source.source_gain = 1.0f;
    this->source.source_pitch = 1.0f;

    source.Init();
}

void AudioSourceProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_AUDIO);
    area->setLabel("Sound");
    area->go_property = static_cast<void*>(this);
    area->rel_path_std = &resource_relpath;
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
    this->buffer_ptr = game_data->resources->getAudioByLabel(resource_relpath);

    if(buffer_ptr == nullptr) return;

    this->source.setAlBuffer(this->buffer_ptr->buffer);
}

void AudioSourceProperty::onUpdate(float deltaTime){
    if(buffer_ptr->resource_state == STATE_LOADING_PROCESS)
        this->buffer_ptr->load();
    if(buffer_ptr->resource_state == STATE_LOADED && isPlaySheduled){
        audio_start();
        isPlaySheduled = false;
    }

    TransformProperty* transform = go_link.updLinkPtr()->getTransformProperty();
    //if object position changed
    if(transform->translation != this->last_pos){
        //apply new position to openal audio source
        this->source.setPosition(transform->translation);
        this->last_pos = transform->translation;
    }
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
    this->resource_relpath = relpath;
    this->updateAudioPtr();
}
void AudioSourceProperty::audio_start(){
    if(buffer_ptr->resource_state == STATE_LOADED){
        //Update source buffer
        if(buffer_ptr == nullptr) return;
        this->source.setAlBuffer(this->buffer_ptr->buffer);
        //play source
        this->source.play();

    }else {
        this->buffer_ptr->load();
        isPlaySheduled = true;
    }
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
    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_MATERIAL);
    area->setLabel("Material");
    area->go_property = static_cast<void*>(this);
    area->rel_path_std = &material_path;
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
    //Add shader group picker
    ComboBoxArea* mt_shader_group_area = new ComboBoxArea;
    mt_shader_group_area->setLabel("Shader Group");
    mt_shader_group_area->go_property = static_cast<void*>(this);
    mt_shader_group_area->result_string_std = &this->group_label;
    //Iterate over all available shader groups and add them to combo box
    for(unsigned int i = 0; i < MtShProps::getMaterialShaderPropertyAmount(); i ++){
        MtShaderPropertiesGroup* ptr = MtShProps::getMtShaderPropertiesGroupByIndex(i);
        mt_shader_group_area->widget.addItem(QString::fromStdString(ptr->groupCaption));
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

                PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
                area->setLabel(QString::fromStdString(texture_p->prop_caption));
                area->go_property = static_cast<void*>(this);
                area->rel_path_std = &texture_conf->path;
                area->isShowNoneItem = true;
                inspector->addPropertyArea(area);

                break;
            }
            case MATSHPROP_TYPE_FLOAT:{
                //Cast pointer
                FloatMaterialShaderProperty* float_p = static_cast<FloatMaterialShaderProperty*>(prop_ptr);
                FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);

                FloatPropertyArea* float_area = new FloatPropertyArea;
                float_area->setLabel(QString::fromStdString(float_p->prop_caption)); //Its label
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
                float3_area->setLabel(QString::fromStdString(float3_p->prop_caption)); //Its label
                float3_area->vector = &float3_conf->value;
                float3_area->go_property = static_cast<void*>(this);
                inspector->addPropertyArea(float3_area);

                break;
            }
            case MATSHPROP_TYPE_FVEC2:{
                break;
            }
            case MATSHPROP_TYPE_IVEC2:{
                break;
            }
            case MATSHPROP_TYPE_INTEGER:{
                //Cast pointer
                IntegerMaterialShaderProperty* integer_p = static_cast<IntegerMaterialShaderProperty*>(prop_ptr);
                IntegerMtShPropConf* integer_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);

                IntPropertyArea* integer_area = new IntPropertyArea;
                integer_area->setLabel(QString::fromStdString(integer_p->prop_caption)); //Its label
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
                color_area->setLabel(QString::fromStdString(color_p->prop_caption)); //Its label
                color_area->color = &color_conf->color;
                color_area->go_property = static_cast<void*>(this);
                inspector->addPropertyArea(color_area);

                break;
            }
            case MATSHPROP_TYPE_TEXTURE3:{
                //Cast pointer
                Texture3MtShPropConf* texture_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);

                QString captions[6] = {"Right", "Left", "Top", "Bottom", "Back", "Front"};

                for(int i = 0; i < 6; i ++){
                    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
                    area->setLabel(captions[i]);
                    area->go_property = static_cast<void*>(this);
                    area->rel_path_std = &texture_conf->texture_str[i];
                    area->isShowNoneItem = true;
                    inspector->addPropertyArea(area);
                }

                break;
            }
        }
    }
}
void MaterialProperty::onValueChanged(){

    Engine::MaterialResource* newmat_ptr_res = game_data->resources->getMaterialByLabel(this->material_path);
    Material* newmat_ptr = nullptr;
    if(newmat_ptr_res)
        newmat_ptr = newmat_ptr_res->material;

    //Check, if material file has changed
    if(newmat_ptr != this->material_ptr){
        this->material_ptr = newmat_ptr;
        this->group_label = newmat_ptr->group_ptr->groupCaption;
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
        //Get pointer to new selected shader group
        MtShaderPropertiesGroup* newgroup_ptr = MtShProps::getMtShaderPropertyGroupByLabel(this->group_label);
        //if new pointer and old aren't match, then change property group
        if(newgroup_ptr != this->material_ptr->group_ptr){
            //Apply changing
            this->material_ptr->setPropertyGroup(MtShProps::getMtShaderPropertyGroupByLabel(this->group_label));
            //Update material interface
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
                //Update pointer to texture resource
                texture_conf->texture = game_data->resources->getTextureByLabel(texture_conf->path);
                break;
            }
            case MATSHPROP_TYPE_TEXTURE3:{
                //Cast pointer
                Texture3MtShPropConf* texture_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);
                texture_conf->texture3D->created = false;
                _inspector_win->updateRequired = true;

                break;
            }
        }
    }
    //save changes to material file
    material_ptr->saveToFile();
    //Recreate thumbnails for all materials
    _editor_win->thumb_master->createMaterialThumbnails();
    //Update thumbnail in file list
    _editor_win->updateFileList();
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

void MaterialProperty::setMaterial(Material* mat){
    this->material_ptr = mat;
    this->material_path = mat->file_path;
    this->group_label = mat->group_ptr->groupCaption;
}

void MaterialProperty::setMaterial(std::string path){
    Material* newmat_ptr = game_data->resources->getMaterialByLabel(path)->material;
    setMaterial(newmat_ptr);
}

void MaterialProperty::onAddToObject(){

}

void ColliderProperty::onObjectDeleted(){
    if(created)
        this->go_link.world_ptr->physical_world->removeRidigbodyFromWorld(this->rigidBody);
} //unregister in world

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

    PhysicalProperty::copyTo(dest);

    ColliderProperty* coll_prop = static_cast<ColliderProperty*>(dest);
    coll_prop->isTrigger = this->isTrigger;
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
        PhysicalProperty::onUpdate(deltaTime);

        TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<TransformProperty>();
        btVector3 current_pos = rigidBody->getCenterOfMassPosition();
        btQuaternion current_rot = rigidBody->getWorldTransform().getRotation();
        //get current position
        float curX = current_pos.getX();
        float curY = current_pos.getY();
        float curZ = current_pos.getZ();

        float rotX = 0, rotY = 0, rotZ = 0;
        //Convert quaternion to euler
        current_rot.getEulerZYX(rotX, rotY, rotZ);
        //Convert radians to degrees
        rotX = rotX / ZS_PI * 180.0f;
        rotY = rotY / ZS_PI * 180.0f;
        rotZ = rotZ / ZS_PI * 180.0f;

        if(transform->translation != ZSVECTOR3(curX, curY, curZ))
            transform->translation = ZSVECTOR3(curX, curY, curZ);
        if(transform->rotation != ZSVECTOR3(rotZ, rotY, rotX))
            transform->rotation = ZSVECTOR3(rotZ, rotY, rotX);
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

void RigidbodyProperty::setLinearVelocity(ZSVECTOR3 lvel){
    if(!created) return;
    this->linearVel = lvel;
    this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));
}

void RigidbodyProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != GO_PROPERTY_TYPE_RIGIDBODY) return;

    //Do base things
    PhysicalProperty::copyTo(dest);

    RigidbodyProperty* rigi_prop = static_cast<RigidbodyProperty*>(dest);
    rigi_prop->gravity = this->gravity;
    rigi_prop->linearVel = this->linearVel;
    rigi_prop->angularVel = this->angularVel;
}

CharacterControllerProperty::CharacterControllerProperty(){
    type = GO_PROPERTY_TYPE_CHARACTER_CONTROLLER;
    created = false;

    gravity = ZSVECTOR3(0.f, -10.f, 0.f);
    linearVel = ZSVECTOR3(0.f, -10.f, 0.f);

    mass = 10;
}

void CharacterControllerProperty::setLinearVelocity(ZSVECTOR3 lvel){
    if(!created) return;
    this->linearVel = lvel;
    this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));
    rigidBody->activate();
}

void CharacterControllerProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    addCustomSizeField(inspector);
}
void CharacterControllerProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != GO_PROPERTY_TYPE_CHARACTER_CONTROLLER) return;

    //Do base things
    PhysicalProperty::copyTo(dest);

    CharacterControllerProperty* chara = static_cast<CharacterControllerProperty*>(dest);

}
void CharacterControllerProperty::onUpdate(float deltaTime){
    if(!created){
        TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<TransformProperty>();

        ZSVECTOR3 scale = transform->_last_scale;
        ZSVECTOR3 pos = transform->_last_translation;
        if(isCustomPhysicalSize){
            scale = cust_size;
        }

        //if uninitialized
        this->shape = new btCapsuleShape(scale.X, scale.Y);

        btVector3 localInertia(0, 0, 0);

        //shape->calculateLocalInertia(mass, localInertia);
        //Declare start transform
        btTransform startTransform;
        startTransform.setIdentity();
        //Set start transform
        startTransform.setOrigin(btVector3( btScalar(pos.X),
                                                    btScalar(pos.Y),
                                                    btScalar(pos.Z)));

        startTransform.setRotation(btQuaternion(transform->_last_rotation.X, transform->_last_rotation.Y, transform->_last_rotation.Z));

         //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
         btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

         btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

         rigidBody = new btRigidBody(cInfo);

         rigidBody->setUserIndex(go_link.updLinkPtr()->array_index);
         //add rigidbody to world
         go_link.world_ptr->physical_world->addRidigbodyToWorld(rigidBody);
        //Set zero values
        this->rigidBody->setGravity(btVector3(gravity.X, gravity.Y, gravity.Z));
        this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));

         created = true;
    }else{
        TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<TransformProperty>();
        btVector3 current_pos = rigidBody->getCenterOfMassPosition();

        //get current position
        float curX = current_pos.getX();
        float curY = current_pos.getY();
        float curZ = current_pos.getZ();

        if(transform->translation != ZSVECTOR3(curX, curY, curZ))
            transform->translation = ZSVECTOR3(curX, curY, curZ);

    }
}

void ScriptGroupProperty::onValueChanged(){
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
        Engine::ScriptResource* res = game_data->resources->getScriptByLabel(path_names[script_i]);
        scripts_attached[script_i].script_content = res->script_content;

        scripts_attached[script_i].name = path_names[script_i];
    }
}

void ScriptGroupProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){

    IntPropertyArea* scriptnum_area = new IntPropertyArea;
    scriptnum_area->setLabel("Scripts"); //Its label
    scriptnum_area->value = &this->scr_num;
    scriptnum_area->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(scriptnum_area);

    for(int script_i = 0; script_i < scr_num; script_i ++){
        PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_FILE);
        area->setLabel("Lua Script");
        area->go_property = static_cast<void*>(this);
        area->rel_path_std = &path_names[static_cast<unsigned int>(script_i)];
        area->extension_mask = ".lua";
        inspector->addPropertyArea(area);
    }
}
void ScriptGroupProperty::onUpdate(float deltaTime){
    for(unsigned int script_i = 0; script_i < this->scripts_attached.size(); script_i ++){
        ObjectScript* script_ptr = &this->scripts_attached[script_i]; //Obtain pointer to script
        //if script isn't created, then create it.
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

void ScriptGroupProperty::shutdown(){
    //Iterate over all scripts and call _DestroyScript() on each
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

void ShadowCasterProperty::onValueChanged(){
    //Update shadowmap texture
    setTextureSize();
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
    textures_size = 0;
    grassType_size = 0;

    this->range = 15;
    this->editHeight = 10;
    this->textureid = 1;
    this->vegetableid = 1;

    edit_mode = 1;

    rigidBody = nullptr;
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
    QRadioButton* veg_radio = new QRadioButton;
    veg_radio->setText("Vegetables");

    group->addRadioButton(directional_radio);
    group->addRadioButton(point_radio);
    group->addRadioButton(veg_radio);
    inspector->registerUiObject(group);
    inspector->getContentLayout()->addLayout(group->btn_layout);

    IntPropertyArea* EditRange = new IntPropertyArea; //New property area
    EditRange->setLabel("brush range"); //Its label
    EditRange->value = &this->range; //Ptr to our vector
    EditRange->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    inspector->addPropertyArea(EditRange);
    //If selected mode is Height paint
    if(edit_mode == 1){

        FloatPropertyArea* EditHeight = new FloatPropertyArea; //New property area
        EditHeight->setLabel("brush height"); //Its label
        EditHeight->value = &this->editHeight; //Ptr to our vector
        EditHeight->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
        inspector->addPropertyArea(EditHeight);
    }
    //if selected mode is texture paint
    if(edit_mode == 2){

        AreaRadioGroup* texturegroup_pick = new AreaRadioGroup; //allocate button layout
        texturegroup_pick->value_ptr = reinterpret_cast<uint8_t*>(&this->textureid);
        texturegroup_pick->go_property = static_cast<void*>(this);

        IntPropertyArea* tSize = new IntPropertyArea; //New property area
        tSize->setLabel("Textures"); //Its label
        tSize->value = &this->textures_size; //Ptr to our vector
        tSize->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
        inspector->addPropertyArea(tSize);

        for(int i = 0; i < this->textures_size; i ++){
            QRadioButton* group_radio = new QRadioButton; //allocate first radio
            group_radio->setText("Group " + QString::number(i));
            if(textureid == i + 1)
                group_radio->setChecked(true);
            //add created radio button
            texturegroup_pick->addRadioButton(group_radio);
            inspector->getContentLayout()->addWidget(group_radio);

            PickResourceArea* diffuse_area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
            diffuse_area->setLabel("Diffuse");
            diffuse_area->go_property = static_cast<void*>(this);
            diffuse_area->rel_path_std = &textures[static_cast<unsigned int>(i)].diffuse_relpath;
            inspector->addPropertyArea(diffuse_area);

            PickResourceArea* normal_area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
            normal_area->setLabel("Normal");
            normal_area->go_property = static_cast<void*>(this);
            normal_area->rel_path_std = &textures[static_cast<unsigned int>(i)].normal_relpath;
            inspector->addPropertyArea(normal_area);
        }
        //Add texture picker UI elements
        inspector->registerUiObject(texturegroup_pick);

    }
    //if selected mode is vegetable paint
    if(edit_mode == 3){

        AreaRadioGroup* vegetablegroup_pick = new AreaRadioGroup; //allocate button layout
        vegetablegroup_pick->value_ptr = reinterpret_cast<uint8_t*>(&this->vegetableid);
        vegetablegroup_pick->go_property = static_cast<void*>(this);

        IntPropertyArea* vSize = new IntPropertyArea; //New property area
        vSize->setLabel("Grass variants"); //Its label
        vSize->value = &this->grassType_size; //Ptr to our vector
        vSize->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
        inspector->addPropertyArea(vSize);

        for(int i = 0; i < this->grassType_size; i ++){
            QRadioButton* group_radio = new QRadioButton; //allocate first radio
            group_radio->setText("Veg " + QString::number(i));
            if(vegetableid == i)
                group_radio->setChecked(true);
            //add created radio button
            vegetablegroup_pick->addRadioButton(group_radio);
            inspector->getContentLayout()->addWidget(group_radio);

            PickResourceArea* diffuse_area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
            diffuse_area->setLabel("Diffuse");
            diffuse_area->go_property = static_cast<void*>(this);
            diffuse_area->rel_path_std = &this->grass[static_cast<unsigned int>(i)].diffuse_relpath;
            inspector->addPropertyArea(diffuse_area);

            Float2PropertyArea* GrassSize = new Float2PropertyArea; //New property area
            GrassSize->setLabel("Heightmap Length"); //Its label
            GrassSize->vector = &this->grass[static_cast<unsigned int>(i)].scale; //Ptr to our vector
            GrassSize->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
            inspector->addPropertyArea(GrassSize);

        }

        //Add texture picker UI elements
        inspector->registerUiObject(vegetablegroup_pick);
    }
}

void TerrainProperty::DrawMesh(RenderPipeline* pipeline){
    data.Draw(false);

    pipeline->grass_shader->Use();
    pipeline->transformBuffer->bind();
    for(int texelZ = 0; texelZ < data.W; texelZ ++){
        for(int texelX = 0; texelX < data.H; texelX ++){
            TransformProperty* t_ptr = this->go_link.updLinkPtr()->getTransformProperty();

            HeightmapTexel* texel_ptr = &this->data.data[texelZ * data.W + texelX];
            if(texel_ptr->grass > 0){
                HeightmapGrass* grass = &this->grass[texel_ptr->grass - 1];
                ZSVECTOR3 pos = t_ptr->translation + ZSVECTOR3(texelZ, texel_ptr->height, texelX);
                ZSMATRIX4x4 m = getScaleMat(ZSVECTOR3(grass->scale.X, grass->scale.Y, grass->scale.X)) * getTranslationMat(pos);

                grass->diffuse->Use(0);

             //
                pipeline->transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &m);
                Engine::getGrassMesh()->Draw();
            }
        }
    }
}

void TerrainProperty::onUpdate(float deltaTime){
    if(data.hasPhysicShapeChanged){
        TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<TransformProperty>();

        //Declare start transform
        btTransform startTransform;
        startTransform.setIdentity();
        //Set start transform
        startTransform.setOrigin(btVector3( btScalar(transform->_last_translation.X), btScalar(transform->_last_translation.Y),
                                                    btScalar(transform->_last_translation.Z)));


         //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
         btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

         btRigidBody::btRigidBodyConstructionInfo cInfo(0, myMotionState, data.shape, btVector3(0,0,0));

         if(rigidBody != nullptr){
             world_ptr->physical_world->removeRidigbodyFromWorld(rigidBody);
             delete rigidBody;
         }
         rigidBody = new btRigidBody(cInfo);

         rigidBody->setUserIndex(go_link.updLinkPtr()->array_index);
         //add rigidbody to world
         go_link.world_ptr->physical_world->addRidigbodyToWorld(rigidBody);

         data.hasPhysicShapeChanged = false;
    }
}

void TerrainProperty::onValueChanged(){
    //check for limitation of texture groups
    if(this->textures_size > 12)
        textures_size = 12;

    if(_last_edit_mode != edit_mode){
        _last_edit_mode = edit_mode;
        _inspector_win->updateRequired = true;
    }
    //if amount of texture pairs changed
    if(static_cast<unsigned int>(this->textures_size) != textures.size()){
        textures.resize(static_cast<unsigned int>(this->textures_size));
        _inspector_win->updateRequired = true;
    }
    //if amount of vegetables changed
    if(static_cast<unsigned int>(this->grassType_size) != grass.size()){
        grass.resize(static_cast<unsigned int>(this->grassType_size));
        _inspector_win->updateRequired = true;
    }
    for(unsigned int i = 0; i < static_cast<unsigned int>(this->textures_size); i ++){
        HeightmapTexturePair* pair = &this->textures[i];
        pair->diffuse = game_data->resources->getTextureByLabel(pair->diffuse_relpath);
        pair->normal = game_data->resources->getTextureByLabel(pair->normal_relpath);
    }
    for(unsigned int i = 0; i < static_cast<unsigned int>(this->grassType_size); i ++){
        HeightmapGrass* pair = &this->grass[i];
        pair->diffuse = game_data->resources->getTextureByLabel(pair->diffuse_relpath);
    }
}

void TerrainProperty::onAddToObject(){
    std::string terrain_random_prefix;
    genRandomString(&terrain_random_prefix, 4);

    //relative path to terrain file
    this->file_label = *this->go_link.updLinkPtr()->label + "_" + QString::fromStdString(terrain_random_prefix) + ".terrain";
    //Allocate terrain
    data.alloc(this->Width, this->Length);
    //Generate opengl mesh to draw
    data.generateGLMesh();
    //absolute path to terrain file
    std::string fpath = project_ptr->root_path + "/" + this->file_label.toStdString();
    //Create and save file
    data.saveToFile(fpath.c_str());
}

void TerrainProperty::getPickedVertexId(int posX, int posY, int screenX, int screenY, unsigned char* data){
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //get pointer to material property
    MaterialProperty* mat = this->go_link.updLinkPtr()->getPropertyPtr<MaterialProperty>();
    TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<TransformProperty>();
    if(mat == nullptr || mat->material_ptr == nullptr) return;
    //Apply material shader
    mat->material_ptr->applyMatToPipeline();
    //Bind terrain buffer and set isPicking to true
    terrainUniformBuffer->bind();
    int dtrue = 1;
    terrainUniformBuffer->writeData(16 * 12 * 2, 4, &dtrue);
    //Bind transform
    transformBuffer->bind();
    transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &transform->transform_mat);
    //Render terrain mesh without textures
    this->data.Draw(true);
    //read picked pixel
    glReadPixels(posX, screenY - posY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
}

void TerrainProperty::modifyTerrainVertex(unsigned char* gl_data, bool isCtrlHold){
    if((gl_data[0] + gl_data[1] + gl_data[2]) == 0) return;
    for(int i = 0; i < Width; i ++){
        for(int y = 0; y < Length; y ++){
            if(i == static_cast<int>(gl_data[0]) * 2 && y == static_cast<int>(gl_data[2]) * 2){
                int mul = 1;
                if(isCtrlHold)
                    mul *= -1;
                //apply change
                HeightmapModifyRequest* req = new HeightmapModifyRequest;
                req->terrain = &data;
                if(edit_mode == 1){
                    req->modify_type = TMT_HEIGHT;
                    req->originX = y;
                    req->originY = i;
                    req->originHeight = editHeight;
                    req->range = range;
                    req->multiplyer = mul;
                }else if(edit_mode == 2){
                    req->modify_type = TMT_TEXTURE;
                    req->originX = i;
                    req->originY = y;
                    req->range = range;
                    req->texture = static_cast<unsigned char>(textureid - 1);
                }else if(edit_mode == 3){
                    req->modify_type = TMT_GRASS;
                    req->originX = y;
                    req->originY = i;
                    req->range = range;
                    req->grass = vegetableid;
                }
                queryTerrainModifyRequest(req);
                return;
            }
        }
    }
}

void TerrainProperty::onMouseClick(int posX, int posY, int screenX, int screenY, bool isLeftButtonHold, bool isCtrlHold){
    if(isLeftButtonHold){
        unsigned char _data[4];
        getPickedVertexId(posX, posY, screenX, screenY, &_data[0]);
        //find picked texel
        modifyTerrainVertex(&_data[0], isCtrlHold);
    }
}

void TerrainProperty::onMouseMotion(int posX, int posY, int screenX, int screenY, bool isLeftButtonHold, bool isCtrlHold){
    if(isLeftButtonHold){
        unsigned char _data[4];
        getPickedVertexId(posX, posY, screenX, screenY, &_data[0]);
        //find picked texel
        modifyTerrainVertex(&_data[0], isCtrlHold);
    }
}

TerrainData* TerrainProperty::getTerrainData(){
    return &data;
}

void TerrainProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't script group

    //Do base things
    GameObjectProperty::copyTo(dest);

    TerrainProperty* _dest = static_cast<TerrainProperty*>(dest);
    _dest->Width = this->Width;
    _dest->Length = this->Length;
    _dest->MaxHeight = this->MaxHeight;
    _dest->file_label = this->file_label;
    _dest->castShadows = this->castShadows;
    _dest->textures_size = this->textures_size;
    _dest->grassType_size = this->grassType_size;
    //Copying terrain data
    data.copyTo(&_dest->data);
    //Copy textures data
    for(unsigned int t_i = 0; t_i < this->textures.size(); t_i ++)
        _dest->textures.push_back(textures[t_i]);

    for(unsigned int g_i = 0; g_i < this->grass.size(); g_i ++)
        _dest->grass.push_back(grass[g_i]);
}

NodeProperty::NodeProperty(){
    type = GO_PROPERTY_TYPE_NODE;

    scale = ZSVECTOR3(1.f, 1.f, 1.f);
    translation = ZSVECTOR3(0.f, 0.f, 0.f);
    rotation = ZSQUATERNION(0.f, 0.f, 0.f, 0.f);
}

void NodeProperty::onPreRender(RenderPipeline* pipeline){
}

void NodeProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){

}

void NodeProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't Node property

    //Do base things
    GameObjectProperty::copyTo(dest);
    NodeProperty* _dest = static_cast<NodeProperty*>(dest);

    _dest->transform_mat = this->transform_mat;
    _dest->node_label = this->node_label;
}

AnimationProperty::AnimationProperty(){
    type = GO_PROPERTY_TYPE_ANIMATION;
    this->anim_label = "@none";
    anim_prop_ptr = nullptr;
    Playing = false;
    start_sec = 0;
}

void onPlay(){
    //Call play() on AnimationProperty
    current_anim->play();
    //Refresh UI
    _inspector_win->updateRequired = true;
}

void onStop(){
    current_anim->stop();
    _inspector_win->updateRequired = true;
}

void AnimationProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    current_anim = this;

    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_ANIMATION);
    area->setLabel("Animation");
    area->go_property = static_cast<void*>(this);
    area->rel_path_std = &this->anim_label;
    inspector->addPropertyArea(area);

    if(Playing == false){
        AreaButton* btn = new AreaButton;
        btn->onPressFuncPtr = &onPlay;
        btn->button->setText("Play"); //Setting text to qt button
        inspector->getContentLayout()->addWidget(btn->button);
        btn->insp_ptr = inspector; //Setting inspector pointer
        inspector->registerUiObject(btn);
    }
    if(Playing == true){
        AreaButton* stopbtn = new AreaButton;
        stopbtn->onPressFuncPtr = &onStop;
        stopbtn->button->setText("Stop"); //Setting text to qt button
        inspector->getContentLayout()->addWidget(stopbtn->button);
        stopbtn->insp_ptr = inspector; //Setting inspector pointer
        inspector->registerUiObject(stopbtn);
    }
}

void AnimationProperty::play(){
    //if user specified animation, then play it!
    if(current_anim->anim_prop_ptr != nullptr){
        //Send animation to multithreaded loading, if need
        current_anim->anim_prop_ptr->load();

        current_anim->start_sec = (static_cast<double>(SDL_GetTicks()) / 1000);
        current_anim->Playing = true;
    }
}
void AnimationProperty::stop(){
    Playing = false;
}

void AnimationProperty::onPreRender(RenderPipeline* pipeline){
    GameObject* obj = go_link.updLinkPtr();

    Engine::Animation* anim_prop_ptr = nullptr;
    //Try to get loading result
    if(this->anim_prop_ptr != nullptr && Playing && this->anim_prop_ptr->resource_state == STATE_LOADING_PROCESS)
        current_anim->anim_prop_ptr->load();

    if(this->anim_prop_ptr != nullptr && Playing && this->anim_prop_ptr->resource_state == STATE_LOADED){
        if(this->anim_prop_ptr != nullptr)
                anim_prop_ptr = this->anim_prop_ptr->animation_ptr;

        //Calcualte current Time
        double curTime = (static_cast<double>(SDL_GetTicks()) / 1000) - this->start_sec;
        //Time in animation ticks
        double Ticks = anim_prop_ptr->TPS * curTime;
        //Calculate current animation time
        double animTime = fmod(Ticks, anim_prop_ptr->duration);

        for(unsigned int channels_i = 0; channels_i < anim_prop_ptr->NumChannels; channels_i ++){
            Engine::AnimationChannel* ch = &anim_prop_ptr->channels[channels_i];
            GameObject* node = obj->getChildObjectWithNodeLabel(ch->bone_name);
            NodeProperty* prop = node->getPropertyPtr<NodeProperty>();
            //Calculate interpolated values
            prop->translation = ch->getPostitionInterpolated(animTime);
            prop->scale = ch->getScaleInterpolated(animTime);
            prop->rotation = ch->getRotationInterpolated(animTime);
        }
    }
    ZSMATRIX4x4 identity_matrix = getIdentity();
    updateNodeTransform(obj, identity_matrix);
}

void AnimationProperty::updateNodeTransform(GameObject* obj, ZSMATRIX4x4 parent){

    if(!obj) return;
    NodeProperty* prop = obj->getPropertyPtr<NodeProperty>();
    if(!prop) return;
    //Assign base node transform
    prop->abs = prop->transform_mat;

    if(this->anim_prop_ptr != nullptr && Playing && this->anim_prop_ptr->resource_state == STATE_LOADED){
        Engine::AnimationChannel* cha = this->anim_prop_ptr->animation_ptr->getChannelByNodeName(prop->node_label);
        if(cha){
            ZSMATRIX4x4 transl = transpose(getTranslationMat(prop->translation));
            ZSMATRIX4x4 _sca = transpose(getScaleMat(prop->scale));
            ZSMATRIX4x4 rot = (getRotationMat(prop->rotation));
            //Multiply all matrices
            prop->abs = transl * rot * _sca;
        }
    }
    //Apply parent transform
    prop->abs = parent * prop->abs;

    for(unsigned int i = 0; i < obj->children.size(); i ++){
        updateNodeTransform(obj->children[i].updLinkPtr(), prop->abs);
    }

}

void AnimationProperty::copyTo(GameObjectProperty *dest){
    if(dest->type != this->type) return; //if it isn't animation

    //Do base things
    GameObjectProperty::copyTo(dest);
    AnimationProperty* _dest = static_cast<AnimationProperty*>(dest);

    _dest->anim_label = this->anim_label;
    _dest->anim_prop_ptr = this->anim_prop_ptr;
}

void AnimationProperty::onValueChanged(){
    updateAnimationPtr();
}

void AnimationProperty::setAnimation(std::string anim){
    this->anim_label = anim;
    updateAnimationPtr();
}

void AnimationProperty::updateAnimationPtr(){
    anim_prop_ptr = game_data->resources->getAnimationByLabel(this->anim_label);
}
