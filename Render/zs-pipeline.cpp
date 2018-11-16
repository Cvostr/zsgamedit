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
    ZSPIRE::Camera* cam_ptr = &editwin_ptr->edit_camera;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    diffuse_shader.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 1);
    //ZSPIRE::getPlaneMesh2D()->Draw();

    SDL_GL_SwapWindow(w);
}

