#pragma once
#include "mat4.h"
#include "vec2.h"
#include "GameObject.h"

#ifndef M_PI
#define M_PI 3.141592f
#endif

namespace osomi {
	class Camera {
	private:
		mat4 view, projection;
		bool shouldReloadView, shouldReloadProjection;
		vec3 eye, center, up;
		float fov, zoom;
		unsigned int type;
		GameObject *following;
		Camera(vec3 _eye, float _fov = 0, unsigned int _type=0, float _zoom=1, vec3 _center = vec3(0, 0, 0)) {
			up = vec3(0, 1, 0);
			eye = _eye;
			center = _center;
			view = mat4::lookat(eye, center, up);
			projection = mat4();
			shouldReloadProjection = true;
			fov = _fov;
			type = _type;
			zoom = 1 / _zoom;
			following = nullptr;
		}
		Camera(float fov, unsigned type) : Camera(vec3(0.f, type == 1 ? 7.5f : 0, type != 1 ? 7.5f : 0.f), fov, type, 2.f) {}
	public:
		Camera(unsigned int type) : Camera(45.f, type) {}
		void updateRender(int w, int h) {
			if (shouldReloadProjection) {
				projection = mat4::perspective(fov, (float)w / h, 0.1f, 100.f);
				shouldReloadProjection = false;
			}
			if (shouldReloadView) {
				view = mat4::lookat(eye, center, up);
				shouldReloadView = false;
			}
		}
		void attachGameObject(GameObject *go) {
			following = go;
		}
		void resize(int w, int h) {
			shouldReloadProjection = true;
		}
		void setPosition(vec2 v) {
			if (type >= 1) {
				eye.x = center.x = v.x;
				eye.z = center.z = v.y;
				center.z -= .1f;
				center.y = 0;
				eye.y = 7.5f * zoom;
			}
			else {
				eye.x = center.x = v.x;
				eye.y = center.y = v.y;
				center.z = 0;
				eye.z = 7.5f * zoom;
			}
			shouldReloadView = true;
		}
		vec2 getPosition() {
			return vec2(eye.x, type >= 1 ? eye.z : eye.y);
		}
		void move(vec2 v) {
			if (type == 1)v.y = -v.y;
			setPosition(getPosition() + v);
		}
		void move(vec3 v) {
			if (type == 0) move(vec2(v.x, v.y));
			else if (type == 1) move(vec2(v.x, v.z));
			else if (type == 2) {
				eye.x = center.x = center.x + v.x;
				eye.y = center.y = center.y + v.y;
				center.z -= v.z;
				eye.z -= v.z;
				shouldReloadView = true;
			}
		}
		void update(float delta) {
			if (following == nullptr)return;
			vec3 d = (following->getPos() - eye) / 0.5f * delta;
			d.z *= -1;
			if (type == 1)d.y = 0;
			move(-d);
		}
		vec3 getEye() { return eye; }
		vec3 getDirection() { return (center - eye).normalize(); }
		mat4 getView() { return view; }
		mat4 getProjection() { return projection; }
		float getFov() { return fov; }
		float getMaxZ() {
			return type == 1?eye.y:eye.z;
		}
		void changeZoom(float z) {
			zoom += z;
		}
		void setEye(vec3 eye) {
			if (eye == center)return;
			this->eye = eye;
		}
	};
}