#include "headers/ThumbnailsMaster.h"
#include <GL/glew.h>

#include <iostream>
#define THUMBNAIL_IMG_SIZE 512

extern RenderPipeline* renderer;
//Hack to support resources
extern ZSGAME_DATA* game_data;
extern Project* project_ptr;

const char texture_shaderFS[310] = "#version 420 core\n\
        in vec2 _UV;\n\
        out vec4 FragColor;\n\
        uniform sampler2D texturem;\n\
        void main(){\n\
        FragColor = texture(texturem, _UV);\n\
        }\n";

const char texture_shaderVS[343] = "#version 420 core\n\
        layout (location = 0) in vec3 pos;\n\
        layout (location = 1) in vec2 uv;\n\
        out vec2 _UV;\n\
        void main(){\n\
        _UV = uv;\n\
        gl_Position = vec4(pos, 1);\n\
        }\n";

const char mesh_shaderVS[590] = "#version 420 core\n\
        layout (location = 0) in vec3 pos;\n\
        layout (location = 1) in vec2 uv;\n\
        out vec2 _UV;\n\
        layout (std140, binding = 0) uniform CamMatrices{\n\
            uniform mat4 cam_projection;\n\
            uniform mat4 cam_view;\n\
            uniform mat4 object_transform;\n\
            uniform vec3 cam_position;\n\
        };\n\
        void main(){\n\
        _UV = uv;\n\
        gl_Position = cam_projection * cam_view * object_transform * vec4(pos, 1);\n\
        }\n";

ThumbnailsMaster::ThumbnailsMaster(){

}

ThumbnailsMaster::~ThumbnailsMaster(){
    //interate over all textures and clear them
    this->texture_thumbnails.clear();
    texture_shader->Destroy();
}

void ThumbnailsMaster::initShader(){
    texture_shader = Engine::allocShader();
    mesh_shader = Engine::allocShader();

    texture_shader->compileFromStr(&texture_shaderVS[0], &texture_shaderFS[0]);
    mesh_shader->compileFromStr(&mesh_shaderVS[0], &texture_shaderFS[0]);
}

void ThumbnailsMaster::createTexturesThumbnails(){
    //Compile texture shader
    initShader();
    glViewport(0, 0, THUMBNAIL_IMG_SIZE, THUMBNAIL_IMG_SIZE);
    texture_shader->Use();

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    //Iterate over all resources
    for(unsigned int res_i = 0; res_i < game_data->resources->getResourcesSize(); res_i ++){
        Engine::ZsResource* resource_ptr = game_data->resources->getResourceByIndex(res_i);
        if(resource_ptr->resource_type != RESOURCE_TYPE_TEXTURE) continue;

        Engine::TextureResource* texture_ptr = static_cast<Engine::TextureResource*>(resource_ptr);
        DrawTexture(texture_ptr);
        //Allocate image buffer
        unsigned char* texture_data = new unsigned char[THUMBNAIL_IMG_SIZE * THUMBNAIL_IMG_SIZE * 4];
        //Read image to buffer from GL buffer
        glReadPixels(0, 0, THUMBNAIL_IMG_SIZE, THUMBNAIL_IMG_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, &texture_data[0]);

        QImage* image = new QImage(texture_data, THUMBNAIL_IMG_SIZE, THUMBNAIL_IMG_SIZE, QImage::Format_RGBA8888);
        texture_thumbnails.insert(std::pair<std::string, QImage*>(project_ptr->root_path + "/" + resource_ptr->rel_path, image));
        //delete[] texture_data;
    }
}

void ThumbnailsMaster::prepareMaterialThumbnailPipeline(){
    glViewport(0, 0, THUMBNAIL_IMG_SIZE, THUMBNAIL_IMG_SIZE);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    {   //send camera data to transform shader and skybox shader
        Engine::Camera cam;
        cam.setProjectionType(ZSCAMERA_PROJECTION_PERSPECTIVE);
        cam.setPosition(ZSVECTOR3(0, 0, -2.6f));
        cam.setFront(ZSVECTOR3(0,0,1));
        cam.setViewport(Engine::ZSVIEWPORT(0,0, THUMBNAIL_IMG_SIZE, THUMBNAIL_IMG_SIZE));
        cam.setZplanes(0.1f, 5000.f);

        renderer->transformBuffer->bind();
        ZSMATRIX4x4 proj = cam.getProjMatrix();
        ZSMATRIX4x4 view = cam.getViewMatrix();
        ZSMATRIX4x4 model = getIdentity();
        renderer->transformBuffer->writeData(0, sizeof (ZSMATRIX4x4), &proj);
        renderer->transformBuffer->writeData(sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &view);
        renderer->transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &model);
        //Send translation to skybox uniform buffer
        renderer->skyboxTransformUniformBuffer->bind();
        renderer->skyboxTransformUniformBuffer->writeData(0, sizeof (ZSMATRIX4x4), &proj);
        renderer->skyboxTransformUniformBuffer->writeData(sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &view);
    }
    {
        //Set lights to lighting shader
        renderer->lightsBuffer->bind();
        unsigned int light_i = 0;
        int light_type = 1;
        float light_intensity = 0.8f;
        ZSVECTOR3 pos = ZSVECTOR3(0,0,0);
        ZSVECTOR3 light_dir = _getDirection(66, 30, 30);
        ZSRGBCOLOR color = ZSRGBCOLOR(255,255,255,255);
        color.updateGL();

        renderer->lightsBuffer->writeData(64 * light_i, sizeof (int), &light_type);
        //glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 4, sizeof (float), &_light_ptr->range);
        renderer->lightsBuffer->writeData(64 * light_i + 8, sizeof (float), &light_intensity);
        //glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 12, sizeof (float), &_light_ptr->spot_angle);
        renderer->lightsBuffer->writeData(64 * light_i + 16, 12, &pos);
        renderer->lightsBuffer->writeData(64 * light_i + 32, 12, &light_dir);
        renderer->lightsBuffer->writeData(64 * light_i + 48, 4, &color.gl_r);
        renderer->lightsBuffer->writeData(64 * light_i + 52, 4, &color.gl_g);
        renderer->lightsBuffer->writeData(64 * light_i + 56, 4, &color.gl_b);

        int ls = 1;
        renderer->lightsBuffer->writeData(64 * MAX_LIGHTS_AMOUNT, 4, &ls);
    }

}

void ThumbnailsMaster::createMaterialThumbnails(){
    prepareMaterialThumbnailPipeline();
    //Iterate over all resources
    for(unsigned int res_i = 0; res_i < game_data->resources->getResourcesSize(); res_i ++){

        Engine::ZsResource* resource_ptr = game_data->resources->getResourceByIndex(res_i);
        if(resource_ptr->resource_type != RESOURCE_TYPE_MATERIAL) continue;

        Engine::MaterialResource* m_ptr = static_cast<Engine::MaterialResource*>(resource_ptr);

        Material* material_ptr = m_ptr->material;
        DrawMaterial(material_ptr);
        //Allocate image buffer
        unsigned char* texture_data = new unsigned char[THUMBNAIL_IMG_SIZE * THUMBNAIL_IMG_SIZE * 4];
        //Read image to buffer from GL buffer
        glReadPixels(0, 0, THUMBNAIL_IMG_SIZE, THUMBNAIL_IMG_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, &texture_data[0]);

        QImage* image = new QImage(texture_data, THUMBNAIL_IMG_SIZE, THUMBNAIL_IMG_SIZE, QImage::Format_RGBA8888);
        //texture_thumbnails.insert(std::pair<std::string, QImage*>(resource_ptr->file_path.toStdString(), image));
        if(isAvailable(project_ptr->root_path + "/" + resource_ptr->rel_path)){
            QImage* img_old = texture_thumbnails.at(project_ptr->root_path + "/" + resource_ptr->rel_path);
            delete img_old;

            texture_thumbnails.at(project_ptr->root_path + "/" + resource_ptr->rel_path) = image;
        }else{
            texture_thumbnails.insert(std::pair<std::string, QImage*>(project_ptr->root_path + "/" + resource_ptr->rel_path, image));
        }
    }
}

void ThumbnailsMaster::createMaterialThumbnail(QString name){
    prepareMaterialThumbnailPipeline();

    Engine::ZsResource* resource_ptr = game_data->resources->getMaterialByLabel(name.toStdString());
    if(resource_ptr->resource_type != RESOURCE_TYPE_MATERIAL) return;

    Engine::MaterialResource* m_ptr = static_cast<Engine::MaterialResource*>(resource_ptr);
    Material* material_ptr = m_ptr->material;

    DrawMaterial(material_ptr);
    //Allocate image buffer
    unsigned char* texture_data = new unsigned char[512 * 512 * 4];
    //Read image to buffer from GL buffer
    glReadPixels(0, 0, 512, 512, GL_RGBA, GL_UNSIGNED_BYTE, &texture_data[0]);

    QImage* image = new QImage(texture_data, 512, 512, QImage::Format_RGBA8888);
    if(isAvailable(project_ptr->root_path + "/" + resource_ptr->rel_path)){
        QImage* img_old = texture_thumbnails.at(project_ptr->root_path + "/" + resource_ptr->rel_path);
        delete img_old;

        texture_thumbnails.at(project_ptr->root_path + "/" + resource_ptr->rel_path) = image;
    }else{
        texture_thumbnails.insert(std::pair<std::string, QImage*>(project_ptr->root_path + "/" + resource_ptr->rel_path, image));
    }

}

void ThumbnailsMaster::DrawTexture(Engine::TextureResource* texture){

    //use texture
    texture->Use(0);
    while(texture->resource_state != STATE_LOADED){
        texture->Use(0);
    }
    texture->Use(0);
    //draw plane
    Engine::getPlaneMesh2D()->Draw();
}

void ThumbnailsMaster::DrawMaterial(Material* material){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    material->applyMatToPipeline();

    Engine::getSphereMesh()->Draw();
}

void ThumbnailsMaster::createMeshesThumbnails(){
    //Compile texture shader
    glViewport(0, 0, THUMBNAIL_IMG_SIZE, THUMBNAIL_IMG_SIZE);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    mesh_shader->Use();

    //Iterate over all resources
    for(unsigned int res_i = 0; res_i < project_struct_ptr->resources.size(); res_i ++){
        Resource* resource_ptr = &this->project_struct_ptr->resources[res_i];
        if(resource_ptr->type != RESOURCE_TYPE_MESH) continue;

        Engine::Mesh* mesh_ptr = static_cast<Engine::Mesh*>(resource_ptr->class_ptr);
        DrawMesh(mesh_ptr);
        //Allocate image buffer
        unsigned char* texture_data = new unsigned char[THUMBNAIL_IMG_SIZE * THUMBNAIL_IMG_SIZE * 4];
        //Read image to buffer from GL buffer
        glReadPixels(0, 0, THUMBNAIL_IMG_SIZE, THUMBNAIL_IMG_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, &texture_data[0]);

        QImage* image = new QImage(texture_data, THUMBNAIL_IMG_SIZE, THUMBNAIL_IMG_SIZE, QImage::Format_RGBA8888);
        texture_thumbnails.insert(std::pair<std::string, QImage*>(resource_ptr->resource_label, image));
        //delete[] texture_data;
    }
}

void ThumbnailsMaster::DrawMesh(Engine::Mesh* mesh){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Engine::Camera cam;
    cam.setProjectionType(ZSCAMERA_PROJECTION_PERSPECTIVE);
    cam.setPosition(ZSVECTOR3(0, 0, -6));
    cam.setFront(ZSVECTOR3(0,0,1));
    cam.setViewport(Engine::ZSVIEWPORT(0,0, 512, 512));
    cam.setZplanes(0.1f, 5000.f);

    renderer->transformBuffer->bind();
    ZSMATRIX4x4 proj = cam.getProjMatrix();
    ZSMATRIX4x4 view = cam.getViewMatrix();
    ZSMATRIX4x4 model = getIdentity();
    renderer->transformBuffer->writeData(0, sizeof (ZSMATRIX4x4), &proj);
    renderer->transformBuffer->writeData(sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &view);
    renderer->transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &model);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    mesh->Draw();
}

bool ThumbnailsMaster::isAvailable(std::string name){
   if(texture_thumbnails.count(name) > 0)
       return true;
   else
    return false;
}
