#include "headers/MatShaderProps.h"
#include <fstream>
#include <iostream>

static std::vector<MtShaderPropertiesGroup*> MatGroups;

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
//Float3 stuff
Float3MaterialShaderProperty::Float3MaterialShaderProperty(){
    type = MATSHPROP_TYPE_FVEC3;
}
Float3MtShPropConf::Float3MtShPropConf(){
    type = MATSHPROP_TYPE_FVEC3;

    value = ZSVECTOR3(0.0f, 0.0f, 0.0f);
}
Float2MaterialShaderProperty::Float2MaterialShaderProperty(){
    type = MATSHPROP_TYPE_FVEC2;
}
Float2MtShPropConf::Float2MtShPropConf(){
    type = MATSHPROP_TYPE_FVEC2;
    value = ZSVECTOR2(0.0f, 0.0f);
}
//Color stuff
ColorMaterialShaderProperty::ColorMaterialShaderProperty(){
    type = MATSHPROP_TYPE_COLOR;
}
ColorMtShPropConf::ColorMtShPropConf(){
    type = MATSHPROP_TYPE_COLOR;
}
//Textures3D stuff
Texture3MaterialShaderProperty::Texture3MaterialShaderProperty(){
    type = MATSHPROP_TYPE_TEXTURE3;

    this->slotToBind = 0;
}
//Configuration class
Texture3MtShPropConf::Texture3MtShPropConf(){
    this->type = MATSHPROP_TYPE_TEXTURE3;

    texture3D = new ZSPIRE::Texture3D;

    for(int i = 0; i < 6; i ++){
        this->texture_str[i] = "@none";
    }

}
void MtShaderPropertiesGroup::loadFromFile(const char* fpath){
    std::ifstream mat_shader_group;
    mat_shader_group.open(fpath);
}
MtShaderPropertiesGroup::MtShaderPropertiesGroup(){
    acceptShadows = false;
    properties.resize(0);
}

MtShaderPropertiesGroup* MtShProps::genDefaultMtShGroup(ZSPIRE::Shader* shader3d, ZSPIRE::Shader* skybox,
                                                        ZSPIRE::Shader* heightmap){

    MtShaderPropertiesGroup* default_group = new MtShaderPropertiesGroup;
    default_group->acceptShadows = true;
    default_group->str_path = "@default";
    default_group->groupCaption = "Default 3D";
    default_group->render_shader = shader3d;

    ColorMaterialShaderProperty* diff_color_prop =
            static_cast<ColorMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_COLOR));
    diff_color_prop->prop_caption = "Color"; //Set caption in Inspector
    diff_color_prop->prop_identifier = "c_diffuse"; //Identifier to save
    diff_color_prop->colorUniform = "diffuse_color";

    TextureMaterialShaderProperty* diff_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
    diff_texture_prop->slotToBind = 0;
    diff_texture_prop->prop_caption = "Diffuse"; //Set caption in Inspector
    diff_texture_prop->ToggleUniform = "hasDiffuseMap";
    diff_texture_prop->prop_identifier = "t_diffuse"; //Identifier to save

    TextureMaterialShaderProperty* normal_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
    normal_texture_prop->slotToBind = 1;
    normal_texture_prop->prop_caption = "Normal";
    normal_texture_prop->ToggleUniform = "hasNormalMap";
    normal_texture_prop->prop_identifier = "t_normal"; //Identifier to save

    TextureMaterialShaderProperty* specular_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
    specular_texture_prop->slotToBind = 2;
    specular_texture_prop->prop_caption = "Specular";
    specular_texture_prop->ToggleUniform = "hasSpecularMap";
    specular_texture_prop->prop_identifier = "t_specular"; //Identifier to save

    TextureMaterialShaderProperty* height_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
    height_texture_prop->slotToBind = 3;
    height_texture_prop->prop_caption = "Height";
    height_texture_prop->ToggleUniform = "hasHeightMap";
    height_texture_prop->prop_identifier = "t_height"; //Identifier to save

    FloatMaterialShaderProperty* shininess_factor_prop =
            static_cast<FloatMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_FLOAT));
    shininess_factor_prop->floatUniform = "material_shininess";
    shininess_factor_prop->prop_caption = "Shininess";
    shininess_factor_prop->prop_identifier = "f_shininess"; //Identifier to save

    MtShProps::addMtShaderPropertyGroup(default_group);

//Default skybox material
    MtShaderPropertiesGroup* default_sky_group = new MtShaderPropertiesGroup;
    default_sky_group->str_path = "@skybox";
    default_sky_group->groupCaption = "Default Skybox";
    default_sky_group->render_shader = skybox;
    Texture3MaterialShaderProperty* sky_texture =
            static_cast<Texture3MaterialShaderProperty*>(default_sky_group->addProperty(MATSHPROP_TYPE_TEXTURE3));
    sky_texture->slotToBind = 0;
    sky_texture->prop_caption = "Sky";
    sky_texture->ToggleUniform = "hasSpecularMap";
    sky_texture->prop_identifier = "skytexture3"; //Identifier to save

    MtShProps::addMtShaderPropertyGroup(default_sky_group);

//Default skybox material
    MtShaderPropertiesGroup* default_heightmap_group = new MtShaderPropertiesGroup;
    default_heightmap_group->str_path = "@heightmap";
    default_heightmap_group->groupCaption = "Default Heightmap";
    default_heightmap_group->acceptShadows = true;
    default_heightmap_group->render_shader = heightmap;

    MtShProps::addMtShaderPropertyGroup(default_heightmap_group);

    return default_group;
}
MtShaderPropertiesGroup* MtShProps::getDefaultMtShGroup(){
    return MatGroups[0];
}

void MtShProps::addMtShaderPropertyGroup(MtShaderPropertiesGroup* group){
    //Check if this property already added
    if(getMtShaderPropertyGroup(group->str_path) == nullptr)
        MatGroups.push_back(group);
}
MtShaderPropertiesGroup* MtShProps::getMtShaderPropertyGroup(std::string group_name){
    for(unsigned int group_i = 0; group_i < MatGroups.size(); group_i ++){
        MtShaderPropertiesGroup* group_ptr = MatGroups[group_i];
        if(group_ptr->str_path.compare(group_name) == false)
            return group_ptr;
    }
    return nullptr;
}

MtShaderPropertiesGroup* MtShProps::getMtShaderPropertyGroupByLabel(QString group_label){
    for(unsigned int group_i = 0; group_i < MatGroups.size(); group_i ++){
        MtShaderPropertiesGroup* group_ptr = MatGroups[group_i];
        if(group_ptr->groupCaption.compare(group_label) == false)
            return group_ptr;
    }
    return nullptr;
}

unsigned int MtShProps::getMaterialShaderPropertyAmount(){
    return static_cast<unsigned int>(MatGroups.size());
}
MtShaderPropertiesGroup* MtShProps::getMtShaderPropertiesGroupByIndex(unsigned int index){
    return MatGroups[index];
}

void MtShProps::clearMtShaderGroups(){
   MatGroups.clear();
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
        case MATSHPROP_TYPE_FVEC3:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new Float3MaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FVEC2:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new Float2MaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_COLOR:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new ColorMaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_TEXTURE3:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new Texture3MaterialShaderProperty); //Allocation of transform in heap
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
        case MATSHPROP_TYPE_FVEC3:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new Float3MtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FVEC2:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new Float2MtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_COLOR:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new ColorMtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_TEXTURE3:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new Texture3MtShPropConf); //Allocation of transform in heap
            break;
        }
    }
    return _ptr;
}

void Material::saveToFile(){
    std::ofstream mat_stream;
    mat_stream.open(file_path, std::ofstream::out);
    //Write material header
    mat_stream << "ZSP_MATERIAL\n";
    //Write group string
    mat_stream << "GROUP " << this->group_str << "\n";
    for(unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i ++){
        //Obtain pointers to prop and prop's configuration
        MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
        MaterialShaderPropertyConf* conf_ptr = this->confs[prop_i];
        //write entry header
        mat_stream << "ENTRY " << prop_ptr->prop_identifier.toStdString() << " "; //Write identifier

        switch(prop_ptr->type){
            case MATSHPROP_TYPE_NONE:{
                break;
            }
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
            case MATSHPROP_TYPE_INTEGER:{
                //Cast pointer
                IntegerMtShPropConf* int_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream << int_conf->value;
                break;
            }
            case MATSHPROP_TYPE_COLOR:{
                //Cast pointer
                ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream << color_conf->color.r << " " << color_conf->color.g << " " << color_conf->color.b;
                break;
            }
            case MATSHPROP_TYPE_FVEC3:{
                //Cast pointer
                Float3MtShPropConf* fvec3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);
                //Write value
                mat_stream << fvec3_conf->value.X << " " << fvec3_conf->value.Y << " " << fvec3_conf->value.Z;
                break;
            }
            case MATSHPROP_TYPE_TEXTURE3:{
                //Cast pointer
                Texture3MtShPropConf* tex3_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);
                //Write value
                for(int i = 0; i < 6; i ++)
                    mat_stream << tex3_conf->texture_str[i].toStdString() << " ";

                break;
            }
        }
    mat_stream << "\n"; //Write divider
    }
    mat_stream.close(); //close stream
}

void Material::loadFromFile(std::string fpath){
    std::cout << "Loading Material " << fpath << std::endl;

    this->file_path = fpath;
    //Define and open file stream
    std::ifstream mat_stream;
    //Open stream
    mat_stream.open(fpath, std::ifstream::in);

    std::string test_header;
    mat_stream >> test_header; //Read header
    if(test_header.compare("ZSP_MATERIAL") != 0) //If it isn't zspire scene
        return; //Go out, we have nothing to do

    while(!mat_stream.eof()){ //While file not finished reading
        std::string prefix;
        mat_stream >> prefix; //Read prefix

        if(prefix.compare("GROUP") == 0){ //if it is game object
            mat_stream >> this->group_str; //Read identifier

            setPropertyGroup(MtShProps::getMtShaderPropertyGroup(group_str));
            //this->group_ptr = ;
        }

        if(prefix.compare("ENTRY") == 0){ //if it is game object
            std::string prop_identifier;
            mat_stream >> prop_identifier; //Read identifier

            for(unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i ++){
                MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
                MaterialShaderPropertyConf* conf_ptr = this->confs[prop_i];
                //check if compare
                if(prop_identifier.compare(prop_ptr->prop_identifier.toStdString()) == 0){
                    switch(prop_ptr->type){
                        case MATSHPROP_TYPE_NONE:{
                            break;
                        }
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
                        case MATSHPROP_TYPE_INTEGER:{
                            //Cast pointer
                            IntegerMtShPropConf* int_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);

                            int value;
                            mat_stream >> value;

                            int_conf->value = value;
                            break;
                        }
                        case MATSHPROP_TYPE_COLOR:{
                            //Cast pointer
                            ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);
                            //Write value
                            mat_stream >> color_conf->color.r >> color_conf->color.g >> color_conf->color.b;
                            color_conf->color.updateGL();
                            break;
                        }
                        case MATSHPROP_TYPE_FVEC3:{
                            //Cast pointer
                            Float3MtShPropConf* fvec3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);
                            //Write value
                            mat_stream >> fvec3_conf->value.X >> fvec3_conf->value.Y >> fvec3_conf->value.Z;
                            break;
                        }
                        case MATSHPROP_TYPE_TEXTURE3:{
                            //Cast pointer
                            Texture3MtShPropConf* texture3_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);

                            for(int i = 0; i < 6; i ++){
                                std::string path;
                                mat_stream >> path;
                                texture3_conf->texture_str[i] = QString::fromStdString(path);
                            }

                            break;
                        }
                   }
                   mat_stream.seekg(1, std::ofstream::cur); //Skip space
                }
            }
        }
    }
    mat_stream.close(); //close material stream
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
    this->group_str = group_ptr->str_path;
}

Material::Material(){
    setPropertyGroup(MtShProps::getDefaultMtShGroup());
    group_str = "@default";
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
