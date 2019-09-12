#ifndef zs_assimploader
#define zs_assimploader

#include "../../triggers.h"
#include "../../Render/headers/zs-mesh.h"
#include "../../Render/headers/zs-animation.h"
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
    aiMatrix4x4 node_transform;

    bool hasBone;

    std::vector<MeshNode> children;
    std::vector<std::string> mesh_names;

    MeshNode() {
        hasBone = false;
    }
};

namespace Engine {

void cmat(aiMatrix4x4 matin, ZSMATRIX4x4* matout);

void processNodeForTree(MeshNode* node, aiNode* node_assimp, const aiScene* scene);
void loadNodeTree(std::string file_path, MeshNode* node);

bool getSizes(std::string file_path, unsigned int* meshes, unsigned int* anims, unsigned int* textures = nullptr, unsigned int* materials = nullptr);

void loadMesh(std::string file_path, ZSPIRE::Mesh* mesh_ptr, int index);

void loadAnimation(std::string file_path, ZSPIRE::Animation* anim, int index);

void processMesh(aiMesh* mesh, const aiScene* scene, ZSPIRE::Mesh* mesh_ptr);
void processNode(aiNode* node, const aiScene* scene);

bool isBoneAvailable(std::string bone_label, const aiScene* scene);
}

#endif
