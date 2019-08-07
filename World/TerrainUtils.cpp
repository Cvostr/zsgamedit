#include "../World/headers/obj_properties.h"
#include <GL/glew.h>
#include "../World/headers/Misc.h"

void TerrainData::alloc(int W, int H){
    this->W = W;
    this->H = H;
    this->data = new HeightmapTexel[W * H];
    flatTerrain(0);

    generateGLMesh();
}
void TerrainData::flatTerrain(int height){
    for(int i = 0; i < W * H; i ++){
        data[i].height = height;
    }
}

void TerrainData::initGL(){
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);
}
void TerrainData::destroyGL(){
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->EBO);
}

void TerrainData::Draw(){
    //if opengl data not generated, exit function
    if(!created) return;
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glDrawElements(GL_TRIANGLES, (W - 1) * (H - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
}

void TerrainData::generateGLMesh(){
    if(created)
        destroyGL();

    initGL();

    HeightmapVertex* vertices = new HeightmapVertex[W * H];
    unsigned int* indices = new unsigned int[(W - 1) * (H - 1) * 2 * 3];

    for(int x = 0; x < W; x ++){
        for(int y = 0; y < H; y ++){
            vertices[x + y * H].pos = ZSVECTOR3(x, data[x + y * H].height, y);
        }
    }
    int inds = 0;
    for(int x = 0; x < W - 1; x ++){
        for(int y = 0; y < H - 1; y ++){
            indices[inds] = y * H + x;
            indices[inds + 1] = y * H + H + x;
            indices[inds + 2] = y * H + H + x + 1;

            indices[inds + 3] = y * H + x;
            indices[inds + 4] = y * H + H + x + 1;
            indices[inds + 5] = y * H + x + 1;


            inds += 6;
        }
    }

    glBindVertexArray(this->VAO); //Bind vertex array
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO); //Bind vertex buffer
    glBufferData(GL_ARRAY_BUFFER, static_cast<int>(W * H) * static_cast<int>(sizeof(HeightmapVertex)), vertices, GL_STATIC_DRAW); //send vertices to buffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO); //Bind index buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<unsigned int>((W - 1) * (H - 1) * 2 * 3) * sizeof(unsigned int), indices, GL_STATIC_DRAW); //Send indices to buffer

    //Vertex pos 3 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), nullptr);
    glEnableVertexAttribArray(0);
    /*//Vertex UV 2 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 3));
    glEnableVertexAttribArray(1);*/
    //Vertex Normals 3 floats
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 3));
    glEnableVertexAttribArray(2);
/*
    //Vertex Tangents 3 floats
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 8));
    glEnableVertexAttribArray(3);
    //Vertex Bitangents 3 floats
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ZSVERTEX), reinterpret_cast<void*>(sizeof(float) * 11));
    glEnableVertexAttribArray(4);*/

    created = true;
}

TerrainData::TerrainData(){
    created = false;
}
