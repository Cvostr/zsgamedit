#ifndef ZSANIMATION_H
#define ZSANIMATION_H

#include <string>
#include "zs-math.h"

namespace ZSPIRE {

class AnimationChannel{
public:
    std::string bone_name;

    unsigned int posKeysNum;
    unsigned int scaleKeysNum;
    unsigned int rotationKeysNum;

    ZSVECTOR3* positions;
    ZSVECTOR3* scalings;
    ZSQUATERNION* rotations;

    AnimationChannel();
};

class Animation{
public:
    double duration;
    double TPS;
    std::string name;

    unsigned int NumChannels;
    AnimationChannel* channels;

};

}

#endif // ZSANIMATION_H
