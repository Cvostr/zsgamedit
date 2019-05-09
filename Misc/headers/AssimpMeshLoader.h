#ifndef zs_assimploader
#define zs_assimploader

#include "../../triggers.h"
#include "../../Render/headers/zs-mesh.h"
#include "zs_types.h"

#ifdef USE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

class MeshNode
{
public:
    std::string node_label;

    std::vector<MeshNode> children;
    std::vector<std::string> mesh_names;
    MeshNode() {}
};

namespace Engine {

void processNodeForTree(MeshNode* node, aiNode* node_assimp, const aiScene* scene);
void loadNodeTree(std::string file_path, MeshNode* node);
unsigned int getMeshesAmount(std::string file_path);
void loadMeshes(std::string file_path, ZSPIRE::Mesh* meshes_array);
void loadMesh(std::string file_path, ZSPIRE::Mesh* mesh_ptr, int index);

void processMesh(aiMesh* mesh, const aiScene* scene, ZSPIRE::Mesh* mesh_ptr);
void processNode(aiNode* node, const aiScene* scene);
}

#endif
