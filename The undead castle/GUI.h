#pragma once
#include "vec3.h"
#include "Texture.h"
#include <string>
using namespace std;
namespace osomi {
	class GUIHandler;
	class GUI {
		friend class GUIHandler;
	private:
		string name;
		vec2 scale, offset;
		vec2 uvScale, uvOffset;
		Texture *t;
		void(*onClick)(float, float);
	public:
		GUI(string _name, void(*_onClick)(float x, float y), Texture *_t, vec2 _scale = vec2(1, 1), vec2 _offset = vec2(0, 0), vec2 _uvScale = vec2(1, 1), vec2 _uvOffset = vec2(0, 0)) : name(_name), scale(_scale), offset(_offset), uvScale(_uvScale), uvOffset(_uvOffset), t(_t), onClick(_onClick) {
			offset.y -= 1 - scale.y;
		}
	protected:
		vec2 &getUvScale() { return uvScale; }
		vec2 &getUvOffset() { return uvOffset; }
		vec2 getScale() { return scale; }
		vec2 getPos() { return offset; }
		string getName() { return name; }
		Texture *getTexture() { return t; }
		void click(float x, float y, bool &activated) {
			vec2 s = -scale + offset;
			if (s == vec2(-0.f, -0.f))s = vec2::zero();
			vec2 e = scale + offset;
			vec2 pos = vec2((x - s.x) / (e.x - s.x), (y - s.y) / (e.y - s.y));
			activated = pos.x >= 0 && pos.y >= 0 && pos.x < 1 && pos.y < 1;
			if (activated) 
				(*onClick)(pos.x, pos.y);
		}
	};
}