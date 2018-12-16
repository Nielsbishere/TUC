#pragma once
#include "Texture.h"
namespace osomi {
	class ModelLoader;
	class MaterialLoader;
	class Material {
		friend class ModelLoader;
		friend class MaterialLoader;
	private:
		Texture *t;
		string name;
	protected:
		Material(string _name, Texture *_t) : t(_t), name(_name) {}
		void setTexture(Texture *_t) {
			if (t != nullptr)return;
			t = _t;
		}
	public:
		Material() : t(nullptr), name("") {}
		bool null() { return t == nullptr && name == ""; }
		string getName() { return name; }
		Texture *getTexture() { return t; }
	};
}