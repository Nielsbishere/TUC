#pragma once
#include "TileMap.h"
#include "GameObject.h"
namespace osomi {
	class Entity : public GameObject {
	protected:
		float health, maxHealth;
		float damage, protection;
		vector<TileMap::Point> pts;
		float timer, maxTime, maxDistance;
		Entity(string name, float danger, float mh, float dmg, float prot) : GameObject("e_" + name, danger), health(mh), maxHealth(mh), damage(dmg), protection(prot) {}
	public:
		void followPathT(vector<TileMap::Point> pt, float w, float h, float T) {
			pts = vector<TileMap::Point>(pt.size());
			for (unsigned int i = 0; i < pt.size(); i++) {
				pts[pts.size() -1 - i].x = pt[i].x * 2 - w / 2 * 2;
				pts[pts.size() -1 - i].z = pt[i].z * 2 - h / 2 * 2;
			}
			timer = 0;
			maxTime = T;
			maxDistance = 0;
			for (unsigned int i = 1; i < pts.size(); i++) 
				maxDistance += abs((pts[i].toVector() + -pts[i-1].toVector()).magnitude());
		}
		void followPath(vector<TileMap::Point> pt, float w, float h, float unitPerSecond) {
			pts = vector<TileMap::Point>(pt.size());
			for (unsigned int i = 0; i < pt.size(); i++) {
				pts[pts.size() -1 - i].x = pt[i].x * 2 - w / 2 * 2;
				pts[pts.size() -1 - i].z = pt[i].z * 2 - h / 2 * 2;
			}
			timer = 0;
			maxDistance = 0;
			for (unsigned int i = 1; i < pts.size(); i++)
				maxDistance += abs((pts[i].toVector() + -pts[i - 1].toVector()).magnitude());
			maxTime = maxDistance / unitPerSecond;
		}
		virtual void update(float delta) {
			GameObject::update(delta);
			if (pts.size() > 1) {
				float distance = Math::min(maxDistance / maxTime * timer, maxDistance);
				if (distance == maxDistance){
					setPos(vec2(pts[pts.size()-1].x, pts[pts.size()-1].z));
					pts.clear();
				}
				else {
					float temp = 0, prev = 0, dis = 0;
					unsigned int sI = 0;
					for (unsigned int i = 1; i < pts.size(); i++) {
						prev = temp;
						vec2 vec = (pts[i - 1].toVector() + -pts[i].toVector());
						dis = abs(vec.magnitude());
						temp += dis;
						if (temp > distance) {
							sI = i - 1;
							break;
						}
					}
					float del = distance - prev;
					float x = pts[sI].x + (pts[sI + 1].x - pts[sI].x) / dis * del;
					float z = pts[sI].z + (pts[sI + 1].z - pts[sI].z) / dis * del;
					setPos(vec2(x, z));
					vec2 d = (pts[sI].toVector() + -pts[sI+1].toVector());
					if (d.x == 0 && d.y < 0)
						setRotation(vec3(0, 0, 0));
					if (d.x == 0 && d.y > 0)
						setRotation(vec3(0, M_PI, 0));
					if (d.x > 0 && d.y == 0)
						setRotation(vec3(0, 1.5*M_PI, 0));
					if (d.x < 0 && d.y == 0)
						setRotation(vec3(0, 0.5*M_PI, 0));
					if(d.x < 0 && d.y < 0)
						setRotation(vec3(0, 0.25*M_PI, 0));
					if (d.x < 0 && d.y > 0)
						setRotation(vec3(0, 0.75*M_PI, 0));
					if (d.x > 0 && d.y > 0)
						setRotation(vec3(0, 1.25*M_PI, 0));
					if (d.x > 0 && d.y < 0)
						setRotation(vec3(0, 1.75*M_PI, 0));
					//.
				}

				timer += delta;
			}
		}
		virtual float getDangerValue() {
			float initial = GameObject::getDangerValue();
			return health / maxHealth * damage * (1 + protection / 100) * initial;
		}
		float getHealth() { return health; }
		float getMaxHealth() { return maxHealth; }
		float getDamage() { return damage; }
		float getProtection() { return protection; }
		bool hasPath() { return pts.size() != 0; }
	};
}