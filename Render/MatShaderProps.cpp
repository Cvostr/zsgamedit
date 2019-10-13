#include "headers/MatShaderProps.h"
#include <fstream>
#include <iostream>
#include <GL/glew.h>
#include "../Misc/headers/zs_types.h"

extern Project* project_ptr;
Material* default3dmat;
static std::vector<MtShaderPropertiesGroup*> MatGroups;

MaterialShaderProperty::MaterialShaderProperty(){
    type = MATSHPROP_TYPE_NONE;
}
MaterialShaderProperty* MtShaderPropertiesGroup::addProperty(int type){
    //Allocate property in heap
    MaterialShaderProperty* newprop_ptr = MtShProps::allocateProperty(type);
    //Push new material
    this->properties.push_back(newprop_ptr);
    //Return new material pointer from vector
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
MtShaderPropertiesGroup::MtShaderPropertiesGroup(ZSPIRE::Shader* shader, const char* UB_CAPTION, unsigned int UB_ConnectID, unsigned int UB_SIZE){
    render_shader = shader;

    if(UB_SIZE == 0 || strlen(UB_CAPTION) == 0) return;
    this->UB_ConnectID = UB_ConnectID;
    //First of all, tell shader uniform buffer point
    //Get Index of buffer
    unsigned int UB_INDEX = shader->getUniformBufferIndex(UB_CAPTION);
    //Set UB binding
    shader->setUniformBufferBinding(UB_INDEX, UB_ConnectID);

    //Generate uniform buffer
    glGenBuffers(1, &this->UB_ID);
    glBindBuffer(GL_UNIFORM_BUFFER, UB_ID);
    //Allocate memory for buffer
    glBufferData(GL_UNIFORM_BUFFER, UB_SIZE, nullptr, GL_STATIC_DRAW);
    //Connect to point (UB_ConnectID)
    glBindBufferBase(GL_UNIFORM_BUFFER, UB_ConnectID, UB_ID);

    acceptShadows = false;
    properties.resize(0);
}

void MtShaderPropertiesGroup::setUB_Data(unsigned int offset, unsigned int size, void* data){
    //Bind uniform buffer ID
    glBindBuffer(GL_UNIFORM_BUFFER, UB_ID);
    //Send data to uniform buffer
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

MtShaderPropertiesGroup* MtShProps::genDefaultMtShGroup(ZSPIRE::Shader* shader3d, ZSPIRE::Shader* skybox,
                                                        ZSPIRE::Shader* heightmap,
                                                        unsigned int uniform_buf_id_took){

    MtShaderPropertiesGroup* default_group = new MtShaderPropertiesGroup(shader3d, "Default3d", uniform_buf_id_took + 1, 36);
    default_group->acceptShadows = true;
    default_group->str_path = "@default";
    default_group->groupCaption = "Default 3D";

    ColorMaterialShaderProperty* diff_color_prop =
            static_cast<ColorMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_COLOR));
    diff_color_prop->prop_caption = "Color"; //Set caption in Inspector
    diff_color_prop->prop_identifier = "c_diffuse"; //Identifier to save
    diff_color_prop->colorUniform = "diffuse_color";
    diff_color_prop->start_offset = 0;

    TextureMaterialShaderProperty* diff_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
    diff_texture_prop->slotToBind = 0;
    diff_texture_prop->prop_caption = "Diffuse"; //Set caption in Inspector
    diff_texture_prop->ToggleUniform = "hasDiffuseMap";
    diff_texture_prop->prop_identifier = "t_diffuse"; //Identifier to save
    diff_texture_prop->start_offset = 12;

    TextureMaterialShaderProperty* normal_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
    normal_texture_prop->slotToBind = 1;
    normal_texture_prop->prop_caption = "Normal";
    normal_texture_prop->ToggleUniform = "hasNormalMap";
    normal_texture_prop->prop_identifier = "t_normal"; //Identifier to save
    normal_texture_prop->start_offset = 16;

    TextureMaterialShaderProperty* specular_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
    specular_texture_prop->slotToBind = 2;
    specular_texture_prop->prop_caption = "Specular";
    specular_texture_prop->ToggleUniform = "hasSpecularMap";
    specular_texture_prop->prop_identifier = "t_specular"; //Identifier to save
    specular_texture_prop->start_offset = 20;


    TextureMaterialShaderProperty* height_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
    height_texture_prop->slotToBind = 3;
    height_texture_prop->prop_caption = "Height";
    height_texture_prop->ToggleUniform = "hasHeightMap";
    height_texture_prop->prop_identifier = "t_height"; //Identifier to save
    height_texture_prop->start_offset = 24;

    FloatMaterialShaderProperty* shininess_factor_prop =
            static_cast<FloatMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_FLOAT));
    shininess_factor_prop->floatUniform = "material_shininess";
    shininess_factor_prop->prop_caption = "Shininess";
    shininess_factor_prop->prop_identifier = "f_shininess"; //Identifier to save
    shininess_factor_prop->start_offset = 28;

    MtShProps::addMtShaderPropertyGroup(default_group);

//Default skybox material
    MtShaderPropertiesGroup* default_sky_group = new MtShaderPropertiesGroup(skybox, "", 0, 0);
    default_sky_group->str_path = "@skybox";
    default_sky_group->groupCaption = "Default Skybox";
    Texture3MaterialShaderProperty* sky_texture =
            static_cast<Texture3MaterialShaderProperty*>(default_sky_group->addProperty(MATSHPROP_TYPE_TEXTURE3));
    sky_texture->slotToBind = 0;
    sky_texture->prop_caption = "Sky";
    sky_texture->ToggleUniform = "hasSpecularMap";
    sky_texture->prop_identifier = "skytexture3"; //Identifier to save

    MtShProps::addMtShaderPropertyGroup(default_sky_group);

//Default terrain material
    MtShaderPropertiesGroup* default_heightmap_group = new MtShaderPropertiesGroup(heightmap, "", 0, 0);
    default_heightmap_group->str_path = "@heightmap";
    default_heightmap_group->groupCaption = "Default Heightmap";
    default_heightmap_group->acceptShadows = true;

    MtShProps::addMtShaderPropertyGroup(default_heightmap_group);

    //Create default base material
    default3dmat = new Material(default_group);
    default3dmat->file_path = "@none";

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

void Material::saveToFile(){
    //if it is a default material, then quit function
    if(file_path[0] == '@') return;

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
    //store pointer of picked group
    this->group_ptr = group_ptr;
    //store string id of picked group
    this->group_str = group_ptr->str_path;
}

void Material::applyMatToPipeline(){
    ZSPIRE::Shader* shader = this->group_ptr->render_shader;
    //iterate over all properties, send them all!
    unsigned int offset = 0;
    for(unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i ++){
        MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
        MaterialShaderPropertyConf* conf_ptr = confs[prop_i];
        switch(prop_ptr->type){
            case MATSHPROP_TYPE_NONE:{
                break;
            }
            case MATSHPROP_TYPE_TEXTURE:{
                //Cast pointer
                TextureMaterialShaderProperty* texture_p = static_cast<TextureMaterialShaderProperty*>(prop_ptr);
                TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);

                int db = 0;

                if(texture_conf->path.compare("@none")){
                    //if texture isn't loaded
                    if(texture_conf->texture == nullptr){
                        Resource* res_ptr = project_ptr->getResource(texture_conf->path);
                        texture_conf->texture = static_cast<ZSPIRE::Texture*>(res_ptr->class_ptr);
                    }
                    //Set opengl texture
                    db = 1;
                    texture_conf->texture->Use(texture_p->slotToBind); //Use texture
                }
                offset = texture_p->start_offset;
                group_ptr->setUB_Data(offset, 4, &db);

                break;
            }
            case MATSHPROP_TYPE_FLOAT:{
                //Cast pointer
                FloatMaterialShaderProperty* float_p = static_cast<FloatMaterialShaderProperty*>(prop_ptr);
                FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);

                offset = float_p->start_offset;
                //set float to buffer
                group_ptr->setUB_Data(offset, 4, &float_conf->value);

                break;
            }
            case MATSHPROP_TYPE_INTEGER:{
                //Cast pointer
                IntegerMaterialShaderProperty* int_p = static_cast<IntegerMaterialShaderProperty*>(prop_ptr);
                IntegerMtShPropConf* int_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);

                offset = int_p->start_offset;
                //set integer to buffer
                group_ptr->setUB_Data(offset, 4, &int_conf->value);

                break;
            }
            case MATSHPROP_TYPE_COLOR:{
                //Cast pointer
                ColorMaterialShaderProperty* color_p = static_cast<ColorMaterialShaderProperty*>(prop_ptr);
                ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);

                color_conf->color.updateGL();

                offset = color_p->start_offset;
                //Write color to buffer
                group_ptr->setUB_Data(offset, 4, &color_conf->color.gl_r);
                group_ptr->setUB_Data(offset + 4, 4, &color_conf->color.gl_g);
                group_ptr->setUB_Data(offset + 8, 4, &color_conf->color.gl_b);


                break;
            }
            case MATSHPROP_TYPE_FVEC3:{
                //Cast pointer
                Float3MaterialShaderProperty* fvec3_p = static_cast<Float3MaterialShaderProperty*>(prop_ptr);
                Float3MtShPropConf* fvec3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);

                offset = fvec3_p->start_offset;
                //Write color to buffer
                group_ptr->setUB_Data(offset, 4, &fvec3_conf->value.X);
                group_ptr->setUB_Data(offset + 4, 4, &fvec3_conf->value.Y);
                group_ptr->setUB_Data(offset + 8, 4, &fvec3_conf->value.Z);

                break;
            }
            case MATSHPROP_TYPE_FVEC2:{
                break;
            }
            case MATSHPROP_TYPE_IVEC2:{
                break;
            }
            case MATSHPROP_TYPE_TEXTURE3:{
                //Cast pointer
                Texture3MaterialShaderProperty* texture_p = static_cast<Texture3MaterialShaderProperty*>(prop_ptr);
                Texture3MtShPropConf* texture_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);

                if(!texture_conf->texture3D->created){
                    texture_conf->texture3D->Init();
                    for(int i = 0; i < 6; i ++){
                        texture_conf->texture3D->pushTexture(i, project_ptr->root_path.toStdString() + "/" + texture_conf->texture_str[i].toStdString());
                    }
                    texture_conf->texture3D->created = true;
                    texture_conf->texture3D->Use(texture_p->slotToBind);
                }else{
                    texture_conf->texture3D->Use(texture_p->slotToBind);
                }
                break;
            }
        }
    }
}

Material::Material(){
    setPropertyGroup(MtShProps::getDefaultMtShGroup());
}

Material::Material(std::string shader_group_str){
    setPropertyGroup(MtShProps::getMtShaderPropertyGroup(shader_group_str));
}

Material::Material(MtShaderPropertiesGroup* _group_ptr){
    setPropertyGroup(_group_ptr);
}

Material::~Material(){

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
