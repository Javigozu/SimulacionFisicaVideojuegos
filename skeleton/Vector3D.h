#pragma once
#include <cmath>

class Vector3D {
private:
	float x;
	float y;
	float z;
public:
	Vector3D() : x(0.0f), y(0.0f), z(0.0f) {}
	Vector3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

	inline float& getX() { return x; }
	inline float& getY() { return y; }
	inline float& getZ() { return z; }
	inline void setX(float _x) { x = _x; }
	inline void setY(float _x) { x = _x; }
	inline void setZ(float _x) { x = _x; }

	inline float magnitude() {
		float sq = x * x + y * y + z * z;
		return sqrt(sq);
	}
	inline void normalize() {
		float mg = magnitude();
		x = x / mg;
		y = y / mg;
		z = z / mg;
	}
	inline Vector3D escalar(float p) {
		return { x * p, y * p, z * p };
	}
	inline float prodEsc(const Vector3D& v) {
		return x * v.x + y * v.y + z * v.z;
	}

	inline void operator= (const Vector3D& v) {
		x = v.x;
		y = v.y;
		z = v.z;
	}
	inline Vector3D operator+ (const Vector3D& v) {
		return { x + v.x, y + v.y, z + v.z };
	}
	inline Vector3D operator- (const Vector3D& v) {
		return { x - v.x, y - v.y, z - v.z };
	}
	inline float operator* (const Vector3D& v) {
		return prodEsc(v);
	}
	inline Vector3D operator* (float p) {
		return escalar(p);
	}
};