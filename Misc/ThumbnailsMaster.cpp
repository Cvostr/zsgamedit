#include "headers/ThumbnailsMaster.h"
#include <GL/glew.h>
#include "../Render/headers/zs-texture.h"
#include "../Render/headers/zs-mesh.h"
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
//FragColor = texture(texturem, _UV);
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

        }

void ThumbnailsMaster::initShader(){
    texture_shader.compileFromStr(&texture_shaderVS[0], &texture_shaderFS[0]);
}

void ThumbnailsMaster::createTexturesThumbnails(){
    initShader();
    glViewport(0, 0, 512, 512);
    texture_shader.Use();
    texture_shader.setGLuniformInt("texturem", 0);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    for(unsigned int res_i = 0; res_i < project_struct_ptr->resources.size(); res_i ++){
        Resource* resource_ptr = &this->project_struct_ptr->resources[res_i];
        ZSPIRE::Texture* texture_ptr = static_cast<ZSPIRE::Texture*>(resource_ptr->class_ptr);

        glClear(GL_COLOR_BUFFER_BIT);

        texture_ptr->Use(0);
        ZSPIRE::getPlaneMesh2D()->Draw();

        unsigned char* texture_data = new unsigned char[512 * 512 * 4];

        glReadPixels(0, 0, 512, 512, GL_RGBA, GL_UNSIGNED_BYTE, &texture_data[0]);

        QImage* image = new QImage(texture_data, 512, 512, QImage::Format_RGBA8888);
        texture_thumbnails.insert(std::pair<std::string, QImage*>(resource_ptr->file_path.toStdString(), image));
        //delete[] texture_data;
    }
}
