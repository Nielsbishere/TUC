#pragma once
#include "Entity.h"
namespace osomi {
	class Player : public Entity {
	public:
		Player(string str) : Entity("Player_" + str, 100, 20, 5, 0) {}
		void move(vec3 v) {
			GameObject::move(v);
		}
	};
}