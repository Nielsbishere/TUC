#pragma once
#include "mat4.h"
#include "Model.h"
#include "Math.h"
namespace osomi {
	class TileMap;
	class Raycast {
		static bool intersect(vec3 min, vec3 max, vec3 s, vec3 d){
			float t1 = (min.x - s.x)/d.x;
			float t2 = (max.x - s.x)/d.x;
			float t3 = (min.y - s.y)/d.y;
			float t4 = (max.y - s.y)/d.y;
			float t5 = (min.z - s.z)/d.z;
			float t6 = (max.z - s.z)/d.z;
			float tmin = Math::max(Math::max(Math::min(t1, t2), Math::min(t3, t4)), Math::min(t5, t6));
			float tmax = Math::min(Math::min(Math::max(t1, t2), Math::max(t3, t4)), Math::max(t5, t6));

			float t;
			if (tmax < 0){
				t = tmax;
				return false;
			}

			if (tmin > tmax){
				t = tmax;
				return false;
			}

			t = tmin;
			return true;
		}
	public:
		static void raycast(vec3 s, vec3 dir, TileMap *tm, vector<GameObject*> &obj, bool &hitsTilemap, unsigned int &_i, unsigned int &_j, unsigned int &_k, GameObject *&go, bool useFullWorld=false);
	};
}