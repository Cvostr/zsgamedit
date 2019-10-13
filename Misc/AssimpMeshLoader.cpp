#include "headers/AssimpMeshLoader.h"
#include <cassert>

static Assimp::Importer importer;
static unsigned int loadflags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;

static unsigned int loadflagsAnim = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices;

#include <iostream>

void Engine::cmat(aiMatrix4x4 matin, ZSMATRIX4x4* matout){

    matout->m[0][0] = matin.a1; matout->m[0][1] = matin.b1;  matout->m[0][2] = matin.c1; matout->m[0][3] = matin.d1;
    matout->m[1][0] = matin.a2; matout->m[1][1] = matin.b2;  matout->m[1][2] = matin.c2; matout->m[1][3] = matin.d2;
    matout->m[2][0] = matin.a3; matout->m[2][1] = matin.b3;  matout->m[2][2] = matin.c3; matout->m[2][3] = matin.d3;
    matout->m[3][0] = matin.a4; matout->m[3][1] = matin.b4;  matout->m[3][2] = matin.c4; matout->m[3][3] = matin.d4;

    *matout = transpose(*matout);
}

bool Engine::getSizes(std::string file_path, unsigned int* meshes, unsigned int* anims, unsigned int* textures, unsigned int* materials){
    const aiScene* scene = importer.ReadFile(file_path, 0);

    if(scene == nullptr) return false;

    *meshes = scene->mNumMeshes;
    *anims = scene->mNumAnimations;

    if(textures != nullptr)
        *textures = scene->mNumTextures;

    if(materials != nullptr)
        *materials = scene->mNumMaterials;

    importer.FreeScene();
    return true;
}


void Engine::processMesh(aiMesh* mesh, ZSPIRE::Mesh* mesh_ptr) {
    mesh_ptr->mesh_label = std::string(mesh->mName.C_Str());

    unsigned int vertices = mesh->mNumVertices;
    unsigned int faces = mesh->mNumFaces;
    unsigned int bones = mesh->mNumBones;
    //Allocate memory for vertices and indices
    mesh_ptr->vertices_arr = new ZSVERTEX[vertices];
    mesh_ptr->indices_arr = new unsigned int[faces * 3];
    //Iterate over all vertices, read data and write to array
    for (unsigned int v = 0; v < vertices; v++) {
        aiVector3D vertex_pos = mesh->mVertices[v];
        aiVector3D vertex_normal = mesh->mNormals[v];
        aiVector3D vertex_tangent = mesh->mTangents[v];
        aiVector3D vertex_bitangent = mesh->mBitangents[v];

        float U = mesh->mTextureCoords[0][v].x;
        float V = mesh->mTextureCoords[0][v].y;
        //Set data to vertex
        mesh_ptr->vertices_arr[v] = ZSVERTEX(ZSVECTOR3(vertex_pos.x, vertex_pos.y, vertex_pos.z), ZSVECTOR2(U, V),
            ZSVECTOR3(vertex_normal.x, vertex_normal.y, vertex_normal.z), ZSVECTOR3(vertex_tangent.x, vertex_tangent.y, vertex_tangent.z),
            ZSVECTOR3(vertex_bitangent.x, vertex_bitangent.y, vertex_bitangent.z)
        );
        //Read bones
        mesh_ptr->vertices_arr[v].bones_num = 0;
        for(unsigned int vw_i = 0; vw_i < MAX_BONE_PER_VERTEX; vw_i ++){
            mesh_ptr->vertices_arr[v].ids[vw_i] = 0;
            mesh_ptr->vertices_arr[v].weights[vw_i] = 0.f;
        }

        vNormalize(&mesh_ptr->vertices_arr[v].normal);

    }

    for(unsigned int bone_i = 0; bone_i < bones; bone_i ++){
        aiBone* bone_ptr = mesh->mBones[bone_i];
        ZSPIRE::Bone bone(bone_ptr->mName.C_Str(), bone_ptr->mNumWeights);
        //Convert assimp bone offset matrix and store it in bone class
        cmat(bone_ptr->mOffsetMatrix, &bone.offset);

        //Iterate over all weights to set them to vertices
        for(unsigned int vw_i = 0; vw_i < bone.vertices_affected; vw_i ++){
            aiVertexWeight* vw = &bone_ptr->mWeights[vw_i];
            ZSVERTEX* vertex = &mesh_ptr->vertices_arr[vw->mVertexId];

            if(vertex->bones_num + 1 > MAX_BONE_PER_VERTEX)
                //Its better way to crash here
                assert(0);

            //Add bone ID
            vertex->ids[vertex->bones_num] = bone_i;
            //Set bone weight
            vertex->weights[vertex->bones_num] = vw->mWeight;
            //Increase bones amount
            vertex->bones_num += 1;
        }

        //Push bone back
        mesh_ptr->bones.push_back(bone);
    }

    for (unsigned int i = 0; i < faces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            mesh_ptr->indices_arr[i * 3 + j] = face.mIndices[j];
    }

    mesh_ptr->Init();
    mesh_ptr->setMeshData(mesh_ptr->vertices_arr, mesh_ptr->indices_arr, vertices, faces * 3);
}


void Engine::loadMesh(std::string file_path, ZSPIRE::Mesh* mesh_ptr, int index){
    const aiScene* scene = importer.ReadFile(file_path, loadflags);
    std::cout << "Loading mesh " << scene->mMeshes[index]->mName.C_Str() << " from file " << file_path << std::endl;

    processMesh(scene->mMeshes[index], mesh_ptr);
    importer.FreeScene();
}

void Engine::loadAnimation(std::string file_path, ZSPIRE::Animation* anim, int index){
    const aiScene* scene = importer.ReadFile(file_path, loadflagsAnim);
    std::cout << "Loading animation " << scene->mAnimations[index]->mName.C_Str() << " from file " << file_path << std::endl;

    aiAnimation* _anim = scene->mAnimations[index];
    anim->name = std::string(_anim->mName.C_Str());
    anim->duration = _anim->mDuration;
    anim->TPS = _anim->mTicksPerSecond;
    anim->NumChannels = _anim->mNumChannels;
    //allocate channels
    anim->channels = new ZSPIRE::AnimationChannel[anim->NumChannels];

    for(unsigned int chan_i = 0; chan_i < anim->NumChannels; chan_i ++){
        //Assimp channel
        aiNodeAnim* ai_channel = _anim->mChannels[chan_i];
        //ZSPIRE channel
        ZSPIRE::AnimationChannel* channel = &anim->channels[chan_i];
        channel->anim_ptr = anim; //Set pointer
        //Copy name
        for(unsigned int f_i = 0; f_i < ai_channel->mNodeName.length; f_i ++){
            channel->bone_name.push_back(ai_channel->mNodeName.data[f_i]);
        }

        //Allocate all keys
        channel->scale = new ZSVECTOR3[ai_channel->mNumScalingKeys];
        channel->pos = new ZSVECTOR3[ai_channel->mNumPositionKeys];
        channel->rot = new ZSQUATERNION[ai_channel->mNumRotationKeys];
        //Allocate times for keys
        channel->posTimes = new double[ai_channel->mNumPositionKeys];
        channel->scaleTimes = new double[ai_channel->mNumScalingKeys];
        channel->rotTimes = new double[ai_channel->mNumRotationKeys];

        //store keys amount
        channel->posKeysNum = ai_channel->mNumPositionKeys;
        channel->scaleKeysNum = ai_channel->mNumScalingKeys;
        channel->rotationKeysNum = ai_channel->mNumRotationKeys;
        //Write all transform data
        for(unsigned int pos_k_i = 0; pos_k_i < channel->posKeysNum; pos_k_i ++){
            aiVector3D ai_pos_key = ai_channel->mPositionKeys[pos_k_i].mValue;
            channel->pos[pos_k_i] = ZSVECTOR3(ai_pos_key.x, ai_pos_key.y, ai_pos_key.z);

            channel->posTimes[pos_k_i] = ai_channel->mPositionKeys[pos_k_i].mTime;
        }
        for(unsigned int scale_k_i = 0; scale_k_i < channel->scaleKeysNum; scale_k_i ++){
            aiVector3D ai_scale_key = ai_channel->mScalingKeys[scale_k_i].mValue;
            channel->scale[scale_k_i] = ZSVECTOR3(ai_scale_key.x, ai_scale_key.y, ai_scale_key.z);

            channel->scaleTimes[scale_k_i] = ai_channel->mScalingKeys[scale_k_i].mTime;
        }
        for(unsigned int rot_k_i = 0; rot_k_i < channel->rotationKeysNum; rot_k_i ++){
            aiQuaternion ai_rot_key = ai_channel->mRotationKeys[rot_k_i].mValue.Normalize();
            channel->rot[rot_k_i] = ZSQUATERNION(ai_rot_key.x, ai_rot_key.y, ai_rot_key.z, ai_rot_key.w);

            channel->rotTimes[rot_k_i] = ai_channel->mRotationKeys[rot_k_i].mTime;
        }

    }
    importer.FreeScene();
}

void Engine::loadNodeTree(std::string file_path, ZS3M::SceneNode* node){
    const aiScene* scene = importer.ReadFile(file_path, loadflags);

    ZS3M::SceneNode* root_node = new ZS3M::SceneNode;
    processNodeForTree(root_node, scene->mRootNode, scene);

    *node = *root_node;
    delete root_node;
    importer.FreeScene();
}


void Engine::processNodeForTree(ZS3M::SceneNode* node, aiNode* node_assimp, const aiScene* scene){
    node->node_label = node_assimp->mName.C_Str(); //assigning node name

    node->hasBone = isBoneAvailable(node->node_label, scene);
    //Write node matrix
    Engine::cmat(node_assimp->mTransformation, &node->node_transform);

    aiVector3D translation, scale;
    aiQuaternion rotation;
    node_assimp->mTransformation.Decompose(scale, rotation, translation);

    node->node_translation = ZSVECTOR3(translation.x, translation.y, translation.z);
    node->node_scaling = ZSVECTOR3(scale.x, scale.y, scale.z);
    node->node_rotation = ZSQUATERNION(rotation.x, rotation.y, rotation.z, rotation.w);

    //iterate over all meshes in this node
    unsigned int meshes_num = node_assimp->mNumMeshes;
    for(unsigned int ch_i = 0; ch_i < meshes_num; ch_i ++){
        aiMesh* mesh = scene->mMeshes[node_assimp->mMeshes[ch_i]];
        //Push mesh
        node->mesh_names.push_back(mesh->mName.C_Str());
    }
    //iterate over all children nodes and write their names
    unsigned int nodes_num = node_assimp->mNumChildren;
    for(unsigned int ch_i = 0; ch_i < nodes_num; ch_i ++){
        aiNode* child = node_assimp->mChildren[ch_i];
        ZS3M::SceneNode* mNode = new ZS3M::SceneNode;
        mNode->node_label = child->mName.C_Str();

        processNodeForTree(mNode, child, scene);
        node->children.push_back(mNode);
    }
}

bool Engine::isBoneAvailable(std::string bone_label, const aiScene* scene){
    unsigned int mesh_num = scene->mNumMeshes;
    for(unsigned int i = 0; i < mesh_num; i ++){
        aiMesh* mesh = scene->mMeshes[i];
        unsigned int bones_num = mesh->mNumBones;
        for(unsigned int b_i = 0; b_i < bones_num; b_i ++){
            aiBone* bn = mesh->mBones[b_i];
            if(bone_label.compare(bn->mName.C_Str()) == false)
                return true;
        }
    }
    return false;
}
