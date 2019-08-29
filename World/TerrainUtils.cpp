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
        for(int t = 1; t < TERRAIN_TEXTURES_AMOUNT; t++)
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

void TerrainData::Draw(bool picking){
    //if opengl data not generated, exit function
    if(!created) return;
    //small optimization in terrain painting
    if(!picking){
        glActiveTexture(GL_TEXTURE24);
        glBindTexture(GL_TEXTURE_2D, texture_mask1);
        glActiveTexture(GL_TEXTURE25);
        glBindTexture(GL_TEXTURE_2D, texture_mask2);
        glActiveTexture(GL_TEXTURE26);
        glBindTexture(GL_TEXTURE_2D, texture_mask3);
    }
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glDrawElements(GL_TRIANGLES, (W - 1) * (H - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
}

void TerrainData::generateGLMesh(){
    if(W < 1 || H < 1)
        return;
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

           _texture2[(x * H + y) * 4] = data[x * H + y].texture_factors[8];
           _texture2[(x * H + y) * 4 + 1] = data[x * H + y].texture_factors[9];
           _texture2[(x * H + y) * 4 + 2] = data[x * H + y].texture_factors[10];
           _texture2[(x * H + y) * 4 + 3] = data[x * H + y].texture_factors[11];
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
        //Get two sides of triangle
        ZSVECTOR3 v12 = v1->pos - v2->pos;
        ZSVECTOR3 v13 = v1->pos - v3->pos;
        //Calculate normal
        v1->normal = vCross(v12, v13);
        //Normalize vector
        vNormalize(&v1->normal);
    }
    //generate tangent, bitangent
    processTangentSpace(vertices, indices, (W - 1) * (H - 1) * 2 * 3, W * H);

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

    created = true;
}

TerrainData::TerrainData(){
    W = 0; H = 0;
    created = false;
}

TerrainData::~TerrainData(){
    if(created)
        delete[] data;
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
    }

    world_stream.close();
}

bool TerrainData::loadFromFile(const char* file_path){
    std::ifstream world_stream;
    world_stream.open(file_path, std::ifstream::binary);

    if(world_stream.fail()){ //Probably, no file
        std::cout << "Terrain : Probably, missing terrain file" << file_path << std::endl;
        return false;
    }

    //read dimensions
    world_stream.read(reinterpret_cast<char*>(&this->W), sizeof(int));
    world_stream.read(reinterpret_cast<char*>(&this->H), sizeof(int));

    if(W < 1 || H < 1){
        std::cout << "Terrain : Can't load terrain dimensions from file " << file_path << ", it's probably corrupted!" << std::endl;
        return false;
    }

    //allocate memory
    alloc(W, H);

    for(int i = 0; i < W * H; i ++){
        world_stream.read(reinterpret_cast<char*>(&data[i].height), sizeof(float));
        for(int tex_factor = 0; tex_factor < TERRAIN_TEXTURES_AMOUNT; tex_factor ++)
            world_stream.read(reinterpret_cast<char*>(&data[i].texture_factors[tex_factor]), sizeof(unsigned char));
    }

    world_stream.close();
    return true;
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

void TerrainData::processTangentSpace(HeightmapVertex* vert_array, unsigned int* indices_array, int indices_num, int vertex_num){
    for(int ind_i = 0; ind_i < indices_num ; ind_i += 3){
        HeightmapVertex v1 = vert_array[indices_array[ind_i]];
        HeightmapVertex v2 = vert_array[indices_array[ind_i + 1]];
        HeightmapVertex v3 = vert_array[indices_array[ind_i + 2]];

        ZSVECTOR3 edge1 = v2.pos - v1.pos;
        ZSVECTOR3 edge2 = v3.pos - v1.pos;
        ZSVECTOR2 deltaUV1 = v2.uv - v1.uv;
        ZSVECTOR2 deltaUV2 = v3.uv - v1.uv;

        float f = 1.0f / (deltaUV1.X * deltaUV2.Y - deltaUV2.X * deltaUV1.Y);

        ZSVECTOR3 tangent, bitangent;
        tangent.X = f * (deltaUV2.Y * edge1.X - deltaUV1.Y * edge2.X);
        tangent.Y = f * (deltaUV2.Y * edge1.Y - deltaUV1.Y * edge2.Y);
        tangent.Z = f * (deltaUV2.Y * edge1.Z - deltaUV1.Y * edge2.Z);
        vNormalize(&tangent);

        bitangent.X = f * (-deltaUV2.X * edge1.X + deltaUV1.X * edge2.X);
        bitangent.Y = f * (-deltaUV2.X * edge1.Y + deltaUV1.X * edge2.Y);
        bitangent.Z = f * (-deltaUV2.X * edge1.Z + deltaUV1.X * edge2.Z);
        vNormalize(&bitangent);
        for(int i = 0; i < 3; i ++){
            vert_array[indices_array[ind_i + i]].tangent = tangent;
            vert_array[indices_array[ind_i + i]].bitangent = bitangent;
        }
    }
}

void TerrainData::copyTo(TerrainData* dest){
    dest->alloc(W, H);
    memcpy(dest->data, data, static_cast<unsigned int>(W * H) * sizeof (HeightmapTexel));
    dest->generateGLMesh();
}
