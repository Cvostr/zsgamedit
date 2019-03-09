#include "headers/MatShaderProps.h"

static MtShaderPropertiesGroup default_group;
static bool default_group_created = false;

MaterialShaderProperty::MaterialShaderProperty(){
    type = MATSHPROP_TYPE_NONE;
}
MaterialShaderProperty* MtShaderPropertiesGroup::addProperty(int type){
    //Allocate property in heap
    MaterialShaderProperty* newprop_ptr = MtShProps::allocateProperty(type);

    this->properties.push_back(newprop_ptr);

    return properties[properties.size() - 1];
}


MaterialShaderPropertyConf::MaterialShaderPropertyConf(){
    this->type = MATSHPROP_TYPE_NONE;
}

//Textures stuff
TextureMaterialShaderProperty::TextureMaterialShaderProperty(){
    type = MATSHPROP_TYPE_TEXTURE;

    this->slotToBind = 0;
}
//Configuration class
TextureMtShPropConf::TextureMtShPropConf(){
    this->type = MATSHPROP_TYPE_TEXTURE;

    this->path = "@none";
    this->texture = nullptr;
}


void MtShaderPropertiesGroup::loadFromFile(const char* fpath){

}
MtShaderPropertiesGroup::MtShaderPropertiesGroup(){
    properties.resize(0);
}


MtShaderPropertiesGroup* MtShProps::genDefaultMtShGroup(ZSPIRE::Shader* shader3d){
    if(default_group_created) return &default_group;

    default_group.render_shader = shader3d;

    TextureMaterialShaderProperty* diff_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group.addProperty(MATSHPROP_TYPE_TEXTURE));
    diff_texture_prop->slotToBind = 0;
    diff_texture_prop->prop_caption = "Diffuse";

    TextureMaterialShaderProperty* normal_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group.addProperty(MATSHPROP_TYPE_TEXTURE));
    normal_texture_prop->slotToBind = 1;
    normal_texture_prop->prop_caption = "Normal";

    default_group_created = true;

    return &default_group;
}
MtShaderPropertiesGroup* MtShProps::getDefaultMtShGroup(){
    return &default_group;
}

MaterialShaderProperty* MtShProps::allocateProperty(int type){
    MaterialShaderProperty* _ptr = nullptr;
    switch (type) {
        case MATSHPROP_TYPE_TEXTURE:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new TextureMaterialShaderProperty); //Allocation of transform in heap
            break;
        }

    }
    return _ptr;
}
MaterialShaderPropertyConf* MtShProps::allocatePropertyConf(int type){
    MaterialShaderPropertyConf* _ptr = nullptr;
    switch (type) {
        case MATSHPROP_TYPE_TEXTURE:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new TextureMtShPropConf); //Allocation of transform in heap
            break;
        }

    }
    return _ptr;
}
