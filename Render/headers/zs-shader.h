#ifndef zs_shader
#define zs_shader

#include "zs-math.h"
#include "../../World/headers/zs-camera.h"
//#include "../../World/headers/props/lightsource.h"

namespace ZSPIRE {
	class Shader {
	protected:
		unsigned int SHADER_ID;

	public:

		void Init();
		bool compileFromFile(const char* VSpath, const char* FSpath);
		void Use();
		void Destroy();

		void setGLuniformColor(const char* uniform_str, ZSRGBCOLOR value);
		void setGLuniformFloat(const char* uniform_str, float value);
		void setGLuniformInt(const char* uniform_str, int value);
		void setGLuniformVec3(const char* uniform_str, ZSVECTOR3 value);
        void setGLuniformVec4(const char* uniform_str, ZSVECTOR4 value);
		void setGLuniformMat4x4(const char* uniform_str, ZSMATRIX4x4 value);

		void setHasDiffuseTextureProperty(bool hasDiffuseMap);
		void setHasNormalTextureProperty(bool hasNormalMap);
		void setTextureCountProperty(int tX, int tY);
		void setTransform(ZSMATRIX4x4 transform);
        void setCamera(Camera* cam, bool sendPos = false);
        void sendLight(unsigned int index, void* _light);
        void unsetLight(unsigned int index);

        bool isCreated;

        Shader(); //Construct to set isCreated var to false
	};
}


#endif
