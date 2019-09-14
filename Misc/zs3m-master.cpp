#include "headers/zs3m-master.h"
#include <fstream>

void ZS3M::SceneFileExport::pushMesh(ZSPIRE::Mesh* mesh){
    this->meshes_toWrite.push_back(mesh);
}
void ZS3M::SceneFileExport::setRootNode(MeshNode* node){
    this->rootNode = node;
}
void ZS3M::SceneFileExport::write(std::string output_file){
    std::ofstream stream;
    stream.open(output_file, std::ofstream::binary);

    stream << "zs3mscene0\n";
    unsigned int meshes_num = static_cast<unsigned int>(this->meshes_toWrite.size());
    stream.write(reinterpret_cast<char*>(meshes_num), sizeof (unsigned int));

    for(unsigned int mesh_i = 0; mesh_i < meshes_num; mesh_i ++){
        ZSPIRE::Mesh* mesh_ptr = this->meshes_toWrite[mesh_i];
        stream << "_MESH " << mesh_ptr->mesh_label << "\n";

        unsigned int vertexNum = mesh_ptr->vertices_num;
        unsigned int indexNum = mesh_ptr->indices_num;
        unsigned int bonesNum = static_cast<unsigned int>(mesh_ptr->bones.size());

        stream.write(reinterpret_cast<char*>(vertexNum), sizeof (unsigned int));
        stream.write(reinterpret_cast<char*>(indexNum), sizeof (unsigned int));
        stream.write(reinterpret_cast<char*>(bonesNum), sizeof (unsigned int));
        stream << "\n"; //Write divider
        //Write all vertices
        for (unsigned int v_i = 0; v_i < vertexNum; v_i ++) {
            ZSVERTEX* v_ptr = &mesh_ptr->vertices_arr[v_i];

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
            stream << "\n"; //Write divider
        }
        for(unsigned int ind_i = 0; ind_i < indexNum; ind_i ++){
            stream.write(reinterpret_cast<char*>(&mesh_ptr->indices_arr[ind_i]), sizeof(unsigned int));
        }
        stream << "\n"; //Write divider
        for (unsigned int b_i = 0; b_i < bonesNum; b_i ++) {
            ZSPIRE::Bone* bone = &mesh_ptr->bones[b_i];
            //Write bone name
            stream << "b " << bone->bone_name << "\n";

            stream << "\n"; //Write divider
        }

    }
    stream.close();
}
