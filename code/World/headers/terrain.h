#ifndef TERRAIN_ED_H
#define TERRAIN_ED_H

#include <world/Terrain.hpp>
#include <threading/Thread.hpp>
#include <threading/Mutex.hpp>

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

#define MAX_REQUESTS 550

class TerrainEditorThread : public Engine::Thread {
private:
    Engine::Mutex* mMutex;

    bool terrain_thread_working = true;
    HeightmapModifyRequest* terrain_mdf_requests[MAX_REQUESTS];
    int requests_num;
public:

    void THRFunc();
    void queryTerrainModifyRequest(HeightmapModifyRequest* req);

    TerrainEditorThread() :
        mMutex(new Engine::Mutex),
        requests_num(0)
    {
        
    }
};

void startTerrainThread();
void stopTerrainThread();
void queryTerrainModifyRequest(HeightmapModifyRequest* req);

#endif // TERRAIN_H
