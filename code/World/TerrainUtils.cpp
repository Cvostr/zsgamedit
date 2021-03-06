#include "headers/terrain.h"
#include <GL/glew.h>
#include <fstream>
#include <vector>
#include <thread>
#include <threading/Mutex.hpp>

TerrainEditorThread _TEThread;

void TerrainEditorThread::THRFunc() {
    while (mShouldRun) {
        //check, if there are some operation pending
        if (requests_num > 0) {
            HeightmapModifyRequest* req = terrain_mdf_requests[0];
            //switch over all modify types
            switch (req->modify_type) {
            case TMT_HEIGHT: {
                //if height of terrain changed
                //Change height
                req->terrain->modifyHeight(req->originX, req->originY, req->originHeight, req->range, req->multiplyer);
                //Recalculate terrain mesh
                req->terrain->updateGeometryBuffers(false);
                req->terrain->hasHeightmapChanged = true;
                req->terrain->hasPhysicShapeChanged = true;
                req->terrain->hasGrassChanged = true;
                //Recalculate grass transforms
                req->terrain->updateGrassBuffers();
                break;
            }
            case TMT_TEXTURE: {
                req->terrain->modifyTexture(req->originX, req->originY, req->range, req->texture);
                req->terrain->updateTextureBuffers(false);
                req->terrain->hasPaintingChanged = true;
                break;

            }
            case TMT_GRASS: {
                req->terrain->plantGrass(req->originX, req->originY, req->range, req->grass);
                req->terrain->hasGrassChanged = true;
                req->terrain->updateGrassBuffers();
                break;
            }
            }
            //Lock mutex
            mMutex->Lock();
            for (unsigned int a_i = 1; a_i < requests_num; a_i++) {
                terrain_mdf_requests[a_i - 1] = terrain_mdf_requests[a_i];
            }
            requests_num--;
            //release mutex
            mMutex->Release();
        }
    }
}

void TerrainEditorThread::queryTerrainModifyRequest(HeightmapModifyRequest* req) {
    mMutex->Lock();
    terrain_mdf_requests[(requests_num++)] = req;
    mMutex->Release();
}

void queryTerrainModifyRequest(HeightmapModifyRequest* req){
    _TEThread.queryTerrainModifyRequest(req);
}

void startTerrainThread(){
    _TEThread.Run();
}

void stopTerrainThread(){
    _TEThread.Stop();
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
        //Write Texture Factors
        for(int tex_factor = 0; tex_factor < TERRAIN_TEXTURES_AMOUNT; tex_factor ++)
            world_stream.write(reinterpret_cast<char*>(&data[i].texture_factors[tex_factor]), sizeof(unsigned char));
        //Write Grass Data
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
            float dist = getDistance(Vec3(x, y, 0), Vec3(originX, originY, 0));
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
            float dist = getDistance(Vec3(x, y, 0), Vec3(originX, originY, 0));
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
            float dist = getDistance(Vec3(x, y, 0), Vec3(originX, originY, 0));
            if(dist <= range){
                //calculate modifier
                data[y * H + x].grass = grass;
            }
        }
    }
}