#pragma once
#include "Object.h"
namespace osomi {
	class State;
	class GameObject {
		friend class State;
	private:
		Object *representation;
		vec3 motion;
		string name;
		float danger;
		bool isDead;
	protected:
		GameObject(string n, float dangerVal) : representation(nullptr), motion(vec3()), name(n), danger(dangerVal), isDead(false) { }
		void setPos(vec2 v) {
			representation->move(vec3(v.x, getPos().y, v.y));
		}
		void rotate(vec3 r) {
			representation->rotate(r);
		}
		void move(vec3 v) {
			representation->translate(vec3(v.x, v.y, v.z));
		}
	public:
		virtual void update(float delta) {
			if (representation == nullptr)return;

			vec3 d = motion * delta;

			//Collision checks

			representation->translate(motion * delta);
		}
		vec3 getPos() {
			if (representation == nullptr)return vec3(0, 0, 0);
			return representation->getPos();
		}
		vec3 getRotation() {
			return representation->getRotation();
		}
		void setMotion(vec3 m) {
			motion = m;
		}
		void setRotation(vec3 r) {
			representation->setRotation(r);
		}
		void addMotion(vec3 mot) {
			motion += mot;
		}
		string getName() { return name; }
		virtual float getDangerValue() {
			return danger;
		}
		mat4 getMat() { return representation->getModel(); }
		Model *get() { return representation->getRender(); }
		bool isAlive() { return !isDead; }
		Object *getObject() { return representation; }
	protected:
		void attachObject(Object *rep) {
			if (representation != nullptr)return;
			representation = rep;
		}
		void die() {
			isDead = true;
		}
	};
}