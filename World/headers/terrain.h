#ifndef TERRAIN_H
#define TERRAIN_H

#define TERRAIN_TEXTURES_AMOUNT 12

#include <QString>
#include <render/zs-texture.h>
#include <render/zs-math.h>

typedef struct HeightmapGrass{
    QString diffuse_relpath;
    Engine::Texture* diffuse;
}HeightmapGrass;

typedef struct HeightmapTexel{
    float height;
    unsigned char texture_factors[TERRAIN_TEXTURES_AMOUNT];
}HeightmapTexel;

typedef struct HeightmapVertex{
    ZSVECTOR3 pos;
    ZSVECTOR2 uv;
    ZSVECTOR3 normal;
    ZSVECTOR3 tangent;
    ZSVECTOR3 bitangent;
}HeightmapVertex;

typedef struct HeightmapTexturePair{
    QString diffuse_relpath;
    QString normal_relpath;

    Engine::Texture* diffuse;
    Engine::Texture* normal;

    HeightmapTexturePair(){
        diffuse_relpath = normal_relpath = "@none";
        diffuse = normal = nullptr;
    }
}HeightmapTexturePair;


class TerrainData{
private:
    bool created;
    //OpenGL texture IDs of texture masks
    unsigned int texture_mask1;
    unsigned int texture_mask2;
    unsigned int texture_mask3;
public:
    int W, H;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    HeightmapTexel* data;

    void alloc(int W, int H);
    void flatTerrain(int height);

    void initGL();
    void destroyGL();
    void Draw(bool picking = false);
    void generateGLMesh();
    void processTangentSpace(HeightmapVertex* vert_array, unsigned int* indices_array, int indices_num, int vertex_num);
    void saveToFile(const char* file_path);
    bool loadFromFile(const char* file_path);

    void modifyHeight(int originX, int originY, float originHeight, int range, int multiplyer);
    void modifyTexture(int originX, int originY, int range, unsigned char texture);

    void sum(unsigned char* ptr, int val);
    void reduce(unsigned char* ptr, int val);

    void copyTo(TerrainData* dest);

    TerrainData();
    ~TerrainData();
};

#endif // TERRAIN_H
