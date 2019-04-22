#ifdef _WIN32
#include <cstring>
#endif

#ifndef zs_texture
#define zs_texture

namespace ZSPIRE {
	class Texture {
	public:
		Texture();

		unsigned int TEXTURE_ID;

		//Only for OGL : initialize texture in GL
		void Init();
		//Loads texture from buffer
        bool LoadDDSTextureFromBuffer(unsigned char* data);
		//Loads texture from file
		bool LoadDDSTextureFromFile(const char* path);
		//Use in rendering pipeline
		void Use(int slot);
		void Destroy();

	};
}



#endif
