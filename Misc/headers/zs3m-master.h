#ifndef ZS3MMASTER_H
#define ZS3MMASTER_H
#include "../../Render/headers/zs-math.h"
#include "../../Render/headers/zs-mesh.h"
#include "AssimpMeshLoader.h"
#include <string>
#include <vector>
namespace ZS3M {


class SceneFileExport{
private:
    std::vector<ZSPIRE::Mesh*> meshes_toWrite;
    MeshNode* rootNode;
public:

    void pushMesh(ZSPIRE::Mesh* mesh);
    void setRootNode(MeshNode* node);
    void write(std::string output_file);
    void writeNode(std::ofstream *stream, MeshNode* node);
    void getNodesNum(unsigned int* nds_ptr, MeshNode* node);
};

}

#endif // ZS3MMASTER_H
