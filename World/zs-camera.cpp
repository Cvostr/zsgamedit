#include "headers/zs-camera.h"

void ZSPIRE::Camera::setFOV(float FOV){
    this->FOV = FOV;
    updateProjectionMat();
}

void ZSPIRE::Camera::setZplanes(float nearZ, float farZ){
    this->nearZ = nearZ;
    this->farZ = farZ;
    updateProjectionMat();
}

void ZSPIRE::Camera::setProjectionType(ZSCAMERAPROJECTIONTYPE type){
    proj_type = type;
    updateProjectionMat();
}

void ZSPIRE::Camera::setViewport(ZSVIEWPORT viewport){
    this->viewport = viewport;
    updateProjectionMat();
}

void ZSPIRE::Camera::updateProjectionMat(){
    if(proj_type == ZSCAMERA_PROJECTION_PERSPECTIVE){
        float aspect = (float) (viewport.endX - viewport.startX) / (float)(viewport.endY - viewport.startY);
        proj = getPerspective(FOV, aspect, nearZ, farZ);
    }else{
		proj = getOrthogonal(0, (float)(viewport.endX - viewport.startX), 0, (float)(viewport.endY - viewport.startY), nearZ, farZ);
    }
}

void ZSPIRE::Camera::updateViewMat(){
    view = matrixLookAt(camera_pos, (camera_pos + camera_front), camera_up);
}

void ZSPIRE::Camera::setPosition(ZSVECTOR3 pos){
    this->camera_pos = pos;
    updateViewMat();
}

void ZSPIRE::Camera::setFront(ZSVECTOR3 front){
    this->camera_front = front;
    updateViewMat();
}
            
void ZSPIRE::Camera::setUp(ZSVECTOR3 up){
    this->camera_up = up;
    updateViewMat();
}

ZSMATRIX4x4 ZSPIRE::Camera::getViewMatrix(){
    return this->view;
}
ZSMATRIX4x4 ZSPIRE::Camera::getProjMatrix(){
    return this->proj;
}

ZSVIEWPORT ZSPIRE::Camera::getViewport(){
    return viewport;
}

ZSVECTOR3 ZSPIRE::Camera::getCameraPosition() {
	return this->camera_pos;
}

ZSVECTOR3 ZSPIRE::Camera::getCameraFrontVec(){
    return this->camera_front;
}

ZSPIRE::Camera::Camera(){

    camera_pos = ZSVECTOR3(0,0,0);
    camera_front = ZSVECTOR3(1,0,0);
    camera_up = ZSVECTOR3(0,1,0);

    proj_type = ZSCAMERA_PROJECTION_PERSPECTIVE;
    nearZ = 0.1f;
    farZ = 100.0f;
    FOV = 45;
    viewport = ZSVIEWPORT(0,0, 640, 480);

    updateProjectionMat();
    updateViewMat();

    isMoving = false;
}

void ZSPIRE::Camera::updateTick(){
    if(!isMoving) return;


    if(getDistance(camera_pos, _dest_pos) < 3){
        camera_pos = _dest_pos;
        updateViewMat();
        isMoving = false;
    }else{
        ZSVECTOR3 delta = camera_pos - _dest_pos;
        float dist = getDistance(camera_pos, _dest_pos);

        ZSVECTOR3 toMove = delta / dist;
        toMove = toMove * 6.0f;
        camera_pos = camera_pos - toMove;
        updateViewMat();
    }

}

void ZSPIRE::Camera::startMoving(){
    if(proj_type == ZSCAMERA_PROJECTION_ORTHOGONAL){
        int viewport_size_x = static_cast<int>(viewport.endX) - viewport.startX;
        int viewport_size_y = static_cast<int>(viewport.endY) - viewport.startY;
        viewport_size_y *= -1;
        ZSVECTOR3 to_add = ZSVECTOR3(viewport_size_x / 2, viewport_size_y / 2, 0);
        _dest_pos = _dest_pos + to_add;
    }

    this->isMoving = true;
}
