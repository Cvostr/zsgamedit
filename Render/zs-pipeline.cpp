#include "headers/zs-pipeline.h"
#include "../World/headers/obj_properties.h"
#include "../World/headers/2dtileproperties.h"
#include "../ProjEd/headers/ProjectEdit.h"
#include <iostream>

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
    this->tile_shader.compileFromFile("Shaders/2d_tile/tile2d.vs", "Shaders/2d_tile/tile2d.fs");
    this->pick_shader.compileFromFile("Shaders/pick/pick.vs", "Shaders/pick/pick.fs");
    this->obj_mark_shader.compileFromFile("Shaders/mark/mark.vs", "Shaders/mark/mark.fs");
    this->deffered_light.compileFromFile("Shaders/postprocess/deffered_light/deffered.vs", "Shaders/postprocess/deffered_light/deffered.fs");
    this->diffuse3d_shader.compileFromFile("Shaders/3d/3d.vs", "Shaders/3d/3d.fs");
    this->ui_shader.compileFromFile("Shaders/ui/ui.vs", "Shaders/ui/ui.fs");
    skybox.compileFromFile("Shaders/skybox/skybox.vs", "Shaders/skybox/skybox.fs");
    shadowMap.compileFromFile("Shaders/shadowmap/shadowmap.vs", "Shaders/shadowmap/shadowmap.fs");
    heightmap.compileFromFile("Shaders/heightmap/heightmap.vs", "Shaders/heightmap/heightmap.fs");

    ZSPIRE::setupDefaultMeshes();

    this->gbuffer.create(bufWidth, bufHeight);
    removeLights();

    MtShProps::genDefaultMtShGroup(&diffuse3d_shader, &skybox, &heightmap);
}

void RenderPipeline::initGizmos(int projectPespective){
    gizmos = new GizmosRenderer(&obj_mark_shader, this->depthTest, this->cullFaces, projectPespective);
}

RenderPipeline::~RenderPipeline(){
    this->tile_shader.Destroy();
    this->pick_shader.Destroy();
    this->obj_mark_shader.Destroy();
    this->deffered_light.Destroy();
    this->diffuse3d_shader.Destroy();
    ui_shader.Destroy();
    skybox.Destroy();
    shadowMap.Destroy();
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
    //initialize gizmos component
    initGizmos(this->project_struct_ptr->perspective);
    //setup GBUFFER
    setup(this->WIDTH, this->HEIGHT);
}

ZSRGBCOLOR RenderPipeline::getColorOfPickedTransformControl(ZSVECTOR3 translation, int mouseX, int mouseY){
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Picking state

    if(depthTest == true) //if depth is enabled
        glDisable(GL_DEPTH_TEST);

    if(cullFaces == true)
        glDisable(GL_CULL_FACE);

    getGizmosRenderer()->drawTransformControls(translation, 200, 25);

    unsigned char data[4];
    glReadPixels(mouseX, this->HEIGHT - mouseY, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    return ZSRGBCOLOR(data[0], data[1], data[2]);
}

unsigned int RenderPipeline::render_getpickedObj(void* projectedit_ptr, int mouseX, int mouseY){
    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    World* world_ptr = &editwin_ptr->world;
    ZSPIRE::Camera* cam_ptr = &editwin_ptr->edit_camera;

    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_BLEND);
    pick_shader.Use();
    pick_shader.setCamera(cam_ptr);
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

void RenderPipeline::render(SDL_Window* w, void* projectedit_ptr)
{
    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    World* world_ptr = &editwin_ptr->world;
    ZSPIRE::Camera* cam_ptr = nullptr; //We'll set it next

    this->deltaTime = editwin_ptr->deltaTime;

    if(editwin_ptr->isWorldCamera){
        cam_ptr = &world_ptr->world_camera;
    }else{
        cam_ptr = &editwin_ptr->edit_camera;
    }

    this->cam = cam_ptr;
    this->win_ptr = editwin_ptr;

    ShadowCasterProperty* shadowcast = static_cast<ShadowCasterProperty*>(this->render_settings.shadowcaster_ptr);
    if(shadowcast != nullptr){
        shadowcast->Draw(cam_ptr, this);
    }

    //Active Geometry framebuffer
    gbuffer.bindFramebuffer();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_BLEND);
    glViewport(0, 0, this->WIDTH, this->HEIGHT);

    this->updateShadersCameraInfo(cam_ptr); //Send camera properties to all drawing shaders

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

    //compare pointers
    if(editwin_ptr->obj_trstate.isTransforming == true && !editwin_ptr->isWorldCamera)
        getGizmosRenderer()->drawTransformControls(editwin_ptr->obj_trstate.obj_ptr->getTransformProperty()->_last_translation, 100, 10);


    //Turn everything off to draw deffered plane correctly
    if(depthTest == true) //if depth is enabled
        glDisable(GL_DEPTH_TEST);

    if(cullFaces == true) //if GL face cull is enabled
        glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); //Back to default framebuffer
    glClear(GL_COLOR_BUFFER_BIT); //Clear screen
    gbuffer.bindTextures(); //Bind gBuffer textures
    deffered_light.Use(); //use deffered shader

    for(unsigned int light_i = 0; light_i < this->lights_ptr.size(); light_i ++){
        LightsourceProperty* _light_ptr = static_cast<LightsourceProperty*>(lights_ptr[light_i]);

        this->deffered_light.sendLight(light_i, _light_ptr);
    }
    //send amount of lights to deffered shader
    this->deffered_light.setGLuniformInt("lights_amount", static_cast<int>(lights_ptr.size()));
    //free lights array
    this->removeLights();

    deffered_light.setGLuniformVec3("ambient_color", ZSVECTOR3(render_settings.ambient_light_color.r / 255.0f,
                                                               render_settings.ambient_light_color.g / 255.0f,
                                                               render_settings.ambient_light_color.b / 255.0f));

    ZSPIRE::getPlaneMesh2D()->Draw(); //Draw screen

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
    f[5] = static_cast<int>(L' ');
    f[6] = static_cast<int>(L'W');
    f[7] = static_cast<int>(L'o');
    f[8] = static_cast<int>(L'r');
    f[9] = static_cast<int>(L'l');
    f[10] = static_cast<int>(L'd');
    //c->DrawString(f, 11, ZSVECTOR2(10,10));

    //std::cout << static_cast<int>(deltaTime) << std::endl;

    SDL_GL_SwapWindow(w); //Send rendered frame
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
    if(hasMesh()){// if object has mesh
        //If we are in default draw mode
        if(pipeline->current_state == PIPELINE_STATE_DEFAULT){
            this->onRender(pipeline);
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
            pipeline->getPickingShader()->setTransform(transform_ptr->transform_mat);
            pipeline->getPickingShader()->setGLuniformVec4("color", ZSVECTOR4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
            DrawMesh();
        }
        if(pipeline->current_state == PIPELINE_STATE_SHADOWDEPTH) {
            TransformProperty* transform_ptr = static_cast<TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
            pipeline->getShadowmapShader()->Use();
            pipeline->getShadowmapShader()->setTransform(transform_ptr->transform_mat);
            if(mesh_prop->castShadows)
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
            if(!editwin_ptr->isWorldCamera) //avoid drawing gizmos during playtime
                pipeline->getGizmosRenderer()->drawPickedMeshWireframe(mesh_prop->mesh_ptr, transform_ptr->transform_mat, color);
        }
    }
}

void GameObject::processObject(RenderPipeline* pipeline){
    if(alive == false) return;
    if(this->active)
        item_ptr->setTextColor(0, QColor(Qt::black));
    else
        item_ptr->setTextColor(0, QColor(Qt::gray));

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
    //int max_dist = static_cast<int>(cam_viewport.endX - cam_viewport.startX);
    //bool difts = isDistanceFits(pipeline->cam->getCameraViewCenterPos(), transform_prop->_last_translation, max_dist);

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

    if(material_ptr == nullptr) return;

    MaterialProperty* material_ptr = static_cast<MaterialProperty*>(this->go_link.updLinkPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_MATERIAL));
    TransformProperty* transform_ptr = static_cast<TransformProperty*>(this->go_link.updLinkPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    MtShaderPropertiesGroup* group_ptr = material_ptr->material_ptr->group_ptr;

    if(transform_ptr == nullptr || group_ptr == nullptr) return ; //if object hasn't property

    //Work with shader
    shader = group_ptr->render_shader;
    shader->Use();
    //Get pointer to shadowcaster
    ShadowCasterProperty* shadowcast = static_cast<ShadowCasterProperty*>(pipeline->getRenderSettings()->shadowcaster_ptr);
    if(shadowcast != nullptr && this->material_ptr->group_ptr->acceptShadows && this->receiveShadows){
        shadowcast->sendData(shader);
    }
    if(!this->receiveShadows || shadowcast == nullptr || !shadowcast->active){
        shader->setGLuniformInt("hasShadowMap", 0);
    }

    shader->setTransform(transform_ptr->transform_mat);

    //iterate over all properties, send them all!
    for(unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i ++){
        MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
        MaterialShaderPropertyConf* conf_ptr = material_ptr->material_ptr->confs[prop_i];
        switch(prop_ptr->type){
            case MATSHPROP_TYPE_NONE:{
                break;
            }
            case MATSHPROP_TYPE_TEXTURE:{
                //Cast pointer
                TextureMaterialShaderProperty* texture_p = static_cast<TextureMaterialShaderProperty*>(prop_ptr);
                TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);

                if(texture_conf->texture != nullptr){
                    shader->setGLuniformInt(texture_p->ToggleUniform.c_str(), 1); //Set texture uniform toggle
                    texture_conf->texture->Use(texture_p->slotToBind); //Use texture
                }else{
                    shader->setGLuniformInt(texture_p->ToggleUniform.c_str(), 0);
                }
                break;
            }
            case MATSHPROP_TYPE_FLOAT:{
                //Cast pointer
                FloatMaterialShaderProperty* float_p = static_cast<FloatMaterialShaderProperty*>(prop_ptr);
                FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);

                shader->setGLuniformFloat(float_p->floatUniform.c_str(), float_conf->value);
                break;
            }
            case MATSHPROP_TYPE_INTEGER:{
                //Cast pointer
                IntegerMaterialShaderProperty* int_p = static_cast<IntegerMaterialShaderProperty*>(prop_ptr);
                IntegerMtShPropConf* int_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);

                shader->setGLuniformInt(int_p->integerUniform.c_str(), int_conf->value);
                break;
            }
            case MATSHPROP_TYPE_COLOR:{
                //Cast pointer
                ColorMaterialShaderProperty* color_p = static_cast<ColorMaterialShaderProperty*>(prop_ptr);
                ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);

                shader->setGLuniformColor(color_p->colorUniform.c_str(), color_conf->color);
                break;
            }
            case MATSHPROP_TYPE_FVEC3:{
                //Cast pointer
                Float3MaterialShaderProperty* fvec3_p = static_cast<Float3MaterialShaderProperty*>(prop_ptr);
                Float3MtShPropConf* fvec3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);

                shader->setGLuniformVec3(fvec3_p->floatUniform.c_str(), fvec3_conf->value);
                break;
            }
            case MATSHPROP_TYPE_TEXTURE3:{
                //Cast pointer
                Texture3MaterialShaderProperty* texture_p = static_cast<Texture3MaterialShaderProperty*>(prop_ptr);
                Texture3MtShPropConf* texture_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);

                if(!texture_conf->texture3D->created){
                    texture_conf->texture3D->Init();
                    for(int i = 0; i < texture_conf->texture_count; i ++){
                        texture_conf->texture3D->pushTexture(i, texture_conf->rel_path.toStdString() + "/" + texture_conf->texture_str[i].toStdString());
                    }
                    texture_conf->texture3D->created = true;
                }else{
                    texture_conf->texture3D->Use(texture_p->slotToBind);
                }
                break;
            }
        }
    }
}

void TileProperty::onRender(RenderPipeline* pipeline){
    ZSPIRE::Shader* tile_shader = pipeline->getTileShader();
    //Receive pointer to tile information
    TransformProperty* transform_ptr = static_cast<TransformProperty*>(this->go_link.updLinkPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));

    if(transform_ptr == nullptr) return;

    tile_shader->Use();
    tile_shader->setTransform(transform_ptr->transform_mat);

    //Checking for diffuse texture
    if(texture_diffuse != nullptr){
        texture_diffuse->Use(0); //Use this texture
        tile_shader->setHasDiffuseTextureProperty(true); //Shader will use picked diffuse texture

    }else{
        tile_shader->setHasDiffuseTextureProperty(false); //Shader will not use diffuse texture
    }
    //Checking for transparent texture
    if(texture_transparent != nullptr){
        texture_transparent->Use(5); //Use this texture
        tile_shader->setGLuniformInt("hasTransparentMap", 1); //Shader will use picked transparent texture

    }else{
        tile_shader->setGLuniformInt("hasTransparentMap", 0); //Shader will not use transparent texture
    }
    //Sending animation info
    if(anim_property.isAnimated && anim_state.playing == true){ //If tile animated, then send anim state to shader
        tile_shader->setGLuniformInt("animated", 1); //Send as animated shader
        //Send current animation state
        tile_shader->setGLuniformInt("total_rows", anim_property.framesX);
        tile_shader->setGLuniformInt("total_cols", anim_property.framesY);

        tile_shader->setGLuniformInt("selected_row", anim_state.cur_frameX);
        tile_shader->setGLuniformInt("selected_col", anim_state.cur_frameY);
    }else{ //No animation or unplayed
         tile_shader->setGLuniformInt("animated", 0);
    }
}

void SkyboxProperty::onPreRender(RenderPipeline* pipeline){
    pipeline->getRenderSettings()->skybox_ptr = static_cast<void*>(this);
}

void SkyboxProperty::DrawSky(RenderPipeline* pipeline){
    if(!this->active) return;
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

    LightsourceProperty* light = this->go_link.updLinkPtr()->getPropertyPtr<LightsourceProperty>();

    ZSVECTOR3 cam_pos = cam->getCameraPosition() + cam->getCameraFrontVec() * 20;
    this->LightProjectionMat = getOrthogonal(-projection_viewport, projection_viewport, -projection_viewport, projection_viewport, nearPlane, farPlane);
    this->LightViewMat = matrixLookAt(cam_pos, cam_pos + light->direction * -1, ZSVECTOR3(0,1,0));

    glViewport(0, 0, TextureWidth, TextureHeight); //Changing viewport

    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer); //Bind framebuffer
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glDisable(GL_CULL_FACE);

    pipeline->getShadowmapShader()->Use();
    pipeline->getShadowmapShader()->setGLuniformMat4x4("cam_projection", LightProjectionMat);
    pipeline->getShadowmapShader()->setGLuniformMat4x4("cam_view", LightViewMat);

    pipeline->renderDepth(this->go_link.world_ptr);

    glFrontFace(GL_CCW);
}

void ShadowCasterProperty::init(){
    glGenFramebuffers(1, &this->shadowBuffer);//Generate framebuffer for texture
    glGenTextures(1, &this->shadowDepthTexture); //Generate texture

    glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
    //Configuring texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->TextureWidth, this->TextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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

void ShadowCasterProperty::sendData(ZSPIRE::Shader* shader){
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, this->shadowDepthTexture);

    shader->setGLuniformMat4x4("LightProjectionMat", this->LightProjectionMat);
    shader->setGLuniformMat4x4("LightViewMat", this->LightViewMat);
    shader->setGLuniformFloat("shadow_bias", this->shadow_bias);
    shader->setGLuniformInt("hasShadowMap", 1);
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

    deffered_light.Use();
    deffered_light.setCamera(cam_ptr, true);

    if(ui_shader.isCreated == true){
        ui_shader.Use();
        ui_shader.setCameraUiProjMatrix(cam_ptr);
    }

    if(heightmap.isCreated == true){
        heightmap.Use();
        heightmap.setCamera(cam_ptr);
    }

    skybox.Use();
    this->skybox.setGLuniformMat4x4("projection", cam_ptr->getProjMatrix());
    ZSMATRIX4x4 viewmat = cam_ptr->getViewMatrix();
    viewmat = removeTranslationFromViewMat(viewmat);
    this->skybox.setGLuniformMat4x4("view", viewmat);
}

void RenderPipeline::addLight(void* light_ptr){
    this->lights_ptr.push_back(light_ptr);
}

void RenderPipeline::removeLights(){
    this->lights_ptr.clear(); //clear ptr vector
}

G_BUFFER_GL::G_BUFFER_GL(){

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, tMasks, 0);

    unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
    glDrawBuffers(5, attachments);

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
    ui_shader.setGLuniformInt("render_mode", 1);
    //Use texture at 0 slot
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    ZSMATRIX4x4 translation = getTranslationMat(X, Y, 0.0f);
    ZSMATRIX4x4 scale = getScaleMat(scaleX, scaleY, 0.0f);
    ZSMATRIX4x4 transform = scale * translation;

    ui_shader.setTransform(transform);

    ZSPIRE::getUiSpriteMesh2D()->Draw();
}

void RenderPipeline::renderGlyph(unsigned int texture_id, int X, int Y, int scaleX, int scaleY, ZSRGBCOLOR color){
    this->ui_shader.Use();
    //tell shader, that we will render glyph
    ui_shader.setGLuniformInt("render_mode", 2);
    //sending glyph color
    ui_shader.setGLuniformColor("text_color", color);
    //Use texture at 0 slot
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    ZSMATRIX4x4 translation = getTranslationMat(X, Y, 0.0f);
    ZSMATRIX4x4 scale = getScaleMat(scaleX, scaleY, 0.0f);
    ZSMATRIX4x4 transform = scale * translation;

    ui_shader.setTransform(transform);

    ZSPIRE::getUiSpriteMesh2D()->Draw();
}

void RenderPipeline::updateWindowSize(int W, int H){
     glViewport(0, 0, W, H);

     this->gbuffer.Destroy();
     this->gbuffer.create(W, H);
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
