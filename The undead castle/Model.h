#pragma once
#include "StringUtils.h"
#include "ModelObject.h"
#include "Math.h"
#include "mat4.h"
#include <vector>
using namespace std;
namespace osomi {
	class Renderer;
	class ModelLoader;
	class Raycast;
	class Model {
		friend class Renderer;
		friend class ModelLoader;
		friend class Raycast;
	private:
		static vector<Model*> models;
		vector<ModelObject> objects;
		string name;
		Model(vector<ModelObject> obj, string _name, float minX, float minY, float minZ, float maxX, float maxY, float maxZ) : objects(obj), name(_name), miX(minX), miY(minY), miZ(minZ), maX(maxX), maY(maxY), maZ(maxZ){}
		~Model() {
			for (unsigned int i = 0; i < objects.size(); i++)
				objects[i].destroy();
			objects.clear();
		}
	public:
		enum EModelSide {
			FRONT = 1 << 0, BACK = 1 << 1, RIGHT = 1 << 2, LEFT = 1 << 3, TOP = 1 << 4, BOTTOM = 1 << 5,
			NONE = 0,
			ALL = ~NONE,
			ANY = ALL ^ (1 << 6)
		};
		vec4 getMin() {
			return vec4(miX, miY, miZ, 1);
		}
		vec4 getMax() {
			return vec4(maX, maY, maZ, 1);
		}
		bool collides(vec3 mi, vec3 ma, mat4 mmat) {
			for (unsigned int i = 0; i < objects.size(); i++) {
				vec4 wmi = mmat * objects[i].getMin();
				vec4 wma = mmat * objects[i].getMax();
				vec3 wmi3 = vec3(wmi.x, wmi.y, wmi.z);
				vec3 wma3 = vec3(wma.x, wma.y, wma.z);
				if (!Math::doesIntersect(mi, ma, wmi3, wma3))continue;
				//Maybe per triangle check; but it is really heavy!
				return true;
			}
			return false;
		}
	protected:
		float miX, miY, miZ, maX, maY, maZ;
		static Model *get(string file) {
			for (unsigned int i = 0; i < models.size(); i++)
				if (models[i]->name == file)
					return models[i];
			return nullptr;
		}
		void render(EModelSide sides, Texture *replacement=nullptr) {
			bool o = sides == ANY;
			for (unsigned int i = 0; i < objects.size(); i++) {
				if (StringUtils::startsWith(objects[i].getName(), "Front_")) {
					if ((sides & TOP) == 0 || o)objects[i].render(replacement);
				}
				else if (StringUtils::startsWith(objects[i].getName(), "Back_")) {
						if ((sides & BOTTOM) == 0 || o)objects[i].render(replacement);
				}
				else if (StringUtils::startsWith(objects[i].getName(), "Right_")) {
					if ((sides & RIGHT) == 0 || o)objects[i].render(replacement);
				}
				else if (StringUtils::startsWith(objects[i].getName(), "Left_")) {
					if ((sides & LEFT) == 0 || o)objects[i].render(replacement);
				}
				else if (StringUtils::startsWith(objects[i].getName(), "Top_")) {
					if ((sides & FRONT) == 0 || o)objects[i].render(replacement);
				}
				else if (StringUtils::startsWith(objects[i].getName(), "Bottom_")) {
					if ((sides & BACK) == 0 || o)objects[i].render(replacement);
				}
				
				else if (StringUtils::startsWith(objects[i].getName(), "!Front_")) {
					if ((sides & TOP) != 0 || o)objects[i].render(replacement);
				}
				else if (StringUtils::startsWith(objects[i].getName(), "!Back_")) {
					if ((sides & BOTTOM) != 0 || o)objects[i].render(replacement);
				}
				else if (StringUtils::startsWith(objects[i].getName(), "!Right_")) {
					if ((sides & RIGHT) != 0 || o)objects[i].render(replacement);
				}
				else if (StringUtils::startsWith(objects[i].getName(), "!Left_")) {
					if ((sides & LEFT) != 0 || o)objects[i].render(replacement);
				}
				else if (StringUtils::startsWith(objects[i].getName(), "!Top_")) {
					if ((sides & FRONT) != 0 || o)objects[i].render(replacement);
				}
				else if (StringUtils::startsWith(objects[i].getName(), "!Bottom_")) {
					if ((sides & BACK) != 0 || o)objects[i].render(replacement);
				}
				else objects[i].render(replacement);
			}
		}
		void render(string name, unsigned int startIndex, unsigned int endIndex) {
			for (unsigned int i = 0; i < objects.size(); i++)
				if (objects[i].getName() == name || name == "")
					objects[i].render(startIndex, endIndex);
		}
		static Model *load(vector<ModelObject> obj, string name, float aabbMinX, float aabbMinY, float aabbMinZ, float aabbMaxX, float aabbMaxY, float aabbMaxZ) {
			Model *m = new Model(obj, name, aabbMinX, aabbMinY, aabbMinZ, aabbMaxX, aabbMaxY, aabbMaxZ);
			models.push_back(m);
			return m;
		}
		static void destroyModels() {
			for (unsigned int i = 0; i < models.size(); i++) 
				delete models[i];
			models.clear();
		}
	};
	void Math::alignAxes(mat4 m, Model *model, vec3 &min, vec3 &max) {
		float minf = numeric_limits<float>::min();
		float maxf = numeric_limits<float>::max();
		min = vec3(maxf, maxf, maxf);
		max = vec3(minf, minf, minf);
		vec4 mmin = model->getMin();
		vec4 mmax = model->getMax();
		float xdist = mmax.x - mmin.x;
		float ydist = mmax.y - mmin.y;
		float zdist = mmax.z - mmin.z;
		for (unsigned int i = 0; i <= 1; i++)
			for (unsigned int j = 0; j <= 1; j++)
				for (unsigned int k = 0; k <= 1; k++) {
					vec4 v = m * vec4(mmin.x + xdist * i, mmin.y + ydist * j, mmin.z + zdist * k, 1);
					if (v.x < min.x)min.x = v.x;
					if (v.x > max.x)max.x = v.x;
					if (v.y < min.y)min.y = v.y;
					if (v.y > max.y)max.y = v.y;
					if (v.z < min.z)min.z = v.z;
					if (v.z > max.z)max.z = v.z;
				}
		mmin = m * mmin;
		mmax = m * mmax;
		min = vec3(mmin.x, mmin.y, mmin.z);
		max = vec3(mmax.x, mmax.y, mmax.z);
	}
}