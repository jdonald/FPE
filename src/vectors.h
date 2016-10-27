#pragma once

#include <math.h>

struct vector2 {
	float x, y;

	vector2() {}
	vector2(float x, float y):x(x), y(y) {}
	
	vector2 operator+(const vector2 &rhs) const { return vector2(x+rhs.x, y+rhs.y); }
	vector2 operator-(const vector2 &rhs) const { return vector2(x-rhs.x, y-rhs.y); }
	void operator+=(const vector2 &rhs) { x += rhs.x; y += rhs.y; }
	void operator-=(const vector2 &rhs) { x -= rhs.x; y -= rhs.y; }

	vector2 operator*(float s) const { return vector2(x*s, y*s); }
	vector2 operator/(float f) const { return vector2(x/f, y/f); }
	void operator*=(float s) { x *= s; y *= s; }
	void operator/=(float f) { x /= f; y /= f; }
};

struct vector3: public vector2 {
	float z;

	vector3() {}
	vector3(float x, float y, float z):vector2(x,y), z(z) {}
	vector3(const vector3 &xyz):vector2((vector2)xyz), z(xyz.z) {}
	vector3(const vector2 &v2):vector2(v2), z(0) {}

	bool operator!=(const vector3 &rhs) { return x != rhs.x || y != rhs.y || z != rhs.z; }
	vector3 operator=(const vector3 &rhs) { x = rhs.x; y = rhs.y; z = rhs.z; return *this; }
	vector3 operator+(const vector3 &rhs) const { return vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
	vector3 operator-(const vector3 &rhs) const { return vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
	vector3 operator*(float s) const { return vector3(x * s, y * s, z * s); }
	vector3 operator*(const vector3 &rhs) const { return vector3(x * rhs.x, y * rhs.y, z * rhs.z); }
	void operator+=(const vector3 &rhs) { x += rhs.x; y += rhs.y; z += rhs.z; }
	void operator-=(const vector3 &rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; }
	void operator/=(float d) { x /= d; y /= d; z /= d; }
	void zero() { x = 0.0f; y = 0.0f; z = 0.0f; }
	float rSquared() const { return x*x+y*y+z*z; }
	float r() const { return sqrtf(rSquared()); }
	void normalize() { operator/=(r()); }
	vector3 normal() const { float l = 1.0f/r(); return vector3(x*l, y*l, z*l); }
	float dot(const vector3 &v) const { return x*v.x + y*v.y + z*v.z; }
	vector3 cross(const vector3 &v) const { return vector3(y*v.z-z*v.y,-(x*v.z-z*v.x),x*v.y-y*v.x); }
	float angleBetween(const vector3 &v) const { return acosf(dot(v)/(r()*v.r())); }

	void rotX(float a) { vector3 rot(x, cosf(a)*y + sinf(a)*z, -sinf(a)*y + cosf(a)*z); *this = rot; }
	void rotY(float a) { vector3 rot(cosf(a)*x - sinf(a)*z, y, sinf(a)*x + cosf(a)*z); *this = rot; }
	void rotZ(float a) { vector3 rot(cosf(a)*x + sinf(a)*y, -sinf(a)*x + cosf(a)*y, z); *this = rot; }
};

struct vector4: public vector3 {
	float w;

	vector4() {}
	vector4(float x, float y, float z, float w):vector3(x, y, z), w(w) {}
	float dot(const vector4 &v) const { return x*v.x + y*v.y + z*v.z + w*v.w; }
};