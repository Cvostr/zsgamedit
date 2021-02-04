#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../../triggers.h"
#include <render/Mesh.hpp>
#include <render/Animation.hpp>
#include <misc/zs3m-master.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Engine {

void cmat(aiMatrix4x4 matin, Mat4* matout);

void processNodeForTree(ZS3M::SceneNode* node, aiNode* node_assimp, const aiScene* scene);
void loadNodeTree(std::string file_path, ZS3M::SceneNode* node);

bool getSizes(std::string file_path, unsigned int* meshes, unsigned int* anims, unsigned int* textures = nullptr, unsigned int* materials = nullptr);

void loadMesh(std::string file_path, Engine::Mesh* mesh_ptr, int index);

void loadAnimation(std::string file_path, Engine::Animation* anim, int index);

void processMesh(aiMesh* mesh, Engine::Mesh* mesh_ptr);

bool isBoneAvailable(std::string bone_label, const aiScene* scene);
}