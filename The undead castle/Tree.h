#pragma once
#include "Damagable.h"
namespace osomi {
	class Tree : public Damagable {
	public:
		Tree() : Damagable("tree", 0, 20){}
	};
}