#include "headers/zs-pipeline.h"
#include "../World/headers/obj_properties.h"
#include "../World/headers/2dtileproperties.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include <iostream>

#define LIGHT_STRUCT_SIZE 64

extern ZSGAME_DATA* game_data;

RenderPipeline::RenderPipeline(){
    this->current_state = PIPELINE_STATE_DEFAULT;

    this->cullFaces = false;
}

GizmosRenderer* RenderPipeline::getGizmosRenderer(){
    return this->gizmos;
}

void RenderPipeline::setup(int bufWidth, int bufHeight){
    this->pick_shader = Engine::allocShader();
    this->obj_mark_shader = Engine::allocShader();

    this->pick_shader->compileFromFile("Shaders/pick/pick.vert", "Shaders/pick/pick.frag");
    this->obj_mark_shader->compileFromFile("Shaders/mark/mark.vert", "Shaders/mark/mark.frag");

    if(this->project_struct_ptr->perspective == PERSP_3D){
        this->gbuffer.create(bufWidth, bufHeight);
    }
    removeLights();

    editorUniformBuffer = Engine::allocUniformBuffer();
    editorUniformBuffer->init(8, 16);


}

void RenderPipeline::initGizmos(int projectPespective){
    gizmos = new GizmosRenderer(obj_mark_shader, this->depthTest, this->cullFaces,
                                projectPespective,
                                transformBuffer,
                                editorUniformBuffer);
}

RenderPipeline::~RenderPipeline(){
    this->pick_shader->Destroy();
    this->obj_mark_shader->Destroy();

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

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(16.0f);
    //If our proj is 3D, then enable depth test by default
    if(this->project_struct_ptr->perspective == PERSP_3D){
        glEnable(GL_DEPTH_TEST);
        depthTest = true;
        cullFaces = true;
        glFrontFace(GL_CCW);
    }else{
        //Project is 2D, disable depth test and face cull
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

ZSRGBCOLOR RenderPipeline::getColorOfPickedTransformControl(int mouseX, int mouseY, void* projectedit_ptr){

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

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Picking state

    if(depthTest == true) //if depth is enabled
        glDisable(GL_DEPTH_TEST);

    if(cullFaces == true)
        glDisable(GL_CULL_FACE);

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

unsigned int RenderPipeline::render_getpickedObj(void* projectedit_ptr, int mouseX, int mouseY){
    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    World* world_ptr = &editwin_ptr->world;

    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_BLEND);
    pick_shader->Use();
    //Picking state
    this->current_state = PIPELINE_STATE_PICKING;

    if(depthTest == true) //if depth is enabled, then disable it
        glEnable(GL_DEPTH_TEST);

    if(cullFaces == true) // if face cull is enabled, then disable it
        glEnable(GL_CULL_FACE);

    //Iterate over all objects in the world
    for(unsigned int obj_i = 0; obj_i < world_ptr->objects.size(); obj_i ++){
        GameObject* obj_ptr = (GameObject*)world_ptr->objects[obj_i];
        if(!obj_ptr->hasParent)
            obj_ptr->processObject(this);
    }


    unsigned char data[4];
    glReadPixels(mouseX, this->HEIGHT - mouseY, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    data[3] = 0;
    unsigned int* pr_data_ = reinterpret_cast<unsigned int*>(&data[0]);
    unsigned int pr_data = *pr_data_;

    if(data[2] == 255) pr_data = 256 * 256 * 256; //If we haven't picked any object

    this->current_state = PIPELINE_STATE_DEFAULT;

    return pr_data;
}

void RenderPipeline::render(SDL_Window* w, void* projectedit_ptr){
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

    switch(this->project_struct_ptr->perspective){
        case PERSP_2D:{
            render2D(projectedit_ptr);
            break;
        }
        case PERSP_3D:{
            render3D(projectedit_ptr, cam);
            Engine::getPlaneMesh2D()->Draw(); //Draw screen
            break;
        }
    }


    //if we control this object
    if(editwin_ptr->obj_trstate.isTransforming == true && !editwin_ptr->isWorldCamera){

        float dist = getDistance(cam_ptr->camera_pos, editwin_ptr->obj_trstate.obj_ptr->getPropertyPtr<Engine::TransformProperty>()->abs_translation);

        if(this->project_struct_ptr->perspective == PERSP_2D) dist = 70.0f;
        getGizmosRenderer()->drawTransformControls(editwin_ptr->obj_trstate.obj_ptr->getPropertyPtr<Engine::TransformProperty>()->abs_translation, dist, dist / 10.f);
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //GlyphFontContainer* c = game_data->resources->getFontByLabel("LiberationMono-Regular.ttf")->font_ptr;
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
        GameObject* obj_ptr = (GameObject*)world_ptr->objects[obj_i];
        if(!obj_ptr->hasParent) //if it is a root object
            obj_ptr->processObject(this); //Draw object
    }
    setLightsToBuffer();

}
void RenderPipeline::render3D(void* projectedit_ptr, Engine::Camera* cam)
{
    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    World* world_ptr = &editwin_ptr->world;
    Engine::Camera* cam_ptr = cam; //We'll set it next

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
        GameObject* obj_ptr = (GameObject*)world_ptr->objects[obj_i];
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
    deffered_light->Use(); //use deffered shader
    //Send lights to OpenGL uniform buffer
    setLightsToBuffer();
}

void RenderPipeline::renderDepth(void* world_ptr){
    World* _world_ptr = static_cast<World*>(world_ptr);
    this->current_state = PIPELINE_STATE_SHADOWDEPTH;
    //Iterate over all objects in the world
    for(unsigned int obj_i = 0; obj_i < _world_ptr->objects.size(); obj_i ++){
        GameObject* obj_ptr = (GameObject*)_world_ptr->objects[obj_i];
        if(!obj_ptr->hasParent) //if it is a root object
            obj_ptr->processObject(this); //Draw object
    }
    this->current_state = PIPELINE_STATE_DEFAULT;
}

void RenderPipeline::setLightsToBuffer(){
    this->lightsBuffer->bind();
    for(unsigned int light_i = 0; light_i < this->lights_ptr.size(); light_i ++){
        LightsourceProperty* _light_ptr = static_cast<LightsourceProperty*>(lights_ptr[light_i]);

        LIGHTSOURCE_TYPE light_type = (_light_ptr->light_type);

        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i, sizeof (LIGHTSOURCE_TYPE), &light_type);
        if(_light_ptr->light_type > LIGHTSOURCE_TYPE_DIRECTIONAL){
            lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 4, sizeof (float), &_light_ptr->range);
            lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 12, sizeof (float), &_light_ptr->spot_angle);
        }
        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 8, sizeof (float), &_light_ptr->intensity);

        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 16, 12, &_light_ptr->last_pos);
        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 32, 12, &_light_ptr->direction);
        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 48, sizeof (int), &_light_ptr->color.gl_r);
        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 52, sizeof (int), &_light_ptr->color.gl_g);
        lightsBuffer->writeData(LIGHT_STRUCT_SIZE * light_i + 56, sizeof (int), &_light_ptr->color.gl_b);
    }

    int ls = static_cast<int>(lights_ptr.size());
    lightsBuffer->writeData(LIGHT_STRUCT_SIZE * MAX_LIGHTS_AMOUNT, 4, &ls);

    ZSVECTOR3 ambient_L = ZSVECTOR3(render_settings.ambient_light_color.r / 255.0f,render_settings.ambient_light_color.g / 255.0f, render_settings.ambient_light_color.b / 255.0f);
    lightsBuffer->writeData(LIGHT_STRUCT_SIZE * MAX_LIGHTS_AMOUNT + 16, 12, &ambient_L);

    //free lights array
    this->removeLights();
}

void GameObject::Draw(RenderPipeline* pipeline){
    Engine::TransformProperty* transform_ptr = static_cast<Engine::TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));

    if(transform_ptr == nullptr) return;


    //Call prerender on each property in object
    if(pipeline->current_state == PIPELINE_STATE_DEFAULT)
        this->onPreRender(pipeline);
    //Getting pointer to mesh
    Engine::MeshProperty* mesh_prop = static_cast<Engine::MeshProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
    if(hasMesh() || hasTerrain()){// if object has mesh
        //If we are in default draw mode
        if(pipeline->current_state == PIPELINE_STATE_DEFAULT){
            this->onRender(pipeline);
            if(hasMesh()){
                //Iterate over all bones
                for(unsigned int bone_i = 0; bone_i < mesh_prop->mesh_ptr->mesh_ptr->bones.size(); bone_i ++){
                    //Check for buffer overflow
                    if(bone_i >= MAX_MESH_BONES)
                        break;
                    //Obtain bone by pointer
                    Engine::Bone* b = &mesh_prop->mesh_ptr->mesh_ptr->bones[bone_i];

                    Engine::GameObject* node = nullptr;
                    GameObject* RootNode = (GameObject*)mesh_prop->skinning_root_node;
                    ZSMATRIX4x4 rootNodeTransform;

                    if(RootNode != nullptr){
                        //if RootNode is specified
                        node = (mesh_prop->skinning_root_node)->getChildObjectWithNodeLabel(b->bone_name);
                        //Get root transform
                        rootNodeTransform = (RootNode->getPropertyPtr<Engine::NodeProperty>()->transform_mat);
                    }

                    if(node != nullptr){
                        Engine::NodeProperty* nd = node->getPropertyPtr<Engine::NodeProperty>();
                        //Calculate result matrix
                        ZSMATRIX4x4 matrix = transpose(invert(rootNodeTransform) * nd->abs * b->offset);
                        //Send skinned matrix to skinning uniform buffer
                        pipeline->skinningUniformBuffer->bind();
                        pipeline->skinningUniformBuffer->writeData(sizeof (ZSMATRIX4x4) * bone_i, sizeof (ZSMATRIX4x4), &matrix);
                    }
                }
            }

            DrawMesh(pipeline);
        }
        //If we picking object
        if(pipeline->current_state == PIPELINE_STATE_PICKING) {
            Engine::TransformProperty* transform_ptr = static_cast<Engine::TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));

            unsigned char* to_send = reinterpret_cast<unsigned char*>(&array_index);
            float r = static_cast<float>(to_send[0]);
            float g = static_cast<float>(to_send[1]);
            float b = static_cast<float>(to_send[2]);
            float a = static_cast<float>(to_send[3]);
            ZSVECTOR4 color = ZSVECTOR4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);

            //set transform to camera buffer
            pipeline->transformBuffer->bind();
            pipeline->transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &transform_ptr->transform_mat);

            pipeline->editorUniformBuffer->bind();
            pipeline->editorUniformBuffer->writeData(0, 16, &color);
            DrawMesh(pipeline);
        }
        if(pipeline->current_state == PIPELINE_STATE_SHADOWDEPTH) {
            Engine::TransformProperty* transform_ptr = static_cast<Engine::TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
            //set transform to camera buffer
            pipeline->transformBuffer->bind();
            pipeline->transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &transform_ptr->transform_mat);

            //Get castShadows boolean from several properties
            bool castShadows = (hasTerrain()) ? getPropertyPtr<TerrainProperty>()->castShadows : mesh_prop->castShadows;

            if(castShadows)
                DrawMesh(pipeline);
        }


        if(this->isPicked == true && pipeline->current_state != PIPELINE_STATE_PICKING && pipeline->current_state != PIPELINE_STATE_SHADOWDEPTH){
            EditWindow* editwin_ptr = static_cast<EditWindow*>(pipeline->win_ptr);
            Engine::TransformProperty* transform_ptr = static_cast<Engine::TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
            ZSRGBCOLOR color = ZSRGBCOLOR(static_cast<int>(0.23f * 255.0f),
                                      static_cast<int>(0.23f * 255.0f),
                                      static_cast<int>(0.54f * 255.0f));
            if(editwin_ptr->obj_trstate.isTransforming == true)
                color = ZSRGBCOLOR(255.0f, 255.0f, 0.0f);
            //draw wireframe mesh for picked object
            if(!editwin_ptr->isWorldCamera && hasMesh()) //avoid drawing gizmos during playtime
                pipeline->getGizmosRenderer()->drawPickedMeshWireframe(mesh_prop->mesh_ptr->mesh_ptr, transform_ptr->transform_mat, color);
        }
    }
}

void GameObject::processObject(RenderPipeline* pipeline){
    if(alive == false) return;

    //Obtain EditWindow pointer to check if scene is running
    EditWindow* editwin_ptr = static_cast<EditWindow*>(pipeline->win_ptr);
    if(active == false) return; //if object is inactive, not to render it

    Engine::TransformProperty* transform_prop = static_cast<Engine::TransformProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    //Call update on every property in objects
    if(editwin_ptr->isSceneRun && pipeline->current_state == PIPELINE_STATE_DEFAULT)
        this->onUpdate(static_cast<int>(pipeline->deltaTime));

    //Obtain camera viewport
    Engine::ZSVIEWPORT cam_viewport = pipeline->cam->getViewport();
    //Distance limit
    //int max_dist = static_cast<int>(cam_viewport.endX - cam_viewport.startX);
    //bool difts = isDistanceFits(pipeline->cam->getCameraViewCenterPos(), transform_prop->_last_translation, max_dist);

    //if(difts)
        this->Draw(pipeline);

    for(unsigned int obj_i = 0; obj_i < this->children.size(); obj_i ++){
        if(!children[obj_i].isEmpty()){ //if link isn't broken
            ((World*)world_ptr)->updateLink(&children[obj_i]);
            GameObject* child_ptr = (GameObject*)this->children[obj_i].ptr;
            child_ptr->processObject(pipeline);
        }
    }
}

void MaterialProperty::onRender(Engine::RenderPipeline* pipeline){
    //Check for validity of pointer
    if(material_ptr == nullptr) return;

    MtShaderPropertiesGroup* group_ptr = material_ptr->group_ptr;
    if( group_ptr == nullptr) return ; //if object hasn't property

    //Get pointer to shadowcaster
    ShadowCasterProperty* shadowcast = static_cast<ShadowCasterProperty*>(pipeline->getRenderSettings()->shadowcaster_ptr);
    //Bind shadow uniform buffer
    pipeline->shadowBuffer->bind();

    int recShadows = 1;

    if(shadowcast == nullptr){
        //In GLSL we should use Integer instead of bool
        recShadows = 0;
    }else if(!receiveShadows || !shadowcast->isActive()){
        recShadows = 0;
    }else
        shadowcast->setTexture();

    pipeline->shadowBuffer->writeData(sizeof (ZSMATRIX4x4) * 2 + 4, 4, &recShadows);

    material_ptr->applyMatToPipeline();
}

void TerrainProperty::onRender(Engine::RenderPipeline* pipeline){
    terrainUniformBuffer = pipeline->terrainUniformBuffer;
    transformBuffer = pipeline->transformBuffer;

    if(data.hasHeightmapChanged){
        this->data.updateGeometryBuffers();
        this->data.updateGeometryBuffersGL();
        data.hasHeightmapChanged = false;
    }

    if(data.hasPaintingChanged){
        this->data.updateTextureBuffers();
        this->data.updateTextureBuffersGL();
        data.hasPaintingChanged = false;
    }

    //Binding terrain buffer
    pipeline->terrainUniformBuffer->bind();

    MaterialProperty* mat = ((World*)world_ptr)->updateLink(&go_link)->getPropertyPtr<MaterialProperty>();
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
            terrainUniformBuffer->writeData(16 * i, 4, &dtrue);
        }else{
            terrainUniformBuffer->writeData(16 * i, 4, &dfalse);
        }
        //Check, if pair has normal texture
        if(pair->normal != nullptr){
            //Use normal texture
            pair->normal->Use(static_cast<int>(12 + i));
            terrainUniformBuffer->writeData(16 * 12 + 16 * i, 4, &dtrue);
        }else{
            terrainUniformBuffer->writeData(16 * 12 + 16 * i, 4, &dfalse);
        }
    }
    //Tell shader, that we rendering terrain in normal mode (non picking)
    terrainUniformBuffer->writeData(16 * 12 * 2, 4, &dfalse);
    //Send dimensions to buffer
    terrainUniformBuffer->writeData(16 * 12 * 2 + 4, 4, &this->data.W);
    terrainUniformBuffer->writeData(16 * 12 * 2 + 8, 4, &this->data.H);

    //Apply material shader
    mat->onRender(pipeline);


}

void TileProperty::onRender(Engine::RenderPipeline* pipeline){
    Engine::Shader* tile_shader = pipeline->getTileShader();

    tile_shader->Use();

    pipeline->tileBuffer->bind();

    //Checking for diffuse texture
    if(texture_diffuse != nullptr){
        texture_diffuse->Use(0); //Use this texture
    }

    int diffuse1_ = texture_diffuse != nullptr;
    pipeline->tileBuffer->writeData(20, 4, &diffuse1_);

    //Checking for transparent texture
    if(texture_transparent != nullptr){
        texture_transparent->Use(1); //Use this texture
    }
    int diffuse2_ = texture_transparent != nullptr;
    pipeline->tileBuffer->writeData(24, 4, &diffuse2_);
    //calculate animation state
    int anim_state_i = anim_property.isAnimated && anim_state.playing;
    //Sending animation info
    if(anim_property.isAnimated && anim_state.playing == true){ //If tile animated, then send anim state to shader
        pipeline->tileBuffer->writeData(16, 4, &anim_state_i);
        pipeline->tileBuffer->writeData(0, 4, &anim_property.framesX);
        pipeline->tileBuffer->writeData(4, 4, &anim_property.framesY);
        pipeline->tileBuffer->writeData(8, 4, &anim_state.cur_frameX);
        pipeline->tileBuffer->writeData(12, 4, &anim_state.cur_frameY);
    }else{ //No animation or unplayed
        pipeline->tileBuffer->writeData(16, 4, &anim_state_i);
    }
}

void SkyboxProperty::onPreRender(Engine::RenderPipeline* pipeline){
    pipeline->getRenderSettings()->skybox_ptr = static_cast<void*>(this);
}

void SkyboxProperty::DrawSky(RenderPipeline* pipeline){
    if(!this->isActive())
        return;
    if(((World*)world_ptr)->updateLink(&this->go_link) == nullptr) return;
    //Get pointer to Material property
    MaterialProperty* mat = ((World*)world_ptr)->updateLink(&this->go_link)->getPropertyPtr<MaterialProperty>();
    if(mat == nullptr) return;
    //Apply material shader
    mat->onRender(pipeline);
    //Draw skybox cube
    glDisable(GL_DEPTH_TEST);
    Engine::getSkyboxMesh()->Draw();
}

void ShadowCasterProperty::onPreRender(Engine::RenderPipeline* pipeline){
    pipeline->getRenderSettings()->shadowcaster_ptr = static_cast<void*>(this);
}

void ShadowCasterProperty::Draw(Engine::Camera* cam, RenderPipeline* pipeline){
    if(!isRenderAvailable()){
        glViewport(0, 0, TextureWidth, TextureHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer); //Bind framebuffer
        glClear(GL_DEPTH_BUFFER_BIT);
        return;
    }

    LightsourceProperty* light = ((World*)world_ptr)->updateLink(&this->go_link)->getPropertyPtr<LightsourceProperty>();

    //ZSVECTOR3 cam_pos = cam->getCameraPosition() + cam->getCameraFrontVec() * 20;
    ZSVECTOR3 cam_pos = cam->getCameraPosition() - light->direction * 20;
    this->LightProjectionMat = getOrthogonal(-projection_viewport, projection_viewport, -projection_viewport, projection_viewport, nearPlane, farPlane);
    this->LightViewMat = matrixLookAt(cam_pos, cam_pos + light->direction * -1, ZSVECTOR3(0,1,0));
    //Changing viewport to texture sizes
    glViewport(0, 0, TextureWidth, TextureHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer); //Bind framebuffer
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glDisable(GL_CULL_FACE);
    //Bind shadow uniform buffer
    pipeline->shadowBuffer->bind();
    //Bind ortho shadow projection
    pipeline->shadowBuffer->writeData(0, sizeof (ZSMATRIX4x4), &LightProjectionMat);
    //Bind shadow view matrix
    pipeline->shadowBuffer->writeData(sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &LightViewMat);
    //Send BIAS value
    pipeline->shadowBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, 4, &shadow_bias);
    //Send Width of shadow texture
    pipeline->shadowBuffer->writeData(sizeof (ZSMATRIX4x4) * 2 + 8, 4, &this->TextureWidth);
    //Send Height of shadow texture
    pipeline->shadowBuffer->writeData(sizeof (ZSMATRIX4x4) * 2 + 12, 4, &this->TextureHeight);
    //Use shadowmap shader to draw objects
    pipeline->getShadowmapShader()->Use();
    //Render to depth all scene
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

void ShadowCasterProperty::setTextureSize(){
    glDeleteTextures(1, &this->shadowDepthTexture);
    glDeleteFramebuffers(1, &shadowBuffer);

    init();
}

void ShadowCasterProperty::setTexture(){
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, this->shadowDepthTexture);
}

Engine::Shader* RenderPipeline::getPickingShader(){
    return this->pick_shader;
}

Engine::Shader* RenderPipeline::getShadowmapShader(){
    return this->shadowMap;
}

Engine::Shader* RenderPipeline::getUiShader(){
    return this->ui_shader;
}
