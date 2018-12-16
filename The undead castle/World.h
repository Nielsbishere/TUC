#pragma once
#include "Shader.h"
#include "TileMap.h"
#include "Raycast.h"
namespace osomi {
	class State;
	class World {
		friend class State;
	private:
		TileMap *tm;
	public:
		World(unsigned int w, unsigned int h, unsigned int l, unsigned int type, Camera *c, Shader *s, TileSet *ts) {
			tm = TileMap::create(w, h, l, type, c, s, ts);
		}
		~World() {
			delete tm;
		}
		void render() {
			tm->render();
		}
		void set(unsigned int i, unsigned int j, unsigned int k, unsigned int b) {
			if (b & 0x80000000)return;
			tm->set(i, j, k, b);
		}
		void init() {
			tm->generateNodeMap();
		}
		void refresh() {
			tm->refreshNodeMap();
		}
		vector<TileMap::Point> astar(vec2 a, vec2 b) {
			return tm->Astar2D(a, b);
		}
		vector<TileMap::Point> astar(vec3 ppos, vec2 b) {
			ppos = tm->toMapCoords(ppos);
			return tm->Astar2D(vec2(ppos.x, ppos.y), b);
		}
		vector<TileMap::Point> astar(vec3 ppos, vec3 pos2) {
			ppos = tm->toMapCoords(ppos);
			pos2 = tm->toMapCoords(pos2);
			return tm->Astar2D(vec2(ppos.x, ppos.y), vec2(pos2.x, pos2.y));
		}
		void getVisible(vec3 &mi, vec3 &ma) {
			tm->getVisible(mi, ma);
		}
		unsigned int get(unsigned int i, unsigned int j, unsigned int k) {
			return tm->get(i, j, k);
		}
		unsigned int getWidth() { return tm->getWidth(); }
		unsigned int getHeight() { return tm->getHeight(); }
	};
}