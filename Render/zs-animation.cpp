#include "headers/zs-animation.h"
#include <cassert>

ZSPIRE::Animation::Animation(){

}
ZSPIRE::Animation::~Animation(){
    delete[] this->channels;
}

ZSPIRE::AnimationChannel::AnimationChannel(){
    this->posKeysNum = 0;
    this->rotationKeysNum = 0;
    this->scaleKeysNum = 0;
}

ZSPIRE::AnimationChannel::~AnimationChannel(){
    delete[] this->pos;
    delete[] this->rot;
    delete[] this->scale;

    delete[] this->rotTimes;
    delete[] this->posTimes;
    delete[] this->scaleTimes;
}

ZSPIRE::AnimationChannel* ZSPIRE::Animation::getChannelByNodeName(std::string node_name){
    for(unsigned int i = 0; i < this->NumChannels; i ++){
        AnimationChannel* cha = &this->channels[i];
        if(cha->bone_name.compare(node_name) == false) return cha;
    }
    return nullptr;
}

unsigned int ZSPIRE::AnimationChannel::getPositionIndex(double Time){
    for(unsigned int i = 0; i < this->posKeysNum - 1; i ++){
        double _time = posTimes[i + 1];
        if(_time > Time){
            return i;
        }
    }
    assert(0);
    return 0;
}
unsigned int ZSPIRE::AnimationChannel::getScaleIndex(double Time){
    for(unsigned int i = 0; i < this->scaleKeysNum - 1; i ++){
        double _time = scaleTimes[i + 1];
        if(_time > Time)
            return i;
    }
    assert(0);
    return 0;
}
unsigned int ZSPIRE::AnimationChannel::getRotationIndex(double Time){
    for(unsigned int i = 0; i < this->rotationKeysNum - 1; i ++){
        double _time = rotTimes[i + 1];
        if(_time > Time){
            return i;
        }
    }
    assert(0);
    return 0;
}

ZSVECTOR3 ZSPIRE::AnimationChannel::getPostitionInterpolated(double Time){
    unsigned int index1 = getPositionIndex(Time);
    unsigned int index2 = index1 + 1;

    double delta = Time - posTimes[index1];
    delta /= anim_ptr->TPS;

    ZSVECTOR3 v1 = pos[index1];
    ZSVECTOR3 v2 = pos[index2];

    return lerp(v1, v2, static_cast<float>(delta));
}

ZSVECTOR3 ZSPIRE::AnimationChannel::getScaleInterpolated(double Time){
    unsigned int index1 = getScaleIndex(Time);
    unsigned int index2 = index1 + 1;

    double delta = Time - scaleTimes[index1];
    delta /= anim_ptr->TPS;

    ZSVECTOR3 v1 = scale[index1];
    ZSVECTOR3 v2 = scale[index2];

    return lerp(v1, v2, static_cast<float>(delta));
}

ZSQUATERNION ZSPIRE::AnimationChannel::getRotationInterpolated(double Time){
    unsigned int index1 = getRotationIndex(Time);
    unsigned int index2 = index1 + 1;

    double delta = Time - posTimes[index1];
    delta /= anim_ptr->TPS;

    ZSQUATERNION q1 = rot[index1];
    ZSQUATERNION q2 = rot[index2];

    return slerp(q1, q2, static_cast<float>(delta));
}

ZSVECTOR3 ZSPIRE::AnimationChannel::getPostitionInterpolated(unsigned int frame){

}
ZSVECTOR3 ZSPIRE::AnimationChannel::getScaleInterpolated(unsigned int frame){

}
ZSQUATERNION ZSPIRE::AnimationChannel::getRotationInterpolated(unsigned int frame){

}
