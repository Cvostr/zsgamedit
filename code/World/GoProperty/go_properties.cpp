#include "../headers/World.h"
#include "world/go_properties.h"
#include "../headers/2dtileproperties.h"
#include "../../ProjEd/headers/ProjectEdit.h"
#include <render/zs-mesh.h>
#include "../../ProjEd/headers/InspEditAreas.h"
#include "../headers/Misc.h"

extern InspectorWin* _inspector_win;
extern EditWindow* _editor_win;
extern Project* project_ptr;
extern ZSGAME_DATA* game_data;
//Selected animation
static Engine::AnimationProperty* current_anim;

bool LabelPropertyDeleteWidget = true;

void setLabelPropertyDeleteWidget(bool flag) {
    LabelPropertyDeleteWidget = flag;
}

Engine::GameObjectProperty* ObjectPropertyLink::updLinkPtr(){
    ptr = object.updLinkPtr()->getPropertyPtrByType(this->prop_type);

    return ptr;
}

ObjectPropertyLink::ObjectPropertyLink(){
    prop_type = PROPERTY_TYPE::GO_PROPERTY_TYPE_NONE;
    ptr = nullptr;
}

QString getPropertyString(PROPERTY_TYPE type){
    PROPERTY_TYPE _type = static_cast<PROPERTY_TYPE>(type);
    switch (_type) {
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM:{ //If type is transfrom
            return QString("Transform");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL:{
            return QString("Label");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH:{
            return QString("Mesh");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_LIGHTSOURCE:{
            return QString("Light");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT:{
            return QString("Angel Script");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_AUDSOURCE:{
            return QString("Audio Source");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL:{
            return QString("Material");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_COLLIDER:{
            return QString("Collider");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_RIGIDBODY:{
            return QString("Rigidbody");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_SHADOWCASTER:{
            return QString("Shadow Caster");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE_GROUP:{
            return QString("Tile Group");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE:{
            return QString("Tile");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_SKYBOX:{
            return QString("Skybox");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TERRAIN:{
            return QString("Terrain");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_NODE:{
            return QString("Node");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_ANIMATION:{
            return QString("Animation");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_CHARACTER_CONTROLLER:{
            return QString("Character Controller");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TRIGGER: {
            return QString("Trigger");
        }
    }
    return QString("NONE");
}

Engine::GameObjectProperty* _allocProperty(PROPERTY_TYPE type){
    Engine::GameObjectProperty* _ptr = nullptr;
    switch (type) {
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM:{ //If type is transfrom
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::TransformProperty); //Allocation of transform in heap
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_NODE:{ //If type is transfrom
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::NodeProperty); //Allocation of transform in heap
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_ANIMATION:{ //If type is transfrom
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::AnimationProperty); //Allocation of transform in heap
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL:{
            LabelProperty* ptr = new LabelProperty;
            _ptr = static_cast<Engine::GameObjectProperty*>(ptr);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::MeshProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_LIGHTSOURCE:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::LightsourceProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::ZPScriptProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_AUDSOURCE:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::AudioSourceProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::MaterialProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_COLLIDER:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::ColliderProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_RIGIDBODY:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::RigidbodyProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_SKYBOX:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::SkyboxProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_SHADOWCASTER:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::ShadowCasterProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TERRAIN:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::TerrainProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_CHARACTER_CONTROLLER:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::CharacterControllerProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TRIGGER: {
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::TriggerProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE_GROUP:{
            TileGroupProperty* ptr = new TileGroupProperty;
            _ptr = static_cast<Engine::GameObjectProperty*>(ptr);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::TileProperty);
            break;
        }
    }
    return _ptr;
}

LabelProperty::LabelProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL; //its an label
    active = true;
    list_item_ptr = nullptr;
}
LabelProperty::~LabelProperty() {

}
//Transform property functions
void Engine::TransformProperty::addPropertyInterfaceToInspector(){

    Float3PropertyArea* area_pos = new Float3PropertyArea; //New property area
    area_pos->setLabel("Position"); //Its label
    area_pos->vector = &this->translation; //Ptr to our vector
    area_pos->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(area_pos);

    Float3PropertyArea* area_scale = new Float3PropertyArea; //New property area
    area_scale->setLabel("Scale"); //Its label
    area_scale->vector = &this->scale; //Ptr to our vector
    area_scale->go_property = static_cast<void*>(this);
    _inspector_win->addPropertyArea(area_scale);

    Float3PropertyArea* area_rotation = new Float3PropertyArea; //New property area
    area_rotation->setLabel("Rotation"); //Its label
    area_rotation->vector = &this->rotation; //Ptr to our vector
    area_rotation->go_property = static_cast<void*>(this);
    _inspector_win->addPropertyArea(area_rotation);
}

//Label property functions
void LabelProperty::addPropertyInterfaceToInspector(){
    StringPropertyArea* area = new StringPropertyArea;
    area->setLabel("Label");
    area->value_ptr = &this->label;
    area->go_property = static_cast<void*>(this);
    _inspector_win->addPropertyArea(area);
}

void LabelProperty::onObjectDeleted() {
    if(LabelPropertyDeleteWidget)
        delete list_item_ptr;
}

void LabelProperty::onValueChanged(){
    World* world_ptr = static_cast<World*>(this->world_ptr); //Obtain pointer to world object
    //lets chack if object already exist in world
    if(!world_ptr->isObjectLabelUnique(this->label)){
        //If object already exist
        int label_add = 0;
        world_ptr->getAvailableNumObjLabel(this->label, &label_add);
        label = label + "_" + std::to_string(label_add);
    }
    //Update Object Tree value
    this->list_item_ptr->setText(0, QString::fromStdString(this->label));
}

void LabelProperty::copyTo(Engine::GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't label

    //Do base things
    GameObjectProperty::copyTo(dest);

    LabelProperty* _dest = static_cast<LabelProperty*>(dest);
    _dest->label = label;
}

void LabelProperty::loadPropertyFromMemory(const char* data, Engine::GameObject* obj) {
    unsigned int offset = 1;
    std::string label;
    while (data[offset] != ' ' && data[offset] != '\n') {
        label += data[offset];
        offset++;
    }
    this->label = label; //Write loaded string
    obj->label_ptr = &this->label; //Making GameObjects's pointer to string in label property
    list_item_ptr->setText(0, QString::fromStdString(label)); //Set text on widget
}

//Mesh property functions
void Engine::MeshProperty::addPropertyInterfaceToInspector(){
    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_MESH);
    area->setLabel("Mesh");
    area->go_property = static_cast<void*>(this);
    area->rel_path_std = &resource_relpath;
    _inspector_win->addPropertyArea(area);

    BoolCheckboxArea* IsCastShadows = new BoolCheckboxArea;
    IsCastShadows->setLabel("Cast shadows ");
    IsCastShadows->go_property = static_cast<void*>(this);
    IsCastShadows->bool_ptr = &this->castShadows;
    _inspector_win->addPropertyArea(IsCastShadows);

    //Check, if mesh is assigned
    if(mesh_ptr){
        //Check, if mesh has bones
        if(mesh_ptr->mesh_ptr->hasBones()){
            GameobjectPickArea* rtnode = new GameobjectPickArea;
            rtnode->gameobject_ptr_ptr = &this->skinning_root_node;
            rtnode->setLabel("Root Node");
            _inspector_win->addPropertyArea(rtnode);
        }
    }
}

void Engine::LightsourceProperty::addPropertyInterfaceToInspector(){
    AreaRadioGroup* group = new AreaRadioGroup; //allocate button layout
    group->value_ptr = reinterpret_cast<uint8_t*>(&this->light_type);
    group->go_property = static_cast<void*>(this);
    group->updateInspectorOnChange = true;

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
    _inspector_win->registerUiObject(group);
    _inspector_win->getContentLayout()->addLayout(group->btn_layout);

    FloatPropertyArea* intensity_area = new FloatPropertyArea;
    intensity_area->setLabel("Intensity"); //Its intensity
    intensity_area->value = &this->intensity;
    intensity_area->go_property = static_cast<void*>(this);
    _inspector_win->addPropertyArea(intensity_area);
    if(this->light_type > LIGHTSOURCE_TYPE::LIGHTSOURCE_TYPE_DIRECTIONAL){

        FloatPropertyArea* range_area = new FloatPropertyArea;
        range_area->setLabel("Range"); //Its range
        range_area->value = &this->range;
        range_area->go_property = static_cast<void*>(this);
        _inspector_win->addPropertyArea(range_area);
        if(this->light_type == LIGHTSOURCE_TYPE::LIGHTSOURCE_TYPE_SPOT){
            FloatPropertyArea* spotangle_area = new FloatPropertyArea;
            spotangle_area->setLabel("Spot Angle"); //Its range
            spotangle_area->value = &this->spot_angle;
            spotangle_area->go_property = static_cast<void*>(this);
            _inspector_win->addPropertyArea(spotangle_area);
        }
    }

    ColorDialogArea* lcolor = new ColorDialogArea;
    lcolor->setLabel("Light color");
    lcolor->color = &this->color;
    lcolor->go_property = static_cast<void*>(this);
    _inspector_win->addPropertyArea(lcolor);
}

void Engine::AudioSourceProperty::addPropertyInterfaceToInspector(){
    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_AUDIO);
    area->setLabel("Sound");
    area->go_property = static_cast<void*>(this);
    area->rel_path_std = &resource_relpath;
    _inspector_win->addPropertyArea(area);

    BoolCheckboxArea* isLooped = new BoolCheckboxArea;
    isLooped->setLabel("Loop ");
    isLooped->go_property = static_cast<void*>(this);
    isLooped->bool_ptr = &this->source.looped;
    _inspector_win->addPropertyArea(isLooped);

    FloatPropertyArea* gain_area = new FloatPropertyArea;
    gain_area->setLabel("Gain"); //Its label
    gain_area->value = &this->source.source_gain;
    gain_area->go_property = static_cast<void*>(this);
    _inspector_win->addPropertyArea(gain_area);

    FloatPropertyArea* pitch_area = new FloatPropertyArea;
    pitch_area->setLabel("Pitch"); //Its label
    pitch_area->value = &this->source.source_pitch;
    pitch_area->go_property = static_cast<void*>(this);
    _inspector_win->addPropertyArea(pitch_area);
}

void Engine::MaterialProperty::addPropertyInterfaceToInspector(){
    //Add area to pick material file
    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_MATERIAL);
    area->setLabel("Material");
    area->go_property = static_cast<void*>(this);
    area->rel_path_std = &material_path;
    _inspector_win->addPropertyArea(area);
    //No material, exiting
    if(material_ptr == nullptr || material_ptr->file_path[0] == '@')
        return;

    if(material_ptr->group_ptr->acceptShadows){
        BoolCheckboxArea* receiveShdws = new BoolCheckboxArea;
        receiveShdws->setLabel("Receive Shadows ");
        receiveShdws->go_property = static_cast<void*>(this);
        receiveShdws->bool_ptr = &this->receiveShadows;
        _inspector_win->addPropertyArea(receiveShdws);
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
    _inspector_win->addPropertyArea(mt_shader_group_area);

    //if material isn't set up, exiting
    if(material_ptr->group_ptr == nullptr)
        return;
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
                _inspector_win->addPropertyArea(area);

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
                _inspector_win->addPropertyArea(float_area);

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
                _inspector_win->addPropertyArea(float3_area);

                break;
            }
            case MATSHPROP_TYPE_FVEC2:{
                //Cast pointer
                Float2MaterialShaderProperty* float2_p = static_cast<Float2MaterialShaderProperty*>(prop_ptr);
                Float2MtShPropConf* float2_conf = static_cast<Float2MtShPropConf*>(conf_ptr);

                Float2PropertyArea* float2_area = new Float2PropertyArea;
                float2_area->setLabel(QString::fromStdString(float2_p->prop_caption)); //Its label
                float2_area->vector = &float2_conf->value;
                float2_area->go_property = static_cast<void*>(this);
                _inspector_win->addPropertyArea(float2_area);
                break;
            }
            case MATSHPROP_TYPE_IVEC2:{
                //Cast pointer
                Int2MaterialShaderProperty* int2_p = static_cast<Int2MaterialShaderProperty*>(prop_ptr);
                Int2MtShPropConf* int2_conf = static_cast<Int2MtShPropConf*>(conf_ptr);

                Int2PropertyArea* int2_area = new Int2PropertyArea;
                int2_area->setLabel(QString::fromStdString(int2_p->prop_caption)); //Its label
                int2_area->vector = &int2_conf->value[0];
                int2_area->go_property = static_cast<void*>(this);
                _inspector_win->addPropertyArea(int2_area);
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
                _inspector_win->addPropertyArea(integer_area);

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
                _inspector_win->addPropertyArea(color_area);

                break;
            }
            case MATSHPROP_TYPE_TEXTURE3:{
                //Cast pointer
                Texture3MtShPropConf* texture_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);

                QString captions[6] = {"Right", "Left", "Top", "Bottom", "Back", "Front"};
                //Draw six texture pickers to pick texture in each side
                for(int i = 0; i < 6; i ++){
                    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
                    area->setLabel(captions[i]);
                    area->go_property = static_cast<void*>(this);
                    area->rel_path_std = &texture_conf->texture_str[i];
                    area->isShowNoneItem = true;
                    _inspector_win->addPropertyArea(area);
                }

                break;
            }
        }
    }
}
void Engine::MaterialProperty::onValueChanged(){

    Engine::MaterialResource* newmat_ptr_res = game_data->resources->getMaterialByLabel(this->material_path);
    Material* newmat_ptr = nullptr;
    if(newmat_ptr_res)
        newmat_ptr = newmat_ptr_res->material;

    //User changed material
    bool isMaterialChanged = newmat_ptr != this->material_ptr;

    //Check, if material file has changed
    if(isMaterialChanged){
        this->material_ptr = newmat_ptr;
        this->group_label = newmat_ptr->group_ptr->groupCaption;
        //update window
        _inspector_win->updateRequired = true;
    }

    bool hasMaterial = material_ptr != nullptr;
    bool hasMaterialGroup = material_ptr->group_ptr != nullptr;

    if(!hasMaterial) return;
    if(!hasMaterialGroup) { //if material has no MaterialShaderPropertyGroup
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

    unsigned int GroupPropertiesSize = static_cast<unsigned int>(material_ptr->group_ptr->properties.size());
    for(unsigned int prop_i = 0; prop_i < GroupPropertiesSize; prop_i ++){
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

void Engine::ColliderProperty::addPropertyInterfaceToInspector(){
    addColliderRadio();

    //Show custom size field
    addCustomSizeField();
}
void Engine::RigidbodyProperty::addPropertyInterfaceToInspector(){
    PhysicalProperty::addColliderRadio();
    PhysicalProperty::addMassField();

    Float3PropertyArea* gravityE = new Float3PropertyArea; //New property area
    gravityE->setLabel("Gravity"); //Its label
    gravityE->vector = &this->gravity; //Ptr to our vector
    gravityE->go_property = static_cast<void*>(this);
    _inspector_win->addPropertyArea(gravityE);

    Float3PropertyArea* linearE = new Float3PropertyArea; //New property area
    linearE->setLabel("Linear"); //Its label
    linearE->vector = &this->linearVel; //Ptr to our vector
    linearE->go_property = static_cast<void*>(this);
    _inspector_win->addPropertyArea(linearE);
}

void Engine::CharacterControllerProperty::addPropertyInterfaceToInspector(){
    FloatPropertyArea* widthField = new FloatPropertyArea; //New property area
    widthField->setLabel("Width"); //Its label
    widthField->value = &this->width; //Ptr to our vector
    widthField->go_property = static_cast<void*>(this);
    _inspector_win->addPropertyArea(widthField);

    FloatPropertyArea* heightField = new FloatPropertyArea; //New property area
    heightField->setLabel("Height"); //Its label
    heightField->value = &this->height; //Ptr to our vector
    heightField->go_property = static_cast<void*>(this);
    _inspector_win->addPropertyArea(heightField);

    Float3PropertyArea* transformOffsetField = new Float3PropertyArea; //New property area
    transformOffsetField->setLabel("Transform Offset"); //Its label
    transformOffsetField->vector = &this->transform_offset; //Ptr to our vector
    transformOffsetField->go_property = static_cast<void*>(this);
    _inspector_win->addPropertyArea(transformOffsetField);
}

void Engine::TriggerProperty::addPropertyInterfaceToInspector() {
    addColliderRadio();
    addCustomSizeField();
}

void Engine::ZPScriptProperty::onValueChanged() {
    std::string old_res_name;
    if (script_res != nullptr)
        old_res_name = script_res->resource_label;
    //update resource pointer
    script_res = game_data->resources->getScriptByLabel(script_path);
    script = new AGScript(game_data->script_manager, go_link.updLinkPtr(), "angel");
    //Compare strings
    if (old_res_name.compare(script_path) != 0) {
        //Strings differ
        makeGlobalVarsList();
        //update interface
        _inspector_win->updateRequired = true;
    }
}

void Engine::ZPScriptProperty::addPropertyInterfaceToInspector(){
   
    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_SCRIPT);
    area->setLabel("Angel Script");
    area->go_property = static_cast<void*>(this);
    area->rel_path_std = &script_path;
    _inspector_win->addPropertyArea(area);

    for (unsigned int var_i = 0; var_i < this->vars.size(); var_i++) {
        GlobVarHandle* handle = vars[var_i];
        if (handle->typeID == asTYPEID_INT32) {
            IntPropertyArea* intH = new IntPropertyArea; //New property area
            intH->setLabel(QString::fromStdString(handle->name)); //Its label
            intH->value = handle->getValue<int>(); //Ptr to our vector
            intH->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
            _inspector_win->addPropertyArea(intH);
        }
        if (handle->typeID == asTYPEID_FLOAT) {
            FloatPropertyArea* floatH = new FloatPropertyArea; //New property area
            floatH->setLabel(QString::fromStdString(handle->name)); //Its label
            floatH->value = handle->getValue<float>(); //Ptr to our vector
            floatH->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
            _inspector_win->addPropertyArea(floatH);
        }
        if (handle->typeID == asTYPEID_BOOL) {
            BoolCheckboxArea* boolH = new BoolCheckboxArea; //New property area
            boolH->setLabel(QString::fromStdString(handle->name)); //Its label
            boolH->bool_ptr = handle->getValue<bool>(); //Ptr to our vector
            boolH->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
            _inspector_win->addPropertyArea(boolH);
        }
        if (handle->typeID == AG_VECTOR3) {
            Float3PropertyArea* vec3H = new Float3PropertyArea; //New property area
            vec3H->setLabel(QString::fromStdString(handle->name)); //Its label
            vec3H->vector = handle->getValue<ZSVECTOR3>(); //Ptr to our vector
            vec3H->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
            _inspector_win->addPropertyArea(vec3H);
        }
        if (handle->typeID == AG_STRING) {
            StringPropertyArea* area = new StringPropertyArea;
            area->setLabel(QString::fromStdString(handle->name));
            area->value_ptr = handle->getValue<std::string>();
            area->go_property = static_cast<void*>(this);
            _inspector_win->addPropertyArea(area);
        }
    }
}

void Engine::ShadowCasterProperty::addPropertyInterfaceToInspector(){

    IntPropertyArea* textureW = new IntPropertyArea; //New property area
    textureW->setLabel("Texture Width"); //Its label
    textureW->value = &this->TextureWidth; //Ptr to our vector
    textureW->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(textureW);

    IntPropertyArea* textureH = new IntPropertyArea; //New property area
    textureH->setLabel("Texture Height"); //Its label
    textureH->value = &this->TextureHeight; //Ptr to our vector
    textureH->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(textureH);

    FloatPropertyArea* bias = new FloatPropertyArea; //New property area
    bias->setLabel("Shadow bias"); //Its label
    bias->value = &this->shadow_bias; //Ptr to our vector
    bias->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(bias);

    FloatPropertyArea* _nearPlane = new FloatPropertyArea; //New property area
    _nearPlane->setLabel("Near plane"); //Its label
    _nearPlane->value = &this->nearPlane; //Ptr to our vector
    _nearPlane->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(_nearPlane);
    FloatPropertyArea* _farPlane = new FloatPropertyArea; //New property area
    _farPlane->setLabel("Far plane"); //Its label
    _farPlane->value = &this->farPlane; //Ptr to our vector
    _farPlane->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(_farPlane);

    IntPropertyArea* _viewport = new IntPropertyArea; //New property area
    _viewport->setLabel("Shadow viewport"); //Its label
    _viewport->value = &this->projection_viewport; //Ptr to our vector
    _viewport->go_property = static_cast<void*>(this); //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(_viewport);
}

void onAnimPlay(){
    //Call play() on AnimationProperty
    current_anim->play();
    //Refresh UI
    _inspector_win->updateRequired = true;
}

void onAnimStop(){
    current_anim->stop();
    _inspector_win->updateRequired = true;
}

void Engine::AnimationProperty::addPropertyInterfaceToInspector(){
    current_anim = this;

    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_ANIMATION);
    area->setLabel("Animation");
    area->go_property = static_cast<void*>(this);
    area->rel_path_std = &this->anim_label;
    _inspector_win->addPropertyArea(area);

    if(Playing == false){
        AreaButton* btn = new AreaButton;
        btn->onPressFuncPtr = &onAnimPlay;
        btn->button->setText("Play"); //Setting text to qt button
        _inspector_win->getContentLayout()->addWidget(btn->button);
        btn->insp_ptr = _inspector_win; //Setting inspector pointer
        _inspector_win->registerUiObject(btn);
    }
    if(Playing == true){
        AreaButton* stopbtn = new AreaButton;
        stopbtn->onPressFuncPtr = &onAnimStop;
        stopbtn->button->setText("Stop"); //Setting text to qt button
        _inspector_win->getContentLayout()->addWidget(stopbtn->button);
        stopbtn->insp_ptr = _inspector_win; //Setting inspector pointer
        _inspector_win->registerUiObject(stopbtn);
    }
}
