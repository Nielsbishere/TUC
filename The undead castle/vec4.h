#pragma once
#include <cmath>
namespace osomi {
	class vec4 {
	public:
		float x, y, z, w;
		vec4() : vec4(0,0,0,0){}
		vec4(float v0, float v1, float v2, float v3) {
			x = v0;
			y = v1;
			z = v2;
			w = v3;
		}
		vec4(vec3 v, float w) : vec4(v.x, v.y, v.z, w){}
		float &operator[](const unsigned int i) {
			switch (i) {
			case 1:
				return y;
			case 2:
				return z;
			case 3:
				return w;
			default:
				return x;
			}
		}
		vec4 operator*(const float f) {
			vec4 vec;
			for (int i = 0; i < 4; i++)
				vec[i] = f * (*this)[i];
			return vec;
		}
		vec4 operator/(const float f) {
			vec4 vec;
			for (int i = 0; i < 4; i++)
				vec[i] = (*this)[i] / f;
			return vec;
		}
		vec4 operator/(vec4 &v) {
			vec4 vec;
			for (int i = 0; i < 4; i++)
				vec[i] = (*this)[i] / v[i];
			return vec;
		}
		vec4 operator+(vec4 &v) {
			vec4 vec;
			for (int i = 0; i < 4; i++)
				vec[i] = v[i] + (*this)[i];
			return vec;
		}
		vec4 operator-(vec4 &v) {
			return (*this) + -v;
		}
		vec4 operator+(float f) {
			vec4 vec;
			for (int i = 0; i < 4; i++)
				vec[i] = f + (*this)[i];
			return vec;
		}
		vec4 operator-(float f) {
			return (*this) + -f;
		}
		vec4 operator-() const {
			return vec4(-x, -y, -z, -w);
		}
		float dot(vec4 &v) {
			float r = 0;
			for (int i = 0; i < 4; i++)
				r += v[i] * (*this)[i];
			return r;
		}
		float operator*(vec4 &v) {
			return dot(v);
		}
		void operator+=(vec4 &v) {
			for (int i = 0; i < 4; i++)
				(*this)[i] += v[i];
		}
		void operator-=(vec4 &v) {
			for (int i = 0; i < 4; i++)
				(*this)[i] -= v[i];
		}
		void operator*=(float f) {
			for (int i = 0; i < 4; i++)
				(*this)[i] *= f;
		}
		void operator*=(vec4 v) {
			for (int i = 0; i < 4; i++)
				(*this)[i] *= v[i];
		}
		void operator/=(float f) {
			for (int i = 0; i < 4; i++)
				(*this)[i] /= f;
		}
		float squareLength() {
			float f = 0;
			for (int i = 0; i < 4; i++)
				f += (*this)[i] * (*this)[i];
			return f;
		}
		float length() {
			return sqrt(squareLength());
		}
		float magnitude() {
			return length();
		}
		vec4 normalize() {
			(*this) /= length();
			return *this;
		}
		bool operator==(vec4 oth) {
			for (int i = 0; i < 4; i++)
				if ((*this)[i] != oth[i])return false;
			return true;
		}
		bool operator!=(vec4 oth) {
			return !(*this == oth);
		}
		void print() {
			printf("%f, %f, %f, %f\n", x, y, z, w);
		}
		vec4 inverse() {
			return vec4(1 / x, 1 / y, 1 / z, 1 / w);
		}
		static vec4 zero() {
			return vec4();
		}
	};
}