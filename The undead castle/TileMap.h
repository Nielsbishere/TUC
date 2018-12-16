#pragma once
#include "Model.h"
#include "Camera.h"
#include "Shader.h"
#include "BinaryHelper.h"
#include "ArrayHelper.h"
#include "TileSet.h"
#include "Node.h"
#include "Heap.h"
#include "Raycast.h"

#include <ctime>

using namespace std;
namespace osomi {
	class TileMap {
	public:
		struct Point {
			float x, z;
			Point(float _x, float _z) : x(_x), z(_z) {}
			Point() : Point(0, 0) {}
			vec2 toVector() { return vec2(x, z); }
		};
	private:
		unsigned int w, h, l, *map;
		unsigned int type;
		bool topDown;
		Camera *c;
		Shader *s;
		TileSet *ts;
		inline bool solid(int i, int j, int z) {
			if (z >= l)return false;
			if (z < 0)return true;
			if (i < 0 || i >= w || j < 0 || j >= h)return false;
			return map[z*h*w + j*w + i] != 0;
		}
		unsigned int getSides(unsigned int i, unsigned int j, unsigned int k) {
			unsigned int side = Model::NONE;
			if (!solid((int)i - 1, j, k))side |= Model::LEFT;
			if (!solid(i + 1, j, k))side |= Model::RIGHT;
			if (!solid(i, (int)j - 1, k))side |= Model::BOTTOM;
			if (!solid(i, j + 1, k))side |= Model::TOP;
			if (!solid(i, j, (int)k - 1))side |= Model::BACK;
			if (!solid(i, j, k + 1))side |= Model::FRONT;
			return side;
		}
		TileMap(unsigned int _w, unsigned int _h, unsigned int _l, unsigned int _type, Shader *_s, Camera *_c, TileSet *_ts) : c(_c), s(_s), w(_w), h(_h), l(_l), ts(_ts), type(_type) {
			map = new unsigned int[w*h*l];
			for (unsigned int i = 0; i < w*h*l; i++)map[i] = i < w * h ? 1 : 0;
			nodes = nullptr;
		}
		vec3 toWorldCoords(vec3 tCoords) {
			float xCoord = tCoords.x * 2 - w / 2 * 2;
			float yCoord = tCoords.y * 2 - h / 2 * 2;
			float zCoord = tCoords.z * 2 - 2;
			return vec3(xCoord, type >= 1 ? zCoord : yCoord, type >= 1 ? yCoord : zCoord);
		}
		bool walkable(float bSize, float x, float y) {
			vec3 mmi = vec3(x, y, 0) * bSize;
			vec3 pos = toWorldCoords(mmi);
			bool hitsTilemap;
			unsigned int i = UINT_MAX, j = UINT_MAX, k = UINT_MAX;
			GameObject *go = nullptr;
			Raycast::raycast(vec3(pos.x, pos.y + 20, pos.z), vec3(0, -1, 0), this, vector<GameObject*>(), hitsTilemap, i, j, k, go, true);
			if (go != nullptr || (hitsTilemap && k != 0 && k != UINT_MAX)) 
				return false;
			return true;
		}
		float nsize = 1;
		Node *nodes;
	public:
		~TileMap() {
			delete[] map;
			if (nodes != nullptr) free(nodes);
		}
		unsigned int getWidth() { return w; }
		unsigned int getHeight() { return h; }
		unsigned int getLength() { return l; }
		mat4 getModel(float x, float y, float z) {
			return mat4::translate(toWorldCoords(vec3(x, y, z)));
		}
		vec3 toMapCoords(vec3 wCoords) {
			float xCoord = (wCoords.x + w / 2 * 2) / 2;
			float y = type >= 1 ? wCoords.z : wCoords.y;
			float z = type >= 1 ? wCoords.y : wCoords.z;
			float yCoord = (y + h / 2 * 2) / 2;
			float zCoord = (z + 2) / 2;
			return vec3((int)xCoord, (int)yCoord, (int)zCoord);
		}
		Model *getModel(unsigned int b) {
			return ts->getModel(b);
		}
		void getVisible(unsigned int &sX, unsigned int &sY, unsigned int &sZ, unsigned int &eX, unsigned int &eY, unsigned int &eZ) {
			vec2 cP = c->getPosition();
			sX = Math::max(floor(cP.x / 2) - 5 + w / 2, 0);
			eX = Math::max(floor(cP.x / 2) + 6 + w / 2, 0);
			sY = Math::max(floor(cP.y / 2) - 3 + h / 2, 0);
			eY = Math::max(floor(cP.y / 2) + 4 + h / 2, 0);
			if (type == 2) {
				sY = Math::max((float)sY - 4, 0);
				eY = Math::max((float)eY - 4, 0);
			}
			sZ = 0;
			eZ = c->getMaxZ() / 2;
		}
		void getVisible(vec3 &mi, vec3 &ma) {
			unsigned int x0, y0, z0, x1, y1, z1;
			getVisible(x0, y0, z0, x1, y1, z1);
			mi = toWorldCoords(vec3(x0, y0, 0));
			ma = toWorldCoords(vec3(x1, y1, 0));
		}
		void render() {
			if (s == nullptr || c == nullptr)return;
			unsigned int sX=0, sY=0, sZ=0, eX=0, eY=0, eZ=0;
			getVisible(sX, sY, sZ, eX, eY, eZ);
			for (float x = sX; x <= eX; x++)
				for (float y = sY; y <= eY; y++) {
					for (float z = sZ; z <= eZ; z++) {
						if (x < 0 || y < 0 || x >= w || y >= h || z < 0 || z >= l)continue;
						unsigned int i = (unsigned int)x, j = (unsigned int)y, k = (unsigned int)z;
						unsigned int b = map[k*w*h + j*w + i];
						if (b == 0)continue;
						b -= 1;
						mat4 m = getModel(x, y, z);
						s->setMVP(m, c->getView(), c->getProjection(), c->getEye());
						ts->render(b, getSides(i, j, k));
					}
				}
		}
		void generateNodeMap() {
			if (nodes != nullptr)return;
			unsigned int nw = w / nsize;
			unsigned int nh = h / nsize;
			nodes = new Node[nw*nh];
			for (unsigned int j = 0; j < nh; j++) {
				for (unsigned int i = 0; i < nw; i++) {
					nodes[j * nw + i] = Node(i, j, !walkable(nsize, i, j));
				}
			}
		}
		void refreshNodeMap() {
			if (nodes == nullptr) {
				generateNodeMap();
				return;
			}
			unsigned int nw = w / nsize;
			unsigned int nh = h / nsize;
			for (unsigned int j = 0; j < nh; j++) {
				for (unsigned int i = 0; i < nw; i++) {
					nodes[j * nw + i] = Node(i, j, !walkable(nsize, i, j));
				}
			}
		}
		vector<Point> Astar2D(vec2 a, vec2 b, unsigned int log=1) {
			#ifdef _DEBUG
			log = 3;
			#endif
			float bSize = 1;
			if (a.x > w || b.x > w || a.x < 0 || b.x < 0 || a.y > h || b.y > h || a.y < 0 || b.y < 0) {
				if(log>=1)printf("Couldn't find path from a to b; out of bounds!\n");
				return vector<Point>();
			}
			unsigned int startX = a.x, startY = a.y;
			startX /= bSize;
			startY /= bSize;

			unsigned int endX = b.x, endY = b.y;
			endX /= bSize;
			endY /= bSize;

			unsigned int t = clock();
			vector<Point> path;
			unsigned int nw = w / nsize;
			unsigned int nh = h / nsize;
			if (nodes != nullptr) {
				for (unsigned int j = 0; j < nh; j++) {
					for (unsigned int i = 0; i < nw; i++)
						nodes[j * nw + i] = Node(i, j, nodes[j*nw+i].blocked);
				}
			}
			else {
				nodes = new Node[nw*nh];
				for (unsigned int j = 0; j < nh; j++) {
					for (unsigned int i = 0; i < nw; i++) {
						nodes[j * nw + i] = Node(i, j, !walkable(nsize, i, j));
					}
				}
			}
			printf("Initing map took %ums\n", clock() - t);
			t = clock();


			Node *start = &nodes[startY*w + startX], *end = &nodes[endY*w + endX];
			//TODO: Fix heap, it is faster but not accurate!
			//Heap<Node, &Node::f, &Node::gval> *o = new Heap<Node, &Node::f, &Node::gval>(w * h);
			vector<Node*> o;
			vector<Node*> c;
			//o->add(start);
			o.push_back(start);
			bool found = false;

			//while (o->size() != 0) {
			while (o.size() != 0) {
				//Node *curr = (Node*)o->first();
				Node *curr = o[0];
				unsigned int index = 0;
				for (unsigned int i = 1; i < o.size(); i++) {
					Node *c = o[i];
					if (c->f() < curr->f() || (c->f() == curr->f() && c->g < curr->g)) {
						curr = c;
						index = i;
					}
				}
				o.erase(o.begin() + index);
				c.push_back(curr);
				if (curr == end) {
					found = true;
					break;
				}
				for (int i = -1; i <= 1; i++)
					for (int j = -1; j <= 1; j++) 
						if (!(i == 0 && j == 0) && i + curr->x >= 0 && j + curr->z >= 0 && i + curr->x < w && j + curr->z < h) {
							Node *n = &nodes[(unsigned int)(j + curr->z)*w + (unsigned int)(i + curr->x)];
							if (n->blocked || ArrayHelper::contains(&c, n))continue;
							if (abs(i) == abs(j)) {
								Node *n1 = &nodes[(unsigned int)(j + curr->z)*w + (unsigned int)(curr->x)];
								Node *n2 = &nodes[(unsigned int)(curr->z)*w + (unsigned int)(i + curr->x)];
								if (n1->blocked || n2->blocked)continue;
							}
							float cost = curr->g + n->dis(curr);
							bool b = false;
							//if (cost < n->g || (b = !o->contains(n))) {
							if (cost < n->g || (b = !ArrayHelper::contains(&o, n))) {
								n->g = cost;
								n->h = n->dis(end);
								n->parent = curr;
								//if (b) o->add(n);
								//else o->update(n);
								if (b)o.push_back(n);
							}
						}
			}
			if (found) {
				Node *p = end;
				while (p != nullptr) {
					//printf("%u %u %p %i\n", p->x, p->z, p, p->blocked);
					path.push_back(Point(p->x*bSize+bSize/2, p->z*bSize+bSize/2));
					p = p->parent;
				}
				if (log >= 1)printf("Found a* solution! %ums\n", clock() - t);
			}
			else if (log >= 1)printf("Couldn't find an a* solution! %ums\n", clock() - t);
			//delete o;

			/*for (unsigned int j = 0; j < nh; j++) {
				for (unsigned int i = 0; i < nw; i++) {
					if (nodes[j*nw + i].blocked)printf("#");
					else printf(" ");
				}
				printf("\n");
			}
			printf("\n");*/
			return path;
		}
		unsigned int get(unsigned int i, unsigned int j, unsigned int k) {
			if (i >= w || j >= h || k >= l)return 0;
			return map[k*w*h + j*w + i];
		}
		void set(unsigned int i, unsigned int j, unsigned int k, unsigned int b) {
			if (i >= w || j >= h || k >= l)return;
			unsigned int b4 = map[k*w*h + j*w + i];
			map[k*w*h + j*w + i] = b;
		}
		void write(string file) {
			ofstream stream(file, ios::binary | ios::out);
			if (!stream.is_open()) {
				printf("Couldn't write the tilemap!\n");
				return;
			}
			unsigned int index = 0;
			BinaryHelper::writeUInt(stream, 4, w);
			BinaryHelper::writeUInt(stream, 4, h);
			BinaryHelper::writeUInt(stream, 4, l);
			for (unsigned int k = 0; k < l; k++)
				for (unsigned int j = 0; j < h; j++)
					for (unsigned int i = 0; i < w; i++)
						BinaryHelper::writeUInt(stream, 4, map[k*w*h + j*w + i]);
		}
		static TileMap *read(string file, Camera *c, Shader *shader, TileSet *ts, unsigned int type=2) {
			ifstream stream(file, ios::binary | ios::in);
			if (!stream.is_open()) {
				printf("Couldn't read the tilemap!\n");
				return nullptr;
			}
			unsigned int length, index = 0;
			stream.seekg(stream.end);
			length = stream.tellg();
			stream.seekg(stream.beg);
			if (length < 12) {
				printf("Couldn't read the tilemap! The file doesn't have the correct header!\n");
				return nullptr;
			}
			unsigned int w = BinaryHelper::readUInt(stream, index += 4, 4), h = BinaryHelper::readUInt(stream, index += 4, 4), l = BinaryHelper::readUInt(stream, index += 4, 4);
			TileMap *t = create(w, h, l, type, c, shader, ts);
			if (t == nullptr)return nullptr;
			if (w == 0 || h == 0 || l == 0 || (w & 0x8000) || (h & 0x8000) || (l & 0x8000)) {
				printf("Couldn't read the tilemap! The length is invalid!\n");
				return nullptr;
			}
			if (index + w * h * l * 4 >= length) {
				printf("Couldn't read the tilemap! Invalid length!\n");
				return nullptr;
			}
			for (unsigned int k = 0; k < l; k++)
				for (unsigned int j = 0; j < h; j++)
					for (unsigned int i = 0; i < w; i++)
						t->set(i, j, k, BinaryHelper::readUInt(stream, index += 4, 4));
			return t;
		}
		static TileMap *create(unsigned int w, unsigned int h, unsigned int l, unsigned int type, Camera *c, Shader *s, TileSet *ts) {
			if (c == nullptr || s == nullptr) {
				printf("Can't create a tilemap without model, camera and/or shader!\n");
				return nullptr;
			}
			return new TileMap(w, h, l, type, s, c, ts);
		}
	};
	void Raycast::raycast(vec3 s, vec3 dir, TileMap *tm, vector<GameObject*> &obj, bool &hitsTilemap, unsigned int &_i, unsigned int &_j, unsigned int &_k, GameObject *&go, bool useFullWorld) {
		float distance = numeric_limits<float>::max();
		for (unsigned int j = 0; j < obj.size(); j++) {
			vec4 omin4 = obj[j]->getMat() * obj[j]->get()->getMin();
			vec4 omax4 = obj[j]->getMat() * obj[j]->get()->getMax();
			vec3 omin = vec3(omin4.x, omin4.y, omin4.z);
			vec3 omax = vec3(omax4.x, omax4.y, omax4.z);
			bool b = intersect(omin, omax, s, dir);
			if (b) {
				float dist = (obj[j]->getMat().getTranslate() - s).magnitude();
				if (dist < distance) {
					distance = dist;
					go = obj[j];
				}
			}
		}
		vec3 mi, ma;
		if (!useFullWorld) {
			tm->getVisible(mi, ma);
			mi = tm->toMapCoords(mi);
			ma = tm->toMapCoords(ma);
		}
		else {
			mi = vec3(0, 0, 0);
			ma = vec3(tm->getWidth() - 1, tm->getHeight() - 1, tm->getLength() - 1);
		}
		for (int i = mi.x; i <= ma.x; i++)
			for (int j = mi.y; j <= ma.y; j++)
				for (int k = 0; k < tm->getLength(); k++) {
					if (i < 0 || j < 0 || k < 0 || i >= tm->getWidth() || j >= tm->getHeight() || k >= tm->getLength())continue;
					unsigned int b;
					if ((b = tm->get(i, j, k)) == 0)continue;
					mat4 m = tm->getModel(i, j, k);
					Model *model = tm->getModel(b - 1);
					vec3 omin, omax;
					if (model == nullptr)continue;
					Math::alignAxes(m, model, omin, omax);
					bool bo = intersect(omin, omax, s, dir);
					if (bo) {
						float dist = (m.getTranslate() - s).magnitude();
						if (dist < distance) {
							distance = dist;
							go = nullptr;
							hitsTilemap = true;
							_i = i;
							_j = j;
							_k = k;
						}
					}
				}
	}
}