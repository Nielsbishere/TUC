#pragma once
#include "Model.h"
namespace osomi {
	class State;
	class TileSet;
	class Renderer {
		friend class State;
		friend class TileSet;
	protected:
		Model *m;
		virtual void render(unsigned int sides, Texture *rep) {
			if (m == nullptr)return;
			m->render((Model::EModelSide)sides, rep);
		}
		virtual void render(unsigned int start, unsigned int end) {
			if (m == nullptr)return;
			m->render("", start, end);
		}
	public:
		Renderer(Model *_m=nullptr) : m(_m) {}
		virtual ~Renderer() {}
		Model *get() { return m; }
	};
}