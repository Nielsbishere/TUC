#pragma once
namespace osomi {
	class vec3 {
	public:
		float x, y, z;
		vec3() : vec3(0,0,0) {}
		vec3(float v0, float v1, float v2) {
			x = v0;
			y = v1;
			z = v2;
		}
		float &operator[](const unsigned int i) {
			switch (i) {
			case 1:
				return y;
			case 2:
				return z;
			default:
				return x;
			}
		}
		vec3 operator*(const float f) {
			vec3 vec;
			for (int i = 0; i < 3; i++)
				vec[i] = f * (*this)[i];
			return vec;
		}
		vec3 operator/(const float f) {
			vec3 vec;
			for (int i = 0; i < 3; i++)
				vec[i] = (*this)[i] / f;
			return vec;
		}
		vec3 operator/(vec3 &v) {
			vec3 vec;
			for (int i = 0; i < 3; i++)
				vec[i] = (*this)[i] / v[i];
			return vec;
		}
		vec3 operator+(vec3 &v) {
			vec3 vec;
			for (int i = 0; i < 3; i++)
				vec[i] = v[i] + (*this)[i];
			return vec;
		}
		vec3 operator+(float f) {
			vec3 vec;
			for (int i = 0; i < 3; i++)
				vec[i] = f + (*this)[i];
			return vec;
		}
		vec3 operator-(float f) {
			return (*this) + -f;
		}
		vec3 operator-(vec3 &v) {
			vec3 vec;
			for (int i = 0; i < 3; i++)
				vec[i] = v[i] - (*this)[i];
			return vec;
		}
		vec3 operator-() const {
			return vec3(-x, -y, -z);
		}
		float dot(vec3 &v) {
			float r = 0;
			for (int i = 0; i < 3; i++)
				r += v[i] * (*this)[i];
			return r;
		}
		float operator*(vec3 &v) {
			return dot(v);
		}
		void operator+=(vec3 &v) {
			for (int i = 0; i < 3; i++)
				(*this)[i] += v[i];
		}
		void operator-=(vec3 &v) {
			for (int i = 0; i < 3; i++)
				(*this)[i] -= v[i];
		}
		void operator*=(float f) {
			for (int i = 0; i < 3; i++)
				(*this)[i] *= f;
		}
		void operator*=(vec3 v) {
			for (int i = 0; i < 3; i++)
				(*this)[i] *= v[i];
		}
		void operator/=(float f) {
			for (int i = 0; i < 3; i++)
				(*this)[i] /= f;
		}
		float squareLength() {
			float f=0;
			for (int i = 0; i < 3; i++)
				f += (*this)[i] * (*this)[i];
			return f;
		}
		float length() {
			return sqrt(squareLength());
		}
		float magnitude() {
			return length();
		}
		vec3 normalize() {
			(*this) /= length();
			return *this;
		}
		vec3 cross(vec3 &v) {
			return vec3(y*v.z - v.y*z, z*v.x - v.z*x, x*v.y - v.x*y);
		}
		bool operator==(vec3 oth) {
			for (int i = 0; i < 3; i++)
				if ((*this)[i] != oth[i])return false;
			return true;
		}
		bool operator!=(vec3 oth) {
			return !(*this == oth);
		}
		void print() {
			printf("%f, %f, %f\n", x, y, z);
		}
		vec3 inverse() {
			return vec3(1 / x, 1 / y, 1 / z);
		}
		static vec3 zero() {
			return vec3();
		}
	};
}