#ifndef ZS3MMASTER_H
#define ZS3MMASTER_H
#include <render/zs-math.h>
#include "../../Render/headers/zs-mesh.h"
#include <string>
#include <vector>
namespace ZS3M {

class SceneNode{

public:
    //Node name string
    std::string node_label;
    //Transform matrix
    ZSMATRIX4x4 node_transform;

    ZSVECTOR3 node_translation;
    ZSVECTOR3 node_scaling;
    ZSQUATERNION node_rotation;

    bool hasBone;

    std::vector<std::string> child_node_labels;
    std::vector<SceneNode*> children;
    std::vector<std::string> mesh_names;

    SceneNode() {
        hasBone = false;
    }
};

class SceneFileExport{
private:
    std::vector<ZSPIRE::Mesh*> meshes_toWrite;
    ZS3M::SceneNode* rootNode;
public:

    void pushMesh(ZSPIRE::Mesh* mesh);
    void setRootNode(ZS3M::SceneNode* node);
    void write(std::string output_file);
    void writeNode(std::ofstream *stream, ZS3M::SceneNode* node);
    void getNodesNum(unsigned int* nds_ptr, ZS3M::SceneNode* node);

    SceneFileExport();
};

class ImportedSceneFile{
private:
    std::vector<ZS3M::SceneNode*> nodes_list;
public:
    std::vector<ZSPIRE::Mesh*> meshes_toWrite;
    ZS3M::SceneNode* rootNode;

    ZS3M::SceneNode* getSceneNodeWithName(std::string label);
    void loadFromBuffer(char* buffer, unsigned int buf_size);
    void loadFromFile(std::string file);

    void makeNodeHierarchy(ZS3M::SceneNode* node);
    ImportedSceneFile();
};

}

#endif // ZS3MMASTER_H
