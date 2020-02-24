#include <world/go_properties.h>
#include "headers/terrain.h"
#include <GL/glew.h>
#include "../World/headers/Misc.h"
#include <fstream>
#include <list>
#include <thread>

static bool terrain_thread_working = true;
static std::list<HeightmapModifyRequest*> terrain_mdf_requests;

void terrain_loop(){
    while(terrain_thread_working){
        if(terrain_mdf_requests.size() > 0){
            HeightmapModifyRequest* req = terrain_mdf_requests.front();

            switch(req->modify_type){
                case TMT_HEIGHT:{
                    req->terrain->modifyHeight(req->originX, req->originY, req->originHeight, req->range, req->multiplyer);
                    req->terrain->hasHeightmapChanged = true;
                    req->terrain->hasPhysicShapeChanged = true;
                    break;
                }
                case TMT_TEXTURE:{
                    req->terrain->modifyTexture(req->originX, req->originY, req->range, req->texture);
                    req->terrain->hasPaintingChanged = true;
                    break;

                }
                case TMT_GRASS:{
                    req->terrain->plantGrass(req->originX, req->originY, req->range, req->grass);
                    break;
                }
            }

            terrain_mdf_requests.remove(req);
        }
    }
}

void queryTerrainModifyRequest(HeightmapModifyRequest* req){
    terrain_mdf_requests.push_back(req);
}

void startTerrainThread(){
    terrain_thread_working = true;
    std::thread loader_loop(terrain_loop);
    loader_loop.detach();
}

void stopTerrainThread(){
    terrain_thread_working = false;
    terrain_mdf_requests.clear();
}

void TerrainData::sum(unsigned char* ptr, int val){
    if(static_cast<int>(*ptr) + val <= 255)
        *ptr += val;
    else {
        *ptr = 255;
    }
}

void TerrainData::reduce(unsigned char* ptr, int val){
    if(static_cast<int>(*ptr) - val >= 0)
        *ptr -= val;
    else {
        *ptr = 0;
    }
}

void TerrainData::initGL(){
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glGenTextures(1, &this->painting.texture_mask1);
    glBindTexture(GL_TEXTURE_2D, this->painting.texture_mask1);
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &this->painting.texture_mask2);
    glBindTexture(GL_TEXTURE_2D, this->painting.texture_mask2);
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &this->painting.texture_mask3);
    glBindTexture(GL_TEXTURE_2D, this->painting.texture_mask3);
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void TerrainData::destroyGL(){
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
}

void TerrainData::Draw(bool picking){
    //if opengl data not generated, exit function
    if(!created) return;
    //small optimization in terrain painting
    if(!picking){
        glActiveTexture(GL_TEXTURE24);
        glBindTexture(GL_TEXTURE_2D, painting.texture_mask1);
        glActiveTexture(GL_TEXTURE25);
        glBindTexture(GL_TEXTURE_2D, painting.texture_mask2);
        glActiveTexture(GL_TEXTURE26);
        glBindTexture(GL_TEXTURE_2D, painting.texture_mask3);
    }
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glDrawElements(GL_TRIANGLES, (W - 1) * (H - 1) * 2 * 3, GL_UNSIGNED_INT, nullptr);
}

void TerrainData::updateTextureBuffersGL(){
    //Create texture masks texture
    glBindTexture(GL_TEXTURE_2D, this->painting.texture_mask1);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            static_cast<int>(W),
            static_cast<int>(H),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            painting._texture
     );
    glBindTexture(GL_TEXTURE_2D, this->painting.texture_mask2);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            static_cast<int>(W),
            static_cast<int>(H),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            painting._texture1
     );
    glBindTexture(GL_TEXTURE_2D, this->painting.texture_mask3);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            static_cast<int>(W),
            static_cast<int>(H),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            painting._texture2
     );

    delete [] painting._texture;
    delete [] painting._texture1;
    delete [] painting._texture2;
}

void TerrainData::updateGeometryBuffersGL(){
    glBindVertexArray(this->VAO); //Bind vertex array
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO); //Bind vertex buffer
    glBufferData(GL_ARRAY_BUFFER, static_cast<int>(W * H) * static_cast<int>(sizeof(HeightmapVertex)), vertices, GL_STATIC_DRAW); //send vertices to buffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO); //Bind index buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<unsigned int>((W - 1) * (H - 1) * 2 * 3) * sizeof(unsigned int), indices, GL_STATIC_DRAW); //Send indices to buffer

    //Vertex pos 3 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), nullptr);
    glEnableVertexAttribArray(0);
    //Vertex UV 2 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), reinterpret_cast<void*>(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    //Vertex Normals 3 floats
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), reinterpret_cast<void*>(sizeof(float) * 5));
    glEnableVertexAttribArray(2);
    //Vertex Tangent 3 floats
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), reinterpret_cast<void*>(sizeof(float) * 8));
    glEnableVertexAttribArray(3);
    //Vertex BiTangent 3 floats
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), reinterpret_cast<void*>(sizeof(float) * 11));
    glEnableVertexAttribArray(4);

    delete[] vertices;
    delete[] indices;
}

void TerrainData::generateGLMesh(){
    if(W < 1 || H < 1)
        return;
    if(!created)
        initGL();

    updateTextureBuffers();
    updateTextureBuffersGL();

    updateGeometryBuffers();
    updateGeometryBuffersGL();

    created = true;
}

void TerrainData::saveToFile(const char* file_path){
    std::ofstream world_stream;
    world_stream.open(file_path, std::ofstream::binary);
    //write dimensions
    world_stream.write(reinterpret_cast<char*>(&this->W), sizeof(int));
    world_stream.write(reinterpret_cast<char*>(&this->H), sizeof(int));

    for(int i = 0; i < W * H; i ++){
        world_stream.write(reinterpret_cast<char*>(&data[i].height), sizeof(float));
        for(int tex_factor = 0; tex_factor < TERRAIN_TEXTURES_AMOUNT; tex_factor ++)
            world_stream.write(reinterpret_cast<char*>(&data[i].texture_factors[tex_factor]), sizeof(unsigned char));
        world_stream.write(reinterpret_cast<char*>(&data[i].grass), sizeof(int));
    }

    world_stream.close();
}

void TerrainData::modifyHeight(int originX, int originY, float originHeight, int range, int multiplyer){
    //Iterate over all pixels
    for(int y = 0; y < W; y ++){
        for(int x = 0; x < H; x ++){
            //if pixel is in circle
            float dist = getDistance(ZSVECTOR3(x, y, 0), ZSVECTOR3(originX, originY, 0));
            if(dist <= range){
                //calculate modifier
                float toApply = originHeight - (dist * dist) / static_cast<float>(range);
                if(toApply > 0)
                    data[y * H + x].height += (toApply * multiplyer);
            }
        }
    }
}

void TerrainData::modifyTexture(int originX, int originY, int range, unsigned char texture){
    int modif = range / 2;
    //Iterate over all pixels
    for(int y = 0; y < W; y ++){
        for(int x = 0; x < H; x ++){
            //if pixel is in circle
            float dist = getDistance(ZSVECTOR3(x, y, 0), ZSVECTOR3(originX, originY, 0));
            if(dist <= range - modif){
                for(unsigned char texture_f = 0; texture_f < TERRAIN_TEXTURES_AMOUNT; texture_f ++){
                    if(texture_f != texture)
                        reduce(&data[y * H + x].texture_factors[texture_f], 25);
                }
                sum(&data[y * H + x].texture_factors[texture], 25);
            }
            for(int i = 0; i < modif / 2; i ++){
                if(dist > range - modif + (i) * 2 && dist <= range - modif + (i + 1) * 2){
                    int mod = 25 / ((i + 1) * 2);
                    for(unsigned char texture_f = 0; texture_f < TERRAIN_TEXTURES_AMOUNT; texture_f ++){
                        if(texture_f != texture)
                            reduce(&data[y * H + x].texture_factors[texture_f], mod);
                    }
                    sum(&data[y * H + x].texture_factors[texture], mod);
                }
            }
        }
    }
}

void TerrainData::plantGrass(int originX, int originY, int range, int grass){
    //Iterate over all pixels
    for(int y = 0; y < W; y ++){
        for(int x = 0; x < H; x ++){
            //if pixel is in circle
            float dist = getDistance(ZSVECTOR3(x, y, 0), ZSVECTOR3(originX, originY, 0));
            if(dist <= range){
                //calculate modifier
                data[y * H + x].grass = grass;
            }
        }
    }
}

void TerrainData::copyTo(TerrainData* dest){
    dest->alloc(W, H);
    memcpy(dest->data, data, static_cast<unsigned int>(W * H) * sizeof (HeightmapTexel));
    dest->generateGLMesh();
}
