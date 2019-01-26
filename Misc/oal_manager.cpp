#include <AL/al.h>
#include <AL/alc.h>

#include <iostream>
#include <cstdio>

#include "headers/oal_manager.h"

ALCdevice* al_device;
ALCcontext* al_context;

bool ZSPIRE::SFX::initAL() {
	al_device = alcOpenDevice(NULL);
	if (!al_device) {
		std::cout << "AL: Can't initialize OpenAL device" << std::endl;
		return false;
	}

	al_context = alcCreateContext(al_device, NULL);

	if (!al_context) {
		std::cout << "AL: Can't initialize OpenAL device context" << std::endl;
		return false;
	}

	alcMakeContextCurrent(al_context);

	std::cout << "AL: OpenAL successfully initialized!" << std::endl;
	//Set default parameters
	setListenerPos(ZSVECTOR3(0.0f, 0.0f, 0.0f));
	setListenerOri(ZSVECTOR3(0.0f, 1.0f, 0.0f));

	return true;
}

void ZSPIRE::SFX::setListenerPos(ZSVECTOR3 pos) {
	alListenerfv(AL_POSITION, &pos.X);
}
void ZSPIRE::SFX::setListenerOri(ZSVECTOR3 ori) {
	alListenerfv(AL_ORIENTATION, &ori.X);
}

void ZSPIRE::SFX::setListenerVolume(float value){
	alListenerf(AL_GAIN, value);
}


void ZSPIRE::SFX::destroyAL(){
	alcDestroyContext(al_context);
	alcCloseDevice(al_device);
}
