#include "headers/zs-pipeline.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include <iostream>

RenderPipeline::RenderPipeline(){
    this->current_state = PIPELINE_STATE_DEFAULT;
}

void RenderPipeline::setup(){
    this->tile_shader.compileFromFile("Shaders/2d_tile/tile2d.vs", "Shaders/2d_tile/tile2d.fs");
    this->pick_shader.compileFromFile("Shaders/pick/pick.vs", "Shaders/pick/pick.fs");
    this->obj_mark_shader.compileFromFile("Shaders/mark/mark.vs", "Shaders/mark/mark.fs");
    this->deffered_light.compileFromFile("Shaders/postprocess/deffered_light/deffered.vs", "Shaders/postprocess/deffered_light/deffered.fs");
    ZSPIRE::createPlane2D();

    this->gbuffer.create(640, 480);
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
    this->cam = cam_ptr;
    this->win_ptr = editwin_ptr;
    gbuffer.bindFramebuffer();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->updateShadersCameraInfo(cam_ptr); //Send camera properties to all drawing shaders

    for(unsigned int obj_i = 0; obj_i < world_ptr->objects.size(); obj_i ++){
        GameObject* obj_ptr = &world_ptr->objects[obj_i];
        if(!obj_ptr->hasParent)
            obj_ptr->Draw(this);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    gbuffer.bindTextures();
    deffered_light.Use();
    ZSPIRE::getPlaneMesh2D()->Draw();

    SDL_GL_SwapWindow(w);
}

void GameObject::Draw(RenderPipeline* pipeline){
    if(active == false) return; //if object is inactive, not to render it
    TransformProperty* transform_prop = static_cast<TransformProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));

    LightsourceProperty* light = static_cast<LightsourceProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_LIGHTSOURCE));
    if(light != nullptr && !light->isSent){ //if object has lightsource
        pipeline->addLight(static_cast<void*>(light)); //put light pointer to vector
    }

    ZSPIRE::Shader* shader = pipeline->processShaderOnObject(static_cast<void*>(this)); //Will be used next time
    if(shader != nullptr && transform_prop != nullptr){

        transform_prop->updateMat();
        shader->setTransform(transform_prop->transform_mat);

        MeshProperty* mesh_prop = static_cast<MeshProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
        if(mesh_prop != nullptr){
            mesh_prop->mesh_ptr->Draw();
            //if object is picked
            if(this->isPicked == true && pipeline->current_state != PIPELINE_STATE_PICKING){
                int cur_state = pipeline->current_state; //Storing current state
                pipeline->current_state = PIPELINE_STATE_MARKED;
                ZSPIRE::Shader* mark_s = pipeline->processShaderOnObject(static_cast<void*>(this));
                mark_s->setTransform(transform_prop->transform_mat);
                EditWindow* w = static_cast<EditWindow*>(pipeline->win_ptr);
                if(w->obj_trstate.isTransforming == true)
                     mark_s->setGLuniformInt("isTransformMark", 1);
                mesh_prop->mesh_ptr->DrawLines();
                pipeline->current_state = cur_state;
                mark_s->setGLuniformInt("isTransformMark", 0);
            }
        }
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

    if(current_state == PIPELINE_STATE_MARKED) {
        obj_mark_shader.Use();
        return &obj_mark_shader;
    }

    if(current_state == PIPELINE_STATE_PICKING) {
        unsigned char* to_send = reinterpret_cast<unsigned char*>(&obj->array_index);
        float r = static_cast<float>(to_send[0]);
        float g = static_cast<float>(to_send[1]);
        float b = static_cast<float>(to_send[2]);
        float a = static_cast<float>(to_send[3]);

        pick_shader.setGLuniformVec4("color", ZSVECTOR4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));

        return &pick_shader;
    }

    switch(obj->render_type){
        case GO_RENDER_TYPE_TILE:{ //if object is 2D tile
            tile_shader.Use();
            result = &tile_shader;
            //Receive pointer to tile information
            TileProperty* tile_ptr = static_cast<TileProperty*>(obj->getPropertyPtrByType(GO_PROPERTY_TYPE_TILE));
            if(tile_ptr->texture_diffuse != nullptr){
                tile_ptr->texture_diffuse->Use(0); //Use this texture
                tile_shader.setHasDiffuseTextureProperty(true); //Shader will use picked diffuse texture

            }else{
                tile_shader.setHasDiffuseTextureProperty(false); //Shader will not use diffuse texture
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
    if(diffuse3d_shader.isCreated == true){
        diffuse3d_shader.Use();
        diffuse3d_shader.setCamera(cam_ptr);
    }

    if(tile_shader.isCreated == true){
        tile_shader.Use();
        tile_shader.setCamera(cam_ptr);
    }
    if(obj_mark_shader.isCreated == true){
        obj_mark_shader.Use();
        obj_mark_shader.setCamera(cam_ptr);
    }
}

void RenderPipeline::addLight(void* light_ptr){
    LightsourceProperty* _light_ptr = static_cast<LightsourceProperty*>(light_ptr);
    _light_ptr->isSent = true;
    _light_ptr->updTransformPtr();
    _light_ptr->id = lights_ptr.size(); //setting id of uniform
    this->lights_ptr.push_back(light_ptr); //pushing pointer
    this->deffered_light.Use(); //correctly put uniforms
    this->deffered_light.sendLight(_light_ptr->id, light_ptr);
    this->deffered_light.setGLuniformInt("lights_amount", lights_ptr.size());
}

G_BUFFER_GL::G_BUFFER_GL(){

}

void G_BUFFER_GL::create(int width, int height){
    glCreateFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    glGenTextures(1, &tDiffuse);
    glBindTexture(GL_TEXTURE_2D, tDiffuse);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tDiffuse, 0);

    glGenTextures(1, &tNormal);
    glBindTexture(GL_TEXTURE_2D, tNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tNormal, 0);

    glGenTextures(1, &tPos);
    glBindTexture(GL_TEXTURE_2D, tPos);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tPos, 0);

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);


    glBindFramebuffer(GL_FRAMEBUFFER, 0); //return back to default
}
void G_BUFFER_GL::bindFramebuffer(){
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
}
void G_BUFFER_GL::bindTextures(){
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, tDiffuse);

    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, tNormal);

    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, tPos);
}
