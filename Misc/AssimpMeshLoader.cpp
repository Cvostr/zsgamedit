#include "headers/AssimpMeshLoader.h"

#ifdef USE_ASSIMP //Optional
static Assimp::Importer importer;
static unsigned int loadflags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_FlipUVs;
#endif

#include <iostream>

unsigned int Engine::getMeshesAmount(std::string file_path){
    const aiScene* scene = importer.ReadFile(file_path, loadflags);

    return scene->mNumMeshes;
}


#ifdef USE_ASSIMP
void Engine::processMesh(aiMesh* mesh, const aiScene* scene, ZSPIRE::Mesh* mesh_ptr) {
    mesh_ptr->mesh_label = std::string(mesh->mName.C_Str());

    unsigned int vertices = mesh->mNumVertices;
    unsigned int faces = mesh->mNumFaces;

    ZSVERTEX* vertices_arr = new ZSVERTEX[vertices];
    unsigned int* indices = new unsigned int[faces * 3];

    for (unsigned int v = 0; v < vertices; v++) {
        aiVector3D vertex_pos = mesh->mVertices[v];
        aiVector3D vertex_normal = mesh->mNormals[v];
        aiVector3D vertex_tangent = mesh->mTangents[v];
        aiVector3D vertex_bitangent = mesh->mBitangents[v];

        float U = mesh->mTextureCoords[0][v].x;
        float V = mesh->mTextureCoords[0][v].y;

        vertices_arr[v] = ZSVERTEX(ZSVECTOR3(vertex_pos.x, vertex_pos.y, vertex_pos.z), ZSVECTOR2(U, V),
            ZSVECTOR3(vertex_normal.x, vertex_normal.y, vertex_normal.z), ZSVECTOR3(vertex_tangent.x, vertex_tangent.y, vertex_tangent.z),
            ZSVECTOR3(vertex_bitangent.x, vertex_bitangent.y, vertex_bitangent.z)
        );

        vNormalize(&vertices_arr[v].normal);

    }

    for (unsigned int i = 0; i < faces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices[i * 3 + j] = face.mIndices[j];
    }

    mesh_ptr->Init();
    mesh_ptr->setMeshData(vertices_arr, indices, vertices, faces * 3);
}

void Engine::processNode(aiNode* node, const aiScene* scene) {
    unsigned int child_nodes_amount = node->mNumChildren;
    unsigned int meshes_amount = node->mNumMeshes;

    //Iterate child nodes
    for (unsigned int i = 0; i < child_nodes_amount; i++) {
        processNode(node->mChildren[i], scene);
    }
    //Iterate meshes
    for (unsigned int i = 0; i < meshes_amount; i++) {
       // processMesh(scene->mMeshes[node->mMeshes[i]], scene);
    }

}
#endif

void Engine::loadMeshes(std::string file_path, ZSPIRE::Mesh* meshes_array){
    const aiScene* scene = importer.ReadFile(file_path, loadflags);


    for(unsigned int mesh_i = 0; mesh_i < scene->mNumMeshes; mesh_i ++){
        processMesh(scene->mMeshes[mesh_i], scene, &meshes_array[mesh_i]);
    }
}

void Engine::loadMesh(std::string file_path, ZSPIRE::Mesh* mesh_ptr, int index){
    const aiScene* scene = importer.ReadFile(file_path, loadflags);
     std::cout << "Loading mesh " << scene->mMeshes[index]->mName.C_Str() << " from file " << file_path << std::endl;

    processMesh(scene->mMeshes[index], scene, mesh_ptr);
}


void Engine::loadNodeTree(std::string file_path, MeshNode* node){
    const aiScene* scene = importer.ReadFile(file_path, loadflags);

    MeshNode* root_node = new MeshNode;
    processNodeForTree(root_node, scene->mRootNode, scene);

    *node = *root_node;
}


void Engine::processNodeForTree(MeshNode* node, aiNode* node_assimp, const aiScene* scene){
    node->node_label = node_assimp->mName.C_Str(); //assigning node name

    unsigned int meshes_num = node_assimp->mNumMeshes;
    for(unsigned int ch_i = 0; ch_i < meshes_num; ch_i ++){
        aiMesh* mesh = scene->mMeshes[node_assimp->mMeshes[ch_i]];
        node->mesh_names.push_back(mesh->mName.C_Str());
    }

    unsigned int nodes_num = node_assimp->mNumChildren;
    for(unsigned int ch_i = 0; ch_i < nodes_num; ch_i ++){
        aiNode* child = node_assimp->mChildren[ch_i];
        MeshNode mNode;
        mNode.node_label = child->mName.C_Str();

        processNodeForTree(&mNode, child, scene);
        node->children.push_back(mNode);
    }
}
