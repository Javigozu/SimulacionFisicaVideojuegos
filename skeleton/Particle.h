#pragma once

#include <PxPhysicsAPI.h>
#include "RenderUtils.hpp"
#include "Vector3D.h"

class Particle {
public:
	Particle(physx::PxGeometry* shape, Vector4 color, Vector3D p, Vector3D v, Vector3D a, double d = 1.0, double mass = 1.0, double g = 9.8);
	~Particle();

	virtual void integrate(double t);
	inline Vector3D getVel() const { return vel; }
	inline Vector3D getAcc() const { return acc; }
	inline void setAcc(Vector3D newAcc) { acc = newAcc; }
	void updateForces(Vector3D f);

	inline double getMass() const { return mass; }
	inline double getDamp() const { return damping; }
	inline double getGrav() const { return gravity; }
	inline double getTime() const { return time; }
	inline Vector3D getPos() const { return { pose.p.x, pose.p.y, pose.p.z }; }
	inline physx::PxGeometry* getGeo() const { return geo; }
	inline Vector4 getColor() { return color; }

	inline void addTime(double t) { time += t; }
protected:
	Vector3D vel;
	Vector3D acc;
	double damping;
	double mass;
	double gravity;
	double time;
	physx::PxTransform pose;
	RenderItem* renderItem;

	physx::PxGeometry* geo;
	Vector4 color;
};