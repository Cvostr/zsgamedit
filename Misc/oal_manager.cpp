#include <AL/al.h>
#include <AL/alc.h>

#include <iostream>
#include <cstdio>
#include <sys/stat.h>

#include "headers/oal_manager.h"

static ALCdevice* al_device;
static ALCcontext* al_context;

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

    if (alGetError() != AL_NO_ERROR)
    {
        fprintf(stderr, "Can't initialize");
    }

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


void SoundBuffer::Init(){
    alGenBuffers(1, &this->al_buffer_id);
}
bool SoundBuffer::loadFileWAV(const char* file_path){
    std::cout << "Loading WAVE sound file " << file_path << std::endl;

    Init();
        unsigned int freq;
        ALenum format;
        unsigned int channels;
        int bits;

        unsigned char* data_buffer;

        FILE* fstream = fopen(file_path, "rb");
        if (!fstream) return false;

        struct stat buff;

    #ifdef _WIN32
        fstat(_fileno(fstream), &buff); //Getting file info
    #endif
    #ifdef __linux__
        fstat(fileno(fstream), &buff); //Getting file info
    #endif

        data_buffer = static_cast<unsigned char*>(malloc(buff.st_size));

        fread(data_buffer, 1, 12, fstream);

        fread(data_buffer, 1, 8, fstream);
        if (data_buffer[0] != 'f' || data_buffer[1] != 'm' || data_buffer[2] != 't' || data_buffer[3] != ' ')
        {
            return false;
        }
        fread(data_buffer, 1, 2, fstream);
        if (data_buffer[1] != 0 || data_buffer[0] != 1)
        {
            fprintf(stderr, "Not PCM :(\n");
            return false;
        }

        fread(data_buffer, 1, 2, fstream);
        channels = data_buffer[1] << 8;
        channels |= data_buffer[0];

        fread(data_buffer, 1, 4, fstream);
        freq = data_buffer[3] << 24;
        freq |= data_buffer[2] << 16;
        freq |= data_buffer[1] << 8;
        freq |= data_buffer[0];

        fread(data_buffer, 1, 6, fstream);
        fread(data_buffer, 1, 2, fstream);

        bits = data_buffer[1] << 8;
        bits |= data_buffer[0];

        if (bits == 8)
        {
            if (channels == 1)
                format = AL_FORMAT_MONO8;
            else if (channels == 2)
                format = AL_FORMAT_STEREO8;
        }
        else if (bits == 16)
        {
            if (channels == 1)
                format = AL_FORMAT_MONO16;
            else if (channels == 2)
                format = AL_FORMAT_STEREO16;
        }
        if (!format)
        {
            fprintf(stderr, "Incompatible format (%d, %d) :(\n", channels, bits);
            return false;
        }

        while(data_buffer[0] != 'd' || data_buffer[1] != 'a' || data_buffer[2] != 't' || data_buffer[3] != 'a')
            fread(data_buffer, 1, 4, fstream);

        fread(data_buffer, 1, 4, fstream);

        int size = data_buffer[3] << 24; //Getting size, 32 bit value
        size |= data_buffer[2] << 16;
        size |= data_buffer[1] << 8;
        size |= data_buffer[0];

        fread(data_buffer, 1, static_cast<size_t>(size), fstream);
        alBufferData(this->al_buffer_id, format, static_cast<void*>((data_buffer)), size, static_cast<int>(freq));
        int err = alGetError();
        if (err != AL_NO_ERROR)
        {
            std::cout << "Error loading " << err << std::endl;
            //Free heap
            free(data_buffer);
            fclose(fstream);
            return false;
        }

        free(data_buffer);

        fclose(fstream);
        return true;
}
void SoundBuffer::Destroy(){
    alDeleteBuffers(1, &this->al_buffer_id);
}
unsigned int SoundBuffer::getBufferIdAL(){
    return this->al_buffer_id;
}
SoundBuffer::SoundBuffer(){

}

void SoundSource::Init(){
    alGenSources(1, &this->al_source_id);

    int err = alGetError();
    if (err != AL_NO_ERROR)
    {
        std::cout <<  "Error creating source " << err << std::endl;

    }
}
void SoundSource::Destroy(){
    alDeleteSources(1, &this->al_source_id);
}
void SoundSource::apply_settings(){
    alSource3f(al_source_id, AL_POSITION, this->source_pos.X, this->source_pos.Y, this->source_pos.Z);
    alSourcef(al_source_id, AL_GAIN, this->source_gain);
    alSourcef(al_source_id, AL_PITCH, this->source_pitch);
}
void SoundSource::setPosition(ZSVECTOR3 pos){
    this->source_pos = pos;
    alSource3f(al_source_id, AL_POSITION, pos.X, pos.Y, pos.Z);
}
void SoundSource::play(){
    alSourcePlay(this->al_source_id);
}
void SoundSource::stop(){
    alSourceStop(this->al_source_id);
}
void SoundSource::setAlBuffer(SoundBuffer* buffer){
    alSourcei(al_source_id, AL_BUFFER, static_cast<ALint>(buffer->getBufferIdAL()));
}
