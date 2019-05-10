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
}

void RenderPipeline::setup(int bufWidth, int bufHeight){
    this->tile_shader.compileFromFile("Shaders/2d_tile/tile2d.vs", "Shaders/2d_tile/tile2d.fs");
    this->pick_shader.compileFromFile("Shaders/pick/pick.vs", "Shaders/pick/pick.fs");
    this->obj_mark_shader.compileFromFile("Shaders/mark/mark.vs", "Shaders/mark/mark.fs");
    this->deffered_light.compileFromFile("Shaders/postprocess/deffered_light/deffered.vs", "Shaders/postprocess/deffered_light/deffered.fs");
    this->diffuse3d_shader.compileFromFile("Shaders/3d/3d.vs", "Shaders/3d/3d.fs");
    ZSPIRE::setupDefaultMeshes();

    this->gbuffer.create(bufWidth, bufHeight);
    removeLights();

    MtShProps::genDefaultMtShGroup(&diffuse3d_shader);
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

ZSRGBCOLOR RenderPipeline::getColorOfPickedTransformControl(ZSVECTOR3 translation, int mouseX, int mouseY){
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Picking state

    if(depthTest == true) //if depth is enabled
        glDisable(GL_DEPTH_TEST);

    if(cullFaces == true)
        glDisable(GL_CULL_FACE);

    getGizmosRenderer()->drawTransformControls(translation, 100, 10);

    unsigned char data[4];
    glReadPixels(mouseX, 480 - mouseY, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    return ZSRGBCOLOR(data[0], data[1], data[2]);
}

unsigned int RenderPipeline::render_getpickedObj(void* projectedit_ptr, int mouseX, int mouseY){
    EditWindow* editwin_ptr = static_cast<EditWindow*>(projectedit_ptr);
    World* world_ptr = &editwin_ptr->world;
    ZSPIRE::Camera* cam_ptr = &editwin_ptr->edit_camera;

    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pick_shader.Use();
    pick_shader.setCamera(cam_ptr);
    //Picking state
    this->current_state = PIPELINE_STATE_PICKING;

    if(depthTest == false) //if depth is disable, then enable it for a little time
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
    ZSPIRE::Camera* cam_ptr = nullptr; //We'll set it next

    this->deltaTime = editwin_ptr->deltaTime;

    if(editwin_ptr->isSceneCamera){
        cam_ptr = &world_ptr->world_camera;
    }else{
        cam_ptr = &editwin_ptr->edit_camera;
    }

    this->cam = cam_ptr;
    this->win_ptr = editwin_ptr;
    //Active Geometry framebuffer
    gbuffer.bindFramebuffer();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->updateShadersCameraInfo(cam_ptr); //Send camera properties to all drawing shaders

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

    deffered_light.setGLuniformVec3("ambient_color", ZSVECTOR3(render_settings.ambient_light_color.r / 255.0f,
                                                               render_settings.ambient_light_color.g / 255.0f,
                                                               render_settings.ambient_light_color.b / 255.0f));

    ZSPIRE::getPlaneMesh2D()->Draw(); //Draw screen

    SDL_GL_SwapWindow(w); //Send rendered frame
}

void GameObject::Draw(RenderPipeline* pipeline){
    ZSPIRE::Shader* shader = pipeline->processShaderOnObject(static_cast<void*>(this)); //Will be used next time
    EditWindow* editwin_ptr = static_cast<EditWindow*>(pipeline->win_ptr);
    TransformProperty* transform_prop = static_cast<TransformProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));

    if(shader != nullptr && transform_prop != nullptr){
        //send transform matrix to shader
        shader->setTransform(transform_prop->transform_mat);
        //Get mesh pointer
        MeshProperty* mesh_prop = static_cast<MeshProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
        if(mesh_prop != nullptr){
            if(mesh_prop->mesh_ptr != nullptr){
                mesh_prop->mesh_ptr->Draw();
                //if object is picked
                if(this->isPicked == true && pipeline->current_state != PIPELINE_STATE_PICKING){
                    ZSRGBCOLOR color = ZSRGBCOLOR(0.23f * 255.0f, 0.23f * 255.0f, 0.54f * 255.0f);
                    if(editwin_ptr->obj_trstate.isTransforming == true)
                         color = ZSRGBCOLOR(255.0f, 255.0f, 0.0f);
                    //draw wireframe mesh for picked object
                    if(!editwin_ptr->isSceneRun) //avoid drawing gizmos during playtime
                        pipeline->getGizmosRenderer()->drawPickedMeshWireframe(mesh_prop->mesh_ptr, transform_prop->transform_mat, color);
                    //compare pointers
                    if(editwin_ptr->obj_trstate.isTransforming == true && this == editwin_ptr->obj_trstate.obj_ptr && !editwin_ptr->isSceneRun)
                        pipeline->getGizmosRenderer()->drawTransformControls(transform_prop->_last_translation, 100, 10);
                }
            }
        }
    }
}

void GameObject::processObject(RenderPipeline* pipeline){
    //Obtain EditWindow pointer to check if scene is running
    EditWindow* editwin_ptr = static_cast<EditWindow*>(pipeline->win_ptr);
    if(active == false || alive == false) return; //if object is inactive, not to render it

    //Call prerender on each property in object
    this->onPreRender(pipeline);

    TransformProperty* transform_prop = static_cast<TransformProperty*>(this->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    //Call update on every property in objects
    if(editwin_ptr->isSceneRun && pipeline->current_state == PIPELINE_STATE_DEFAULT)
        this->onUpdate(static_cast<int>(pipeline->deltaTime));

    //Obtain camera viewport
    ZSVIEWPORT cam_viewport = pipeline->cam->getViewport();
    //Distance limit
    int max_dist = static_cast<int>(cam_viewport.endX - cam_viewport.startX);
    bool difts = isDistanceFits(pipeline->cam->getCameraViewCenterPos(), transform_prop->_last_translation, max_dist);

    if(difts)
        this->Draw(pipeline);

    for(unsigned int obj_i = 0; obj_i < this->children.size(); obj_i ++){
        if(!children[obj_i].isEmpty()){ //if link isn't broken
            children[obj_i].updLinkPtr();
            GameObject* child_ptr = this->children[obj_i].ptr;
            child_ptr->processObject(pipeline);
        }
    }
}

ZSPIRE::Shader* RenderPipeline::processShaderOnObject(void* _obj){
    GameObject* obj = static_cast<GameObject*>(_obj);
    ZSPIRE::Shader* result = nullptr;

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
            if(tile_ptr == nullptr){ //if no tile property anymore
                obj->render_type = GO_RENDER_TYPE_NONE;
                return result;
            }
            //Checking for diffuse texture
            if(tile_ptr->texture_diffuse != nullptr){
                tile_ptr->texture_diffuse->Use(0); //Use this texture
                tile_shader.setHasDiffuseTextureProperty(true); //Shader will use picked diffuse texture

            }else{
                tile_shader.setHasDiffuseTextureProperty(false); //Shader will not use diffuse texture
            }
            //Checking for transparent texture
            if(tile_ptr->texture_transparent != nullptr){
                tile_ptr->texture_transparent->Use(5); //Use this texture
                tile_shader.setGLuniformInt("hasTransparentMap", 1); //Shader will use picked transparent texture

            }else{
                tile_shader.setGLuniformInt("hasTransparentMap", 0); //Shader will not use transparent texture
            }
            //Sending animation info
            if(tile_ptr->anim_property.isAnimated && tile_ptr->anim_state.playing == true){ //If tile animated, then send anim state to shader
                tile_shader.setGLuniformInt("animated", 1); //Send as animated shader
                //Send current animation state
                tile_shader.setGLuniformInt("total_rows", tile_ptr->anim_property.framesX);
                tile_shader.setGLuniformInt("total_cols", tile_ptr->anim_property.framesY);

                tile_shader.setGLuniformInt("selected_row", tile_ptr->anim_state.cur_frameX);
                tile_shader.setGLuniformInt("selected_col", tile_ptr->anim_state.cur_frameY);
            }else{ //No animation or unplayed
                 tile_shader.setGLuniformInt("animated", 0);
            }
            break;
        }
        case GO_RENDER_TYPE_NONE:{
            result = nullptr;
            break;
        }
        case GO_RENDER_TYPE_MATERIAL:{
            MaterialProperty* material_ptr = static_cast<MaterialProperty*>(obj->getPropertyPtrByType(GO_PROPERTY_TYPE_MATERIAL));
            if(material_ptr == nullptr) return result; //if object hasn't property

            MtShaderPropertiesGroup* group_ptr = material_ptr->group_ptr;

            if(material_ptr->group_ptr == nullptr) return result; //if material hasn't group
            //Work with shader
            result = group_ptr->render_shader;
            result->Use();
            //iterate over all properties, send them all!
            for(unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i ++){
                MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
                MaterialShaderPropertyConf* conf_ptr = material_ptr->material_ptr->confs[prop_i];
                switch(prop_ptr->type){
                    case MATSHPROP_TYPE_TEXTURE:{
                        //Cast pointer
                        TextureMaterialShaderProperty* texture_p = static_cast<TextureMaterialShaderProperty*>(prop_ptr);
                        TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);

                        if(texture_conf->texture != nullptr){
                            result->setGLuniformInt(texture_p->ToggleUniform.c_str(), 1);
                            texture_conf->texture->Use(texture_p->slotToBind);
                        }else{
                            result->setGLuniformInt(texture_p->ToggleUniform.c_str(), 0);
                        }
                        break;
                    }
                    case MATSHPROP_TYPE_FLOAT:{
                        //Cast pointer
                        FloatMaterialShaderProperty* float_p = static_cast<FloatMaterialShaderProperty*>(prop_ptr);
                        FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);

                        result->setGLuniformFloat(float_p->floatUniform.c_str(), float_conf->value);
                        break;
                    }
                    case MATSHPROP_TYPE_INTEGER:{
                        //Cast pointer
                        IntegerMaterialShaderProperty* int_p = static_cast<IntegerMaterialShaderProperty*>(prop_ptr);
                        IntegerMtShPropConf* int_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);

                        result->setGLuniformInt(int_p->integerUniform.c_str(), int_conf->value);
                        break;
                    }
                    case MATSHPROP_TYPE_COLOR:{
                        //Cast pointer
                        ColorMaterialShaderProperty* color_p = static_cast<ColorMaterialShaderProperty*>(prop_ptr);
                        ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);

                        result->setGLuniformColor(color_p->colorUniform.c_str(), color_conf->color);
                        break;
                    }
                }
            }

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
    if(deffered_light.isCreated == true){
        deffered_light.Use();
        deffered_light.setCamera(cam_ptr, true);
    }
}

void RenderPipeline::addLight(void* light_ptr){
    LightsourceProperty* _light_ptr = static_cast<LightsourceProperty*>(light_ptr);
    _light_ptr->isSent = true;
    _light_ptr->updTransformPtr();
    _light_ptr->id = static_cast<unsigned char>(lights_ptr.size()); //setting id of uniform
    _light_ptr->deffered_shader_ptr = &this->deffered_light; //putting ptr to deffered shader
    this->lights_ptr.push_back(light_ptr); //pushing pointer
    this->deffered_light.Use(); //correctly put uniforms
    this->deffered_light.sendLight(_light_ptr->id, light_ptr);
    this->deffered_light.setGLuniformInt("lights_amount", static_cast<int>(lights_ptr.size()));
}

void RenderPipeline::removeLights(){
    this->lights_ptr.clear(); //clear ptr vector
    this->deffered_light.setGLuniformInt("lights_amount", 0); //set null lights in shader
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

    unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments);

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
}

void G_BUFFER_GL::Destroy(){
    //Remove textures
    glDeleteTextures(1, &tDiffuse);
    glDeleteTextures(1, &tNormal);
    glDeleteTextures(1, &tPos);
    glDeleteTextures(1, &tTransparent);

    //delete framebuffer & renderbuffer
    glDeleteRenderbuffers(1, &this->depthBuffer);
    glDeleteFramebuffers(1, &this->gBuffer);
}
