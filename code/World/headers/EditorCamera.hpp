#pragma once

#include <world/Camera.hpp>

namespace Editor {
	class EditorCamera : public Engine::Camera {
	public:
		Vec3 _dest_pos;
		bool isMoving;

		void updateTick(float deltaTime);

		void startMoving();
		void stopMoving();

		EditorCamera() {
			Camera();
			isMoving = false;
		}
	};
}