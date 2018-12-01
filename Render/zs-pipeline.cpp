#include "headers/zs-pipeline.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include <iostream>

RenderPipeline::RenderPipeline(){
    this->current_state = PIPELINE_STATE_DEFAULT;
}

void RenderPipeline::setup(){
    this->tile_shader.compileFromFile("shaders/2d_tile/tile2d.vs", "shaders/2d_tile/tile2d.fs");
    this->pick_shader.compileFromFile("shaders/pick/pick.vs", "shaders/pick/pick.fs");
    ZSPIRE::createPlane2D();
}

bool RenderPipeline::InitGLEW(){
    glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
        {
            std::cout << "OPENGL GLEW: Creation failed ";
            return false;
        }
        return true;
}

unsigned int RenderPipeline::render_getpickedObj(void* projectedit_ptr, int mouseX, int mouseY){
    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    World* world_ptr = &editwin_ptr->world;
    ZSPIRE::Camera* cam_ptr = &editwin_ptr->edit_camera;

    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pick_shader.Use();
    pick_shader.setCamera(cam_ptr);

    this->current_state = PIPELINE_STATE_PICKING;
    for(unsigned int obj_i = 0; obj_i < world_ptr->objects.size(); obj_i ++){
        GameObject* obj_ptr = &world_ptr->objects[obj_i];
        if(!obj_ptr->hasParent)
            obj_ptr->Draw(this);
    }

    unsigned char data[4];
    glReadPixels(mouseX, 480 - mouseY, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //unsigned int pr_data = data[0] + data[1] * 256 + data[2] * 256 * 256; //Calc object ID
    data[3] = 0;
    unsigned int* pr_data_ = reinterpret_cast<unsigned int*>(&data[0]);
    unsigned int pr_data = *pr_data_;

    if(data[2] == 255) pr_data = 256 * 256 * 256; //If we haven't picked any object

    this->current_state = PIPELINE_STATE_DEFAULT;

    return pr_data;
}

void RenderPipeline::render(SDL_Window* w, void* projectedit_ptr)
{

    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    World* world_ptr = &editwin_ptr->world;
    ZSPIRE::Camera* cam_ptr = &editwin_ptr->edit_camera;

    glClearColor(1,0,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->updateShadersCameraInfo(cam_ptr); //Send camera properties to all drawing shaders

    for(unsigned int obj_i = 0; obj_i < world_ptr->objects.size(); obj_i ++){
        GameObject* obj_ptr = &world_ptr->objects[obj_i];
        if(!obj_ptr->hasParent)
            obj_ptr->Draw(this);
    }


    SDL_GL_SwapWindow(w);
}

void GameObject::Draw(RenderPipeline* pipeline){
    TransformProperty* transform_prop = static_cast<TransformProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    transform_prop->updateMat();
    ZSPIRE::Shader* shader = pipeline->processShaderOnObject(static_cast<void*>(this)); //Will be used next time
    if(shader != nullptr){

    shader->setTransform(transform_prop->transform_mat);
/*
    unsigned char* to_send = reinterpret_cast<unsigned char*>(&this->array_index);
    float r = static_cast<float>(to_send[0]);
    float g = static_cast<float>(to_send[1]);
    float b = static_cast<float>(to_send[2]);
    float a = static_cast<float>(to_send[3]);

    shader->setGLuniformVec4("color", ZSVECTOR4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
*/
    MeshProperty* mesh_prop = static_cast<MeshProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
    if(mesh_prop != nullptr)
        mesh_prop->mesh_ptr->Draw();
    }
    for(unsigned int obj_i = 0; obj_i < this->children.size(); obj_i ++){
        if(!children[obj_i].isEmpty()){
            children[obj_i].updLinkPtr();
            GameObject* child_ptr = this->children[obj_i].ptr;
            child_ptr->Draw(pipeline);
        }
    }
}

ZSPIRE::Shader* RenderPipeline::processShaderOnObject(void* _obj){
    GameObject* obj = static_cast<GameObject*>(_obj);
    ZSPIRE::Shader* result;

    if(current_state == PIPELINE_STATE_PICKING) {
        //pick_shader.Use();
        unsigned char* to_send = reinterpret_cast<unsigned char*>(&obj->array_index);
        float r = static_cast<float>(to_send[0]);
        float g = static_cast<float>(to_send[1]);
        float b = static_cast<float>(to_send[2]);
        float a = static_cast<float>(to_send[3]);

        pick_shader.setGLuniformVec4("color", ZSVECTOR4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));

        return &pick_shader;
    }

    switch(obj->render_type){
        case GO_RENDER_TYPE_TILE:{
            tile_shader.Use();
            result = &tile_shader;
            //Receive pointer to tile information
            TileProperty* tile_ptr = static_cast<TileProperty*>(obj->getPropertyPtrByType(GO_PROPERTY_TYPE_TILE));
            if(tile_ptr->texture_diffuse != nullptr){
                tile_ptr->texture_diffuse->Use(0); //Use this texture
            }else{
                tile_shader.setHasDiffuseTextureProperty(true); //Shader will not use diffuse texture
            }
            break;
        }
    case GO_RENDER_TYPE_NONE:{
        result = nullptr;
        break;
    }
    }
    return result;
}

void RenderPipeline::updateShadersCameraInfo(ZSPIRE::Camera* cam_ptr){
    if(diffuse_shader.isCreated == true){
        diffuse_shader.Use();
        diffuse_shader.setCamera(cam_ptr);
    }

    if(tile_shader.isCreated == true){
        tile_shader.Use();
        tile_shader.setCamera(cam_ptr);
    }
}
