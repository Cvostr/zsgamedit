#include "../headers/World.h"
#include "../../World/headers/World.h"
#include "world/go_properties.h"
#include <world/tile_properties.h>
#include "../../ProjEd/headers/ProjectEdit.h"
#include <render/zs-mesh.h>
#include "../../ProjEd/headers/InspEditAreas.h"

extern Project* project_ptr;
extern ZSGAME_DATA* game_data;

void Engine::GameObject::saveProperties(std::ofstream* stream){
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
            
            case PROPERTY_TYPE::GO_PROPERTY_TYPE_TERRAIN:{
                Engine::TerrainProperty* ptr = static_cast<Engine::TerrainProperty*>(property_ptr);
                *stream << ptr->file_label << '\0'; //Write material relpath
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
                    *stream << texture_pair->diffuse_relpath << '\0' << texture_pair->normal_relpath << '\0'; //Write material relpath
                }
                //Write info about all vegetable types
                for(int grass_i = 0; grass_i < ptr->grassType_size; grass_i ++){
                    HeightmapGrass* grass_ptr = &ptr->getTerrainData()->grass[static_cast<unsigned int>(grass_i)];
                    //Write grass diffuse texture
                    *stream << grass_ptr->diffuse_relpath << '\0';
                    //Write grass size
                    stream->write(reinterpret_cast<char*>(&grass_ptr->scale.X), sizeof(float));
                    stream->write(reinterpret_cast<char*>(&grass_ptr->scale.Y), sizeof(float));
                    *stream << "\n";
                }
                break;
            }
           
            default : {
                saveProperty(property_ptr, stream);
            }
        }
    }
    for (unsigned int script_i = 0; script_i < this->scripts_num; script_i++) {
        Engine::ZPScriptProperty* script = static_cast<Engine::ZPScriptProperty*>(scripts[script_i]);
        saveProperty(script, stream);
        *stream << '\n';
    }
}

void Engine::GameObject::saveProperty(GameObjectProperty* prop, std::ofstream* stream) {
    auto ptr = prop;
    ptr->savePropertyToStream(stream, this);
}