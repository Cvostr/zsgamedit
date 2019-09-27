#include "headers/ThumbnailsMaster.h"
#include <GL/glew.h>

#include <iostream>

#define MAX_LIGHTS_AMOUNT 150

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

ThumbnailsMaster::ThumbnailsMaster(){

}

ThumbnailsMaster::~ThumbnailsMaster(){
    //interate over all textures and clear them
    this->texture_thumbnails.clear();
    texture_shader.Destroy();
}

void ThumbnailsMaster::initShader(){
    texture_shader.compileFromStr(&texture_shaderVS[0], &texture_shaderFS[0]);
}

void ThumbnailsMaster::createTexturesThumbnails(){
    //Compile texture shader
    initShader();
    glViewport(0, 0, 512, 512);
    texture_shader.Use();
    texture_shader.setGLuniformInt("texturem", 0);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    //Iterate over all resources
    for(unsigned int res_i = 0; res_i < project_struct_ptr->resources.size(); res_i ++){
        Resource* resource_ptr = &this->project_struct_ptr->resources[res_i];
        if(resource_ptr->type != RESOURCE_TYPE_TEXTURE) continue;

        ZSPIRE::Texture* texture_ptr = static_cast<ZSPIRE::Texture*>(resource_ptr->class_ptr);
        DrawTexture(texture_ptr);
        //Allocate image buffer
        unsigned char* texture_data = new unsigned char[512 * 512 * 4];
        //Read image to buffer from GL buffer
        glReadPixels(0, 0, 512, 512, GL_RGBA, GL_UNSIGNED_BYTE, &texture_data[0]);

        QImage* image = new QImage(texture_data, 512, 512, QImage::Format_RGBA8888);
        texture_thumbnails.insert(std::pair<std::string, QImage*>(resource_ptr->file_path.toStdString(), image));
        //delete[] texture_data;
    }
}

void ThumbnailsMaster::createMaterialThumbnails(){
    //Compile texture shader
    glViewport(0, 0, 512, 512);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glBindBuffer(GL_UNIFORM_BUFFER, 14);
    int light_i = 0;
    int light_type = 1;
    float light_intensity = 0.8f;
    ZSVECTOR3 pos = ZSVECTOR3(0,0,0);
    ZSVECTOR3 light_dir = _getDirection(66, 30, 30);
    ZSRGBCOLOR color = ZSRGBCOLOR(255,255,255,255);
    color.updateGL();

        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i, sizeof (int), &light_type);
        //glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 4, sizeof (float), &_light_ptr->range);
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 8, sizeof (float), &light_intensity);
        //glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 12, sizeof (float), &_light_ptr->spot_angle);
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 16, 12, &pos);
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 32, 12, &light_dir);
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 48, 4, &color.gl_r);
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 52, 4, &color.gl_g);
        glBufferSubData(GL_UNIFORM_BUFFER, 64 * light_i + 56, 4, &color.gl_b);


    int ls = 1;
    glBufferSubData(GL_UNIFORM_BUFFER, 64 * MAX_LIGHTS_AMOUNT, 4, &ls);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //Iterate over all resources
    for(unsigned int res_i = 0; res_i < project_struct_ptr->resources.size(); res_i ++){
        Resource* resource_ptr = &this->project_struct_ptr->resources[res_i];
        if(resource_ptr->type != RESOURCE_TYPE_MATERIAL) continue;

        Material* material_ptr = static_cast<Material*>(resource_ptr->class_ptr);
        DrawMaterial(material_ptr);
        //Allocate image buffer
        unsigned char* texture_data = new unsigned char[512 * 512 * 4];
        //Read image to buffer from GL buffer
        glReadPixels(0, 0, 512, 512, GL_RGBA, GL_UNSIGNED_BYTE, &texture_data[0]);

        QImage* image = new QImage(texture_data, 512, 512, QImage::Format_RGBA8888);
        texture_thumbnails.insert(std::pair<std::string, QImage*>(resource_ptr->file_path.toStdString(), image));
        //delete[] texture_data;
    }
}

void ThumbnailsMaster::DrawTexture(ZSPIRE::Texture* texture){
    //use texture
    texture->Use(0);
    //draw plane
    ZSPIRE::getPlaneMesh2D()->Draw();
}

void ThumbnailsMaster::DrawMaterial(Material* material){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    material->group_ptr->render_shader->Use();
    material->applyMatToPipeline();

    ZSPIRE::Camera cam;
    cam.setProjectionType(ZSCAMERA_PROJECTION_PERSPECTIVE);
    cam.setPosition(ZSVECTOR3(0, 0, -3));
    cam.setFront(ZSVECTOR3(0,0,1));
    cam.setViewport(ZSVIEWPORT(0,0, 512, 512));
    cam.setZplanes(0.1f, 5000.f);

    glBindBuffer(GL_UNIFORM_BUFFER, 13);
    ZSMATRIX4x4 proj = cam.getProjMatrix();
    ZSMATRIX4x4 view = cam.getViewMatrix();
    ZSMATRIX4x4 model = getIdentity();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof (ZSMATRIX4x4), &proj);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &view);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &model);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    ZSPIRE::getSphereMesh()->Draw();
}

void ThumbnailsMaster::DrawMesh(ZSPIRE::Mesh* mesh){
    mesh->Draw();
}

bool ThumbnailsMaster::isAvailable(std::string name){
   if(texture_thumbnails.count(name) > 0)
       return true;
   else
    return false;
}
