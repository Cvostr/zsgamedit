#include "../headers/World.h"
#include "../../World/headers/World.h"
#include "world/go_properties.h"
#include "../headers/2dtileproperties.h"
#include "../../ProjEd/headers/ProjectEdit.h"
#include <render/zs-mesh.h>
#include "../../ProjEd/headers/InspEditAreas.h"

extern Project* project_ptr;
extern ZSGAME_DATA* game_data;

void GameObject::saveProperties(std::ofstream* stream){
    for(unsigned int prop_i = 0; prop_i < props_num; prop_i ++){
        Engine::GameObjectProperty* property_ptr = this->properties[prop_i];
        *stream << "\nG_PROPERTY ";
        stream->write(reinterpret_cast<char*>(&property_ptr->type), sizeof(int));
        stream->write(reinterpret_cast<char*>(&property_ptr->active), sizeof(bool));
        *stream << " ";

        switch(property_ptr->type){
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_NONE:{
                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM:{
                Engine::TransformProperty* ptr = static_cast<Engine::TransformProperty*>(property_ptr);
                float posX = ptr->translation.X;
                float posY = ptr->translation.Y;
                float posZ = ptr->translation.Z;

                float scaleX = ptr->scale.X;
                float scaleY = ptr->scale.Y;
                float scaleZ = ptr->scale.Z;

                float rotX = ptr->rotation.X;
                float rotY = ptr->rotation.Y;
                float rotZ = ptr->rotation.Z;

                stream->write(reinterpret_cast<char*>(&posX), sizeof(float));//Writing position X
                stream->write(reinterpret_cast<char*>(&posY), sizeof(float)); //Writing position Y
                stream->write(reinterpret_cast<char*>(&posZ), sizeof(float)); //Writing position Z

                stream->write(reinterpret_cast<char*>(&scaleX), sizeof(float));//Writing scale X
                stream->write(reinterpret_cast<char*>(&scaleY), sizeof(float)); //Writing scale Y
                stream->write(reinterpret_cast<char*>(&scaleZ), sizeof(float)); //Writing scale Z

                stream->write(reinterpret_cast<char*>(&rotX), sizeof(float));//Writing rotation X
                stream->write(reinterpret_cast<char*>(&rotY), sizeof(float)); //Writing rotation Y
                stream->write(reinterpret_cast<char*>(&rotZ), sizeof(float)); //Writing rotation Z
                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL:{
                LabelProperty* ptr = static_cast<LabelProperty*>(property_ptr);
                *stream << ptr->label;
                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH:{
                Engine::MeshProperty* ptr = static_cast<Engine::MeshProperty*>(property_ptr);
                *stream << ptr->resource_relpath << "\n";
                if(ptr->skinning_root_node != nullptr)
                    *stream << *ptr->skinning_root_node->label_ptr << "\n";
                else
                    *stream << "@none\n";
                stream->write(reinterpret_cast<char*>(&ptr->castShadows), sizeof(bool));
                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_ANIMATION:{
                Engine::AnimationProperty* ptr = static_cast<Engine::AnimationProperty*>(property_ptr);
                *stream << ptr->anim_label << "\n";
                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_NODE:{
                Engine::NodeProperty* ptr = static_cast<Engine::NodeProperty*>(property_ptr);
                //Write node name
                *stream << ptr->node_label << "\n";
                //Write node transform matrix
                for(unsigned int m_i = 0; m_i < 4; m_i ++){
                    for(unsigned int m_j = 0; m_j < 4; m_j ++){
                        float m_v = ptr->transform_mat.m[m_i][m_j];
                        stream->write(reinterpret_cast<char*>(&m_v), sizeof(float));
                    }
                }
                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_LIGHTSOURCE:{
                Engine::LightsourceProperty* ptr = static_cast<Engine::LightsourceProperty*>(property_ptr);
                Engine::LIGHTSOURCE_TYPE type = ptr->light_type;
                float intensity = ptr->intensity;
                float range = ptr->range;

                int color_r = ptr->color.r;
                int color_g = ptr->color.g;
                int color_b = ptr->color.b;

                stream->write(reinterpret_cast<char*>(&type), sizeof(Engine::LIGHTSOURCE_TYPE));
                stream->write(reinterpret_cast<char*>(&intensity), sizeof(float));
                stream->write(reinterpret_cast<char*>(&range), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->spot_angle), sizeof(float));

                stream->write(reinterpret_cast<char*>(&color_r), sizeof(int));
                stream->write(reinterpret_cast<char*>(&color_g), sizeof(int));
                stream->write(reinterpret_cast<char*>(&color_b), sizeof(int));

                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_AUDSOURCE:{
                Engine::AudioSourceProperty* ptr = static_cast<Engine::AudioSourceProperty*>(property_ptr);
                if(ptr->resource_relpath.empty()) //check if object has no audioclip
                    *stream << "@none";
                else
                    *stream << ptr->resource_relpath << "\n";

                stream->write(reinterpret_cast<char*>(&ptr->source.source_gain), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->source.source_pitch), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->source.looped), sizeof(bool));
                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL:{
                Engine::MaterialProperty* ptr = static_cast<Engine::MaterialProperty*>(property_ptr);
                //Write path to material string
                if(ptr->material_ptr != nullptr)
                    *stream << ptr->material_path << "\n"; //Write material relpath
                else
                    *stream << "@none" << "\n";

                stream->write(reinterpret_cast<char*>(&ptr->receiveShadows), sizeof(bool));

                break;
            }
            
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_COLLIDER:{
                Engine::ColliderProperty* ptr = static_cast<Engine::ColliderProperty*>(property_ptr);
                //write collider type
                stream->write(reinterpret_cast<char*>(&ptr->coll_type), sizeof(COLLIDER_TYPE));
                //boolean if collider has custom size and transform
                stream->write(reinterpret_cast<char*>(&ptr->isCustomPhysicalSize), sizeof(bool));
                if(ptr->isCustomPhysicalSize){
                    stream->write(reinterpret_cast<char*>(&ptr->cust_size.X), sizeof(float));
                    stream->write(reinterpret_cast<char*>(&ptr->cust_size.Y), sizeof(float));
                    stream->write(reinterpret_cast<char*>(&ptr->cust_size.Z), sizeof(float));
                    stream->write(reinterpret_cast<char*>(&ptr->transform_offset.X), sizeof(float));
                    stream->write(reinterpret_cast<char*>(&ptr->transform_offset.Y), sizeof(float));
                    stream->write(reinterpret_cast<char*>(&ptr->transform_offset.Z), sizeof(float));
                }
                *stream << "\n"; //write divider

                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_TRIGGER: {
                Engine::TriggerProperty* ptr = static_cast<Engine::TriggerProperty*>(property_ptr);
                //write collider type
                stream->write(reinterpret_cast<char*>(&ptr->coll_type), sizeof(COLLIDER_TYPE));
                //boolean if collider has custom size and transform
                stream->write(reinterpret_cast<char*>(&ptr->isCustomPhysicalSize), sizeof(bool));
                if (ptr->isCustomPhysicalSize) {
                    stream->write(reinterpret_cast<char*>(&ptr->cust_size.X), sizeof(float));
                    stream->write(reinterpret_cast<char*>(&ptr->cust_size.Y), sizeof(float));
                    stream->write(reinterpret_cast<char*>(&ptr->cust_size.Z), sizeof(float));
                    stream->write(reinterpret_cast<char*>(&ptr->transform_offset.X), sizeof(float));
                    stream->write(reinterpret_cast<char*>(&ptr->transform_offset.Y), sizeof(float));
                    stream->write(reinterpret_cast<char*>(&ptr->transform_offset.Z), sizeof(float));
                }
                *stream << "\n"; //write divider

                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_RIGIDBODY:{
                Engine::RigidbodyProperty* ptr = static_cast<Engine::RigidbodyProperty*>(property_ptr);
                //write collider type
                stream->write(reinterpret_cast<char*>(&ptr->coll_type), sizeof(COLLIDER_TYPE));
                //write isTrigger boolean
                stream->write(reinterpret_cast<char*>(&ptr->mass), sizeof(float));
                //write gravity
                stream->write(reinterpret_cast<char*>(&ptr->gravity.X), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->gravity.Y), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->gravity.Z), sizeof(float));
                //write linear velocity
                stream->write(reinterpret_cast<char*>(&ptr->linearVel.X), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->linearVel.Y), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->linearVel.Z), sizeof(float));

                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_SHADOWCASTER:{
                Engine::ShadowCasterProperty* ptr = static_cast<Engine::ShadowCasterProperty*>(property_ptr);
                //write collider type
                stream->write(reinterpret_cast<char*>(&ptr->TextureWidth), sizeof(int));
                stream->write(reinterpret_cast<char*>(&ptr->TextureHeight), sizeof(int));
                stream->write(reinterpret_cast<char*>(&ptr->shadow_bias), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->nearPlane), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->farPlane), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->projection_viewport), sizeof(float));
                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_TERRAIN:{
                Engine::TerrainProperty* ptr = static_cast<Engine::TerrainProperty*>(property_ptr);
                *stream << ptr->file_label << "\n"; //Write material relpath
                //write dimensions
                stream->write(reinterpret_cast<char*>(&ptr->Width), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->Length), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->MaxHeight), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->castShadows), sizeof(bool));
                stream->write(reinterpret_cast<char*>(&ptr->textures_size), sizeof(int));
                stream->write(reinterpret_cast<char*>(&ptr->grassType_size), sizeof(int));

                *stream << "\n";

                std::string fpath = project_ptr->root_path + "/" + ptr->file_label;
                ptr->getTerrainData()->saveToFile(fpath.c_str());
                //Write textures relative pathes
                for(int texture_i = 0; texture_i < ptr->textures_size; texture_i ++){
                    HeightmapTexturePair* texture_pair = &ptr->textures[static_cast<unsigned int>(texture_i)];
                    *stream << texture_pair->diffuse_relpath << " " << texture_pair->normal_relpath << "\n"; //Write material relpath
                }
                //Write info about all vegetable types
                for(int grass_i = 0; grass_i < ptr->grassType_size; grass_i ++){
                    HeightmapGrass* grass_ptr = &ptr->getTerrainData()->grass[static_cast<unsigned int>(grass_i)];
                    //Write grass diffuse texture
                    *stream << grass_ptr->diffuse_relpath << "\n";
                    //Write grass size
                    stream->write(reinterpret_cast<char*>(&grass_ptr->scale.X), sizeof(float));
                    stream->write(reinterpret_cast<char*>(&grass_ptr->scale.Y), sizeof(float));
                    *stream << "\n";
                }


                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE_GROUP:{
                TileGroupProperty* ptr = static_cast<TileGroupProperty*>(property_ptr);
                int isCreated = static_cast<int>(ptr->isCreated);
                int geometryWidth = ptr->geometry.tileWidth;
                int geometryHeight = ptr->geometry.tileHeight;
                int amountX = ptr->tiles_amount_X;
                int amountY = ptr->tiles_amount_Y;

                stream->write(reinterpret_cast<char*>(&isCreated), sizeof(int));
                stream->write(reinterpret_cast<char*>(&geometryWidth), sizeof(int));
                stream->write(reinterpret_cast<char*>(&geometryHeight), sizeof(int));
                stream->write(reinterpret_cast<char*>(&amountX), sizeof(int));
                stream->write(reinterpret_cast<char*>(&amountY), sizeof(int));

                *stream << "\n"; //write divider
                *stream << ptr->diffuse_relpath << " " << ptr->mesh_string;
                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE:{
                Engine::TileProperty* ptr = static_cast<Engine::TileProperty*>(property_ptr);
                if(ptr->diffuse_relpath.empty()) //check if object has no texture
                    *stream << "@none";
                else
                    *stream << ptr->diffuse_relpath << "\n";

                if(ptr->transparent_relpath.empty()) //check if object has no texture
                    *stream << "@none";
                else
                    *stream << ptr->transparent_relpath << "\n";

                //Animation stuff
                stream->write(reinterpret_cast<char*>(&ptr->anim_property.isAnimated), sizeof(bool));
                if(ptr->anim_property.isAnimated){ //if animated, then write animation properties
                    stream->write(reinterpret_cast<char*>(&ptr->anim_property.framesX), sizeof(int));
                    stream->write(reinterpret_cast<char*>(&ptr->anim_property.framesY), sizeof(int));
                }
                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_SKYBOX:{
                break;
            }
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_CHARACTER_CONTROLLER:{
                Engine::CharacterControllerProperty* ptr = static_cast<Engine::CharacterControllerProperty*>(property_ptr);
                stream->write(reinterpret_cast<char*>(&ptr->width), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->height), sizeof(float));

                stream->write(reinterpret_cast<char*>(&ptr->transform_offset.X), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->transform_offset.Y), sizeof(float));
                stream->write(reinterpret_cast<char*>(&ptr->transform_offset.Z), sizeof(float));

                    break;
            }
        }
    }
    for (unsigned int script_i = 0; script_i < this->scripts_num; script_i++) {
        Engine::ZPScriptProperty* script = static_cast<Engine::ZPScriptProperty*>(scripts[script_i]);
        *stream << "\nG_SCRIPT ";
        stream->write(reinterpret_cast<char*>(&script->active), sizeof(bool));
        *stream << " ";
    }
}