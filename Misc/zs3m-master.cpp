#include "headers/zs3m-master.h"
#include <fstream>
#include <iostream>

ZS3M::SceneFileExport::SceneFileExport(){

}
void ZS3M::SceneFileExport::pushMesh(ZSPIRE::Mesh* mesh){
    this->meshes_toWrite.push_back(mesh);
}
void ZS3M::SceneFileExport::setRootNode(ZS3M::SceneNode* node){
    this->rootNode = node;
}
void ZS3M::SceneFileExport::write(std::string output_file){
    std::cout << "ZS3M: Writing ZS3M scene " << output_file << std::endl;
    std::ofstream stream;
    stream.open(output_file.c_str(), std::ofstream::binary);

    stream << "zs3mscene\n";
    unsigned int model_ver = 1000;
    unsigned int meshes_num = static_cast<unsigned int>(this->meshes_toWrite.size());
    unsigned int nodes_num = 0;
    getNodesNum(&nodes_num, this->rootNode);

    stream.write(reinterpret_cast<char*>(&model_ver), sizeof (unsigned int));
    stream.write(reinterpret_cast<char*>(&meshes_num), sizeof (unsigned int));
    stream.write(reinterpret_cast<char*>(&nodes_num), sizeof (unsigned int));
    stream << "\n"; //Write divider
    //Write all nodes
    writeNode(&stream, rootNode);

    //Iterate over all meshes
    for(unsigned int mesh_i = 0; mesh_i < meshes_num; mesh_i ++){
        ZSPIRE::Mesh* mesh_ptr = this->meshes_toWrite[mesh_i];
        std::cout << "ZS3M: Writing Mesh " << mesh_ptr->mesh_label << std::endl;
        stream << "_MESH " << (mesh_ptr->mesh_label + '\0');

        unsigned int vertexNum = mesh_ptr->vertices_num;
        unsigned int indexNum = mesh_ptr->indices_num;
        unsigned int bonesNum = static_cast<unsigned int>(mesh_ptr->bones.size());
        //Write base numbers
        stream.write(reinterpret_cast<char*>(&vertexNum), sizeof (unsigned int));
        stream.write(reinterpret_cast<char*>(&indexNum), sizeof (unsigned int));
        stream.write(reinterpret_cast<char*>(&bonesNum), sizeof (unsigned int));
        stream << "\n"; //Write divider
        //Write all vertices
        for (unsigned int v_i = 0; v_i < vertexNum; v_i ++) {
            ZSVERTEX* v_ptr = &mesh_ptr->vertices_arr[v_i];
            //Write vertex vectors
            stream.write(reinterpret_cast<char*>(&v_ptr->pos), sizeof(float) * 3);
            stream.write(reinterpret_cast<char*>(&v_ptr->uv), sizeof(float) * 2);
            stream.write(reinterpret_cast<char*>(&v_ptr->normal), sizeof(float) * 3);
            stream.write(reinterpret_cast<char*>(&v_ptr->tangent), sizeof(float) * 3);
            stream.write(reinterpret_cast<char*>(&v_ptr->bitangent), sizeof(float) * 3);
            stream.write(reinterpret_cast<char*>(&v_ptr->bones_num), sizeof(unsigned int));
            for(unsigned int vb_i = 0; vb_i < v_ptr->bones_num; vb_i ++){
                unsigned int bone_id = v_ptr->ids[vb_i];
                float b_weight = v_ptr->weights[vb_i];
                //Write bone values
                stream.write(reinterpret_cast<char*>(&bone_id), sizeof(unsigned int));
                stream.write(reinterpret_cast<char*>(&b_weight), sizeof(float));
            }
        }
        for(unsigned int ind_i = 0; ind_i < indexNum; ind_i ++){
            stream.write(reinterpret_cast<char*>(&mesh_ptr->indices_arr[ind_i]), sizeof(unsigned int));
        }
        stream << "\n"; //Write divider

        for (unsigned int b_i = 0; b_i < bonesNum; b_i ++) {
            ZSPIRE::Bone* bone = &mesh_ptr->bones[b_i];
            //Write bone name
            stream << bone->bone_name + '\0';
            //Write offset matrix
            for(unsigned int m_i = 0; m_i < 4; m_i ++){
                for(unsigned int m_j = 0; m_j < 4; m_j ++){
                    float m_v = bone->offset.m[m_i][m_j];
                    stream.write(reinterpret_cast<char*>(&m_v), sizeof(float));
                }
            }
            stream << "\n"; //Write divider
        }

    }
    stream.close();
}

void ZS3M::SceneFileExport::writeNode(std::ofstream *stream, ZS3M::SceneNode* node){
    //Write node header
    *stream << "_NODE " << (node->node_label + '\0');

    unsigned int meshesNum = static_cast<unsigned int>(node->mesh_names.size());
    unsigned int childrenNum = static_cast<unsigned int>(node->children.size());

    stream->write(reinterpret_cast<char*>(&meshesNum), sizeof(unsigned int));
    stream->write(reinterpret_cast<char*>(&childrenNum), sizeof(unsigned int));

    *stream << "\n"; //Write divider

    for(unsigned int mesh_i = 0; mesh_i < node->mesh_names.size(); mesh_i ++){
        //Write mesh name
        *stream << (node->mesh_names[mesh_i] + '\0');
    }
    for(unsigned int ch_i = 0; ch_i < node->children.size(); ch_i ++){
        //Write child node string
        *stream << (node->children[ch_i]->node_label + '\0');
    }
    //Write node base matrix
    for(unsigned int m_i = 0; m_i < 4; m_i ++){
        for(unsigned int m_j = 0; m_j < 4; m_j ++){
            float m_v = node->node_transform.m[m_i][m_j];
            stream->write(reinterpret_cast<char*>(&m_v), sizeof(float));
        }
    }
    stream->write(reinterpret_cast<char*>(&node->node_translation.X), sizeof(float));//Writing position X
    stream->write(reinterpret_cast<char*>(&node->node_translation.Y), sizeof(float)); //Writing position Y
    stream->write(reinterpret_cast<char*>(&node->node_translation.Z), sizeof(float)); //Writing position Z

    stream->write(reinterpret_cast<char*>(&node->node_scaling.X), sizeof(float));//Writing scale X
    stream->write(reinterpret_cast<char*>(&node->node_scaling.Y), sizeof(float)); //Writing scale Y
    stream->write(reinterpret_cast<char*>(&node->node_scaling.Z), sizeof(float)); //Writing scale Z

    stream->write(reinterpret_cast<char*>(&node->node_rotation.X), sizeof(float));//Writing rotation X
    stream->write(reinterpret_cast<char*>(&node->node_rotation.Y), sizeof(float)); //Writing rotation Y
    stream->write(reinterpret_cast<char*>(&node->node_rotation.Z), sizeof(float)); //Writing rotation Z
    stream->write(reinterpret_cast<char*>(&node->node_rotation.W), sizeof(float)); //Writing rotation W
    *stream << "\n"; //Write divider
    //Write all children
    for(unsigned int ch_i = 0; ch_i < node->children.size(); ch_i ++){
        writeNode(stream, node->children[ch_i]);
    }
}

void ZS3M::SceneFileExport::getNodesNum(unsigned int* nds_ptr, ZS3M::SceneNode* node){
    *nds_ptr += 1;
    //Write all children
    for(unsigned int ch_i = 0; ch_i < node->children.size(); ch_i ++){
        getNodesNum(nds_ptr, node->children[ch_i]);
    }
}

void ZS3M::ImportedSceneFile::loadFromBuffer(char* buffer, unsigned int buf_size){
    char header[10];
    memcpy(&header, buffer, 9);

    if(strcmp(header, "zs3mscene") == true){
        return;
    }

    unsigned int model_ver = 0;
    unsigned int meshes_num = 0;
    unsigned int nodes_num = 0;

    memcpy(reinterpret_cast<char*>(&model_ver), &buffer[10], sizeof(unsigned int));
    memcpy(reinterpret_cast<char*>(&meshes_num), &buffer[14], sizeof(unsigned int));
    memcpy(reinterpret_cast<char*>(&nodes_num), &buffer[18], sizeof(unsigned int));

    if(model_ver != 1000){
        return;
    }

    unsigned int cur_pos = 21;
    char prefix[5];

    while(cur_pos < buf_size){
        cur_pos += 1;
        memcpy(&prefix, &buffer[cur_pos], 5);

        if(prefix[0] == '_' && prefix[1] == 'N' && prefix[2] == 'O' && prefix[3] == 'D' && prefix[4] == 'E'){
            cur_pos += 6;
            ZS3M::SceneNode* node = new ZS3M::SceneNode;

            char node_label[64];
            //Read NODE label string
            strcpy(&node_label[0], &buffer[cur_pos]);
            node->node_label = std::string(node_label);
            cur_pos += strlen(node_label) + 1;

            unsigned int meshesNum = 0;
            unsigned int childrenNum = 0;

            //Read data
            memcpy(reinterpret_cast<char*>(&meshesNum), &buffer[cur_pos], sizeof (unsigned int));
            memcpy(reinterpret_cast<char*>(&childrenNum), &buffer[cur_pos + sizeof (unsigned int)], sizeof (unsigned int));
            cur_pos += 9;

            //iterate over all meshes, connected to this node
            for(unsigned int mesh_i = 0; mesh_i < meshesNum; mesh_i ++){
                char mesh_label[64];
                //Read NODE label string
                strcpy(&mesh_label[0], &buffer[cur_pos]);
                cur_pos += strlen(mesh_label) + 1;
                //Put mesh name
                node->mesh_names.push_back(mesh_label);
            }
            //iterate over all children nodes, connected to this node
            for(unsigned int ch_i = 0; ch_i < childrenNum; ch_i ++){
                //Read child name
                char child_node_label[64];
                strcpy(&child_node_label[0], &buffer[cur_pos]);
                cur_pos += strlen(child_node_label) + 1;
                //Put child name
                node->child_node_labels.push_back(std::string(child_node_label));
            }
            //Read node base matrix
            for(unsigned int m_i = 0; m_i < 4; m_i ++){
                for(unsigned int m_j = 0; m_j < 4; m_j ++){
                    float* m_v = &node->node_transform.m[m_i][m_j];
                    memcpy(reinterpret_cast<char*>(m_v), &buffer[cur_pos], 4);
                    cur_pos += 4;
                }
            }
            memcpy(reinterpret_cast<char*>(&node->node_translation.X), &buffer[cur_pos], sizeof(float));
            cur_pos += sizeof(float);
            memcpy(reinterpret_cast<char*>(&node->node_translation.Y), &buffer[cur_pos], sizeof(float));
            cur_pos += sizeof(float);
            memcpy(reinterpret_cast<char*>(&node->node_translation.Z), &buffer[cur_pos], sizeof(float));
            cur_pos += sizeof(float);
            memcpy(reinterpret_cast<char*>(&node->node_scaling.X), &buffer[cur_pos], sizeof(float));
            cur_pos += sizeof(float);
            memcpy(reinterpret_cast<char*>(&node->node_scaling.Y), &buffer[cur_pos], sizeof(float));
            cur_pos += sizeof(float);
            memcpy(reinterpret_cast<char*>(&node->node_scaling.Z), &buffer[cur_pos], sizeof(float));
            cur_pos += sizeof(float);
            memcpy(reinterpret_cast<char*>(&node->node_rotation.X), &buffer[cur_pos], sizeof(float));
            cur_pos += sizeof(float);
            memcpy(reinterpret_cast<char*>(&node->node_rotation.Y), &buffer[cur_pos], sizeof(float));
            cur_pos += sizeof(float);
            memcpy(reinterpret_cast<char*>(&node->node_rotation.Z), &buffer[cur_pos], sizeof(float));
            cur_pos += sizeof(float);
            memcpy(reinterpret_cast<char*>(&node->node_rotation.W), &buffer[cur_pos], sizeof(float));
            cur_pos += sizeof(float);


            //Add node to list
            nodes_list.push_back(node);
        }

        //We reached mesh
        if(prefix[0] == '_' && prefix[1] == 'M' && prefix[2] == 'E' && prefix[3] == 'S' && prefix[4] == 'H'){
            cur_pos += 6;

            char mesh_label[64];
            //Read mesh label string
            strcpy(&mesh_label[0], &buffer[cur_pos]);
            cur_pos += strlen(mesh_label) + 1;

            unsigned int vertexNum = 0;
            unsigned int indexNum = 0;
            unsigned int bonesNum = 0;

            memcpy(reinterpret_cast<char*>(&vertexNum), &buffer[cur_pos], sizeof (unsigned int));
            memcpy(reinterpret_cast<char*>(&indexNum), &buffer[cur_pos + 4], sizeof (unsigned int));
            memcpy(reinterpret_cast<char*>(&bonesNum), &buffer[cur_pos + 8], sizeof (unsigned int));
            cur_pos += 13;

            ZSPIRE::Mesh* newmesh = new ZSPIRE::Mesh;
            newmesh->mesh_label = mesh_label;
            newmesh->vertices_num = vertexNum;
            newmesh->indices_num = indexNum;
            //Allocate arrays for vectors
            newmesh->vertices_arr = new ZSVERTEX[vertexNum];
            newmesh->indices_arr = new unsigned int[indexNum];

            for(unsigned int v_i = 0; v_i < vertexNum; v_i ++){
                ZSVERTEX v_ptr;
                //Read vertex vectors
                memcpy(reinterpret_cast<char*>(&v_ptr.pos), &buffer[cur_pos], sizeof(float) * 3);
                cur_pos += sizeof(float) * 3;
                memcpy(reinterpret_cast<char*>(&v_ptr.uv), &buffer[cur_pos], sizeof(float) * 2);
                cur_pos += sizeof(float) * 2;
                memcpy(reinterpret_cast<char*>(&v_ptr.normal), &buffer[cur_pos], sizeof(float) * 3);
                cur_pos += sizeof(float) * 3;
                memcpy(reinterpret_cast<char*>(&v_ptr.tangent), &buffer[cur_pos], sizeof(float) * 3);
                cur_pos += sizeof(float) * 3;
                memcpy(reinterpret_cast<char*>(&v_ptr.bitangent), &buffer[cur_pos], sizeof(float) * 3);
                cur_pos += sizeof(float) * 3;
                memcpy(reinterpret_cast<char*>(&v_ptr.bones_num), &buffer[cur_pos], sizeof(unsigned int));
                cur_pos += sizeof(unsigned int);
                //Read vertex bone data
                for(unsigned int vb_i = 0; vb_i < 12; vb_i ++){
                    unsigned int* bone_id = &v_ptr.ids[vb_i];
                    float* b_weight = &v_ptr.weights[vb_i];

                    *bone_id = 0;
                    *b_weight = 0.0f;
                }
                for(unsigned int vb_i = 0; vb_i < v_ptr.bones_num; vb_i ++){
                    unsigned int* bone_id = &v_ptr.ids[vb_i];
                    float* b_weight = &v_ptr.weights[vb_i];
                    //Read bone values
                    memcpy(reinterpret_cast<char*>(bone_id), &buffer[cur_pos], sizeof(unsigned int));
                    cur_pos += sizeof(unsigned int);
                    memcpy(reinterpret_cast<char*>(b_weight), &buffer[cur_pos], sizeof(float));
                    cur_pos += sizeof(float);
                }
                newmesh->vertices_arr[v_i] = v_ptr;
            }
            //Read mesh indices
            for(unsigned int in_i = 0; in_i < indexNum; in_i ++){
                memcpy(reinterpret_cast<char*>(&newmesh->indices_arr[in_i]), &buffer[cur_pos], sizeof(unsigned int));
                cur_pos += sizeof(unsigned int);
            }
            cur_pos += 1;

            newmesh->Init();
            newmesh->setMeshData(newmesh->vertices_arr, newmesh->indices_arr, newmesh->vertices_num, newmesh->indices_num);

            delete[] newmesh->vertices_arr;
            delete[] newmesh->indices_arr;

            for (unsigned int b_i = 0; b_i < bonesNum; b_i ++) {
                char bone_label[64];
                //Read mesh label string
                strcpy(&bone_label[0], &buffer[cur_pos]);
                cur_pos += strlen(bone_label) + 1;

                ZSPIRE::Bone bone(bone_label);
                for(unsigned int m_i = 0; m_i < 4; m_i ++){
                    for(unsigned int m_j = 0; m_j < 4; m_j ++){
                        float* m_v = &bone.offset.m[m_i][m_j];
                        memcpy(reinterpret_cast<char*>(m_v), &buffer[cur_pos], sizeof(float));
                        cur_pos += sizeof(float);
                    }
                }
                newmesh->bones.push_back(bone);
                cur_pos += 1;
            }

            //Push mesh as output result
            this->meshes_toWrite.push_back(newmesh);

        }
    }

    rootNode = nodes_list[0];

    makeNodeHierarchy(rootNode);
}

void ZS3M::ImportedSceneFile::makeNodeHierarchy(ZS3M::SceneNode* node){
    for(unsigned int i = 0; i < node->child_node_labels.size(); i ++){
        std::string child_name = node->child_node_labels[i];

        ZS3M::SceneNode* nn = getSceneNodeWithName(child_name);

        if(nn == nullptr)
            continue;

        node->children.push_back(nn);

        makeNodeHierarchy(node->children[i]);
    }
}

ZS3M::SceneNode* ZS3M::ImportedSceneFile::getSceneNodeWithName(std::string label){
    for(unsigned int i = 0; i < this->nodes_list.size(); i ++){
        ZS3M::SceneNode* node = nodes_list[i];
        if(node->node_label.compare(label) == false){
            return node;
        }
    }
    return nullptr;
}

void ZS3M::ImportedSceneFile::loadFromFile(std::string file){
    std::ifstream stream;
    stream.open(file, std::iostream::binary | std::iostream::ate);
    unsigned int zs3m_size = static_cast<unsigned int>(stream.tellg());
    stream.seekg(0, std::ifstream::beg);
    char* file_buffer = new char[zs3m_size];
    stream.read(file_buffer, zs3m_size);

    loadFromBuffer(file_buffer, zs3m_size);

}

ZS3M::ImportedSceneFile::ImportedSceneFile(){

}


ZS3M::AnimationFileExport::AnimationFileExport(ZSPIRE::Animation* anim_ptr){
    this->anim_ptr = anim_ptr;
}

void ZS3M::AnimationFileExport::writeChannel(std::ofstream* stream, unsigned int index){
    ZSPIRE::AnimationChannel* channel = &anim_ptr->channels[index];
    //Write channel prefix and its name
    *stream << "_CHAN" << '\0' << channel->bone_name + '\0';

    stream->write(reinterpret_cast<char*>(&channel->posKeysNum), sizeof (unsigned int));
    stream->write(reinterpret_cast<char*>(&channel->scaleKeysNum), sizeof (unsigned int));
    stream->write(reinterpret_cast<char*>(&channel->rotationKeysNum), sizeof (unsigned int));

    for(unsigned int pos_i = 0; pos_i < channel->posKeysNum; pos_i ++){
        //Writing pos vector
        stream->write(reinterpret_cast<char*>(&channel->pos[pos_i].X), sizeof (float));
        stream->write(reinterpret_cast<char*>(&channel->pos[pos_i].Y), sizeof (float));
        stream->write(reinterpret_cast<char*>(&channel->pos[pos_i].Z), sizeof (float));
        //Write pos key time
        stream->write(reinterpret_cast<char*>(&channel->posTimes[pos_i]), sizeof (double));

    }
    for(unsigned int sca_i = 0; sca_i < channel->scaleKeysNum; sca_i ++){
        //Writing scale vector
        stream->write(reinterpret_cast<char*>(&channel->scale[sca_i].X), sizeof (float));
        stream->write(reinterpret_cast<char*>(&channel->scale[sca_i].Y), sizeof (float));
        stream->write(reinterpret_cast<char*>(&channel->scale[sca_i].Z), sizeof (float));
        //Write pos key time
        stream->write(reinterpret_cast<char*>(&channel->scaleTimes[sca_i]), sizeof (double));
    }
    for(unsigned int rot_i = 0; rot_i < channel->rotationKeysNum; rot_i ++){
        //Writing pos vector
        stream->write(reinterpret_cast<char*>(&channel->rot[rot_i].X), sizeof (float));
        stream->write(reinterpret_cast<char*>(&channel->rot[rot_i].Y), sizeof (float));
        stream->write(reinterpret_cast<char*>(&channel->rot[rot_i].Z), sizeof (float));
        stream->write(reinterpret_cast<char*>(&channel->rot[rot_i].W), sizeof (float));
        //Write pos key time
        stream->write(reinterpret_cast<char*>(&channel->rotTimes[rot_i]), sizeof (double));
    }
    *stream << '\n';
}
void ZS3M::AnimationFileExport::write(std::string output_file){
    std::ofstream stream;
    stream.open(output_file.c_str(), std::ofstream::binary);

    stream << "zs3manim" << '\0' << anim_ptr->name + '\0'; //Write animation
    //Write main animation data
    stream.write(reinterpret_cast<char*>(&anim_ptr->TPS), sizeof (double));
    stream.write(reinterpret_cast<char*>(&anim_ptr->duration), sizeof (double));
    stream.write(reinterpret_cast<char*>(&anim_ptr->NumChannels), sizeof (unsigned int));
    //Write divider
    stream << "\n";

    for(unsigned int cha_i = 0; cha_i < anim_ptr->NumChannels; cha_i ++){
        writeChannel(&stream, cha_i);
    }

    stream.close();
}

ZS3M::ImportedAnimationFile::ImportedAnimationFile(){

}

void ZS3M::ImportedAnimationFile::loadFromBuffer(char* buffer, unsigned int size){
    char prefix[64];
    unsigned int byte_offset = 0;

    strcpy(&prefix[0], &buffer[byte_offset]);
    byte_offset += strlen(prefix) + 1;
    if(strcmp(prefix, "zs3manim") == true){
        return;
    }
    //Allocate animation class
    anim_ptr = new ZSPIRE::Animation; //allocate animation
    //Read animation name
    strcpy(&prefix[0], &buffer[byte_offset]);
    byte_offset += strlen(prefix) + 1;
    //Set animation name
    anim_ptr->name = std::string(prefix);
    //Read Tick Per Second property
    memcpy(reinterpret_cast<char*>(&anim_ptr->TPS), &buffer[byte_offset], sizeof(double));
    byte_offset += sizeof(double);
    //Read duration property
    memcpy(reinterpret_cast<char*>(&anim_ptr->duration), &buffer[byte_offset], sizeof(double));
    byte_offset += sizeof(double);
    //Read Channel amount
    memcpy(reinterpret_cast<char*>(&anim_ptr->NumChannels), &buffer[byte_offset], sizeof (unsigned int));
    byte_offset += sizeof (unsigned int);
    //Allocate all channels
    anim_ptr->channels = new ZSPIRE::AnimationChannel[anim_ptr->NumChannels];
    unsigned int ch_i = 0;

    while (byte_offset < size) {
        byte_offset += 1;
        memcpy(&prefix, &buffer[byte_offset], 6);

        if(strcmp(prefix, "_CHAN") == false){
            byte_offset += 6;
            //Read node, channel will work with
            strcpy(&prefix[0], &buffer[byte_offset]);
            byte_offset += strlen(prefix) + 1;
            //Allocate animation channel
            ZSPIRE::AnimationChannel* chan = &anim_ptr->channels[ch_i];
            chan->bone_name = std::string(prefix);
            chan->anim_ptr = anim_ptr;

            memcpy(reinterpret_cast<char*>(&chan->posKeysNum), &buffer[byte_offset], sizeof(unsigned int));
            byte_offset += sizeof(unsigned int);
            memcpy(reinterpret_cast<char*>(&chan->scaleKeysNum), &buffer[byte_offset], sizeof(unsigned int));
            byte_offset += sizeof(unsigned int);
            memcpy(reinterpret_cast<char*>(&chan->rotationKeysNum), &buffer[byte_offset], sizeof(unsigned int));
            byte_offset += sizeof(unsigned int);

            //Allocate pos vectors and time intervals array
            chan->pos = new ZSVECTOR3[chan->posKeysNum];
            chan->posTimes = new double[chan->posKeysNum];
            //Allocate scale vectors and time intervals array
            chan->scale = new ZSVECTOR3[chan->scaleKeysNum];
            chan->scaleTimes = new double[chan->scaleKeysNum];
            //Allocate rotation vectors and time intervals array
            chan->rot = new ZSQUATERNION[chan->rotationKeysNum];
            chan->rotTimes = new double[chan->rotationKeysNum];

            for(unsigned int pos_i = 0; pos_i < chan->posKeysNum; pos_i ++){
                //reading pos vector
                memcpy(reinterpret_cast<char*>(&chan->pos[pos_i].X), &buffer[byte_offset], sizeof(float));
                byte_offset += sizeof(float);
                memcpy(reinterpret_cast<char*>(&chan->pos[pos_i].Y), &buffer[byte_offset], sizeof(float));
                byte_offset += sizeof(float);
                memcpy(reinterpret_cast<char*>(&chan->pos[pos_i].Z), &buffer[byte_offset], sizeof(float));
                byte_offset += sizeof(float);
                memcpy(reinterpret_cast<char*>(&chan->posTimes[pos_i]), &buffer[byte_offset], sizeof(double));
                byte_offset += sizeof(double);
            }
            for(unsigned int sca_i = 0; sca_i < chan->scaleKeysNum; sca_i ++){
                //reading scale vector
                memcpy(reinterpret_cast<char*>(&chan->scale[sca_i].X), &buffer[byte_offset], sizeof(float));
                byte_offset += sizeof(float);
                memcpy(reinterpret_cast<char*>(&chan->scale[sca_i].Y), &buffer[byte_offset], sizeof(float));
                byte_offset += sizeof(float);
                memcpy(reinterpret_cast<char*>(&chan->scale[sca_i].Z), &buffer[byte_offset], sizeof(float));
                byte_offset += sizeof(float);
                memcpy(reinterpret_cast<char*>(&chan->scaleTimes[sca_i]), &buffer[byte_offset], sizeof(double));
                byte_offset += sizeof(double);
            }
            for(unsigned int rot_i = 0; rot_i < chan->rotationKeysNum; rot_i ++){
                //reading scale vector
                memcpy(reinterpret_cast<char*>(&chan->rot[rot_i].X), &buffer[byte_offset], sizeof(float));
                byte_offset += sizeof(float);
                memcpy(reinterpret_cast<char*>(&chan->rot[rot_i].Y), &buffer[byte_offset], sizeof(float));
                byte_offset += sizeof(float);
                memcpy(reinterpret_cast<char*>(&chan->rot[rot_i].Z), &buffer[byte_offset], sizeof(float));
                byte_offset += sizeof(float);
                memcpy(reinterpret_cast<char*>(&chan->rot[rot_i].W), &buffer[byte_offset], sizeof(float));
                byte_offset += sizeof(float);
                memcpy(reinterpret_cast<char*>(&chan->rotTimes[rot_i]), &buffer[byte_offset], sizeof(double));
                byte_offset += sizeof(double);
            }
            ch_i += 1;
        }
    }
}

void ZS3M::ImportedAnimationFile::loadFromFile(std::string file){
    std::ifstream stream;
    stream.open(file, std::iostream::binary | std::iostream::ate);
    unsigned int zs3m_size = static_cast<unsigned int>(stream.tellg());
    stream.seekg(0, std::ifstream::beg);
    char* file_buffer = new char[zs3m_size];
    stream.read(file_buffer, zs3m_size);

    loadFromBuffer(file_buffer, zs3m_size);
}
