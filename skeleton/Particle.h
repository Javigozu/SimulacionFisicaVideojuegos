#pragma once

#include <PxPhysicsAPI.h>
#include "RenderUtils.hpp"
#include "Vector3D.h"

class Particle {
public:
	Particle(Vector3D p, Vector3D v, Vector3D a, double d = 0.0, double mass = 1.0, double g = 9.8);
	~Particle();

	virtual void integrate(double t);
	inline void setAcc(Vector3D newAcc) { acc = newAcc; }
protected:
	Vector3D vel;
	Vector3D acc;
	double damping;
	double mass;
	double gravity;
	physx::PxTransform pose;
	RenderItem* renderItem;
};