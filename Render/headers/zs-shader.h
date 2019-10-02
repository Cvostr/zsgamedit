#ifndef zs_shader
#define zs_shader

#include "zs-math.h"
#include "../../World/headers/zs-camera.h"

namespace ZSPIRE {
	class Shader {
	protected:
		unsigned int SHADER_ID;

	public:

		void Init();
        bool compileFromFile(const char* VSpath, const char* FSpath);
        bool compileFromStr(const char* _VS, const char* _FS);
		void Use();
		void Destroy();

        unsigned int getUniformBufferIndex(const char* UB_NAME);
        void setUniformBufferBinding(unsigned int UB_INDEX, unsigned int binding);

        void setGLuniformColor(const char* uniform_str, ZSRGBCOLOR value);
        void setGLuniformVec3(const char* uniform_str, ZSVECTOR3 value);
        void setGLuniformVec4(const char* uniform_str, ZSVECTOR4 value);

        bool isCreated;

        Shader(); //Construct to set isCreated var to false
	};
}


#endif
