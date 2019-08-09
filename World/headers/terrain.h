#ifndef TERRAIN_H
#define TERRAIN_H

#include "../../Render/headers/zs-math.h"

typedef struct HeightmapTexel{
    float height;
}HeightmapTexel;

typedef struct HeightmapVertex{
    ZSVECTOR3 pos;
    ZSVECTOR2 uv;
    ZSVECTOR3 normal;
}HeightmapVertex;

class TerrainData{
private:
    bool created;
    int W, H;
public:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    HeightmapTexel* data;

    void alloc(int W, int H);
    void flatTerrain(int height);

    void initGL();
    void destroyGL();
    void Draw();
    void generateGLMesh();
    void saveToFile(const char* file_path);
    void loadFromFile(const char* file_path);

    TerrainData();
};

#endif // TERRAIN_H
