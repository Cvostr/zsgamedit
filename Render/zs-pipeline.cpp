#include "headers/zs-pipeline.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include <iostream>



void RenderPipeline::setup(){
    this->diffuse_shader.compileFromFile("shaders/object.vs", "shaders/object.fs");
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

void RenderPipeline::render(SDL_Window* w, void* projectedit_ptr)
{

    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    World* world_ptr = &editwin_ptr->world;
    ZSPIRE::Camera* cam_ptr = &editwin_ptr->edit_camera;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    diffuse_shader.Use();
    diffuse_shader.setCamera(cam_ptr);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 1);

    for(unsigned int obj_i = 0; obj_i < world_ptr->objects.size(); obj_i ++){
        GameObject* obj_ptr = &world_ptr->objects[obj_i];
        obj_ptr->Draw(&diffuse_shader);
    }


    SDL_GL_SwapWindow(w);
}

void GameObject::Draw(ZSPIRE::Shader* shader){
    TransformProperty* transform_prop = static_cast<TransformProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    shader->setTransform(transform_prop->transform_mat);

    MeshProperty* mesh_prop = static_cast<MeshProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
    if(mesh_prop != nullptr)
        mesh_prop->mesh_ptr->Draw();
}
