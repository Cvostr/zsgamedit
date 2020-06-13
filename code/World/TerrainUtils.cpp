#include <world/go_properties.h>
#include "headers/terrain.h"
#include <GL/glew.h>
#include "../World/headers/Misc.h"
#include <fstream>
#include <vector>
#include <thread>

#define MAX_REQUESTS 150

static bool terrain_thread_working = true;
static HeightmapModifyRequest* terrain_mdf_requests[MAX_REQUESTS];
unsigned int requests_num = 0;

void terrain_loop(){
    while(terrain_thread_working){
        if(requests_num > 0){
            HeightmapModifyRequest* req = terrain_mdf_requests[MAX_REQUESTS - 1 - (--requests_num)];

            switch(req->modify_type){
                case TMT_HEIGHT:{
                    req->terrain->modifyHeight(req->originX, req->originY, req->originHeight, req->range, req->multiplyer);
                    req->terrain->updateGeometryBuffers(false);
                    req->terrain->hasHeightmapChanged = true;
                    req->terrain->hasPhysicShapeChanged = true;
                    req->terrain->hasGrassChanged = true;
                    req->terrain->updateGrassBuffers();
                    break;
                }
                case TMT_TEXTURE:{
                    req->terrain->modifyTexture(req->originX, req->originY, req->range, req->texture);
                    req->terrain->updateTextureBuffers(false);
                    req->terrain->hasPaintingChanged = true;
                    break;

                }
                case TMT_GRASS:{
                    req->terrain->plantGrass(req->originX, req->originY, req->range, req->grass);
                    req->terrain->hasGrassChanged = true;
                    req->terrain->updateGrassBuffers();
                    break;
                }
            }
        }
    }
}

void queryTerrainModifyRequest(HeightmapModifyRequest* req){
    terrain_mdf_requests[MAX_REQUESTS - 1 - (requests_num ++)] = req;
}

void startTerrainThread(){
    terrain_thread_working = true;
    std::thread loader_loop(terrain_loop);
    loader_loop.detach();
}

void stopTerrainThread(){
    terrain_thread_working = false;
    requests_num = 0;
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


void TerrainData::saveToFile(const char* file_path){
    std::ofstream world_stream;
    world_stream.open(file_path, std::ofstream::binary);
    //write dimensions
    world_stream.write(reinterpret_cast<char*>(&this->W), sizeof(int));
    world_stream.write(reinterpret_cast<char*>(&this->H), sizeof(int));
    //Write vertex to file
    for(int i = 0; i < W * H; i ++){
        //Write vertex height
        world_stream.write(reinterpret_cast<char*>(&data[i].height), sizeof(float));
        for(int tex_factor = 0; tex_factor < TERRAIN_TEXTURES_AMOUNT; tex_factor ++)
            world_stream.write(reinterpret_cast<char*>(&data[i].texture_factors[tex_factor]), sizeof(unsigned char));
        world_stream.write(reinterpret_cast<char*>(&data[i].grass), sizeof(int));
    }
    //Close stream
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
                if (toApply > 0) {
                    data[y * H + x].height += (toApply * multiplyer);
                    data[y * H + x].modified = true;
                }
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
            //if vertex is inside circle
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