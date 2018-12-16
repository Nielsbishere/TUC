#pragma once
#include "GameObject.h"
namespace osomi {
	class Damagable : public GameObject{
	private:
		float health, maxHealth;
	public:
		Damagable(string name, float danger, float _maxHealth) : GameObject(name, danger), maxHealth(_maxHealth), health(_maxHealth) {}
		void damage(float damage) {
			if (!getObject()->shaking()) {
				getObject()->shake(5, 2, 5);
				health -= damage;
				if (health <= 0) {
					health = 0;
					die();
				}
			}
		}
	};
}