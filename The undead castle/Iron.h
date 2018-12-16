#pragma once
#include "Damagable.h"
namespace osomi {
	class Iron : public Damagable {
	public:
		Iron() : Damagable("iron ore", 0, 150) {}
	};
}