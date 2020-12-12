#include "world/go_properties.h"
#include "../../ProjEd/headers/InspEditAreas.h"
#include "../../ProjEd/headers/ProjectEdit.h"

extern InspectorWin* _inspector_win;
extern ZSGAME_DATA* game_data;
extern EditWindow* _editor_win;

void Engine::MaterialProperty::addPropertyInterfaceToInspector() {
    //Add area to pick material file
    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_MATERIAL);
    area->setLabel("Material");
    area->go_property = this;
    area->pResultString = &material_path;
    _inspector_win->addPropertyArea(area);
    //No material, exiting
    if (material_ptr == nullptr || material_ptr->file_path[0] == '@')
        return;

    if (material_ptr->group_ptr->acceptShadows) {
        BoolCheckboxArea* receiveShdws = new BoolCheckboxArea;
        receiveShdws->setLabel("Receive Shadows ");
        receiveShdws->go_property = this;
        receiveShdws->pResultBool = &this->receiveShadows;
        _inspector_win->addPropertyArea(receiveShdws);
    }
    //Add shader group picker
    ComboBoxArea* mt_shader_group_area = new ComboBoxArea;
    mt_shader_group_area->setLabel("Shader Group");
    mt_shader_group_area->go_property = this;
    mt_shader_group_area->pResultString = &this->group_label;
    //Iterate over all available shader groups and add them to combo box
    for (unsigned int i = 0; i < MtShProps::getMaterialShaderPropertyAmount(); i++) {
        MtShaderPropertiesGroup* ptr = MtShProps::getMtShaderPropertiesGroupByIndex(i);
        mt_shader_group_area->widget.addItem(QString::fromStdString(ptr->groupCaption));
    }
    _inspector_win->addPropertyArea(mt_shader_group_area);

    //if material isn't set up, exiting
    if (material_ptr->group_ptr == nullptr)
        return;
    //If set up, iterating over all items
    for (unsigned int prop_i = 0; prop_i < material_ptr->group_ptr->properties.size(); prop_i++) {
        MaterialShaderProperty* prop_ptr = material_ptr->group_ptr->properties[prop_i];
        MaterialShaderPropertyConf* conf_ptr = this->material_ptr->confs[prop_i];
        switch (prop_ptr->type) {
        case MATSHPROP_TYPE_NONE: {
            break;
        }
        case MATSHPROP_TYPE_TEXTURE: {
            //Cast pointer
            TextureMaterialShaderProperty* texture_p = static_cast<TextureMaterialShaderProperty*>(prop_ptr);
            TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);

            PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
            area->setLabel(QString::fromStdString(texture_p->prop_caption));
            area->go_property = this;
            area->pResultString = &texture_conf->path;
            area->isShowNoneItem = true;
            _inspector_win->addPropertyArea(area);

            break;
        }
        case MATSHPROP_TYPE_FLOAT: {
            FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);

            FloatPropertyArea* float_area = new FloatPropertyArea;
            float_area->setLabel(QString::fromStdString(prop_ptr->prop_caption)); //Its label
            float_area->value = &float_conf->value;
            float_area->go_property = this;
            _inspector_win->addPropertyArea(float_area);

            break;
        }
        case MATSHPROP_TYPE_FVEC3: {
            Float3MtShPropConf* float3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);

            Float3PropertyArea* float3_area = new Float3PropertyArea;
            float3_area->setLabel(QString::fromStdString(prop_ptr->prop_caption)); //Its label
            float3_area->vector = &float3_conf->value;
            float3_area->go_property = this;
            _inspector_win->addPropertyArea(float3_area);

            break;
        }
        case MATSHPROP_TYPE_FVEC2: {
            Float2MtShPropConf* float2_conf = static_cast<Float2MtShPropConf*>(conf_ptr);

            Float2PropertyArea* float2_area = new Float2PropertyArea;
            float2_area->setLabel(QString::fromStdString(prop_ptr->prop_caption)); //Its label
            float2_area->vector = &float2_conf->value;
            float2_area->go_property = this;
            _inspector_win->addPropertyArea(float2_area);
            break;
        }
        case MATSHPROP_TYPE_IVEC2: {
            Int2MtShPropConf* int2_conf = static_cast<Int2MtShPropConf*>(conf_ptr);

            Int2PropertyArea* int2_area = new Int2PropertyArea;
            int2_area->setLabel(QString::fromStdString(prop_ptr->prop_caption)); //Its label
            int2_area->vector = &int2_conf->value[0];
            int2_area->go_property = this;
            _inspector_win->addPropertyArea(int2_area);
            break;
        }
        case MATSHPROP_TYPE_INTEGER: {
            IntegerMtShPropConf* integer_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);

            IntPropertyArea* integer_area = new IntPropertyArea;
            integer_area->setLabel(QString::fromStdString(prop_ptr->prop_caption)); //Its label
            integer_area->value = &integer_conf->value;
            integer_area->go_property = this;
            _inspector_win->addPropertyArea(integer_area);

            break;
        }
        case MATSHPROP_TYPE_COLOR: {
            ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);

            ColorDialogArea* color_area = new ColorDialogArea;
            color_area->setLabel(QString::fromStdString(prop_ptr->prop_caption)); //Its label
            color_area->color = &color_conf->color;
            color_area->go_property = this;
            _inspector_win->addPropertyArea(color_area);

            break;
        }
        case MATSHPROP_TYPE_TEXTURE3: {
            //Cast pointer
            Texture3MtShPropConf* texture_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);

            QString captions[6] = { "Right", "Left", "Top", "Bottom", "Back", "Front" };
            //Draw six texture pickers to pick texture in each side
            for (int i = 0; i < 6; i++) {
                PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
                area->setLabel(captions[i]);
                area->go_property = this;
                area->pResultString = &texture_conf->texture_str[i];
                area->isShowNoneItem = true;
                _inspector_win->addPropertyArea(area);
            }

            break;
        }
        }
    }
}

void Engine::MaterialProperty::onValueChanged() {
    Material* newmat_ptr = game_data->resources->getMaterialByLabel("@default")->material;
    //Resource of new material
    Engine::MaterialResource* newmat_ptr_res = game_data->resources->getMaterialByLabel(this->material_path);
    //if resource exists
    if (newmat_ptr_res)
        newmat_ptr = newmat_ptr_res->material;

    //User changed material
    bool isMaterialChanged = newmat_ptr != this->material_ptr;

    //Check, if material file has changed
    if (isMaterialChanged) {
        this->material_ptr = newmat_ptr;
        this->group_label = newmat_ptr->group_ptr->groupCaption;
        //update window
        _inspector_win->updateRequired = true;
    }

    bool hasMaterial = material_ptr != nullptr;
    if (!hasMaterial) return;

    bool hasMaterialGroup = material_ptr->group_ptr != nullptr;
    if (!hasMaterialGroup) { //if material has no MaterialShaderPropertyGroup
        //if user specified group first time
        if (MtShProps::getMtShaderPropertyGroupByLabel(this->group_label) != nullptr) {
            //then apply that group
            material_ptr->setPropertyGroup(MtShProps::getMtShaderPropertyGroupByLabel(this->group_label));

        }
        else { //user haven't specified
            return; //go out
        }
    }
    else { //Material already had group, check, if user decided to change it
       //Get pointer to new selected shader group
        MtShaderPropertiesGroup* newgroup_ptr = MtShProps::getMtShaderPropertyGroupByLabel(this->group_label);
        //if new pointer and old aren't match, then change property group
        if (newgroup_ptr != this->material_ptr->group_ptr) {
            //Apply changing
            this->material_ptr->setPropertyGroup(MtShProps::getMtShaderPropertyGroupByLabel(this->group_label));
            //Update material interface
            _inspector_win->updateRequired = true;
        }
    }

    unsigned int GroupPropertiesSize = static_cast<unsigned int>(material_ptr->group_ptr->properties.size());
    for (unsigned int prop_i = 0; prop_i < GroupPropertiesSize; prop_i++) {
        MaterialShaderProperty* prop_ptr = material_ptr->group_ptr->properties[prop_i];
        MaterialShaderPropertyConf* conf_ptr = this->material_ptr->confs[prop_i];
        switch (prop_ptr->type) {
        case MATSHPROP_TYPE_TEXTURE: {
            //Cast pointer
            TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);
            //Update pointer to texture resource
            texture_conf->texture = game_data->resources->getTextureByLabel(texture_conf->path);
            break;
        }
        case MATSHPROP_TYPE_TEXTURE3: {
            //Cast pointer
            Texture3MtShPropConf* texture_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);
            texture_conf->texture3D->mCreated = false;
            _inspector_win->updateRequired = true;
            break;
        }
        }
    }
    //save changes to material file
    material_ptr->saveToFile();
    //Recreate thumbnails for all materials
    _editor_win->thumb_master->createMaterialThumbnail(newmat_ptr_res->resource_label);
    //Update thumbnail in file list
    _editor_win->updateFileListItemIcon(QString::fromStdString(material_ptr->file_path));
    //Redraw thumbnails in inspector
    _inspector_win->ThumbnailUpdateRequired = true;
}