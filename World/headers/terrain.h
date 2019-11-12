#ifndef TERRAIN_H
#define TERRAIN_H

#define TERRAIN_TEXTURES_AMOUNT 12

#include <QString>
#include <render/zs-texture.h>
#include <render/zs-math.h>

#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

enum TERRAIN_MODIFY_TYPE{
    TMT_HEIGHT,
    TMT_TEXTURE,
    TMT_GRASS
};

typedef struct HeightmapGrass{
    QString diffuse_relpath;
    Engine::Texture* diffuse;

    ZSVECTOR3 scale;

    HeightmapGrass(){
        diffuse_relpath = "@none";
        diffuse = nullptr;

        scale = ZSVECTOR3(1.f, 1.f, 1.f);
    }

}HeightmapGrass;

typedef struct HeightmapTexel{
    float height;
    unsigned char texture_factors[TERRAIN_TEXTURES_AMOUNT];

    HeightmapTexel(){
    }
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

    unsigned char* _texture;
    unsigned char* _texture1;
    unsigned char* _texture2;

    HeightmapVertex* vertices;
    unsigned int* indices;

public:
    int W, H;
    bool hasHeightmapChanged;
    bool hasPaintingChanged;

    btBvhTriangleMeshShape* shape;
    bool hasPhysicShapeChanged;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    HeightmapTexel* data;

    void alloc(int W, int H);
    void flatTerrain(int height);

    void initGL();
    void initPhysics();
    void destroyGL();
    void Draw(bool picking = false);
    void generateGLMesh();
    void processTangentSpace(HeightmapVertex* vert_array, unsigned int* indices_array, int indices_num, int vertex_num);
    void saveToFile(const char* file_path);
    bool loadFromFile(const char* file_path);

    void modifyHeight(int originX, int originY, float originHeight, int range, int multiplyer);
    void modifyTexture(int originX, int originY, int range, unsigned char texture);
    void plantGrass(int originX, int originY, int range, int grass);

    void sum(unsigned char* ptr, int val);
    void reduce(unsigned char* ptr, int val);

    void copyTo(TerrainData* dest);

    void updateTextureBuffers();
    void updateTextureBuffersGL();

    void updateGeometryBuffers();
    void updateGeometryBuffersGL();

    TerrainData();
    ~TerrainData();
};

void startTerrainThread();


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

void queryTerrainModifyRequest(HeightmapModifyRequest* req);

#endif // TERRAIN_H
