#pragma once
#pragma once
#include <GL/glew.h>
#include "Texture.h"
#include "TileRenderer.h"
#include <vector>
using namespace std;
namespace osomi {
	class TileMap;
	class TileSet {
		friend class TileMap;
	public:
		class TileEntry {
			friend class TileSet;
		private:
			unsigned int id;
			Texture *t;
			TileRenderer *r;
			vec3 scale;
		protected:
			TileEntry(unsigned int i, Texture *_t, TileRenderer *_r, vec3 sc=vec3(0,0,0)) : id(i), t(_t), r(_r), scale(sc){}
			bool hasTexture() {
				return t != nullptr;
			}
			bool null() {
				return r == nullptr && !hasTexture();
			}
			void render(unsigned int sides) {
				if (r == nullptr)return;
				r->render(sides, t);
			}
			vec3 getScale() { return scale; }
		};
	private:
		vector<TileEntry> tiles;
		vector<TileEntry> items;
	public:
		TileSet() {}
		void assign(unsigned int i, Texture *t, TileRenderer *r, vec3 sc=vec3(1,1,1)) {
			if (i & 0x80000000) {
				if ((i & 0x7FFFFFFF) < items.size()) {
					if (items[i & 0x7FFFFFFF].null())
						items[i & 0x7FFFFFFF] = TileEntry(items.size(), t, r, sc);
					else
						throw exception("Tried to assign an already assigned tile entry!\n");
				}
				else {
					while (items.size() < (i & 0x7FFFFFFF))items.push_back(TileEntry(items.size(), nullptr, nullptr));
					items.push_back(TileEntry(items.size(), t, r, sc));
				}
			}
			else {
				if (i > tiles.size()) {
					if (tiles[i].null())
						tiles[i] = TileEntry(tiles.size(), t, r, sc);
					else
						throw exception("Tried to assign an already assigned tile entry!\n");
				}
				else {
					while (tiles.size() < i)tiles.push_back(TileEntry(tiles.size(), nullptr, nullptr));
					tiles.push_back(TileEntry(tiles.size(), t, r, sc));
				}
			}
		}
		void render(unsigned int id, unsigned int sides) {
			if (id & 0x80000000) {
				if ((id & 0x7FFFFFFF) >= items.size())return;
				items[id & 0x7FFFFFFF].render(sides);
			}
			else {
				if (id >= tiles.size())return;
				tiles[id].render(sides);
			}
		}
		vec3 getScale(unsigned int id) {
			if (id & 0x80000000) {
				if ((id & 0x7FFFFFFF) >= items.size())return vec3(1, 1, 1);
				return items[id & 0x7FFFFFFF].getScale();
			}
			else return vec3(1, 1, 1);
		}
		unsigned int size(bool isBlock) {
			return isBlock ? tiles.size() : items.size();
		}
	protected:
		Model *getModel(unsigned int id) {
			if (id & 0x80000000) {
				if (id & 0x7FFFFFFF >= items.size())return nullptr;
				return items[id & 0x7FFFFFFF].r->m;
			}
			else {
				if (id >= tiles.size())return nullptr;
				return tiles[id].r->m;
			}
		}
	};
}