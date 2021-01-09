#include "../headers/World.h"
#include "../../ProjEd/headers/ProjectEdit.h"
#include <render/Mesh.hpp>
#include "../../ProjEd/headers/InspEditAreas.h"
#include "../headers/Misc.h"

#include <world/ObjectsComponents/AudioSourceComponent.hpp>
#include <world/ObjectsComponents/AnimationComponent.hpp>
#include <world/ObjectsComponents/MeshComponent.hpp>
#include <world/ObjectsComponents/LightSourceComponent.hpp>
#include <world/ObjectsComponents/RigidbodyComponent.hpp>
#include <world/ObjectsComponents/ColliderComponent.hpp>
#include <world/ObjectsComponents/CharacterController.hpp>
#include <world/ObjectsComponents/TriggerComponent.hpp>
#include <world/ObjectsComponents/ShadowCasterComponent.hpp>
#include <world/ObjectsComponents/WindZoneComponent.hpp>

extern InspectorWin* _inspector_win;
extern EditWindow* _editor_win;
extern Project* project_ptr;
extern ZSGAME_DATA* game_data;
//Selected animation
static Engine::AnimationProperty* current_anim;

Engine::IGameObjectComponent* ObjectPropertyLink::updLinkPtr(){
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
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_WINDZONE: {
            return QString("Wind Zone");
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_PARTICLE_EMITTER: {
            return QString("Particle Emitter");
        }
    }
    return QString("NONE");
}

//Transform property functions
void Engine::TransformProperty::addPropertyInterfaceToInspector(){

    Float3PropertyArea* area_pos = new Float3PropertyArea; //New property area
    area_pos->setLabel("Position"); //Its label
    area_pos->vector = &this->translation; //Ptr to our vector
    area_pos->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(area_pos);

    Float3PropertyArea* area_scale = new Float3PropertyArea; //New property area
    area_scale->setLabel("Scale"); //Its label
    area_scale->vector = &this->scale; //Ptr to our vector
    area_scale->go_property = this;
    _inspector_win->addPropertyArea(area_scale);

    Float3PropertyArea* area_rotation = new Float3PropertyArea; //New property area
    area_rotation->setLabel("Rotation"); //Its label
    area_rotation->vector = &this->rotation; //Ptr to our vector
    area_rotation->go_property = this;
    _inspector_win->addPropertyArea(area_rotation);
}

//Label property functions
void Engine::LabelProperty::addPropertyInterfaceToInspector(){
    StringPropertyArea* area = new StringPropertyArea;
    area->setLabel("Label");
    area->value_ptr = &this->label;
    area->go_property = this;
    _inspector_win->addPropertyArea(area);
}

void Engine::LabelProperty::onValueChanged(){
    //lets chack if object already exist in world
    if(!mWorld->isObjectLabelUnique(this->label)){
        //If object already exist
        int label_add = 0;
        mWorld->getAvailableNumObjLabel(this->label, &label_add);
        label = label + "_" + std::to_string(label_add);
    }
}

//Mesh property functions
void Engine::MeshProperty::addPropertyInterfaceToInspector(){
    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_MESH);
    area->setLabel("Mesh");
    area->go_property = this;
    area->pResultString = &resource_relpath;
    _inspector_win->addPropertyArea(area);

    BoolCheckboxArea* IsCastShadows = new BoolCheckboxArea;
    IsCastShadows->setLabel("Cast shadows ");
    IsCastShadows->go_property = this;
    IsCastShadows->pResultBool = &this->castShadows;
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

void Engine::LightsourceComponent::addPropertyInterfaceToInspector(){
    AreaRadioGroup* group = new AreaRadioGroup; //allocate button layout
    group->value_ptr = reinterpret_cast<uint8_t*>(&this->light_type);
    group->go_property = this;
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
    intensity_area->go_property = this;
    _inspector_win->addPropertyArea(intensity_area);
    if(this->light_type > LIGHTSOURCE_TYPE::LIGHTSOURCE_TYPE_DIRECTIONAL){

        FloatPropertyArea* range_area = new FloatPropertyArea;
        range_area->setLabel("Range"); //Its range
        range_area->value = &this->range;
        range_area->go_property = this;
        _inspector_win->addPropertyArea(range_area);
        if(this->light_type == LIGHTSOURCE_TYPE::LIGHTSOURCE_TYPE_SPOT){
            FloatPropertyArea* spotangle_area = new FloatPropertyArea;
            spotangle_area->setLabel("Spot Angle"); //Its range
            spotangle_area->value = &this->spot_angle;
            spotangle_area->go_property = this;
            _inspector_win->addPropertyArea(spotangle_area);
        }
    }

    ColorDialogArea* lcolor = new ColorDialogArea;
    lcolor->setLabel("Light color");
    lcolor->color = &this->color;
    lcolor->go_property = this;
    _inspector_win->addPropertyArea(lcolor);
}

void Engine::AudioSourceProperty::addPropertyInterfaceToInspector(){
    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_AUDIO);
    area->setLabel("Sound");
    area->go_property = this;
    area->pResultString = &resource_relpath;
    _inspector_win->addPropertyArea(area);

    BoolCheckboxArea* isLooped = new BoolCheckboxArea;
    isLooped->setLabel("Loop ");
    isLooped->go_property = this;
    isLooped->pResultBool = &this->source.looped;
    _inspector_win->addPropertyArea(isLooped);

    FloatPropertyArea* gain_area = new FloatPropertyArea;
    gain_area->setLabel("Gain"); //Its label
    gain_area->value = &this->source.source_gain;
    gain_area->go_property = this;
    _inspector_win->addPropertyArea(gain_area);

    FloatPropertyArea* pitch_area = new FloatPropertyArea;
    pitch_area->setLabel("Pitch"); //Its label
    pitch_area->value = &this->source.source_pitch;
    pitch_area->go_property = this;
    _inspector_win->addPropertyArea(pitch_area);
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
    gravityE->go_property = this;
    _inspector_win->addPropertyArea(gravityE);

    Float3PropertyArea* linearE = new Float3PropertyArea; //New property area
    linearE->setLabel("Linear"); //Its label
    linearE->vector = &this->linearVel; //Ptr to our vector
    linearE->go_property = this;
    _inspector_win->addPropertyArea(linearE);
}

void Engine::CharacterControllerProperty::addPropertyInterfaceToInspector(){
    FloatPropertyArea* widthField = new FloatPropertyArea; //New property area
    widthField->setLabel("Width"); //Its label
    widthField->value = &this->width; //Ptr to our vector
    widthField->go_property = this;
    _inspector_win->addPropertyArea(widthField);

    FloatPropertyArea* heightField = new FloatPropertyArea; //New property area
    heightField->setLabel("Height"); //Its label
    heightField->value = &this->height; //Ptr to our vector
    heightField->go_property = this;
    _inspector_win->addPropertyArea(heightField);

    Float3PropertyArea* transformOffsetField = new Float3PropertyArea; //New property area
    transformOffsetField->setLabel("Transform Offset"); //Its label
    transformOffsetField->vector = &this->transform_offset; //Ptr to our vector
    transformOffsetField->go_property = this;
    _inspector_win->addPropertyArea(transformOffsetField);
}

void Engine::TriggerProperty::addPropertyInterfaceToInspector() {
    addColliderRadio();
    addCustomSizeField();
}

void Engine::ShadowCasterProperty::addPropertyInterfaceToInspector(){

    SpinBoxArea* cascadeSw = new SpinBoxArea;
    cascadeSw->setLabel("Cascades amount");
    cascadeSw->pResultInt = &this->mCascadesNum;
    cascadeSw->go_property = this;
    _inspector_win->addPropertyArea(cascadeSw);

    IntPropertyArea* textureW = new IntPropertyArea; //New property area
    textureW->setLabel("Texture Size"); //Its label
    textureW->value = &this->TextureSize; //Ptr to our vector
    textureW->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(textureW);

    FloatPropertyArea* bias = new FloatPropertyArea; //New property area
    bias->setLabel("Shadow bias"); //Its label
    bias->value = &this->mShadowBias; //Ptr to our vector
    bias->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(bias);

    IntPropertyArea* PcfNum = new IntPropertyArea; //New property area
    PcfNum->setLabel("PCF pass"); //Its label
    PcfNum->value = &this->mPcfNum; //Ptr to our vector
    PcfNum->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(PcfNum);

    FloatPropertyArea* ShadStrength = new FloatPropertyArea; //New property area
    ShadStrength->setLabel("Strength"); //Its label
    ShadStrength->value = &this->mShadowStrength; //Ptr to our vector
    ShadStrength->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(ShadStrength);

    FloatPropertyArea* _nearPlane = new FloatPropertyArea; //New property area
    _nearPlane->setLabel("Near plane"); //Its label
    _nearPlane->value = &this->nearPlane; //Ptr to our vector
    _nearPlane->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(_nearPlane);
    FloatPropertyArea* _farPlane = new FloatPropertyArea; //New property area
    _farPlane->setLabel("Far plane"); //Its label
    _farPlane->value = &this->farPlane; //Ptr to our vector
    _farPlane->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(_farPlane);

    IntPropertyArea* _viewport = new IntPropertyArea; //New property area
    _viewport->setLabel("Shadow viewport"); //Its label
    _viewport->value = &this->projection_viewport; //Ptr to our vector
    _viewport->go_property = this; //Pointer to this to activate matrix recalculaton
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
    area->go_property = this;
    area->pResultString = &this->anim_label;
    _inspector_win->addPropertyArea(area);

    if(Playing == false){
        AreaButton* btn = new AreaButton;
        btn->onPressFuncPtr = &onAnimPlay;
        btn->button->setText("Play"); //Setting text to qt button
        _inspector_win->getContentLayout()->addWidget(btn->button);
        _inspector_win->registerUiObject(btn);
    }
    if(Playing == true){
        AreaButton* stopbtn = new AreaButton;
        stopbtn->onPressFuncPtr = &onAnimStop;
        stopbtn->button->setText("Stop"); //Setting text to qt button
        _inspector_win->getContentLayout()->addWidget(stopbtn->button);
        _inspector_win->registerUiObject(stopbtn);
    }
}

void Engine::WindZoneComponent::addPropertyInterfaceToInspector() {
    FloatPropertyArea* _farPlane = new FloatPropertyArea; //New property area
    _farPlane->setLabel("Wind strength"); //Its label
    _farPlane->value = &this->mStrength; //Ptr to our vector
    _farPlane->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(_farPlane);
}