#include "world/go_properties.h"
#include "../../ProjEd/headers/InspEditAreas.h"

extern ZSGAME_DATA* game_data;
extern InspectorWin* _inspector_win;

void Engine::ZPScriptProperty::onValueChanged() {
    std::string old_res_name;
    if (script_res != nullptr)
        old_res_name = script_res->resource_label;
    //update resource pointer
    script_res = game_data->resources->getScriptByLabel(script_path);
    script = new AGScript(game_data->script_manager, go_link.updLinkPtr(), "angel");
    //Compare strings
    if (old_res_name.compare(script_res->resource_label) != 0) {
        //Strings differ
        makeGlobalVarsList();
        //update interface
        _inspector_win->updateRequired = true;
    }
}

void Engine::ZPScriptProperty::addPropertyInterfaceToInspector() {

    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_SCRIPT);
    area->setLabel("Angel Script");
    area->go_property = this;
    area->pResultString = &script_path;
    _inspector_win->addPropertyArea(area);
    if (this->script_res == nullptr)
        return;
    //Check, if script contain errors
    if (this->script_res->hasError) {
        //Show error text message
        QString out = "Script contains errors!\nPlease, fix them in order to start scene";
        AreaText* group_info = new AreaText;
        group_info->label->setText(out);
        _inspector_win->getContentLayout()->addWidget(group_info->label);
        _inspector_win->registerUiObject(group_info);
    }
    else {
        for (unsigned int var_i = 0; var_i < this->vars.size(); var_i++) {
            GlobVarHandle* handle = vars[var_i];
            if (handle->typeID == asTYPEID_INT32) {
                IntPropertyArea* intH = new IntPropertyArea; //New property area
                intH->setLabel(QString::fromStdString(handle->name)); //Its label
                intH->value = handle->getValue<int>(); //Ptr to our vector
                intH->go_property = this; //Pointer to this to activate matrix recalculaton
                _inspector_win->addPropertyArea(intH);
            }
            if (handle->typeID == asTYPEID_FLOAT) {
                FloatPropertyArea* floatH = new FloatPropertyArea; //New property area
                floatH->setLabel(QString::fromStdString(handle->name)); //Its label
                floatH->value = handle->getValue<float>(); //Ptr to our vector
                floatH->go_property = this; //Pointer to this to activate matrix recalculaton
                _inspector_win->addPropertyArea(floatH);
            }
            if (handle->typeID == asTYPEID_BOOL) {
                BoolCheckboxArea* boolH = new BoolCheckboxArea; //New property area
                boolH->setLabel(QString::fromStdString(handle->name)); //Its label
                boolH->pResultBool = handle->getValue<bool>(); //Ptr to our vector
                boolH->go_property = this; //Pointer to this to activate matrix recalculaton
                _inspector_win->addPropertyArea(boolH);
            }
            if (handle->typeID == AG_VECTOR3) {
                Float3PropertyArea* vec3H = new Float3PropertyArea; //New property area
                vec3H->setLabel(QString::fromStdString(handle->name)); //Its label
                vec3H->vector = handle->getValue<ZSVECTOR3>(); //Ptr to our vector
                vec3H->go_property = this; //Pointer to this to activate matrix recalculaton
                _inspector_win->addPropertyArea(vec3H);
            }
            if (handle->typeID == AG_QUAT) {
                Float4PropertyArea* vec4H = new Float4PropertyArea; //New property area
                vec4H->setLabel(QString::fromStdString(handle->name)); //Its label
                vec4H->vector = handle->getValue<ZSVECTOR4>(); //Ptr to our vector
                vec4H->go_property = this; //Pointer to this to activate matrix recalculaton
                _inspector_win->addPropertyArea(vec4H);
            }
            if (handle->typeID == AG_RGB_COLOR) {
                ColorDialogArea* color_area = new ColorDialogArea;
                color_area->setLabel(QString::fromStdString(handle->name)); //Its label
                color_area->color = handle->getValue<ZSRGBCOLOR>();
                color_area->go_property = this;
                _inspector_win->addPropertyArea(color_area);
            }
            if (handle->typeID == AG_STRING) {
                StringPropertyArea* area = new StringPropertyArea;
                area->setLabel(QString::fromStdString(handle->name));
                area->value_ptr = handle->getValue<std::string>();
                area->go_property = this;
                _inspector_win->addPropertyArea(area);
            }
        }
    }
}