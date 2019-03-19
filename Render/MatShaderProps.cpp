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
//Integer stuff
IntegerMaterialShaderProperty::IntegerMaterialShaderProperty(){
    type = MATSHPROP_TYPE_INTEGER;
}
IntegerMtShPropConf::IntegerMtShPropConf(){
    type = MATSHPROP_TYPE_INTEGER;

    value = 0;
}
//Float stuff
FloatMaterialShaderProperty::FloatMaterialShaderProperty(){
    type = MATSHPROP_TYPE_FLOAT;
}
FloatMtShPropConf::FloatMtShPropConf(){
    type = MATSHPROP_TYPE_FLOAT;

    value = 0.0f;
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
    diff_texture_prop->prop_caption = "Diffuse"; //Set caption in Inspector
    diff_texture_prop->ToggleUniform = "hasDiffuseMap";
    diff_texture_prop->prop_identifier = "t_diffuse"; //Identifier to save

    TextureMaterialShaderProperty* normal_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group.addProperty(MATSHPROP_TYPE_TEXTURE));
    normal_texture_prop->slotToBind = 1;
    normal_texture_prop->prop_caption = "Normal";
    normal_texture_prop->ToggleUniform = "hasNormalMap";
    normal_texture_prop->prop_identifier = "t_normal"; //Identifier to save

    TextureMaterialShaderProperty* specular_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group.addProperty(MATSHPROP_TYPE_TEXTURE));
    specular_texture_prop->slotToBind = 2;
    specular_texture_prop->prop_caption = "Specular";
    specular_texture_prop->ToggleUniform = "hasSpecularMap";
    specular_texture_prop->prop_identifier = "t_specular"; //Identifier to save

    FloatMaterialShaderProperty* shininess_factor_prop =
            static_cast<FloatMaterialShaderProperty*>(default_group.addProperty(MATSHPROP_TYPE_FLOAT));
    shininess_factor_prop->integerUniform = "material_shininess";
    shininess_factor_prop->prop_caption = "Shininess";
    shininess_factor_prop->prop_identifier = "f_shininess"; //Identifier to save

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
        case MATSHPROP_TYPE_INTEGER:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new IntegerMaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FLOAT:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new FloatMaterialShaderProperty); //Allocation of transform in heap
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
        case MATSHPROP_TYPE_INTEGER:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new IntegerMtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FLOAT:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new FloatMtShPropConf); //Allocation of transform in heap
            break;
        }

    }
    return _ptr;
}

void Material::saveToFile(){
    std::ofstream mat_stream;
    mat_stream.open(file_path, std::ofstream::out);

    for(unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i ++){
        //Obtain pointers to prop and prop's configuration
        MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
        MaterialShaderPropertyConf* conf_ptr = this->confs[prop_i];

        mat_stream << "ENTRY " << prop_ptr->prop_identifier.toStdString() << " "; //Write identifier

        switch(prop_ptr->type){
            case MATSHPROP_TYPE_TEXTURE:{
                //Cast pointer
                TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream << texture_conf->path.toStdString();
                break;
            }
            case MATSHPROP_TYPE_FLOAT:{
                //Cast pointer
                FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream << float_conf->value;
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
    //Open stream
    mat_stream.open(fpath, std::ifstream::in);

    while(!mat_stream.eof()){ //While file not finished reading
        std::string prefix;
        mat_stream >> prefix; //Read prefix
        if(prefix.compare("ENTRY") == 0){ //if it is game object
            std::string prop_identifier;
            mat_stream >> prop_identifier; //Read identifier

            for(unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i ++){
                MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
                MaterialShaderPropertyConf* conf_ptr = this->confs[prop_i];
                //check if compare
                if(prop_identifier.compare(prop_ptr->prop_identifier.toStdString()) == 0){
                    switch(prop_ptr->type){
                        case MATSHPROP_TYPE_TEXTURE:{
                            //Cast pointer
                            TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);

                            std::string path;
                            mat_stream >> path;

                            texture_conf->path = QString::fromStdString(path);
                            break;
                        }
                        case MATSHPROP_TYPE_FLOAT:{
                            //Cast pointer
                            FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);

                            float value;
                            mat_stream >> value;

                            float_conf->value = value;
                            break;
                        }
                   }
                    mat_stream.seekg(1, std::ofstream::cur); //Skip space
                }
            }
        }
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
