#ifndef zs_texture
#define zs_texture

#include <string>

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

    class Texture3D{
    public:
        Texture3D();

        bool created;
        unsigned int TEXTURE_ID;

        void Init();
        bool pushTextureBuffer(int index, unsigned char* data);
        bool pushTexture(int index, std::string path);
        //Use in rendering pipeline
        void Use(int slot);
    };
}



#endif
