#pragma once
#include "Damagable.h"
namespace osomi {
	class Rock : public Damagable {
	public:
		Rock() : Damagable("rock", 0, 50) {}
	};
}