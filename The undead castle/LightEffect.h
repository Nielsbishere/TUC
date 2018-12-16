#pragma once
#include "Math.h"
#include "Light.h"
namespace osomi {
	class LightEffect {
	private:
		Light *current;
		float duration, timer;
		vec3 first, last;
		bool isLinear, pulse;
		void updateColor() {
			float perc = Math::clamp(timer / duration, 0.f, 1.f);
			if(pulse)perc = (float)abs(perc - 0.5) * 2;
			if (isLinear) current->setColor(first * (1 - perc) + last * perc);
			else {
				float nperc = pow(10.f, perc - 1) / 9 * 10;
				current->setColor(first * (1 - nperc) + last * nperc);
			}
		}
	public:
		LightEffect(Light *l, float _duration, vec3 start, vec3 end, bool linear=true, bool isPulsing=true) {
			duration = _duration;
			timer = 0;
			first = start;
			last = end;
			current = l;
			isLinear = linear;
			pulse = isPulsing;
		}
		bool update(float delta) {
			if (timer > duration) {
				timer = duration;
				updateColor();
				return true;
			}
			else if (timer == duration)return true;
			updateColor();
			timer += delta;
			return false;
		}
		Light *getLight() {
			return current;
		}
	};
}