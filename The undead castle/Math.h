#pragma once
#include "vec3.h"
#include "mat4.h"
namespace osomi {
	class Model;
	class Math {
	private:
		static bool intersect(float a0, float a1, float b0, float b1) {
			return a0 < b1 && a1 > b0;
		}
	public:
		static float clamp(float f, float min, float max) {
			if (f < min)f = min;
			if (f > max)f = max;
			return f;
		}
		static float min(float a, float b) {
			if (a < b)return a;
			return b;
		}
		static float max(float a, float b) {
			if (a > b)return a;
			return b;
		}
		static bool doesIntersect(vec3 mi0, vec3 ma0, vec3 mi1, vec3 ma1) {
			return intersect(mi0.x, ma0.x, mi1.x, ma1.x) && intersect(mi0.y, ma0.y, mi1.y, ma1.y) && intersect(mi0.z, ma0.z, mi1.z, ma1.z);
		}
		static bool doesIntersect2D(vec3 mi0, vec3 ma0, vec3 mi1, vec3 ma1) {
			return intersect(mi0.x, ma0.x, mi1.x, ma1.x) && intersect(mi0.z, ma0.z, mi1.z, ma1.z);
		}
		static void alignAxes(mat4 m, Model *model, vec3 &min, vec3 &max);
	};
}