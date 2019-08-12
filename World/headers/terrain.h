#ifndef TERRAIN_H
#define TERRAIN_H

#define TEXTURES_AMOUNT 8

#include "../../Render/headers/zs-math.h"

typedef struct HeightmapTexel{
    float height;
    unsigned char texture_factors[TEXTURES_AMOUNT];
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

    unsigned int texture_mask1;
    unsigned int texture_mask2;
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

    void modifyHeight(int originX, int originY, float originHeight, int range, int multiplyer);
    void modifyTexture(int originX, int originY, int range, unsigned char texture);

    void sum(unsigned char* ptr, int val);

    TerrainData();
};

#endif // TERRAIN_H
