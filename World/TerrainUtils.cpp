#include "../World/headers/obj_properties.h"
#include <GL/glew.h>
#include "../World/headers/Misc.h"
#include <fstream>

void TerrainData::alloc(int W, int H){
    if(created)
        delete[] data;
    this->W = W;
    this->H = H;
    this->data = new HeightmapTexel[W * H];
    flatTerrain(0);
}
void TerrainData::flatTerrain(int height){
    for(int i = 0; i < W * H; i ++){
        data[i].height = height;
        this->data[i].texture_factors[0] = 255;
        for(int t = 1; t < TEXTURES_AMOUNT; t++)
            this->data[i].texture_factors[t] = 0;
    }
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

    glGenTextures(1, &this->texture_mask1);
    glBindTexture(GL_TEXTURE_2D, this->texture_mask1);
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &this->texture_mask2);
    glBindTexture(GL_TEXTURE_2D, this->texture_mask2);
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &this->texture_mask3);
    glBindTexture(GL_TEXTURE_2D, this->texture_mask3);
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

void TerrainData::Draw(){
    //if opengl data not generated, exit function
    if(!created) return;

    glActiveTexture(GL_TEXTURE16);
    glBindTexture(GL_TEXTURE_2D, texture_mask1);
    glActiveTexture(GL_TEXTURE17);
    glBindTexture(GL_TEXTURE_2D, texture_mask2);
    glActiveTexture(GL_TEXTURE18);
    glBindTexture(GL_TEXTURE_2D, texture_mask3);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glDrawElements(GL_TRIANGLES, (W - 1) * (H - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
}

void TerrainData::generateGLMesh(){
    if(!created)
        initGL();

    //write temporary array to store texture ids
    unsigned char* _texture = new unsigned char[W * H * 4];
    unsigned char* _texture1 = new unsigned char[W * H * 4];
    unsigned char* _texture2 = new unsigned char[W * H * 4];
    for(int y = 0; y < H; y ++){
        for(int x = 0; x < W; x ++){
           _texture[(x * H + y) * 4] = data[x * H + y].texture_factors[0];
           _texture[(x * H + y) * 4 + 1] = data[x * H + y].texture_factors[1];
           _texture[(x * H + y) * 4 + 2] = data[x * H + y].texture_factors[2];
           _texture[(x * H + y) * 4 + 3] = data[x * H + y].texture_factors[3];

           _texture1[(x * H + y) * 4] = data[x * H + y].texture_factors[4];
           _texture1[(x * H + y) * 4 + 1] = data[x * H + y].texture_factors[5];
           _texture1[(x * H + y) * 4 + 2] = data[x * H + y].texture_factors[6];
           _texture1[(x * H + y) * 4 + 3] = data[x * H + y].texture_factors[7];

           _texture2[(x * H + y) * 4] = data[x * H + y].texture_factors[4];
           _texture2[(x * H + y) * 4 + 1] = data[x * H + y].texture_factors[5];
           _texture2[(x * H + y) * 4 + 2] = data[x * H + y].texture_factors[6];
           _texture2[(x * H + y) * 4 + 3] = data[x * H + y].texture_factors[7];
        }
    }

    //Create texture masks texture
    glBindTexture(GL_TEXTURE_2D, this->texture_mask1);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            static_cast<int>(W),
            static_cast<int>(H),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            _texture
     );
    glBindTexture(GL_TEXTURE_2D, this->texture_mask2);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            static_cast<int>(W),
            static_cast<int>(H),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            _texture1
     );
    glBindTexture(GL_TEXTURE_2D, this->texture_mask3);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            static_cast<int>(W),
            static_cast<int>(H),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            _texture2
     );

    delete [] _texture;
    delete [] _texture1;
    delete [] _texture2;


    HeightmapVertex* vertices = new HeightmapVertex[W * H];
    unsigned int* indices = new unsigned int[(W - 1) * (H - 1) * 2 * 3];

    for(int y = 0; y < H; y ++){
        for(int x = 0; x < W; x ++){
            vertices[x * H + y].pos = ZSVECTOR3(x, data[x * H + y].height, y);
            vertices[x * H + y].uv = ZSVECTOR2(static_cast<float>(x) / W, static_cast<float>(y) / H);
        }
    }
    unsigned int inds = 0;
    for(int y = 0; y < H - 1; y ++){
        for(int x = 0; x < W - 1; x ++){
            indices[inds] = static_cast<unsigned int>(x * H + y);
            indices[inds + 2] = static_cast<unsigned int>(x * H + H + y);
            indices[inds + 1] = static_cast<unsigned int>(x * H + H + y + 1);

            indices[inds + 3] = static_cast<unsigned int>(x * H + y);
            indices[inds + 5] = static_cast<unsigned int>(x * H + H + y + 1);
            indices[inds + 4] = static_cast<unsigned int>(x * H + y + 1);

            inds += 6;
        }
    }
    for(unsigned int i = 0; i < inds - 2; i ++){
        HeightmapVertex* v1 = &vertices[indices[i]];
        HeightmapVertex* v2 = &vertices[indices[i + 1]];
        HeightmapVertex* v3 = &vertices[indices[i + 2]];

        ZSVECTOR3 v12 = v1->pos - v2->pos;
        ZSVECTOR3 v13 = v1->pos - v3->pos;

        v1->normal = vCross(v12, v13);
    }

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

    delete[] vertices;
    delete[] indices;


    created = true;
}

TerrainData::TerrainData(){
    created = false;
}

void TerrainData::saveToFile(const char* file_path){
    std::ofstream world_stream;
    world_stream.open(file_path, std::ofstream::binary);
    //write dimensions
    world_stream.write(reinterpret_cast<char*>(&this->W), sizeof(int));
    world_stream.write(reinterpret_cast<char*>(&this->H), sizeof(int));

    for(int i = 0; i < W * H; i ++){
        world_stream.write(reinterpret_cast<char*>(&data[i].height), sizeof(float));
        for(int tex_factor = 0; tex_factor < TEXTURES_AMOUNT; tex_factor ++)
            world_stream.write(reinterpret_cast<char*>(&data[i].texture_factors[tex_factor]), sizeof(unsigned char));
    }

    world_stream.close();
}

void TerrainData::loadFromFile(const char* file_path){
    std::ifstream world_stream;
    world_stream.open(file_path, std::ifstream::binary);
    //read dimensions
    world_stream.read(reinterpret_cast<char*>(&this->W), sizeof(int));
    world_stream.read(reinterpret_cast<char*>(&this->H), sizeof(int));
    //allocate memory
    alloc(W, H);

    for(int i = 0; i < W * H; i ++){
        world_stream.read(reinterpret_cast<char*>(&data[i].height), sizeof(float));
        for(int tex_factor = 0; tex_factor < TEXTURES_AMOUNT; tex_factor ++)
            world_stream.read(reinterpret_cast<char*>(&data[i].texture_factors[tex_factor]), sizeof(unsigned char));
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
                for(unsigned char texture_f = 0; texture_f < TEXTURES_AMOUNT; texture_f ++){
                    if(texture_f != texture)
                        reduce(&data[y * H + x].texture_factors[texture_f], 25);
                }
                sum(&data[y * H + x].texture_factors[texture], 25);
            }
            for(int i = 0; i < modif / 2; i ++){
                if(dist > range - modif + (i) * 2 && dist <= range - modif + (i + 1) * 2){
                    int mod = 25 / ((i + 1) * 2);
                    for(unsigned char texture_f = 0; texture_f < TEXTURES_AMOUNT; texture_f ++){
                        if(texture_f != texture)
                            reduce(&data[y * H + x].texture_factors[texture_f], mod);
                    }
                    sum(&data[y * H + x].texture_factors[texture], mod);
                }
            }
        }
    }
}
