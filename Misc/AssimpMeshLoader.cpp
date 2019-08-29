#include "headers/AssimpMeshLoader.h"

#ifdef USE_ASSIMP //Optional
static Assimp::Importer importer;
static unsigned int loadflags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_FlipUVs;
#endif

#include <iostream>

void cmat(aiMatrix4x4 matin, ZSMATRIX4x4* matout){

        matout->m[0][0] = matin.a1;
        matout->m[0][1] = matin.b1;
        matout->m[0][2] = matin.c1;
        matout->m[0][3] = matin.d1;

        matout->m[1][0] = matin.a2;
        matout->m[1][1] = matin.b2;
        matout->m[1][2] = matin.c2;
        matout->m[1][3] = matin.d2;

        matout->m[2][0] = matin.a3;
        matout->m[2][1] = matin.b3;
        matout->m[2][2] = matin.c3;
        matout->m[2][3] = matin.d3;

        matout->m[3][0] = matin.a4;
        matout->m[3][1] = matin.b4;
        matout->m[3][2] = matin.c4;
        matout->m[3][3] = matin.d4;
    //*matout = transpose(*matout);
}

unsigned int Engine::getMeshesAmount(std::string file_path){
    const aiScene* scene = importer.ReadFile(file_path, loadflags);

    return scene->mNumMeshes;
}


#ifdef USE_ASSIMP
void Engine::processMesh(aiMesh* mesh, const aiScene* scene, ZSPIRE::Mesh* mesh_ptr) {
    mesh_ptr->mesh_label = std::string(mesh->mName.C_Str());

    unsigned int vertices = mesh->mNumVertices;
    unsigned int faces = mesh->mNumFaces;
    unsigned int bones = mesh->mNumBones;

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

        vertices_arr[v].bones_num = 0;
        for(unsigned int vw_i = 0; vw_i < MAX_BONE_PER_VERTEX; vw_i ++){
            vertices_arr[v].ids[vw_i] = 0;
            vertices_arr[v].weights[vw_i] = 1.f;
        }

        vNormalize(&vertices_arr[v].normal);

    }

    for(unsigned int bone_i = 0; bone_i < bones; bone_i ++){
        aiBone* bone_ptr = mesh->mBones[bone_i];
        ZSPIRE::Bone bone(bone_ptr->mName.C_Str(), bone_ptr->mNumWeights);
        cmat(bone_ptr->mOffsetMatrix, &bone.offset);

        aiVector3t<float> node_scale;
        aiVector3t<float> node_translation;
        aiVector3t<float> node_rotation;
        //Decompose them!
        bone_ptr->mOffsetMatrix.Decompose(node_scale, node_rotation, node_translation);
        //Store them in engine node
        bone.pos = ZSVECTOR3(node_translation.x, node_translation.y, node_translation.z);
        bone.scale = ZSVECTOR3(node_scale.x, node_scale.y, node_scale.z);
        //This f%cking rotation is in radians, blyat
        bone.rot = ZSVECTOR3(node_rotation.x * 180.f / ZS_PI,
                                   node_rotation.y * 180.f / ZS_PI,
                                   node_rotation.z * 180.f / ZS_PI);

        //Iterate over all weights to set them to vertices
        for(unsigned int vw_i = 0; vw_i < bone.vertices_affected; vw_i ++){
            aiVertexWeight* vw = &bone_ptr->mWeights[vw_i];
            ZSVERTEX* vertex = &vertices_arr[vw->mVertexId];
            //Add bone ID
            vertex->ids[vertex->bones_num] = bone_i;
            //Set bone weight
            vertex->weights[vertex->bones_num] = vw->mWeight;
            //Increase bones amount
            vertex->bones_num += 1;

            if(vertex->bones_num > MAX_BONE_PER_VERTEX)
                //Its better way to crash here
                assert(0);
        }

        //Push bone back
        mesh_ptr->bones.push_back(bone);
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
    processNodeForTree(root_node, scene->mRootNode, scene, aiVector3t<float>(1,1,1), aiVector3t<float>(0,0,0), aiVector3t<float>(0,0,0));

    *node = *root_node;
}


void Engine::processNodeForTree(MeshNode* node, aiNode* node_assimp, const aiScene* scene, aiVector3t<float> _node_scale,
                                                                                            aiVector3t<float> _node_translation,
                                                                                            aiVector3t<float> _node_rotation){
    node->node_label = node_assimp->mName.C_Str(); //assigning node name

    //Declare vectors to store decomposed transform components
    aiVector3t<float> node_scale;
    aiVector3t<float> node_translation;
    aiVector3t<float> node_rotation;
    //Decompose them!
    aiMatrix4x4 result_mat = node_assimp->mTransformation;

    result_mat.Decompose(node_scale, node_rotation, node_translation);

    node_scale.x *= _node_scale.x;
    node_scale.y *= _node_scale.y;
    node_scale.z *= _node_scale.z;

    node_rotation += _node_rotation;
    node_translation += _node_translation;

    //Store them in engine node
    node->translation = ZSVECTOR3(node_translation.x, node_translation.y, node_translation.z);
    node->scale = ZSVECTOR3(node_scale.x, node_scale.y, node_scale.z);
    //This f%cking rotation is in radians, blyat
    node->rotation = ZSVECTOR3(node_rotation.x * 180.f / ZS_PI,
                                   node_rotation.y * 180.f / ZS_PI,
                                   node_rotation.z * 180.f / ZS_PI);


    /*node->translation = ZSVECTOR3(0, 0, 0);
    node->scale = ZSVECTOR3(1, 1, 1);
    //This f%cking rotation is in radians, blyat
    node->rotation = ZSVECTOR3(0, 0, 0);*/

    //iterate over all meshes in this node
    unsigned int meshes_num = node_assimp->mNumMeshes;
    for(unsigned int ch_i = 0; ch_i < meshes_num; ch_i ++){
        aiMesh* mesh = scene->mMeshes[node_assimp->mMeshes[ch_i]];
        node->mesh_names.push_back(mesh->mName.C_Str());
    }
    //iterate over all children nodes and write their names
    unsigned int nodes_num = node_assimp->mNumChildren;
    for(unsigned int ch_i = 0; ch_i < nodes_num; ch_i ++){
        aiNode* child = node_assimp->mChildren[ch_i];
        MeshNode mNode;
        mNode.node_label = child->mName.C_Str();

        processNodeForTree(&mNode, child, scene, node_scale, node_translation, node_rotation);
        node->children.push_back(mNode);
    }
}
