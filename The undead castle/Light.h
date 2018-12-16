#pragma once
#include "vec3.h"
namespace osomi {
	struct Light {
	private:
		vec3 pos, col;
		float power;
	public:
		bool draw;
		Light(vec3 _pos, vec3 _col, float pow, bool showCube = false) : power(pow), draw(showCube) {
			pos = _pos;
			col = _col;
			power = pow;
		}
		vec3 getPosition() {
			return pos;
		}
		void setPosition(vec3 p) {
			pos = p;
		}
		void move(vec3 m) {
			setPosition(pos + m);
		}
		GLfloat *getPos() {
			return &pos[0];
		}
		GLfloat *getCol() {
			return &col[0];
		}
		void setColor(vec3 _col) {
			col = _col;
		}
		vec3 getColor() {
			return col;
		}
		float getPower() {
			return power;
		}
	};
}