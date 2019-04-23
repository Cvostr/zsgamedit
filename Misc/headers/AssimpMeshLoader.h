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

namespace Engine {


unsigned int getMeshesAmount(std::string file_path);
void loadMeshes(std::string file_path, ZSPIRE::Mesh* meshes_array);
void loadMesh(std::string file_path, ZSPIRE::Mesh* mesh_ptr, int index);

void processMesh(aiMesh* mesh, const aiScene* scene, ZSPIRE::Mesh* mesh_ptr);
void processNode(aiNode* node, const aiScene* scene);
}

#endif
