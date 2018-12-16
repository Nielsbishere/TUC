#pragma once
#include <cmath>
namespace osomi {
	class vec2 {
	public:
		float x, y;
		vec2() : vec2(0,0){}
		vec2(float v0, float v1) {
			x = v0;
			y = v1;
		}
		float &operator[](const unsigned int i) {
			switch (i) {
			case 1:
				return y;
			default:
				return x;
			}
		}
		vec2 operator*(const float f) {
			vec2 vec;
			for (int i = 0; i < 2; i++)
				vec[i] = f * (*this)[i];
			return vec;
		}
		vec2 operator/(const float f) {
			vec2 vec;
			for (int i = 0; i < 2; i++)
				vec[i] = (*this)[i] / f;
			return vec;
		}
		vec2 operator/(vec2 &v) {
			vec2 vec;
			for (int i = 0; i < 2; i++)
				vec[i] = (*this)[i] / v[i];
			return vec;
		}
		vec2 operator+(vec2 &v) {
			vec2 vec;
			for (int i = 0; i < 2; i++)
				vec[i] = v[i] + (*this)[i];
			return vec;
		}
		vec2 operator-(vec2 &v) {
			return (*this) + -v;
		}
		vec2 operator+(float f) {
			vec2 vec;
			for (int i = 0; i < 2; i++)
				vec[i] = f + (*this)[i];
			return vec;
		}
		vec2 operator-(float f) {
			return (*this) + -f;
		}
		vec2 operator-() const {
			return vec2(-x, -y);
		}
		float dot(vec2 &v) {
			float r = 0;
			for (int i = 0; i < 2; i++)
				r += v[i] * (*this)[i];
			return r;
		}
		float operator*(vec2 &v) {
			return dot(v);
		}
		void operator+=(vec2 &v) {
			for (int i = 0; i < 2; i++)
				(*this)[i] += v[i];
		}
		void operator-=(vec2 &v) {
			for (int i = 0; i < 2; i++)
				(*this)[i] -= v[i];
		}
		void operator*=(float f) {
			for (int i = 0; i < 2; i++)
				(*this)[i] *= f;
		}
		void operator*=(vec2 v) {
			for (int i = 0; i < 2; i++)
				(*this)[i] *= v[i];
		}
		void operator/=(float f) {
			for (int i = 0; i < 2; i++)
				(*this)[i] /= f;
		}
		float squareLength() {
			float f = 0;
			for (int i = 0; i < 2; i++)
				f += (*this)[i] * (*this)[i];
			return f;
		}
		float length() {
			return sqrt(squareLength());
		}
		float magnitude() {
			return length();
		}
		vec2 normalize() {
			(*this) /= length();
			return *this;
		}
		bool operator==(vec2 oth) {
			for (int i = 0; i < 2; i++)
				if ((*this)[i] != oth[i])return false;
			return true;
		}
		bool operator!=(vec2 oth) {
			return !(*this == oth);
		}
		void print() {
			printf("%f, %f\n", x, y);
		}
		vec2 swap() {
			float x = y;
			float y = this->x;
			return vec2(x, y);
		}
		vec2 inverse() {
			return vec2(1 / x, 1 / y);
		}
		static vec2 zero() {
			return vec2();
		}
	};
}