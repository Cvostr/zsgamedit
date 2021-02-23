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

    if (mLooping) {
        BoolCheckboxArea* _Prewarm = new BoolCheckboxArea;
        _Prewarm->setLabel("Prewarm");
        _Prewarm->go_property = this;
        _Prewarm->pResultBool = &this->mPrewarm;
        _inspector_win->addPropertyArea(_Prewarm);
    }

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