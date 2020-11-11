#ifndef TERRAIN_ED_H
#define TERRAIN_ED_H

#include <world/Terrain.hpp>

enum TERRAIN_MODIFY_TYPE{
    TMT_HEIGHT,
    TMT_TEXTURE,
    TMT_GRASS
};


typedef struct HeightmapModifyRequest{
    TerrainData* terrain;
    TERRAIN_MODIFY_TYPE modify_type;

    int originX;
    int originY;
    int range;

    float originHeight;
    int multiplyer;

    unsigned char texture;
    int grass;

}HeightmapModifyRequest;

void startTerrainThread();
void stopTerrainThread();
void queryTerrainModifyRequest(HeightmapModifyRequest* req);

#endif // TERRAIN_H
