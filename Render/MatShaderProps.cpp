#include "headers/MatShaderProps.h"
#include <fstream>

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

    default_group.str_path = "@default";
    default_group.render_shader = shader3d;

    TextureMaterialShaderProperty* diff_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group.addProperty(MATSHPROP_TYPE_TEXTURE));
    diff_texture_prop->slotToBind = 0;
    diff_texture_prop->prop_caption = "Diffuse";
    diff_texture_prop->ToggleUniform = "hasDiffuseMap";

    TextureMaterialShaderProperty* normal_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group.addProperty(MATSHPROP_TYPE_TEXTURE));
    normal_texture_prop->slotToBind = 1;
    normal_texture_prop->prop_caption = "Normal";
    normal_texture_prop->ToggleUniform = "hasNormalMap";

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

void Material::saveToFile(){
    std::ofstream mat_stream;
    mat_stream.open(file_path, std::ofstream::out);

    for(unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i ++){

        MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
        MaterialShaderPropertyConf* conf_ptr = this->confs[prop_i];
        switch(prop_ptr->type){
            case MATSHPROP_TYPE_TEXTURE:{
                //Cast pointer
               // TextureMaterialShaderProperty* texture_p = static_cast<TextureMaterialShaderProperty*>(prop_ptr);
                TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);

                mat_stream << texture_conf->path.toStdString();
                break;
            }
        }
    mat_stream << "\n"; //Write divider
    }
}

void Material::loadFromFile(std::string fpath){
    this->file_path = fpath;
    //Define and open file stream
    std::ifstream mat_stream;
    mat_stream.open(fpath, std::ifstream::in);
    for(unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i ++){

        MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
        MaterialShaderPropertyConf* conf_ptr = this->confs[prop_i];
        switch(prop_ptr->type){
            case MATSHPROP_TYPE_TEXTURE:{
                //Cast pointer
                TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);

                std::string path;
                mat_stream >> path;

                texture_conf->path = QString::fromStdString(path);
                break;
            }
        }
        mat_stream.seekg(1, std::ofstream::cur); //Skip space
    }
}
void Material::setPropertyGroup(MtShaderPropertiesGroup* group_ptr){
    this->clear(); //clear all confs, first
    //Iterate over all properties in group
    for(unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i ++){
        //Obtain pointer to property in group
        MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
        //Add PropertyConf with the same type
        this->addPropertyConf(prop_ptr->type);
    }
    this->group_ptr = group_ptr;
}

Material::Material(){
    setPropertyGroup(MtShProps::getDefaultMtShGroup());
}

void Material::clear(){
    for(unsigned int prop_i = 0; prop_i < this->confs.size(); prop_i ++){
        delete this->confs[prop_i];
    }
    confs.clear(); //Resize array to 0
}

MaterialShaderPropertyConf* Material::addPropertyConf(int type){
    //Allocate new property
    MaterialShaderPropertyConf* newprop_ptr =
            static_cast<MaterialShaderPropertyConf*>(MtShProps::allocatePropertyConf(type));
    //Push pointer to vector
    this->confs.push_back(newprop_ptr);

    return confs[confs.size() - 1];
}
