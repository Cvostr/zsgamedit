#include "../headers/World.h"
#include "../headers/obj_properties.h"
#include "../headers/2dtileproperties.h"
#include "../../ProjEd/headers/ProjectEdit.h"
#include "../../Render/headers/zs-mesh.h"
#include "../../ProjEd/headers/InspEditAreas.h"


void GameObject::saveProperties(std::ofstream* stream){
    unsigned int props_num = static_cast<unsigned int>(this->props_num);

    for(unsigned int prop_i = 0; prop_i < props_num; prop_i ++){
        GameObjectProperty* property_ptr = static_cast<GameObjectProperty*>(this->properties[prop_i]);
        *stream << "\nG_PROPERTY ";
        stream->write(reinterpret_cast<char*>(&property_ptr->type), sizeof(int));
        stream->write(reinterpret_cast<char*>(&property_ptr->active), sizeof(bool));
        *stream << " ";

        switch(property_ptr->type){
        case GO_PROPERTY_TYPE_NONE:{
            break;
        }
        case GO_PROPERTY_TYPE_TRANSFORM:{
            TransformProperty* ptr = static_cast<TransformProperty*>(property_ptr);
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
        case GO_PROPERTY_TYPE_LABEL:{
            LabelProperty* ptr = static_cast<LabelProperty*>(property_ptr);
            *stream << ptr->label.toStdString();
            break;
        }
        case GO_PROPERTY_TYPE_MESH:{
            MeshProperty* ptr = static_cast<MeshProperty*>(property_ptr);
            *stream << ptr->resource_relpath.toStdString();
            break;
        }
        case GO_PROPERTY_TYPE_LIGHTSOURCE:{
            LightsourceProperty* ptr = static_cast<LightsourceProperty*>(property_ptr);
            LIGHTSOURCE_TYPE type = ptr->light_type;
            float intensity = ptr->intensity;
            float range = ptr->range;

            float color_r = ptr->color.r;
            float color_g = ptr->color.g;
            float color_b = ptr->color.b;

            stream->write(reinterpret_cast<char*>(&type), sizeof(LIGHTSOURCE_TYPE));
            stream->write(reinterpret_cast<char*>(&intensity), sizeof(float));
            stream->write(reinterpret_cast<char*>(&range), sizeof(float));

            stream->write(reinterpret_cast<char*>(&color_r), sizeof(float));
            stream->write(reinterpret_cast<char*>(&color_g), sizeof(float));
            stream->write(reinterpret_cast<char*>(&color_b), sizeof(float));

            break;
        }
        case GO_PROPERTY_TYPE_AUDSOURCE:{
            AudioSourceProperty* ptr = static_cast<AudioSourceProperty*>(property_ptr);
            if(ptr->resource_relpath.isEmpty()) //check if object has no texture
                *stream << "@none";
            else
                *stream << ptr->resource_relpath.toStdString() << "\n";

            stream->write(reinterpret_cast<char*>(&ptr->source.source_gain), sizeof(float));
            stream->write(reinterpret_cast<char*>(&ptr->source.source_pitch), sizeof(float));
            stream->write(reinterpret_cast<char*>(&ptr->source.looped), sizeof(bool));
            break;
        }
        case GO_PROPERTY_TYPE_MATERIAL:{
            MaterialProperty* ptr = static_cast<MaterialProperty*>(property_ptr);
            //Write path to material string
            if(ptr->material_ptr != nullptr)
                *stream << ptr->material_path.toStdString(); //Write material relpath
            else
                *stream << "@none";

            break;
        }
        case GO_PROPERTY_TYPE_SCRIPTGROUP:{
            ScriptGroupProperty* ptr = static_cast<ScriptGroupProperty*>(property_ptr);
            int script_num = static_cast<int>(ptr->scr_num);
            //write amount of scripts
            stream->write(reinterpret_cast<char*>(&script_num), sizeof(int));
            *stream << "\n"; //write divider
            for(unsigned int script_w_i = 0; script_w_i < static_cast<unsigned int>(script_num); script_w_i ++){
                 *stream << ptr->path_names[script_w_i].toStdString() << "\n";
            }

            break;
        }
        case GO_PROPERTY_TYPE_COLLIDER:{
            ColliderProperty* ptr = static_cast<ColliderProperty*>(property_ptr);
            //write collider type
            stream->write(reinterpret_cast<char*>(&ptr->coll_type), sizeof(COLLIDER_TYPE));
            //write isTrigger boolean
            stream->write(reinterpret_cast<char*>(&ptr->isTrigger), sizeof(bool));
            *stream << "\n"; //write divider

            break;
        }
        case GO_PROPERTY_TYPE_RIGIDBODY:{
            RigidbodyProperty* ptr = static_cast<RigidbodyProperty*>(property_ptr);
            //write collider type
            stream->write(reinterpret_cast<char*>(&ptr->hasGravity), sizeof(bool));
            //write isTrigger boolean
            stream->write(reinterpret_cast<char*>(&ptr->mass), sizeof(float));

            break;
        }
        case GO_PROPERTY_TYPE_TILE_GROUP:{
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
            *stream << ptr->diffuse_relpath.toStdString() << " " << ptr->mesh_string.toStdString();
            break;
        }
        case GO_PROPERTY_TYPE_TILE:{
            TileProperty* ptr = static_cast<TileProperty*>(property_ptr);
            if(ptr->diffuse_relpath.isEmpty()) //check if object has no texture
                *stream << "@none";
            else
                *stream << ptr->diffuse_relpath.toStdString() << "\n";

            if(ptr->transparent_relpath.isEmpty()) //check if object has no texture
                *stream << "@none";
            else
                *stream << ptr->transparent_relpath.toStdString() << "\n";

            //Animation stuff
            stream->write(reinterpret_cast<char*>(&ptr->anim_property.isAnimated), sizeof(bool));
            if(ptr->anim_property.isAnimated){ //if animated, then write animation properties
                stream->write(reinterpret_cast<char*>(&ptr->anim_property.framesX), sizeof(int));
                stream->write(reinterpret_cast<char*>(&ptr->anim_property.framesY), sizeof(int));
            }
            break;
        }
        }
    }
}

//----------------------------------------------LOADING PROPS AREA

void GameObject::loadProperty(std::ifstream* world_stream){
    PROPERTY_TYPE type;
    world_stream->seekg(1, std::ofstream::cur); //Skip space
    world_stream->read(reinterpret_cast<char*>(&type), sizeof(int));
    //Spawn new property with readed type
    addProperty(type);
    GameObjectProperty* prop_ptr = getPropertyPtrByType(type); //get created property
    //since more than 1 properties same type can't be on one gameobject
    world_stream->read(reinterpret_cast<char*>(&prop_ptr->active), sizeof(bool));
    switch(type){
        case GO_PROPERTY_TYPE_NONE :{
            break;
        }
        case GO_PROPERTY_TYPE_LABEL :{
            std::string label;
            *world_stream >> label;
            LabelProperty* lptr = static_cast<LabelProperty*>(prop_ptr);
            this->label = &lptr->label; //Making GameObjects's pointer to string in label property
            lptr->label = QString::fromStdString(label); //Write loaded string
            lptr->list_item_ptr->setText(0, lptr->label); //Set text on widget
            break;
        }
        case GO_PROPERTY_TYPE_TRANSFORM :{
            world_stream->seekg(1, std::ofstream::cur); //Skip space
            TransformProperty* t_ptr = static_cast<TransformProperty*>(prop_ptr);
            world_stream->read(reinterpret_cast<char*>(&t_ptr->translation.X), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->translation.Y), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->translation.Z), sizeof(float));

            world_stream->read(reinterpret_cast<char*>(&t_ptr->scale.X), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->scale.Y), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->scale.Z), sizeof(float));

            world_stream->read(reinterpret_cast<char*>(&t_ptr->rotation.X), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->rotation.Y), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->rotation.Z), sizeof(float));

            //t_ptr->updateMat(); //After everything is loaded, update matrices

        break;
    }
    case GO_PROPERTY_TYPE_MESH :{
        std::string rel_path;
        *world_stream >> rel_path;
        MeshProperty* lptr = static_cast<MeshProperty*>(prop_ptr);
        lptr->resource_relpath = QString::fromStdString(rel_path); //Write loaded mesh relative path
        lptr->updateMeshPtr(); //Pointer will now point to mesh resource

        break;
    }
    case GO_PROPERTY_TYPE_LIGHTSOURCE:{
        LightsourceProperty* ptr = static_cast<LightsourceProperty*>(prop_ptr);
        world_stream->seekg(1, std::ofstream::cur);

        world_stream->read(reinterpret_cast<char*>(&ptr->light_type), sizeof(LIGHTSOURCE_TYPE));
        world_stream->read(reinterpret_cast<char*>(&ptr->intensity), sizeof(float));
        world_stream->read(reinterpret_cast<char*>(&ptr->range), sizeof(float));

        float cl_r;
        float cl_g;
        float cl_b;

        world_stream->read(reinterpret_cast<char*>(&cl_r), sizeof(float));
        world_stream->read(reinterpret_cast<char*>(&cl_g), sizeof(float));
        world_stream->read(reinterpret_cast<char*>(&cl_b), sizeof(float));
        ptr->color = ZSRGBCOLOR(cl_r, cl_g, cl_b);

        break;
    }
    case GO_PROPERTY_TYPE_SCRIPTGROUP:{
        ScriptGroupProperty* ptr = static_cast<ScriptGroupProperty*>(prop_ptr);
        world_stream->seekg(1, std::ofstream::cur);
        //Read scripts number
        world_stream->read(reinterpret_cast<char*>(&ptr->scr_num), sizeof(int));
        world_stream->seekg(1, std::ofstream::cur);
        //resize arrays
        ptr->path_names.resize(static_cast<unsigned int>(ptr->scr_num));
        ptr->scripts_attached.resize(static_cast<unsigned int>(ptr->scr_num));
        //iterate over all scripts and read their path
        Project* project_ptr = static_cast<Project*>(this->world_ptr->proj_ptr);
        for(unsigned int script_w_i = 0; script_w_i < static_cast<unsigned int>(ptr->scr_num); script_w_i ++){
            std::string scr_path;
            *world_stream >> scr_path;
            ptr->path_names[script_w_i] = QString::fromStdString(scr_path);

            ptr->scripts_attached[script_w_i].fpath = project_ptr->root_path + "/" + ptr->path_names[script_w_i];
            ptr->scripts_attached[script_w_i].name = ptr->path_names[script_w_i].toStdString();
        }

        //ptr->onValueChanged();

        break;
    }
    case GO_PROPERTY_TYPE_AUDSOURCE:{
        std::string rel_path;
        *world_stream >> rel_path;
        AudioSourceProperty* lptr = static_cast<AudioSourceProperty*>(prop_ptr);
        if(rel_path.compare("@none") != 0){
            lptr->resource_relpath = QString::fromStdString(rel_path); //Write loaded mesh relative path
            lptr->updateAudioPtr(); //Pointer will now point to mesh resource
        }
        world_stream->seekg(1, std::ofstream::cur);
        //Load settings
        world_stream->read(reinterpret_cast<char*>(&lptr->source.source_gain), sizeof(float));
        world_stream->read(reinterpret_cast<char*>(&lptr->source.source_pitch), sizeof(float));
        world_stream->read(reinterpret_cast<char*>(&lptr->source.looped), sizeof(bool));
        lptr->source.apply_settings(); //Apply settings to openal

        break;
    }
    case GO_PROPERTY_TYPE_MATERIAL:{
        MaterialProperty* ptr = static_cast<MaterialProperty*>(prop_ptr);

        std::string path;
        //reading path
        *world_stream >> path;
        //Assigning path
        ptr->material_path = QString::fromStdString(path);
        if(path.compare("@none"))
            ptr->onValueChanged();

        break;
    }
    case GO_PROPERTY_TYPE_COLLIDER:{
        ColliderProperty* ptr = static_cast<ColliderProperty*>(prop_ptr);
        world_stream->seekg(1, std::ofstream::cur);
        //read collider type
        world_stream->read(reinterpret_cast<char*>(&ptr->coll_type), sizeof(COLLIDER_TYPE));
        //read isTrigger boolean
        world_stream->read(reinterpret_cast<char*>(&ptr->isTrigger), sizeof(bool));

        world_ptr->pushCollider(ptr); //send collider to world

        break;
    }
    case GO_PROPERTY_TYPE_RIGIDBODY:{
        RigidbodyProperty* ptr = static_cast<RigidbodyProperty*>(prop_ptr);
        world_stream->seekg(1, std::ofstream::cur);
        //read collider type
        world_stream->read(reinterpret_cast<char*>(&ptr->hasGravity), sizeof(bool));
        //read isTrigger boolean
        world_stream->read(reinterpret_cast<char*>(&ptr->mass), sizeof(float));

        break;
    }
    case GO_PROPERTY_TYPE_TILE_GROUP :{
        world_stream->seekg(1, std::ofstream::cur); //Skip space
        TileGroupProperty* t_ptr = static_cast<TileGroupProperty*>(prop_ptr);
        int isCreated = 0;
        world_stream->read(reinterpret_cast<char*>(&isCreated), sizeof(int));
        world_stream->read(reinterpret_cast<char*>(&t_ptr->geometry.tileWidth), sizeof(int));
        world_stream->read(reinterpret_cast<char*>(&t_ptr->geometry.tileHeight), sizeof(int));

        world_stream->read(reinterpret_cast<char*>(&t_ptr->tiles_amount_X), sizeof(int));
        world_stream->read(reinterpret_cast<char*>(&t_ptr->tiles_amount_Y), sizeof(int));

        t_ptr->isCreated = static_cast<bool>(isCreated);

        world_stream->seekg(1, std::ofstream::cur); //Skip space

        std::string diffuse_relpath, mesh_relpath;

        *world_stream >> diffuse_relpath >> mesh_relpath;

        t_ptr->diffuse_relpath = QString::fromStdString(diffuse_relpath);
        t_ptr->mesh_string = QString::fromStdString(mesh_relpath);

        break;
    }
    case GO_PROPERTY_TYPE_TILE:{
        std::string diffuse_rel_path;
        *world_stream >> diffuse_rel_path;
        TileProperty* lptr = static_cast<TileProperty*>(prop_ptr);
        if(diffuse_rel_path.compare("@none") != 0){
            lptr->diffuse_relpath = QString::fromStdString(diffuse_rel_path); //Write loaded mesh relative path
        }

        std::string transparent_rel_path;
        *world_stream >> transparent_rel_path;
        if(transparent_rel_path.compare("@none") != 0){
            lptr->transparent_relpath = QString::fromStdString(transparent_rel_path); //Write loaded mesh relative path
        }

        lptr->updTexturePtr(); //set pointers to textures in tile property

        world_stream->seekg(1, std::ofstream::cur);
        world_stream->read(reinterpret_cast<char*>(&lptr->anim_property.isAnimated), sizeof(bool));
        if(lptr->anim_property.isAnimated){ //if animated, then write animation properties
            world_stream->read(reinterpret_cast<char*>(&lptr->anim_property.framesX), sizeof(int));
            world_stream->read(reinterpret_cast<char*>(&lptr->anim_property.framesY), sizeof(int));
        }

        break;
        }
    }
}
