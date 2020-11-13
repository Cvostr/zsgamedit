#include "headers/zs-pipeline.h"
#include "../World/headers/World.h"
#include "world/go_properties.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include <iostream>

#define LIGHT_STRUCT_SIZE 64

extern ZSGAME_DATA* game_data;

RenderPipelineEditor::RenderPipelineEditor(){
    this->pick_shader = nullptr;
    this->obj_mark_shader = nullptr;
    this->obj_grid_shader = nullptr;
    editorUniformBuffer = nullptr;
    gizmos = nullptr;
    cam = nullptr;
    win_ptr = nullptr;
}

GizmosRenderer* RenderPipelineEditor::getGizmosRenderer(){
    return this->gizmos;
}

void RenderPipelineEditor::setup(int bufWidth, int bufHeight){
    this->pick_shader = Engine::allocShader();
    this->obj_mark_shader = Engine::allocShader();
    this->obj_grid_shader = Engine::allocShader();

    this->pick_shader->compileFromFile("Shaders/pick/pick.vert", "Shaders/pick/pick.frag");
    this->obj_mark_shader->compileFromFile("Shaders/mark/mark.vert", "Shaders/mark/mark.frag");
    this->obj_grid_shader->compileFromFile("Shaders/mark/grid.vert", "Shaders/mark/mark.frag");
    //create geometry buffer
    create_G_Buffer(bufWidth, bufHeight);

    removeLights();

    editorUniformBuffer = Engine::allocUniformBuffer();
    editorUniformBuffer->init(8, 16);
}

void RenderPipelineEditor::initGizmos(int projectPespective){
    gizmos = new GizmosRenderer(obj_mark_shader, 
                                obj_grid_shader,
                                this->cullFaces,
                                projectPespective,
                                transformBuffer,
                                editorUniformBuffer,
                                this->instancedTransformBuffer);
}

RenderPipelineEditor::~RenderPipelineEditor(){
    this->pick_shader->Destroy();
    this->obj_mark_shader->Destroy();

    removeLights();
    delete gizmos;
}

bool RenderPipelineEditor::InitGLEW(){
    glewExperimental = GL_TRUE;
    std::cout << "Calling GLEW creation" << std::endl;

    if (glewInit() != GLEW_OK){
        std::cout << "OPENGL GLEW: Creation failed ";
        return false;
    }

    std::cout << "GLEW creation successful" << std::endl;
        return true;
}

void RenderPipelineEditor::init(){
    setFullscreenViewport(this->WIDTH, this->HEIGHT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(16.0f);

    //setup GBUFFER and shaders
    setup(this->WIDTH, this->HEIGHT);
    //initialize gizmos component
    initGizmos(this->project_struct_ptr->perspective);
}

ZSRGBCOLOR RenderPipelineEditor::getColorOfPickedTransformControl(int mouseX, int mouseY, void* projectedit_ptr){

    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    Engine::Camera* cam_ptr = nullptr; //We'll set it next
    World* world_ptr = &editwin_ptr->world;

    if(editwin_ptr->isWorldCamera){
        //if isWorldCamera is true, then we are in gameplay camera
        cam_ptr = &world_ptr->world_camera;
    }else{
        //if isWorldCamera is false, then we are in editor camera
        cam_ptr = &editwin_ptr->edit_camera;
    }

    setClearColor(0,0,0,1);
    ClearFBufferGL(true, true);
    //Picking state

    setDepthState(false);
    setFaceCullState(false);

    if(editwin_ptr->obj_trstate.isTransforming == true && !editwin_ptr->isWorldCamera){
        //Calclate distance between camera and object
        float dist = getDistance(cam_ptr->camera_pos, editwin_ptr->obj_trstate.obj_ptr->getPropertyPtr<Engine::TransformProperty>()->abs_translation);

        if(this->project_struct_ptr->perspective == 2) dist = 85.0f;
        //Draw gizmos
        getGizmosRenderer()->drawTransformControls(editwin_ptr->obj_trstate.obj_ptr->getPropertyPtr<Engine::TransformProperty>()->abs_translation, dist, dist / 10.f);
    }

    unsigned char data[4];
    glReadPixels(mouseX, this->HEIGHT - mouseY, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    return ZSRGBCOLOR(data[0], data[1], data[2]);
}

unsigned int RenderPipelineEditor::render_getpickedObj(void* projectedit_ptr, int mouseX, int mouseY){
    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    World* world_ptr = &editwin_ptr->world;

    setClearColor(1,1,1,1);
    this->ClearFBufferGL(true, true);
    setBlendingState(false);
    pick_shader->Use();
    //Picking state
    this->current_state = PIPELINE_STATE::PIPELINE_STATE_PICKING;

    setDepthState(true);

    if (cullFaces == true)
        setFaceCullState(true);

    //Iterate over all objects in the world
    for(unsigned int obj_i = 0; obj_i < world_ptr->objects.size(); obj_i ++){
        Engine::GameObject* obj_ptr = world_ptr->objects[obj_i];
        //Check, if object deleted or deactivated
        if (obj_ptr->alive == false || obj_ptr->isActive() == false) 
            //if so, then skip it
            continue;

        //If we picking object
        if (current_state == PIPELINE_STATE::PIPELINE_STATE_PICKING) {
            Engine::TransformProperty* transform_ptr = obj_ptr->getTransformProperty();

            unsigned char* to_send = reinterpret_cast<unsigned char*>(&obj_ptr->array_index);
            float r = static_cast<float>(to_send[0]);
            float g = static_cast<float>(to_send[1]);
            float b = static_cast<float>(to_send[2]);
            float a = static_cast<float>(to_send[3]);
            ZSVECTOR4 color = ZSVECTOR4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);

            //set transform to camera buffer
            transformBuffer->bind();
            transformBuffer->writeData(sizeof(Mat4) * 2, sizeof(Mat4), &transform_ptr->transform_mat);

            editorUniformBuffer->bind();
            editorUniformBuffer->writeData(0, 16, &color);
            obj_ptr->DrawMesh(this);
        }
    }


    unsigned char data[4];
    glReadPixels(mouseX, this->HEIGHT - mouseY, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    data[3] = 0;
    unsigned int* pr_data_ = reinterpret_cast<unsigned int*>(&data[0]);
    unsigned int pr_data = *pr_data_;

    if(data[2] == 255) 
        pr_data = 256 * 256 * 256; //If we haven't picked any object

    this->current_state = PIPELINE_STATE::PIPELINE_STATE_DEFAULT;

    return pr_data;
}

void RenderPipelineEditor::render(SDL_Window* w, void* projectedit_ptr){
    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    Engine::Camera* cam_ptr = nullptr; //We'll set it next
    World* world_ptr = &editwin_ptr->world;

    if(editwin_ptr->isWorldCamera){
        //if isWorldCamera is true, then we are in gameplay camera
        cam_ptr = &world_ptr->world_camera;
    }else{
        //if isWorldCamera is false, then we are in editor camera
        cam_ptr = &editwin_ptr->edit_camera;
    }

    this->cam = cam_ptr;
    this->win_ptr = editwin_ptr;
    this->updateShadersCameraInfo(cam_ptr); //Send camera properties to all drawing shaders
    setLightsToBuffer();

    switch(this->project_struct_ptr->perspective){
        case PERSP_2D:{
            render2D();
            break;
        }
        case PERSP_3D:{
            render3D(cam);
            break;
        }
    }
    renderGizmos(projectedit_ptr, cam);

    SDL_GL_SwapWindow(w); //Send rendered frame
}

void RenderPipelineEditor::renderGizmos(void* projectedit_ptr, Engine::Camera* cam) {
    //Get all pointers
    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    World* world_ptr = &editwin_ptr->world;
    //iterate over all objects
    for (unsigned int obj_i = 0; obj_i < world_ptr->objects.size(); obj_i++) {
        //get object pointer
        Engine::GameObject* obj_ptr = world_ptr->objects[obj_i];
        //Check, if object is alive and active
        if (obj_ptr->mActive && world_ptr->isPicked(obj_ptr) && current_state == PIPELINE_STATE::PIPELINE_STATE_DEFAULT) {
            setDepthState(false);
            setFaceCullState(false);

            EditWindow* editwin_ptr = static_cast<EditWindow*>(win_ptr);
            Engine::TransformProperty* transform_ptr = obj_ptr->getTransformProperty();
            Engine::MeshProperty* mesh_prop = obj_ptr->getPropertyPtr<Engine::MeshProperty>();
            ZSRGBCOLOR color = ZSRGBCOLOR(static_cast<int>(0.23f * 255.0f),
                static_cast<int>(0.23f * 255.0f),
                static_cast<int>(0.54f * 255.0f));
            if (editwin_ptr->obj_trstate.isTransforming == true)
                color = ZSRGBCOLOR(255.0f, 255.0f, 0.0f);
            //draw wireframe mesh for picked object
            if (!editwin_ptr->isWorldCamera && obj_ptr->hasMesh() && obj_ptr->mActive) { //avoid drawing gizmos during playtime
                //Draw pick mesh
                getGizmosRenderer()->drawPickedMeshWireframe(mesh_prop->mesh_ptr->mesh_ptr, transform_ptr->transform_mat, color);
                //Draw collider
                getGizmosRenderer()->drawObjectRigidbodyShape(obj_ptr->getPhysicalProperty());
            }
            //Return Depth and face cull back
            setDepthState(true);
            glEnable(GL_CULL_FACE);
        }
    }

    //if we control this object
    //Draw Transform controls
    if (editwin_ptr->obj_trstate.isTransforming == true && !editwin_ptr->isWorldCamera) {
        //calculate distance between object and camera
        //To set constant size of controls on any distance
        float dist = getDistance(cam->camera_pos, editwin_ptr->obj_trstate.obj_ptr->getPropertyPtr<Engine::TransformProperty>()->abs_translation);
        //if we are in 2D mode, then distance is constant
        if (this->project_struct_ptr->perspective == PERSP_2D) dist = 70.0f;
        getGizmosRenderer()->drawTransformControls(editwin_ptr->obj_trstate.obj_ptr->getPropertyPtr<Engine::TransformProperty>()->abs_translation, dist, dist / 10.f);
    }
}

Engine::Shader* RenderPipelineEditor::getPickingShader(){
    return this->pick_shader;
}
