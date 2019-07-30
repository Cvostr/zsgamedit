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
        float aspect = static_cast<float>((viewport.endX - viewport.startX)) / static_cast<float>(viewport.endY - viewport.startY);
        proj = getPerspective(FOV, aspect, nearZ, farZ);
    }else{
        proj = getOrthogonal(0, static_cast<float>(viewport.endX - viewport.startX), 0, static_cast<float>(viewport.endY - viewport.startY), nearZ, farZ);
    }
    ui_proj = getOrthogonal(0, static_cast<float>(viewport.endX - viewport.startX), 0, static_cast<float>(viewport.endY - viewport.startY));
}

void ZSPIRE::Camera::updateViewMat(){
    if(isAlListenerCamera){
        ZSPIRE::SFX::setListenerPos(camera_pos);
        ZSPIRE::SFX::setListenerOri(camera_front, camera_up);
    }
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
ZSMATRIX4x4 ZSPIRE::Camera::getUiProjMatrix(){
    return this->ui_proj;
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

ZSVECTOR3 ZSPIRE::Camera::getCameraRightVec(){
    return vCross(camera_front, camera_up);
}

ZSVECTOR3 ZSPIRE::Camera::getCameraUpVec(){
    return camera_up;
}

ZSVECTOR3 ZSPIRE::Camera::getCameraViewCenterPos(){
    if(this->proj_type == ZSCAMERA_PROJECTION_ORTHOGONAL){

        int viewport_y = static_cast<int>(viewport.endY - viewport.startY) / 2;
        int viewport_x = (viewport.endX - viewport.startX) / 2;
        viewport_x *= -1;
        ZSVECTOR3 result = camera_pos + ZSVECTOR3(viewport_x, (viewport_y), 0);
        return result;
    }
    return this->camera_pos;
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
    isAlListenerCamera = false;
}

void ZSPIRE::Camera::updateTick(float deltaTime){
    if(!isMoving) return;


    if(getDistance(camera_pos, _dest_pos) < 6){
        camera_pos = _dest_pos;
        updateViewMat();
        isMoving = false;
    }else{
        ZSVECTOR3 delta = camera_pos - _dest_pos;
        float dist = getDistance(camera_pos, _dest_pos);

        ZSVECTOR3 toMove = delta / dist;
        toMove = toMove * (deltaTime) - (10 / dist);
        camera_pos = camera_pos - toMove;
        updateViewMat();
    }

}

void ZSPIRE::Camera::startMoving(){
    if(proj_type == ZSCAMERA_PROJECTION_ORTHOGONAL){
        int viewport_size_x = static_cast<int>(viewport.endX) - static_cast<int>(viewport.startX);
        int viewport_size_y = static_cast<int>(viewport.endY) - static_cast<int>(viewport.startY);
        viewport_size_y *= -1;
        ZSVECTOR3 to_add = ZSVECTOR3(viewport_size_x / 2, viewport_size_y / 2, 0);
        _dest_pos = _dest_pos + to_add;
    }

    this->isMoving = true;
}

void ZSPIRE::Camera::stopMoving(){
    this->isMoving = false;
}
