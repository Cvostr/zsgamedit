#include "headers/zs-renderer-editor.hpp"
#include "../World/headers/World.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include <iostream>

#include <world/ObjectsComponents/MeshComponent.hpp>
#include <world/ObjectsComponents/LightSourceComponent.hpp>
#include <ogl/GLFramebuffer.hpp>

#define LIGHT_STRUCT_SIZE 64

extern ZSGAME_DATA* game_data;
//Hack to support meshes
extern ZSpireEngine* engine_ptr;

RenderPipelineEditor::RenderPipelineEditor():
    pick_shader(nullptr),
    obj_mark_shader(nullptr),
    obj_grid_shader(nullptr),
    editorUniformBuffer(nullptr),
    gizmos(nullptr),
    win_ptr(nullptr) {   }

GizmosRenderer* RenderPipelineEditor::getGizmosRenderer(){
    return this->gizmos;
}

void RenderPipelineEditor::setup(int bufWidth, int bufHeight){
    this->pick_shader = Engine::allocShader();
    this->obj_mark_shader = Engine::allocShader();
    this->obj_grid_shader = Engine::allocShader();
    sprite_shader_3d = Engine::allocShader();

    this->pick_shader->compileFromFile("Shaders/pick/pick.vert", "Shaders/pick/pick.frag");
    this->obj_mark_shader->compileFromFile("Shaders/mark/mark.vert", "Shaders/mark/mark.frag");
    this->obj_grid_shader->compileFromFile("Shaders/mark/grid.vert", "Shaders/mark/mark.frag");
    sprite_shader_3d->compileFromFile("Shaders/ui/sprite.vert", "Shaders/ui/ui.frag");
    //create geometry buffer
    create_G_Buffer_GL(bufWidth, bufHeight);

    editorUniformBuffer = Engine::allocUniformBuffer();
    editorUniformBuffer->init(8, 16);

    gbuffer = new Engine::GLframebuffer(bufWidth, bufHeight);



    gbuffer->AddDepth();
    gbuffer->AddTexture(Engine::FORMAT_RGBA); //Diffuse map
    gbuffer->AddTexture(Engine::FORMAT_RGB16F); //Normal map
    gbuffer->AddTexture(Engine::FORMAT_RGB16F); //Position map
    gbuffer->AddTexture(Engine::FORMAT_RGBA); //Transparent map
    gbuffer->AddTexture(Engine::FORMAT_RGBA); //Masks map
    gbuffer->Create();

    df_light_buffer = new Engine::GLframebuffer(bufWidth, bufHeight);
    df_light_buffer->AddTexture(Engine::FORMAT_RGBA); //Diffuse map
    df_light_buffer->AddTexture(Engine::FORMAT_RGBA); //Bloom map
    df_light_buffer->Create();
}

void RenderPipelineEditor::initGizmos(int projectPespective){
    gizmos = new GizmosRenderer(obj_mark_shader, 
                                obj_grid_shader,
                                sprite_shader_3d,
                                cullFaces,
                                projectPespective,
                                transformBuffer,
                                editorUniformBuffer,
                                instancedTransformBuffer,
                                uiUniformBuffer);
}

RenderPipelineEditor::~RenderPipelineEditor(){
    this->pick_shader->Destroy();
    this->obj_mark_shader->Destroy();
    obj_grid_shader->Destroy();
    sprite_shader_3d->Destroy();

    delete gizmos;

    delete gbuffer;
    delete df_light_buffer;
}

void RenderPipelineEditor::OnUpdateWindowSize(int W, int H) {
    if (engine_ptr->desc->game_perspective == PERSP_3D) {
        gbuffer->SetSize(W, H);
        df_light_buffer->SetSize(W, H);
    }
    ui_buffer->SetSize(W, H);
}

void RenderPipelineEditor::OnCreate(){
    Engine::Window* win = engine_ptr->GetWindow();
    setFullscreenViewport(win->GetWindowWidth(), win->GetWindowHeight());

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(16.0f);

    //setup GBUFFER and shaders
    setup(win->GetWindowWidth(), win->GetWindowHeight());
    //initialize gizmos component
    initGizmos(engine_ptr->desc->game_perspective);
}

RGBAColor RenderPipelineEditor::getColorOfPickedTransformControl(int mouseX, int mouseY, void* projectedit_ptr){

    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    Engine::Camera* cam_ptr = &editwin_ptr->edit_camera; //We'll set it next
    World* world_ptr = &editwin_ptr->world;


    setClearColor(0,0,0,1);
    ClearFBufferGL(true, true);
    //Picking state

    setDepthState(false);
    setFaceCullState(false);

    if(editwin_ptr->obj_trstate.isTransforming == true && !editwin_ptr->isWorldCamera){
        //Calclate distance between camera and object
        float dist = getDistance(cam_ptr->getCameraPosition(), editwin_ptr->obj_trstate.obj_ptr->getPropertyPtr<Engine::TransformProperty>()->abs_translation);

        if(engine_ptr->desc->game_perspective == 2) dist = 85.0f;
        //Draw gizmos
        getGizmosRenderer()->drawTransformControls(editwin_ptr->obj_trstate.obj_ptr->getPropertyPtr<Engine::TransformProperty>()->abs_translation, dist, dist / 10.f);
    }

    unsigned char data[4];
    Engine::Window* win = engine_ptr->GetWindow();
    glReadPixels(mouseX, win->GetWindowHeight() - mouseY, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    return RGBAColor(data[0], data[1], data[2]);
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
        if (obj_ptr->mAlive == false || obj_ptr->isActive() == false)
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
            Vec4 color = Vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);

            //set transform to camera buffer
            transformBuffer->bind();
            transformBuffer->writeData(sizeof(Mat4) * 2, sizeof(Mat4), &transform_ptr->transform_mat);

            editorUniformBuffer->bind();
            editorUniformBuffer->writeData(0, 16, &color);
            obj_ptr->DrawMesh(this);
        }
    }

    Engine::Window* win = engine_ptr->GetWindow();
    unsigned char data[4];
    glReadPixels(mouseX, win->GetWindowHeight() - mouseY, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    data[3] = 0;
    unsigned int* pr_data_ = reinterpret_cast<unsigned int*>(&data[0]);
    unsigned int pr_data = *pr_data_;

    if(data[2] == 255) 
        pr_data = 256 * 256 * 256; //If we haven't picked any object

    this->current_state = PIPELINE_STATE::PIPELINE_STATE_DEFAULT;

    return pr_data;
}

void RenderPipelineEditor::render(Engine::Window* window, void* projectedit_ptr){
    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    Engine::Window* win = engine_ptr->GetWindow();

    World* world_ptr = &editwin_ptr->world;

    this->win_ptr = editwin_ptr;
    
    setLightsToBuffer();

    lookForCameras(world_ptr);
    if (editwin_ptr->isWorldCamera) {
        switch (engine_ptr->desc->game_perspective) {
        case PERSP_2D: {
            render2D();
            break;
        }
        case PERSP_3D: {
            render3D();
            break;
        }
        }
    }
    else {
        Engine::Camera* cam_ptr = &editwin_ptr->edit_camera; //We'll set it next
        this->updateShadersCameraInfo(cam_ptr); //Send camera properties to all drawing shaders
        setFrontFace(CCF_DIRECTION_CCW);
        TryRenderShadows(cam_ptr);
        mMainCamera = cam_ptr;

        {
            //Bind Geometry Buffer to make Deferred Shading
            ((Engine::GLframebuffer*)gbuffer)->bind();
            setClearColor(0, 0, 0, 0);
            ClearFBufferGL(true, true);
            setFullscreenViewport(win->GetWindowWidth(), win->GetWindowHeight());
            {
                //Render Skybox
                setDepthState(false);
                setBlendingState(false);
                setFaceCullState(false);
                TryRenderSkybox();
            }
            glEnablei(GL_BLEND, 0);
            glBlendFunci(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 0);
            {
                //Render World
                setDepthState(true);
                setFaceCullState(true);
                //Render whole world
                processObjects(world_ptr);
            }
        }

        //Process Deffered lights
        {
            ((Engine::GLframebuffer*)df_light_buffer)->bind();
            ClearFBufferGL(true, false); //Clear screen
            //Disable depth rendering to draw plane correctly
            setDepthState(false);
            setFaceCullState(false);
            ((Engine::GLframebuffer*)gbuffer)->bindTextures(0); //Bind gBuffer textures
            deffered_light->Use(); //use deffered shader
            Engine::getPlaneMesh2D()->Draw(); //Draw screen
        }

        //Render ALL UI
        {
            ((Engine::GLframebuffer*)ui_buffer)->bind();
            setClearColor(0, 0, 0, 0);
            ClearFBufferGL(true, false); //Clear screen 
            setDepthState(false);
            setBlendingState(true);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            renderUI();
        }

        //Draw result into main buffer
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            ((Engine::GLframebuffer*)df_light_buffer)->bindTextures(0);
            ((Engine::GLframebuffer*)ui_buffer)->bindTextures(1);
            final_shader->Use();
            Engine::getPlaneMesh2D()->Draw(); //Draw screen
        }

        renderGizmos(projectedit_ptr, cam_ptr);
    }
    //if(!editwin_ptr->isWorldCamera)
        

    window->SwapGL();
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
        if (obj_ptr->mAlive && obj_ptr->hasLightsource()) {
            Engine::TransformProperty* transform_ptr = obj_ptr->getTransformProperty();
            Engine::LightsourceComponent* light_ptr = obj_ptr->getPropertyPtr<Engine::LightsourceComponent>();
            getGizmosRenderer()->drawGizmoSprite(getGizmosRenderer()->SunTextureSprite, transform_ptr->abs_translation, Vec3(2,2,2), cam->getViewMatrix(), light_ptr->color);
        }
        if (obj_ptr->mAlive && obj_ptr->mActive && world_ptr->isPicked(obj_ptr) && current_state == PIPELINE_STATE::PIPELINE_STATE_DEFAULT) {
            setDepthState(false);
            setFaceCullState(false);

            EditWindow* editwin_ptr = static_cast<EditWindow*>(win_ptr);
            Engine::TransformProperty* transform_ptr = obj_ptr->getTransformProperty();
            Engine::MeshProperty* mesh_prop = obj_ptr->getPropertyPtr<Engine::MeshProperty>();
            RGBAColor color = RGBAColor(static_cast<int>(0.23f * 255.0f),
                static_cast<int>(0.23f * 255.0f),
                static_cast<int>(0.54f * 255.0f));
            if (editwin_ptr->obj_trstate.isTransforming == true)
                color = RGBAColor(255.0f, 255.0f, 0.0f);
            //draw wireframe mesh for picked object
            if (obj_ptr->hasMesh() && obj_ptr->mActive) { //avoid drawing gizmos during playtime
                //Draw pick mesh
                getGizmosRenderer()->drawPickedMeshWireframe(mesh_prop->mesh_ptr->mesh_ptr, transform_ptr->transform_mat, color);
                //Draw collider
                getGizmosRenderer()->drawObjectRigidbodyShape(obj_ptr->getPhysicalProperty());
            }
            //Return Depth and face cull back
            setDepthState(true);
            setFaceCullState(true);
        }
    }

    //if we control this object
    //Draw Transform controls
    if (editwin_ptr->obj_trstate.isTransforming == true && !editwin_ptr->isWorldCamera) {
        //calculate distance between object and camera
        //To set constant size of controls on any distance
        float dist = getDistance(cam->getCameraPosition(), editwin_ptr->obj_trstate.obj_ptr->getPropertyPtr<Engine::TransformProperty>()->abs_translation);
        //if we are in 2D mode, then distance is constant
        if (engine_ptr->desc->game_perspective == PERSP_2D) dist = 70.0f;
        getGizmosRenderer()->drawTransformControls(editwin_ptr->obj_trstate.obj_ptr->getPropertyPtr<Engine::TransformProperty>()->abs_translation, dist, dist / 10.f);
    }
}

Engine::Shader* RenderPipelineEditor::getPickingShader(){
    return this->pick_shader;
}
