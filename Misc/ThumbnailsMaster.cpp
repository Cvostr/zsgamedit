#include "headers/ThumbnailsMaster.h"
#include <GL/glew.h>

#include <iostream>

const char texture_shaderFS[310] = "#version 150 core\n\
        #extension GL_ARB_explicit_attrib_location : require\n\
        #extension GL_ARB_explicit_uniform_location : require\n\
        in vec2 _UV;\n\
        out vec4 FragColor;\n\
        uniform sampler2D texturem;\n\
        void main(){\n\
        FragColor = texture(texturem, _UV);\n\
        }\n";

const char texture_shaderVS[343] = "#version 150 core\n\
        #extension GL_ARB_explicit_attrib_location : require\n\
        #extension GL_ARB_explicit_uniform_location : require\n\
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

    glEnable(GL_DEPTH_TEST);

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
/*
    glBindBuffer(GL_UNIFORM_BUFFER, camBuffer);
    ZSMATRIX4x4 proj = cam_ptr->getProjMatrix();
    ZSMATRIX4x4 view = cam_ptr->getViewMatrix();
    ZSVECTOR3 cam_pos = cam_ptr->getCameraPosition();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof (ZSMATRIX4x4), &proj);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4), sizeof (ZSMATRIX4x4), &view);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof (ZSMATRIX4x4) * 3, sizeof(ZSVECTOR3), &cam_pos);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
*/
    ZSPIRE::getSphereMesh();
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
