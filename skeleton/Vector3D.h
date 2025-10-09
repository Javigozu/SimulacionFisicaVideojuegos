#pragma once
#include <cmath>
#include <PxPhysicsAPI.h>

class Vector3D {
private:
	double x;
	double y;
	double z;
public:
	Vector3D() : x(0.0f), y(0.0f), z(0.0f) {}
	Vector3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
	Vector3D(const physx::PxVec3& v) : x(v.x), y(v.y), z(v.z) {}

	inline double& getX() { return x; }
	inline double& getY() { return y; }
	inline double& getZ() { return z; }
	inline void setX(double _x) { x = _x; }
	inline void setY(double _x) { x = _x; }
	inline void setZ(double _x) { x = _x; }

	inline double magnitude() const {
		double sq = x * x + y * y + z * z;
		return sqrt(sq);
	}
	inline Vector3D normalize() {
		double mg = magnitude();
		x = x / mg;
		y = y / mg;
		z = z / mg;
		return *this;
	}
	inline Vector3D escalar(double p) const {
		return { x * p, y * p, z * p };
	}
	inline double prodEsc(const Vector3D& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	inline void operator= (const Vector3D& v) {
		x = v.x;
		y = v.y;
		z = v.z;
	}
	inline Vector3D operator+ (const Vector3D& v) const{
		return { x + v.x, y + v.y, z + v.z };
	}
	inline Vector3D operator- (const Vector3D& v) const{
		return { x - v.x, y - v.y, z - v.z };
	}
	inline double operator* (const Vector3D& v) const{
		return prodEsc(v);
	}
	inline Vector3D operator* (double p) const{
		return escalar(p);
	}
};