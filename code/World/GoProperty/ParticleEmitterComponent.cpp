#include "../../ProjEd/headers/ProjectEdit.h"
#include "../../ProjEd/headers/InspEditAreas.h"

#include <world/ObjectsComponents/ParticleEmitterComponent.hpp>

extern InspectorWin* _inspector_win;
extern EditWindow* _editor_win;
extern Project* project_ptr;
extern ZSGAME_DATA* game_data;

void onSimulationStart();
void onSimulationStop();

static Engine::ParticleEmitterComponent* current_particle_emitter;

void Engine::ParticleEmitterComponent::addPropertyInterfaceToInspector() {
    current_particle_emitter = this;
    //Shape radio
    {
        AreaRadioGroup* shape_pick = new AreaRadioGroup; //allocate button layout
        shape_pick->value_ptr = reinterpret_cast<uint8_t*>(&this->mShape);
        shape_pick->go_property = this;

        QRadioButton* sphere_radio = new QRadioButton; //allocate first radio
        sphere_radio->setText("Sphere");
        if (mShape == PE_SHAPE_SPHERE)
            sphere_radio->setChecked(true);

        QRadioButton* box_radio = new QRadioButton; //allocate second radio
        box_radio->setText("Box");
        if (mShape == PE_SHAPE_BOX)
            box_radio->setChecked(true);

        QRadioButton* cone_radio = new QRadioButton; //allocate second radio
        cone_radio->setText("Cone");
        if (mShape == PE_SHAPE_BOX)
            cone_radio->setChecked(true);

        //add created radio button
        shape_pick->addRadioButton(sphere_radio);
        shape_pick->addRadioButton(box_radio);
        shape_pick->addRadioButton(cone_radio);

        _inspector_win->registerUiObject(shape_pick);
        _inspector_win->getContentLayout()->addWidget(shape_pick->mRadioGroup);
    }

    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_MESH);
    area->setLabel("Mesh");
    area->go_property = this;
    area->pResultString = &mMeshResLabel;
    _inspector_win->addPropertyArea(area);

    PickResourceArea* mat_area = new PickResourceArea(RESOURCE_TYPE_MATERIAL);
    mat_area->setLabel("Material");
    mat_area->go_property = this;
    mat_area->pResultString = &mMaterialResLabel;
    _inspector_win->addPropertyArea(mat_area);

    FloatPropertyArea* _Duration = new FloatPropertyArea; //New property area
    _Duration->setLabel("Duration"); //Its label
    _Duration->value = &this->mDuration; //Ptr to our vector
    _Duration->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(_Duration);

    BoolCheckboxArea* _Looping = new BoolCheckboxArea;
    _Looping->setLabel("Looping");
    _Looping->go_property = this;
    _Looping->pResultBool = &this->mLooping;
    _inspector_win->addPropertyArea(_Looping);

    BoolCheckboxArea* _Prewarm = new BoolCheckboxArea;
    _Prewarm->setLabel("Prewarm");
    _Prewarm->go_property = this;
    _Prewarm->pResultBool = &this->mPrewarm;
    _inspector_win->addPropertyArea(_Prewarm);
    

    FloatPropertyArea* _Lifetime = new FloatPropertyArea; //New property area
    _Lifetime->setLabel("Lifetime"); //Its label
    _Lifetime->value = &this->mLifetime; //Ptr to our vector
    _Lifetime->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(_Lifetime);

    IntPropertyArea* _MaxParticles = new IntPropertyArea; //New property area
    _MaxParticles->setLabel("Max Particles"); //Its label
    _MaxParticles->value = &this->mMaxParticles; //Ptr to our vector
    _MaxParticles->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(_MaxParticles);

    IntPropertyArea* _MinParticlesPerSec = new IntPropertyArea; //New property area
    _MinParticlesPerSec->setLabel("Min Particles per sec"); //Its label
    _MinParticlesPerSec->value = &this->mEmissionRate.Min; //Ptr to our vector
    _MinParticlesPerSec->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(_MinParticlesPerSec);

    IntPropertyArea* _MaxParticlesPerSec = new IntPropertyArea; //New property area
    _MaxParticlesPerSec->setLabel("Max Particles per sec"); //Its label
    _MaxParticlesPerSec->value = &this->mEmissionRate.Max; //Ptr to our vector
    _MaxParticlesPerSec->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(_MaxParticlesPerSec);

    Float3PropertyArea* min_direction_area = new Float3PropertyArea;
    min_direction_area->setLabel("Min Direction"); //Its label
    min_direction_area->vector = &mDirection.Min;
    min_direction_area->go_property = this;
    _inspector_win->addPropertyArea(min_direction_area);

    Float3PropertyArea* max_direction_area = new Float3PropertyArea;
    max_direction_area->setLabel("Max Direction"); //Its label
    max_direction_area->vector = &mDirection.Max;
    max_direction_area->go_property = this;
    _inspector_win->addPropertyArea(max_direction_area);

    Float3PropertyArea* constant_force_area = new Float3PropertyArea;
    constant_force_area->setLabel("Constant Force"); //Its label
    constant_force_area->vector = &mConstantForce;
    constant_force_area->go_property = this;
    _inspector_win->addPropertyArea(constant_force_area);

    FloatPropertyArea* damping_force = new FloatPropertyArea; //New property area
    damping_force->setLabel("Damping Force"); //Its label
    damping_force->value = &this->mDampingForce; //Ptr to our vector
    damping_force->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(damping_force);

    Float2PropertyArea* min_size_area = new Float2PropertyArea;
    min_size_area->setLabel("Min Size"); //Its label
    min_size_area->vector = &mSize.OriginalValue.Min;
    min_size_area->go_property = this;
    _inspector_win->addPropertyArea(min_size_area);

    Float2PropertyArea* max_size_area = new Float2PropertyArea;
    max_size_area->setLabel("Max Size"); //Its label
    max_size_area->vector = &mSize.OriginalValue.Max;
    max_size_area->go_property = this;
    _inspector_win->addPropertyArea(max_size_area);

    FloatPropertyArea* SizeAdd = new FloatPropertyArea; //New property area
    SizeAdd->setLabel("Size Add"); //Its label
    SizeAdd->value = &this->mSize.Add; //Ptr to our vector
    SizeAdd->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(SizeAdd);

    FloatPropertyArea* SizeMul = new FloatPropertyArea; //New property area
    SizeMul->setLabel("Size Mul"); //Its label
    SizeMul->value = &this->mSize.Mul; //Ptr to our vector
    SizeMul->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(SizeMul);

    FloatPropertyArea* MinVelocity = new FloatPropertyArea; //New property area
    MinVelocity->setLabel("Min Velocity"); //Its label
    MinVelocity->value = &this->mVelocity.Min; //Ptr to our vector
    MinVelocity->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(MinVelocity);

    FloatPropertyArea* MaxVelocity = new FloatPropertyArea; //New property area
    MaxVelocity->setLabel("Max Velocity"); //Its label
    MaxVelocity->value = &this->mVelocity.Max; //Ptr to our vector
    MaxVelocity->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(MaxVelocity);

    FloatPropertyArea* MinRotation = new FloatPropertyArea; //New property area
    MinRotation->setLabel("Min Rotation"); //Its label
    MinRotation->value = &this->mRotation.Min; //Ptr to our vector
    MinRotation->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(MinRotation);

    FloatPropertyArea* MaxRotation = new FloatPropertyArea; //New property area
    MaxRotation->setLabel("Max Rotation"); //Its label
    MaxRotation->value = &this->mRotation.Max; //Ptr to our vector
    MaxRotation->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(MaxRotation);

    FloatPropertyArea* MinRotationSpeed = new FloatPropertyArea; //New property area
    MinRotationSpeed->setLabel("Min Rotation Speed"); //Its label
    MinRotationSpeed->value = &this->mRotationSpeed.Min; //Ptr to our vector
    MinRotationSpeed->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(MinRotationSpeed);

    FloatPropertyArea* MaxRotationSpeed = new FloatPropertyArea; //New property area
    MaxRotationSpeed->setLabel("Max Rotation Speed"); //Its label
    MaxRotationSpeed->value = &this->mRotationSpeed.Max; //Ptr to our vector
    MaxRotationSpeed->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(MaxRotationSpeed);

    if (mSimulating == false) {
        AreaButton* btn = new AreaButton;
        btn->onPressFuncPtr = &onSimulationStart;
        btn->button->setText("Start"); //Setting text to qt button
        _inspector_win->getContentLayout()->addWidget(btn->button);
        _inspector_win->registerUiObject(btn);
    }
    if (mSimulating == true) {
        AreaButton* stopbtn = new AreaButton;
        stopbtn->onPressFuncPtr = &onSimulationStop;
        stopbtn->button->setText("Stop"); //Setting text to qt button
        _inspector_win->getContentLayout()->addWidget(stopbtn->button);
        _inspector_win->registerUiObject(stopbtn);
    }
}

void onSimulationStart() {
    //Call play() on AnimationProperty
    current_particle_emitter->StartSimulation();
    //Refresh UI
    _inspector_win->updateRequired = true;
}

void onSimulationStop() {
    current_particle_emitter->StopSimulation();
    _inspector_win->updateRequired = true;
}

void Engine::ParticleEmitterComponent::onValueChanged() {
    updateMeshPtr();
}