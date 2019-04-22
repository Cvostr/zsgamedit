#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII
#define SLOTS_COUNT 4

#include "headers/zs-texture.h"


#include <GL/glew.h>
//Working with filesystem and mem
#include <fstream>

#include "../triggers.h"
#include <iostream>

#ifdef __linux__
#define _fileno fileno
#endif

static unsigned int tex_slots[SLOTS_COUNT] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };


void ZSPIRE::Texture::Init() {
	glGenTextures(1, &this->TEXTURE_ID); //Initializing texture in GL
	glBindTexture(GL_TEXTURE_2D, this->TEXTURE_ID); //We now working with this texture

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
}

ZSPIRE::Texture::Texture() {

}

void ZSPIRE::Texture::Use(int slot) {

    if ((slot < SLOTS_COUNT) && (this->TEXTURE_ID != tex_slots[slot])) {
		glActiveTexture(GL_TEXTURE0 + slot); //Activating texture slot
		glBindTexture(GL_TEXTURE_2D, this->TEXTURE_ID); //Sending texture to activated slot
        tex_slots[slot] = this->TEXTURE_ID;
	}
}


bool ZSPIRE::Texture::LoadDDSTextureFromFile(const char* path) {

#ifdef ZS_LOG
	std::cout << "TEXTURE: Loading texture from file : " << path << std::endl;
#endif

    std::ifstream texture_stream;
    texture_stream.open(path, std::ifstream::binary | std::ifstream::ate);

    if (texture_stream.fail()) { //Opening file stream failed, no file
		std::cout << "TEXTURE: FATAL: Error opening file stream! Perhaps, file " << path << " is missing!" << std::endl;
		return false;
	}

    //texture file size
    int size = static_cast<int>(texture_stream.tellg());
    texture_stream.seekg(0);

    unsigned char header[128];
    //reading texture header
    texture_stream.read(reinterpret_cast<char*>(&header[0]), 128);

	if (header[0] != 'D' && header[1] != 'D' && header[2] != 'S') { //File found, but isn't DDS texture
		std::cout << "TEXTURE: FATAL: Error processing file! Perhaps, file " << path << " is not DDS texture!" << std::endl;
        texture_stream.close();
        return false;
	}
    //back to start
    texture_stream.seekg(0);

    unsigned char * data = new unsigned char[size]; //Allocating buffer for file in heap
    texture_stream.read(reinterpret_cast<char*>(data), size); //Reading file to buffer
    LoadDDSTextureFromBuffer(data); //Read texture from buffer

    delete[] (data); //freeing buffer
    texture_stream.close(); //closing stream

	return true;
}

void ZSPIRE::Texture::Destroy() {

	glDeleteTextures(1, &this->TEXTURE_ID);

#ifdef ZS_LOG
	std::cout << "TEXTURE: realease sucess!" << std::endl;
#endif
}

bool ZSPIRE::Texture::LoadDDSTextureFromBuffer(unsigned char* data){

	Init();

    int HEIGHT = *(reinterpret_cast<int*>(&(data[12]))); //Getting height of texture in px info
    int WIDTH = *(reinterpret_cast<int*>(&(data[16]))); //Getting width of texture in px info
    unsigned int linearSize = *(reinterpret_cast<unsigned int*>(&(data[20])));
    unsigned int mipMapCount = *(reinterpret_cast<unsigned int*>(&(data[28])));
    unsigned int fourCC = *(reinterpret_cast<unsigned int*>(&(data[84])));

	unsigned char * bufferT;
	unsigned int bufsize;
	
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;//Getting buffer size
	
	bufferT = data + 128; //jumping over header
	
	
	unsigned int format; //Getting texture format
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		
		return 0;
	}
	//Getting block size
	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	int nwidth = WIDTH;
	int nheight = HEIGHT;

	//Mipmaps
	for (unsigned int level = 0; level < mipMapCount && (nwidth || nheight); ++level) //Iterating over mipmaps
	{
		unsigned int size = ((nwidth + 3) / 4)*((nheight + 3) / 4)*blockSize; //Calculating mip texture size
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, nwidth, nheight,
			0, size, bufferT + offset);

		offset += size;
		nwidth /= 2;
		nheight /= 2;
	}


	return true;
}

