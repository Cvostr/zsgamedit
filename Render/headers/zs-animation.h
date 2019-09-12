#ifndef ZSANIMATION_H
#define ZSANIMATION_H

#include <string>
#include "zs-math.h"

namespace ZSPIRE {

class AnimationChannel;

class Animation{
public:
    double duration;
    double TPS;
    std::string name;

    unsigned int NumChannels;
    AnimationChannel* channels;

    AnimationChannel* getChannelByNodeName(std::string node_name);

};

class AnimationChannel{
public:
    std::string bone_name;
    Animation* anim_ptr;

    unsigned int posKeysNum;
    unsigned int scaleKeysNum;
    unsigned int rotationKeysNum;

    ZSVECTOR3* pos;
    ZSVECTOR3* scale;
    ZSQUATERNION* rot;

    double* posTimes;
    double* scaleTimes;
    double* rotTimes;

    unsigned int getPositionIndex(double Time);
    unsigned int getScaleIndex(double Time);
    unsigned int getRotationIndex(double Time);

    ZSVECTOR3 getPostitionInterpolated(double Time);
    ZSVECTOR3 getScaleInterpolated(double Time);
    ZSQUATERNION getRotationInterpolated(double Time);

    AnimationChannel();
};

}

#endif // ZSANIMATION_H
