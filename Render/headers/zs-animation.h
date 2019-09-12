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

    ZSVECTOR3* pos;
    ZSVECTOR3* scale;
    aiQuaternion* rot;

    double* posTimes;
    double* scaleTimes;
    double* rotTimes;

    ZSVECTOR3 getPosition(double Time);
    ZSVECTOR3 getScale(double Time);
    aiQuaternion getRotation(double Time);

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
