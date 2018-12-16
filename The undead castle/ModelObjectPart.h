#pragma once
#include "Texture.h"
#include "Material.h"
namespace osomi {
	class ModelObjectPart {
	private:
		Material mat;
		unsigned int start, end;
	public:
		ModelObjectPart(unsigned int s, unsigned int e, Material m): start(s), end(e), mat(m){}
		unsigned int getStart() { return start; }
		unsigned int getEnd() { return end; }
		Texture *getTexture() { return mat.getTexture(); }
	};
}