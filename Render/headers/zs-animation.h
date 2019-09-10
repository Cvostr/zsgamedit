#ifndef ZSANIMATION_H
#define ZSANIMATION_H

#include <string>
#include "zs-math.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace ZSPIRE {

class AnimationChannel{
public:
    std::string bone_name;

    unsigned int posKeysNum;
    unsigned int scaleKeysNum;
    unsigned int rotationKeysNum;

    //ZSVECTOR3* positions;
    //ZSVECTOR3* scalings;
    //ZSQUATERNION* rotations;

    aiVector3D* pos;
    aiVector3D* scale;
    aiQuaternion* rot;

    AnimationChannel();
};

class Animation{
public:
    double duration;
    double TPS;
    std::string name;

    unsigned int NumChannels;
    AnimationChannel* channels;

    AnimationChannel* getChannelByNodeName(std::string node_name);

};

}

#endif // ZSANIMATION_H
