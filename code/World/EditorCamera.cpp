#include "headers/EditorCamera.hpp"

using namespace Editor;

void EditorCamera::startMoving() {
    if (mProjectionType == ZSCAMERA_PROJECTION_ORTHOGONAL) {
        int viewport_size_x = static_cast<int>(mViewport.endX) - static_cast<int>(mViewport.startX);
        int viewport_size_y = static_cast<int>(mViewport.endY) - static_cast<int>(mViewport.startY);
        viewport_size_y *= -1;
        Vec3 to_add = Vec3(static_cast<float>(viewport_size_x) / 2.f, static_cast<float>(viewport_size_y) / 2.f, 0);
        _dest_pos = _dest_pos + to_add;
    }

    this->isMoving = true;
}

void EditorCamera::stopMoving() {
    this->isMoving = false;
}

void EditorCamera::updateTick(float deltaTime) {
    if (!isMoving) return;

    Vec3 real_dest = (_dest_pos - (5) * mCameraFront);

    if (getDistance(mCameraPos, real_dest) < 2) {
        isMoving = false;
    }
    else {
        Vec3 delta = real_dest - mCameraPos;
        float dist = getDistance(mCameraPos, real_dest);

        Vec3 toMove = delta / dist;
        toMove = toMove * (deltaTime) * 80;
        mCameraPos += toMove;
        updateViewMat();
    }
}
