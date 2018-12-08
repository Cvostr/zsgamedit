#ifndef zs_camera
#define zs_camera

#include "../../Render/headers/zs-math.h"

#define ZSCAMERA_PROJECTION_ORTHOGONAL 1
#define ZSCAMERA_PROJECTION_PERSPECTIVE 2

typedef unsigned int ZSCAMERAPROJECTIONTYPE;

typedef struct ZSVIEWPORT{
    unsigned int startX;
    unsigned int startY;

    unsigned int endX;
    unsigned int endY;

    ZSVIEWPORT(unsigned int _startX, unsigned int _startY, unsigned int _endX, unsigned int _endY){
        startX = _startX;
        startY = _startY;
        endX = _endX;
        endY = _endY;
    }

    ZSVIEWPORT(){}

}ZSVIEWPORT;

namespace ZSPIRE{
    class Camera{
        protected:
            ZSVECTOR3 camera_pos; //Camera position vector
            ZSVECTOR3 camera_up; //Camera up direction vector
            ZSVECTOR3 camera_front; //Camera front direction vector

            float FOV; //Field of view
            float nearZ; //Nearest Z occlusion plane
            float farZ;

            ZSVIEWPORT viewport;

            ZSMATRIX4x4 proj; //Calculated Projection matrix
            ZSMATRIX4x4 view; //Calculated View matrix

            ZSCAMERAPROJECTIONTYPE proj_type;

            bool isMoving;

        public:
            ZSVECTOR3 _dest_pos;

            ZSVECTOR3 getCameraPosition();
            ZSVECTOR3 getCameraUpVec();
            ZSVECTOR3 getCameraRightVec();
            ZSVECTOR3 getCameraFrontVec();

            ZSVIEWPORT getViewport();

            void updateProjectionMat();
            void updateViewMat();

            void setPosition(ZSVECTOR3 pos);
            void setFront(ZSVECTOR3 front);
            void setUp(ZSVECTOR3 up);

            void setFOV(float FOV);
            void setZplanes(float nearZ, float farZ);
            void setViewport(ZSVIEWPORT viewport);

            void setProjectionType(ZSCAMERAPROJECTIONTYPE type);
            void startMoving();

            ZSMATRIX4x4 getViewMatrix();
            ZSMATRIX4x4 getProjMatrix();
            void updateTick();

            Camera();
    };
}

#endif
