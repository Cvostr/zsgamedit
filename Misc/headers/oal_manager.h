#ifndef zs_oal_manager
#define zs_oal_manager

#include "../../Render/headers/zs-math.h"

namespace ZSPIRE {
	namespace SFX {
		//Initializes sound OpenAL device to begin work
		bool initAL();
		//Closes initialized device
		void destroyAL();

		void setListenerPos(ZSVECTOR3 pos);
		void setListenerOri(ZSVECTOR3 ori);
		void setListenerVolume(float value);
	}
}


#endif
