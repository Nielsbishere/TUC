#pragma once
#include <cmath>
namespace osomi {
	class Node {
	public:
		unsigned int x, z;
		bool blocked;
		Node *parent;
		float g, h;
		Node(unsigned int _x, unsigned int _z, bool _blocked, Node *_parent = nullptr) : x(_x), z(_z), blocked(_blocked), parent(_parent), g(0), h(0) {}
		Node() : Node(0, 0, false, nullptr) {}
		float dis(Node *n) {
			float dx = fabs((float)n->x - x);
			float dz = fabs((float)n->z - z);
			float h = dx > dz ? dx : dz;
			float l = dx <= dz ? dx : dz;
			return 14 * l + 10 * (h - l);
		}
		float f(){
			return g + h;
		}
		float gval() {
			return g;
		}
	};
}