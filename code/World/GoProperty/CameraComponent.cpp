#include "../../ProjEd/headers/ProjectEdit.h"
#include "../../ProjEd/headers/InspEditAreas.h"

#include <world/ObjectsComponents/CameraComponent.hpp>

extern InspectorWin* _inspector_win;
extern EditWindow* _editor_win;
extern Project* project_ptr;
extern ZSGAME_DATA* game_data;

void Engine::CameraComponent::addPropertyInterfaceToInspector() {

    AreaRadioGroup* projection_pick = new AreaRadioGroup; //allocate button layout
    projection_pick->value_ptr = reinterpret_cast<uint8_t*>(&this->mProjectionType);
    projection_pick->go_property = this;

    QRadioButton* ortho_radio = new QRadioButton; //allocate first radio
    ortho_radio->setText("Ortho");
    if (mProjectionType == ZSCAMERA_PROJECTION_ORTHOGONAL)
        ortho_radio->setChecked(true);

    QRadioButton* persp_radio = new QRadioButton; //allocate second radio
    persp_radio->setText("Perspective");
    if (mProjectionType == ZSCAMERA_PROJECTION_PERSPECTIVE)
        persp_radio->setChecked(true);

    //add created radio button
    projection_pick->addRadioButton(ortho_radio);
    projection_pick->addRadioButton(persp_radio);

    _inspector_win->registerUiObject(projection_pick);
    _inspector_win->getContentLayout()->addWidget(projection_pick->mRadioGroup);

    FloatPropertyArea* Near = new FloatPropertyArea; //New property area
    Near->setLabel("Near Z"); //Its label
    Near->value = &this->mNearZ; //Ptr to our vector
    Near->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(Near);

    FloatPropertyArea* Far = new FloatPropertyArea; //New property area
    Far->setLabel("Far Z"); //Its label
    Far->value = &this->mFarZ; //Ptr to our vector
    Far->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(Far);

    FloatPropertyArea* _FOV = new FloatPropertyArea; //New property area
    _FOV->setLabel("FOV"); //Its label
    _FOV->value = &this->mFOV; //Ptr to our vector
    _FOV->go_property = this; //Pointer to this to activate matrix recalculaton
    _inspector_win->addPropertyArea(_FOV);


    AreaRadioGroup* cullface_pick = new AreaRadioGroup; //allocate button layout
    cullface_pick->value_ptr = reinterpret_cast<uint8_t*>(&this->mCullFaceDirection);
    cullface_pick->go_property = this;

    QRadioButton* cw_radio = new QRadioButton; //allocate first radio
    cw_radio->setText("CW");
    if (mCullFaceDirection == CCF_DIRECTION_CW)
        cw_radio->setChecked(true);

    QRadioButton* ccw_radio = new QRadioButton; //allocate second radio
    ccw_radio->setText("CCW");
    if (mCullFaceDirection == CCF_DIRECTION_CCW)
        ccw_radio->setChecked(true);

    //add created radio button
    cullface_pick->addRadioButton(cw_radio);
    cullface_pick->addRadioButton(ccw_radio);

    _inspector_win->registerUiObject(cullface_pick);
    _inspector_win->getContentLayout()->addWidget(cullface_pick->mRadioGroup);

    BoolCheckboxArea* _ismain = new BoolCheckboxArea;
    _ismain->setLabel("is Main Camera");
    _ismain->go_property = this;
    _ismain->pResultBool = &this->mIsMainCamera;
    _inspector_win->addPropertyArea(_ismain);

    if (!mIsMainCamera) {
        PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
        area->setLabel("Target Texture");
        area->go_property = this;
        area->pResultString = &this->TargetResourceName;
        _inspector_win->addPropertyArea(area);
    }
}

void Engine::CameraComponent::onValueChanged() {
    updateProjectionMat();
    _inspector_win->updateRequired = true;
    UpdateTextureResource();
}