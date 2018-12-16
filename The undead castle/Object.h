#pragma once
#include "mat4.h"
#include "Renderer.h"

#define M_PI 3.141592f

namespace osomi {
	class State;
	class GameObject;
	class Object {
		friend class State;
	private:
		vec3 pos, size, rotation;
		float isShaking, shakeValue, shakeTimes, shakeSpeed;
		GameObject *linked;
		bool ghost;
	protected:
		Renderer render;
	public:
		mat4 getModel() {
			return mat4::translate(pos) * mat4::rotate(rotation) * mat4::scale(size);
		}
		Model *getRender() {
			return render.get();
		}
		Object(Renderer r, vec3 _pos, bool _ghost=false) {
			pos = _pos;
			render = r;
			size = vec3(1, 1, 1.f);
			rotation = vec3::zero();
			isShaking = 0;
			linked = nullptr;
			ghost = _ghost;
		}
		Object() {
			render = Renderer();
		}
		vec3 getPos() { return pos; }
		vec3 getRotation() { return rotation; }
		void translate(vec3 p) { pos += p; }
		void move(vec3 p) { pos = p; }
		void scale(vec3 s) { size *= s; }
		void setSize(vec3 s) { size = s; }
		void rotate(vec3 v) { rotation += v; }
		void setRotation(vec3 v) { rotation = v; }
		void shake(float val, float times, float speed) {
			if (isShaking == 0) {
				isShaking = 1;
				shakeValue = val;
				shakeTimes = times;
				shakeSpeed = speed;
			}
		}
		void link(GameObject *go) {
			if (linked != nullptr)return;
			linked = go;
		}
		GameObject *getAttached() { return linked; }
		void update(float delta) {
			if (isShaking >= 1) {
				isShaking += delta;
				rotation = vec3(0, shakeValue / 180.0 * M_PI * sin((isShaking - 1) * shakeSpeed * M_PI * 2), 0);
				if ((isShaking - 1) * shakeSpeed >= shakeTimes) {
					rotation = vec3(0, 0, 0);
					isShaking = 0;
				}
			}
		}
		bool shaking() {
			return isShaking != 0;
		}
		bool isGhost() { return ghost; }
	};
}