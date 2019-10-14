#include "headers/zs-pipeline.h"
#include "../World/headers/obj_properties.h"
#include "../World/headers/2dtileproperties.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include <iostream>

#define MAX_LIGHTS_AMOUNT 150

RenderPipeline::RenderPipeline(){
    this->current_state = PIPELINE_STATE_DEFAULT;

    this->cullFaces = false;
}

RenderSettings* RenderPipeline::getRenderSettings(){
    return &this->render_settings;
}

GizmosRenderer* RenderPipeline::getGizmosRenderer(){
    return this->gizmos;
}

void RenderSettings::defaults(){
    ambient_light_color = ZSRGBCOLOR(255, 255, 255, 255);

    this->skybox_ptr = nullptr;
    this->shadowcaster_ptr = nullptr;
}

void RenderPipeline::setup(int bufWidth, int bufHeight){
    if(this->project_struct_ptr->perspective == 2){
        this->tile_shader.compileFromFile("Shaders/2d_tile/tile2d.vert", "Shaders/2d_tile/tile2d.frag");
    }
    this->pick_shader.compileFromFile("Shaders/pick/pick.vert", "Shaders/pick/pick.frag");
    this->obj_mark_shader.compileFromFile("Shaders/mark/mark.vert", "Shaders/mark/mark.frag");
    this->ui_shader.compileFromFile("Shaders/ui/ui.vert", "Shaders/ui/ui.frag");
    if(this->project_struct_ptr->perspective == 3){
        this->deffered_light.compileFromFile("Shaders/postprocess/deffered_light/deffered.vert",
                                             "Shaders/postprocess/deffered_light/deffered.frag");
        this->diffuse3d_shader.compileFromFile("Shaders/3d/3d.vert", "Shaders/3d/3d.frag");
        skybox.compileFromFile("Shaders/skybox/skybox.vert", "Shaders/skybox/skybox.frag");
        shadowMap.compileFromFile("Shaders/shadowmap/shadowmap.vert", "Shaders/shadowmap/shadowmap.frag");
        heightmap.compileFromFile("Shaders/heightmap/heightmap.vert", "Shaders/heightmap/heightmap.frag");
        this->gbuffer.create(bufWidth, bufHeight);
    }
    ZSPIRE::setupDefaultMeshes();
    removeLights();

    glGenBuffers(1, &camBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, camBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 3 + 16 * 2, nullptr, GL_STATIC_DRAW);
    //Connect to point 0 (zero)
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, camBuffer);

    glGenBuffers(1, &lightsBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, lightsBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 64 * MAX_LIGHTS_AMOUNT + 16 * 2, nullptr, GL_STATIC_DRAW);
    //Connect to point 1 (one)
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightsBuffer);

    glGenBuffers(1, &shadowBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, shadowBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2 + 16, nullptr, GL_STATIC_DRAW);
    //Connect to point 2 (two)
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, shadowBuffer);

    glGenBuffers(1, &terrainUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, terrainUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 12 * 16 * 2 + 4 * 3, nullptr, GL_STATIC_DRAW);
    //Connect to point 3 (three)
    glBindBufferBase(GL_UNIFORM_BUFFER, 3, terrainUniformBuffer);

    glGenBuffers(1, &skinningUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, skinningUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 150, nullptr, GL_STATIC_DRAW);
    //Connect to point 4 (four)
    glBindBufferBase(GL_UNIFORM_BUFFER, 4, skinningUniformBuffer);

    for(unsigned int i = 0; i < MAX_LIGHTS_AMOUNT; i ++){
        ZSMATRIX4x4 m = getIdentity();
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * i, sizeof (ZSMATRIX4x4), &m);
    }

    glGenBuffers(1, &tileMaterialUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, tileMaterialUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 28, nullptr, GL_STATIC_DRAW);
    //Connect to point 5
    glBindBufferBase(GL_UNIFORM_BUFFER, 5, tileMaterialUniformBuffer);

    glGenBuffers(1, &skyboxTransformUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, skyboxTransformUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2, nullptr, GL_STATIC_DRAW);
    //Connect to point 6
    glBindBufferBase(GL_UNIFORM_BUFFER, 6, skyboxTransformUniformBuffer);

    glGenBuffers(1, &uiUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, uiUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2 + 16 + 16, nullptr, GL_STATIC_DRAW);
    //Connect to point 7
    glBindBufferBase(GL_UNIFORM_BUFFER, 7, uiUniformBuffer);

    MtShProps::genDefaultMtShGroup(&diffuse3d_shader, &skybox, &heightmap, 7);
}

void RenderPipeline::initGizmos(int projectPespective){
    gizmos = new GizmosRenderer(&obj_mark_shader, this->depthTest, this->cullFaces, projectPespective, camBuffer);
}

RenderPipeline::~RenderPipeline(){
    glDeleteBuffers(6, &camBuffer);

    this->tile_shader.Destroy();
    this->pick_shader.Destroy();
    this->obj_mark_shader.Destroy();
    this->deffered_light.Destroy();
    this->diffuse3d_shader.Destroy();
    ui_shader.Destroy();
    skybox.Destroy();
    shadowMap.Destroy();
    heightmap.Destroy();
    ZSPIRE::freeDefaultMeshes();

    this->gbuffer.Destroy();
    removeLights();
    delete gizmos;
}

bool RenderPipeline::InitGLEW(){
    glewExperimental = GL_TRUE;
    std::cout << "Calling GLEW creation" << std::endl;

    if (glewInit() != GLEW_OK){
        std::cout << "OPENGL GLEW: Creation failed ";
        return false;
    }

    std::cout << "GLEW creation successful" << std::endl;
        return true;
}

void RenderPipeline::init(){
    glViewport(0, 0, this->WIDTH, this->HEIGHT);

    InitGLEW();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(16.0f);
    //If our proj is 3D, then enable depth test by default
    if(this->project_struct_ptr->perspective == 3){
        glEnable(GL_DEPTH_TEST);
        depthTest = true;
        cullFaces = true;
        glFrontFace(GL_CCW);
    }else{
        glDisable(GL_DEPTH_TEST);
        depthTest = false;
        cullFaces = false;
        glDisable(GL_CULL_FACE);
    }

    //setup GBUFFER and shaders
    setup(this->WIDTH, this->HEIGHT);
    //initialize gizmos component
    initGizmos(this->project_struct_ptr->perspective);
}

ZSRGBCOLOR RenderPipeline::getColorOfPickedTransformControl(ZSVECTOR3 translation, int mouseX, int mouseY, void* projectedit_ptr){

        EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
        ZSPIRE::Camera* cam_ptr = nullptr; //We'll set it next
        World* world_ptr = &editwin_ptr->world;

        if(editwin_ptr->isWorldCamera){
            //if isWorldCamera is true, then we are in gameplay camera
            cam_ptr = &world_ptr->world_camera;
        }else{
            //if isWorldCamera is false, then we are in editor camera
            cam_ptr = &editwin_ptr->edit_camera;
        }
    //}

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Picking state

    if(depthTest == true) //if depth is enabled
        glDisable(GL_DEPTH_TEST);

    if(cullFaces == true)
        glDisable(GL_CULL_FACE);

    if(editwin_ptr->obj_trstate.isTransforming == true && !editwin_ptr->isWorldCamera){

        float dist = getDistance(cam_ptr->camera_pos, editwin_ptr->obj_trstate.obj_ptr->getTransformProperty()->_last_translation);

        if(this->project_struct_ptr->perspective == 2) dist = 70.0f;
        getGizmosRenderer()->drawTransformControls(editwin_ptr->obj_trstate.obj_ptr->getTransformProperty()->_last_translation, dist, dist / 10.f);
    }

    unsigned char data[4];
    glReadPixels(mouseX, this->HEIGHT - mouseY, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    return ZSRGBCOLOR(data[0], data[1], data[2]);
}

unsigned int RenderPipeline::render_getpickedObj(void* projectedit_ptr, int mouseX, int mouseY){
    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    World* world_ptr = &editwin_ptr->world;

    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_BLEND);
    pick_shader.Use();
    //Picking state
    this->current_state = PIPELINE_STATE_PICKING;

    if(depthTest == true) //if depth is disable, then enable it for a little time
        glEnable(GL_DEPTH_TEST);

    if(cullFaces == true) // if face cull is enabled, then disable it
        glDisable(GL_CULL_FACE);

    //Iterate over all objects in the world
    for(unsigned int obj_i = 0; obj_i < world_ptr->objects.size(); obj_i ++){
        GameObject* obj_ptr = &world_ptr->objects[obj_i];
        if(!obj_ptr->hasParent)
            obj_ptr->processObject(this);
    }

    if(depthTest == false) //if depth is enabled, then disable it
        glDisable(GL_DEPTH_TEST);

    if(cullFaces == true) //if cull faces is enabled, then enable GL function
        glEnable(GL_CULL_FACE);

    unsigned char data[4];
    glReadPixels(mouseX, this->HEIGHT - mouseY, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    data[3] = 0;
    unsigned int* pr_data_ = reinterpret_cast<unsigned int*>(&data[0]);
    unsigned int pr_data = *pr_data_;

    if(data[2] == 255) pr_data = 256 * 256 * 256; //If we haven't picked any object

    this->current_state = PIPELINE_STATE_DEFAULT;

    return pr_data;
}

void RenderPipeline::setLightsToBuffer(){
    glBindBuffer(GL_UNIFORM_BUFFER, lightsBuffer);
    for(unsigned int light_i = 0; light_i < this->lights_ptr.size(); light_i ++){
        LightsourceProperty* _light_ptr = static_cast<LightsourceProperty*>(lights_ptr[light_i]);
        //Set light type to buffer
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i, sizeof (int), &_light_ptr->light_type);
        //Set light range to buffer
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 4, sizeof (float), &_light_ptr->range);
        //Set lights intensity to buffer
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 8, sizeof (float), &_light_ptr->intensity);
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 12, sizeof (float), &_light_ptr->spot_angle);
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 16, 12, &_light_ptr->last_pos);
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 32, 12, &_light_ptr->direction);
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 48, 4, &_light_ptr->color.gl_r);
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 52, 4, &_light_ptr->color.gl_g);
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 56, 4, &_light_ptr->color.gl_b);
    }

    int ls = static_cast<int>(lights_ptr.size());
    glBufferSubData(GL_UNIFORM_BUFFER, 64 * MAX_LIGHTS_AMOUNT, 4, &ls);

    ZSVECTOR3 ambient_L = ZSVECTOR3(render_settings.ambient_light_color.r / 255.0f,render_settings.ambient_light_color.g / 255.0f, render_settings.ambient_light_color.b / 255.0f);
    glBufferSubData(GL_UNIFORM_BUFFER, 64 * MAX_LIGHTS_AMOUNT + 16, 12, &ambient_L);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    //free lights array
    this->removeLights();
}

void RenderPipeline::render(SDL_Window* w, void* projectedit_ptr){
    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    ZSPIRE::Camera* cam_ptr = nullptr; //We'll set it next
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

    switch(this->project_struct_ptr->perspective){
        case 2:{
            render2D(projectedit_ptr);
            break;
        }
        case 3:{
            render3D(projectedit_ptr, cam);
            break;
        }
    }


    ZSPIRE::getPlaneMesh2D()->Draw(); //Draw screen

    //if we control this object
    if(editwin_ptr->obj_trstate.isTransforming == true && !editwin_ptr->isWorldCamera){

        float dist = getDistance(cam_ptr->camera_pos, editwin_ptr->obj_trstate.obj_ptr->getTransformProperty()->_last_translation);

        if(this->project_struct_ptr->perspective == 2) dist = 70.0f;
        getGizmosRenderer()->drawTransformControls(editwin_ptr->obj_trstate.obj_ptr->getTransformProperty()->_last_translation, dist, dist / 10.f);
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->ui_shader.Use();
    GlyphFontContainer* c = editwin_ptr->getFontContainer("LiberationMono-Regular.ttf");
    int f[12];
    f[0] = static_cast<int>(L'H');
    f[1] = static_cast<int>(L'e');
    f[2] = static_cast<int>(L'l');
    f[3] = static_cast<int>(L'l');
    f[4] = static_cast<int>(L'o');
    f[5] = static_cast<int>(L'q');
    f[6] = static_cast<int>(L'W');
    f[7] = static_cast<int>(L'o');
    f[8] = static_cast<int>(L'r');
    f[9] = static_cast<int>(L'l');
    f[10] = static_cast<int>(L'd');
    //c->DrawString(f, 11, ZSVECTOR2(10,10));

    SDL_GL_SwapWindow(w); //Send rendered frame
}
void RenderPipeline::render2D(void* projectedit_ptr){
    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    World* world_ptr = &editwin_ptr->world;

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND); //Disable blending to render Skybox and shadows
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, this->WIDTH, this->HEIGHT);


    //Iterate over all objects in the world
    for(unsigned int obj_i = 0; obj_i < world_ptr->objects.size(); obj_i ++){
        GameObject* obj_ptr = &world_ptr->objects[obj_i];
        if(!obj_ptr->hasParent) //if it is a root object
            obj_ptr->processObject(this); //Draw object
    }
    setLightsToBuffer();

}
void RenderPipeline::render3D(void* projectedit_ptr, ZSPIRE::Camera* cam)
{
    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    World* world_ptr = &editwin_ptr->world;
    ZSPIRE::Camera* cam_ptr = cam; //We'll set it next

    ShadowCasterProperty* shadowcast = static_cast<ShadowCasterProperty*>(this->render_settings.shadowcaster_ptr);
    if(shadowcast != nullptr){ //we have shadowcaster
        shadowcast->Draw(cam_ptr, this); //draw shadowcaster
    }

    //Active Geometry framebuffer
    gbuffer.bindFramebuffer();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_BLEND); //Disable blending to render Skybox and shadows
    glViewport(0, 0, this->WIDTH, this->HEIGHT);


    SkyboxProperty* skybox = static_cast<SkyboxProperty*>(this->render_settings.skybox_ptr);
    if(skybox != nullptr)
        skybox->DrawSky(this);

    if(depthTest == true) //if depth is enabled
        glEnable(GL_DEPTH_TEST);

    if(cullFaces == true)
        glEnable(GL_CULL_FACE);

    //Iterate over all objects in the world
    for(unsigned int obj_i = 0; obj_i < world_ptr->objects.size(); obj_i ++){
        GameObject* obj_ptr = &world_ptr->objects[obj_i];
        if(!obj_ptr->hasParent) //if it is a root object
            obj_ptr->processObject(this); //Draw object
    }

    //Turn everything off to draw deffered plane correctly
    if(depthTest == true) //if depth is enabled
        glDisable(GL_DEPTH_TEST);

    if(cullFaces == true) //if GL face cull is enabled
        glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); //Back to default framebuffer
    glClear(GL_COLOR_BUFFER_BIT); //Clear screen
    gbuffer.bindTextures(); //Bind gBuffer textures
    deffered_light.Use(); //use deffered shader
    //Send lights to OpenGL uniform buffer
    setLightsToBuffer();
}

void RenderPipeline::renderDepth(void* world_ptr){
    World* _world_ptr = static_cast<World*>(world_ptr);
    this->current_state = PIPELINE_STATE_SHADOWDEPTH;
    //Iterate over all objects in the world
    for(unsigned int obj_i = 0; obj_i < _world_ptr->objects.size(); obj_i ++){
        GameObject* obj_ptr = &_world_ptr->objects[obj_i];
        if(!obj_ptr->hasParent) //if it is a root object
            obj_ptr->processObject(this); //Draw object
    }
    this->current_state = PIPELINE_STATE_DEFAULT;
}

void GameObject::Draw(RenderPipeline* pipeline){
    //Call prerender on each property in object
    if(pipeline->current_state == PIPELINE_STATE_DEFAULT)
        this->onPreRender(pipeline);
    //Getting pointer to mesh
    MeshProperty* mesh_prop = static_cast<MeshProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
    if(hasMesh() || hasTerrain()){// if object has mesh
        //If we are in default draw mode
        if(pipeline->current_state == PIPELINE_STATE_DEFAULT){
            this->onRender(pipeline);
            if(hasMesh()){
                //Iterate over all bones
                for(unsigned int bone_i = 0; bone_i < mesh_prop->mesh_ptr->bones.size(); bone_i ++){
                    //Obtain bone by pointer
                    ZSPIRE::Bone* b = &mesh_prop->mesh_ptr->bones[bone_i];

                    GameObject* node = nullptr;
                    GameObject* RootNode = mesh_prop->skinning_root_node;
                    ZSMATRIX4x4 rootNodeTransform;

                    if(RootNode != nullptr){
                        //if RootNode is specified
                        node = mesh_prop->skinning_root_node->getChildObjectWithNodeLabel(QString::fromStdString(b->bone_name));
                        //Get root transform
                        rootNodeTransform = (RootNode->getPropertyPtr<NodeProperty>()->transform_mat);
                    }

                    if(node != nullptr){
                        NodeProperty* nd = node->getPropertyPtr<NodeProperty>();
                        //Calculate result matrix
                        ZSMATRIX4x4 matrix = transpose(invert(rootNodeTransform) * nd->abs * b->offset);
                        //Send skinned matrix to skinning uniform buffer
                        glBindBuffer(GL_UNIFORM_BUFFER, pipeline->skinningUniformBuffer);
                        glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * bone_i, sizeof (ZSMATRIX4x4), &matrix);
                    }
                }
            }

            DrawMesh();
        }
        //If we picking object
        if(pipeline->current_state == PIPELINE_STATE_PICKING) {
            TransformProperty* transform_ptr = static_cast<TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));

            unsigned char* to_send = reinterpret_cast<unsigned char*>(&array_index);
            float r = static_cast<float>(to_send[0]);
            float g = static_cast<float>(to_send[1]);
            float b = static_cast<float>(to_send[2]);
            float a = static_cast<float>(to_send[3]);

            //set transform to camera buffer
            glBindBuffer(GL_UNIFORM_BUFFER, pipeline->camBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &transform_ptr->transform_mat);

            pipeline->getPickingShader()->setGLuniformVec4("color", ZSVECTOR4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
            DrawMesh();
        }
        if(pipeline->current_state == PIPELINE_STATE_SHADOWDEPTH) {
            TransformProperty* transform_ptr = static_cast<TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
            //set transform to camera buffer
            glBindBuffer(GL_UNIFORM_BUFFER, pipeline->camBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &transform_ptr->transform_mat);

            //Get castShadows boolean from several properties
            bool castShadows = (hasTerrain()) ? getPropertyPtr<TerrainProperty>()->castShadows : mesh_prop->castShadows;

            if(castShadows)
                DrawMesh();
        }


        if(this->isPicked == true && pipeline->current_state != PIPELINE_STATE_PICKING && pipeline->current_state != PIPELINE_STATE_SHADOWDEPTH){
            EditWindow* editwin_ptr = static_cast<EditWindow*>(pipeline->win_ptr);
            TransformProperty* transform_ptr = static_cast<TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
            ZSRGBCOLOR color = ZSRGBCOLOR(static_cast<int>(0.23f * 255.0f),
                                      static_cast<int>(0.23f * 255.0f),
                                      static_cast<int>(0.54f * 255.0f));
            if(editwin_ptr->obj_trstate.isTransforming == true)
                color = ZSRGBCOLOR(255.0f, 255.0f, 0.0f);
            //draw wireframe mesh for picked object
            if(!editwin_ptr->isWorldCamera && hasMesh()) //avoid drawing gizmos during playtime
                pipeline->getGizmosRenderer()->drawPickedMeshWireframe(mesh_prop->mesh_ptr, transform_ptr->transform_mat, color);
        }
    }
}

void GameObject::processObject(RenderPipeline* pipeline){
    if(alive == false) return;


    //Obtain EditWindow pointer to check if scene is running
    EditWindow* editwin_ptr = static_cast<EditWindow*>(pipeline->win_ptr);
    if(active == false) return; //if object is inactive, not to render it

    TransformProperty* transform_prop = static_cast<TransformProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    //Call update on every property in objects
    if(editwin_ptr->isSceneRun && pipeline->current_state == PIPELINE_STATE_DEFAULT)
        this->onUpdate(static_cast<int>(pipeline->deltaTime));

    //Obtain camera viewport
    ZSVIEWPORT cam_viewport = pipeline->cam->getViewport();
    //Distance limit
    int max_dist = static_cast<int>(cam_viewport.endX - cam_viewport.startX);
    bool difts = isDistanceFits(pipeline->cam->getCameraViewCenterPos(), transform_prop->_last_translation, max_dist);

    //if(difts)
        this->Draw(pipeline);

    for(unsigned int obj_i = 0; obj_i < this->children.size(); obj_i ++){
        if(!children[obj_i].isEmpty()){ //if link isn't broken
            children[obj_i].updLinkPtr();
            GameObject* child_ptr = this->children[obj_i].ptr;
            child_ptr->processObject(pipeline);
        }
    }
}

void MaterialProperty::onRender(RenderPipeline* pipeline){
    ZSPIRE::Shader* shader;
    //Check for validity of pointer
    if(material_ptr == nullptr) return;

    TransformProperty* transform_ptr = static_cast<TransformProperty*>(this->go_link.updLinkPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    MtShaderPropertiesGroup* group_ptr = material_ptr->group_ptr;

    if(transform_ptr == nullptr || group_ptr == nullptr) return ; //if object hasn't property

    //Work with shader
    shader = group_ptr->render_shader;
    shader->Use();
    //Get pointer to shadowcaster
    ShadowCasterProperty* shadowcast = static_cast<ShadowCasterProperty*>(pipeline->getRenderSettings()->shadowcaster_ptr);
    if(shadowcast != nullptr && this->material_ptr->group_ptr->acceptShadows && this->receiveShadows){
        shadowcast->setTexture();
        glBindBuffer(GL_UNIFORM_BUFFER, pipeline->shadowBuffer);
        //In GLSL we should use Integer instead of bool
        int recShadows = static_cast<int>(this->receiveShadows);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2 + 4, 4, &recShadows);
    }
    if(!this->receiveShadows || shadowcast == nullptr || !shadowcast->active){
        glBindBuffer(GL_UNIFORM_BUFFER, pipeline->shadowBuffer);
        //In GLSL we should use Integer instead of bool
        int recShadows = 0;
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2 + 4, 4, &recShadows);
    }

    //Send transform matrix to shader
    glBindBuffer(GL_UNIFORM_BUFFER, pipeline->camBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &transform_ptr->transform_mat);

    material_ptr->applyMatToPipeline();


}

void TerrainProperty::onRender(RenderPipeline* pipeline){
    terrainUniformBuffer = pipeline->terrainUniformBuffer;

    if(hasChanged){
        this->data.generateGLMesh();
        hasChanged = false;
    }
    //Binding terrain buffer
    glBindBuffer(GL_UNIFORM_BUFFER, pipeline->terrainUniformBuffer);

    MaterialProperty* mat = this->go_link.updLinkPtr()->getPropertyPtr<MaterialProperty>();
    if(mat == nullptr) return;

    int dtrue = 1;
    int dfalse = 0;

    //Iterate over all textures to use them
    for(unsigned int i = 0; i < static_cast<unsigned int>(this->textures_size); i ++){
        //Get pair pointer
        HeightmapTexturePair* pair = &this->textures[i];
        //Check, if pair has diffuse texture
        if(pair->diffuse != nullptr){
            //Use diffuse texture
            pair->diffuse->Use(static_cast<int>(i));
            glBufferSubData(GL_UNIFORM_BUFFER, 16 * i, 4, &dtrue);
        }else{
            glBufferSubData(GL_UNIFORM_BUFFER, 16 * i, 4, &dfalse);
        }
        //Check, if pair has normal texture
        if(pair->normal != nullptr){
            //Use normal texture
            pair->normal->Use(static_cast<int>(12 + i));
            glBufferSubData(GL_UNIFORM_BUFFER, 16 * 12 + 16 * i, 4, &dtrue);
        }else{
            glBufferSubData(GL_UNIFORM_BUFFER, 16 * 12 + 16 * i, 4, &dfalse);
        }
    }
    //Tell shader, that we rendering terrain in normal mode (non picking)
    glBufferSubData(GL_UNIFORM_BUFFER, 16 * 12 * 2, 4, &dfalse);
    //Send dimensions to buffer
    glBufferSubData(GL_UNIFORM_BUFFER, 16 * 12 * 2 + 4, 4, &this->data.W);
    glBufferSubData(GL_UNIFORM_BUFFER, 16 * 12 * 2 + 8, 4, &this->data.H);

    //Apply material shader
    mat->onRender(pipeline);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void TileProperty::onRender(RenderPipeline* pipeline){
    ZSPIRE::Shader* tile_shader = pipeline->getTileShader();
    //Receive pointer to tile information
    TransformProperty* transform_ptr = static_cast<TransformProperty*>(this->go_link.updLinkPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));

    if(transform_ptr == nullptr) return;

    glBindBuffer(GL_UNIFORM_BUFFER, pipeline->camBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &transform_ptr->transform_mat);

    tile_shader->Use();

    glBindBuffer(GL_UNIFORM_BUFFER, pipeline->tileMaterialUniformBuffer);

    //Checking for diffuse texture
    if(texture_diffuse != nullptr){
        texture_diffuse->Use(0); //Use this texture
    }

    int diffuse1_ = texture_diffuse != nullptr;
    glBufferSubData(GL_UNIFORM_BUFFER, 20, 4, &diffuse1_);

    //Checking for transparent texture
    if(texture_transparent != nullptr){
        texture_transparent->Use(1); //Use this texture
    }
    int diffuse2_ = texture_transparent != nullptr;
    glBufferSubData(GL_UNIFORM_BUFFER, 24, 4, &diffuse2_);
    //calculate animation state
    int anim_state_i = anim_property.isAnimated && anim_state.playing;
    //Sending animation info
    if(anim_property.isAnimated && anim_state.playing == true){ //If tile animated, then send anim state to shader
        glBufferSubData(GL_UNIFORM_BUFFER, 16, 4, &anim_state_i);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &anim_property.framesX);
        glBufferSubData(GL_UNIFORM_BUFFER, 4, 4, &anim_property.framesY);
        glBufferSubData(GL_UNIFORM_BUFFER, 8, 4, &anim_state.cur_frameX);
        glBufferSubData(GL_UNIFORM_BUFFER, 12, 4, &anim_state.cur_frameY);
    }else{ //No animation or unplayed
         glBufferSubData(GL_UNIFORM_BUFFER, 16, 4, &anim_state_i);
    }
}

void SkyboxProperty::onPreRender(RenderPipeline* pipeline){
    pipeline->getRenderSettings()->skybox_ptr = static_cast<void*>(this);
}

void SkyboxProperty::DrawSky(RenderPipeline* pipeline){
    if(!this->active) return;
    if(this->go_link.updLinkPtr() == nullptr) return;
    //Get pointer to Material property
    MaterialProperty* mat = this->go_link.updLinkPtr()->getPropertyPtr<MaterialProperty>();
    if(mat == nullptr) return;
    //Apply material shader
    mat->onRender(pipeline);
    //Draw skybox cube
    glDisable(GL_DEPTH_TEST);
    ZSPIRE::getSkyboxMesh()->Draw();
}

void ShadowCasterProperty::onPreRender(RenderPipeline* pipeline){
    pipeline->getRenderSettings()->shadowcaster_ptr = static_cast<void*>(this);
}

void ShadowCasterProperty::Draw(ZSPIRE::Camera* cam, RenderPipeline* pipeline){
    if(!this->active) return;
    if(!this->initialized) init();
    if(this->go_link.updLinkPtr() == nullptr) return;

    LightsourceProperty* light = this->go_link.updLinkPtr()->getPropertyPtr<LightsourceProperty>();

    if(light == nullptr) return;

    //ZSVECTOR3 cam_pos = cam->getCameraPosition() + cam->getCameraFrontVec() * 20;
    ZSVECTOR3 cam_pos = cam->getCameraPosition() - light->direction * 20;
    this->LightProjectionMat = getOrthogonal(-projection_viewport, projection_viewport, -projection_viewport, projection_viewport, nearPlane, farPlane);
    this->LightViewMat = matrixLookAt(cam_pos, cam_pos + light->direction * -1, ZSVECTOR3(0,1,0));

    glViewport(0, 0, TextureWidth, TextureHeight); //Changing viewport

    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer); //Bind framebuffer
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glDisable(GL_CULL_FACE);
    //Bind shadow uniform buffer
    glBindBuffer(GL_UNIFORM_BUFFER, pipeline->shadowBuffer);
    //Bind ortho shadow projection
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof (ZSMATRIX4x4), &LightProjectionMat);
    //Bind shadow view matrix
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &LightViewMat);
    //Send BIAS value
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2, 4, &shadow_bias);
    //Send Width of shadow texture
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2 + 8, 4, &this->TextureWidth);
    //Send Height of shadow texture
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2 + 12, 4, &this->TextureHeight);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    pipeline->getShadowmapShader()->Use();

    pipeline->renderDepth(this->go_link.world_ptr);

    glFrontFace(GL_CCW);
}

void ShadowCasterProperty::init(){
    glGenFramebuffers(1, &this->shadowBuffer);//Generate framebuffer for texture
    glGenTextures(1, &this->shadowDepthTexture); //Generate texture

    glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
    //Configuring texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->TextureWidth, this->TextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    //Binding framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    //Connecting depth texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadowDepthTexture, 0);
    //We won't render color
    glDrawBuffer(false);
    glReadBuffer(false);
    //Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->initialized = true;
}

void ShadowCasterProperty::setTexture(){
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, this->shadowDepthTexture);
}

void RenderPipeline::updateShadersCameraInfo(ZSPIRE::Camera* cam_ptr){
    glBindBuffer(GL_UNIFORM_BUFFER, camBuffer);
    ZSMATRIX4x4 proj = cam_ptr->getProjMatrix();
    ZSMATRIX4x4 view = cam_ptr->getViewMatrix();
    ZSVECTOR3 cam_pos = cam_ptr->getCameraPosition();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof (ZSMATRIX4x4), &proj);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &view);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 3, sizeof(ZSVECTOR3), &cam_pos);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    //Setting UI camera to UI buffer
    glBindBuffer(GL_UNIFORM_BUFFER, uiUniformBuffer);
    proj = cam_ptr->getUiProjMatrix();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof (ZSMATRIX4x4), &proj);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    //Setting cameras to skybox shader
    proj = cam_ptr->getProjMatrix();
    view = cam_ptr->getViewMatrix();
    view = removeTranslationFromViewMat(view);
    glBindBuffer(GL_UNIFORM_BUFFER, skyboxTransformUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof (ZSMATRIX4x4), &proj);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &view);

}

void RenderPipeline::addLight(void* light_ptr){
    this->lights_ptr.push_back(light_ptr);
}

void RenderPipeline::removeLights(){
    this->lights_ptr.clear(); //clear ptr vector
}

G_BUFFER_GL::G_BUFFER_GL(){
    created = false;
}

void G_BUFFER_GL::create(int width, int height){
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    glGenTextures(1, &tDiffuse);
    glBindTexture(GL_TEXTURE_2D, tDiffuse);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tDiffuse, 0);

    glGenTextures(1, &tNormal);
    glBindTexture(GL_TEXTURE_2D, tNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tNormal, 0);

    glGenTextures(1, &tPos);
    glBindTexture(GL_TEXTURE_2D, tPos);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tPos, 0);

    glGenTextures(1, &tTransparent);
    glBindTexture(GL_TEXTURE_2D, tTransparent);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, tTransparent, 0);

    glGenTextures(1, &tMasks);
    glBindTexture(GL_TEXTURE_2D, tMasks);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, tMasks, 0);

    unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
    glDrawBuffers(5, attachments);

    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); //return back to default
    created = true;
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

    glActiveTexture(GL_TEXTURE13);
    glBindTexture(GL_TEXTURE_2D, tTransparent);

    glActiveTexture(GL_TEXTURE14);
    glBindTexture(GL_TEXTURE_2D, tMasks);
}

void G_BUFFER_GL::Destroy(){
    //Remove textures
    glDeleteTextures(1, &tDiffuse);
    glDeleteTextures(1, &tNormal);
    glDeleteTextures(1, &tPos);
    glDeleteTextures(1, &tTransparent);
    glDeleteTextures(1, &tMasks);

    //delete framebuffer & renderbuffer
    glDeleteRenderbuffers(1, &this->depthBuffer);
    glDeleteFramebuffers(1, &this->gBuffer);
}

void RenderPipeline::renderSprite(ZSPIRE::Texture* texture_sprite, int X, int Y, int scaleX, int scaleY){
    renderSprite(texture_sprite->TEXTURE_ID, X, Y, scaleX, scaleY);
}

void RenderPipeline::renderSprite(unsigned int texture_id, int X, int Y, int scaleX, int scaleY){
    this->ui_shader.Use();
    glBindBuffer(GL_UNIFORM_BUFFER, uiUniformBuffer);

    int _render_mode = 1;
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2 , 4, &_render_mode);
    //Use texture at 0 slot
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    ZSMATRIX4x4 translation = getTranslationMat(X, Y, 0.0f);
    ZSMATRIX4x4 scale = getScaleMat(scaleX, scaleY, 0.0f);
    ZSMATRIX4x4 transform = scale * translation;

    //Push glyph transform
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &transform);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    ZSPIRE::getUiSpriteMesh2D()->Draw();
}

void RenderPipeline::renderGlyph(unsigned int texture_id, int X, int Y, int scaleX, int scaleY, ZSRGBCOLOR color){
    this->ui_shader.Use();
    glBindBuffer(GL_UNIFORM_BUFFER, uiUniformBuffer);
    //tell shader, that we will render glyph
    int _render_mode = 2;
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2 , 4, &_render_mode);
    //sending glyph color
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2 + 16, 4, &color.gl_r);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2 + 4 + 16, 4, &color.gl_g);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2 + 8 + 16, 4, &color.gl_b);

    //Use texture at 0 slot
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    ZSMATRIX4x4 translation = getTranslationMat(X, Y, 0.0f);
    ZSMATRIX4x4 scale = getScaleMat(scaleX, scaleY, 0.0f);
    ZSMATRIX4x4 transform = scale * translation;

    //Push glyph transform
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &transform);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    ZSPIRE::getUiSpriteMesh2D()->Draw();
}

void RenderPipeline::updateWindowSize(int W, int H){
     glViewport(0, 0, W, H);

    if(gbuffer.created){
        this->gbuffer.Destroy();
        this->gbuffer.create(W, H);
    }
}

ZSPIRE::Shader* RenderPipeline::getTileShader(){
    return &this->tile_shader;
}

ZSPIRE::Shader* RenderPipeline::getPickingShader(){
    return &this->pick_shader;
}

ZSPIRE::Shader* RenderPipeline::getShadowmapShader(){
    return &this->shadowMap;
}

ZSPIRE::Shader* RenderPipeline::getUiShader(){
    return &this->ui_shader;
}
