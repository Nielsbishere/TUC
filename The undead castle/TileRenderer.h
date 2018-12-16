#pragma once
#include "Renderer.h"
namespace osomi {
	class TileRenderer : public Renderer {
	public:
		TileRenderer(Model *_m = nullptr) : Renderer(_m) {}
	};
}