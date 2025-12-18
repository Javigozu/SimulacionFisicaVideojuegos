#pragma once
#include "ForceGenerator.h"
#include <vector>

class SolidSystem {
private:
	std::vector<physx::PxRigidDynamic*> bodies;
	std::vector<ForceGenerator*> forces;
public:
	SolidSystem();
	~SolidSystem();
	void update(double t);
	void addBody(physx::PxRigidDynamic* d);
	void addForce(ForceGenerator* g);
};