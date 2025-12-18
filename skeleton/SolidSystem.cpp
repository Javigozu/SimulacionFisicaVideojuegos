#include "SolidSystem.h"

using namespace std;
using namespace physx;

SolidSystem::SolidSystem() : bodies(), forces()
{
}
SolidSystem::~SolidSystem() {}

void SolidSystem::update(double t) {
	for (auto& it = bodies.begin(); it != bodies.end();) {
		PxRigidDynamic* b = *it;
		Vector3D fuerzas = { 0.0,0.0,0.0 };
		for (auto& f : forces) {
			if (f->getActive()) {
				f->updateTime(t);
				fuerzas = fuerzas + f->applyForce(b);
			}
		}
		b->addForce(PxVec3(fuerzas.getX(), fuerzas.getY(), fuerzas.getZ()));
		++it;
	}
}
void SolidSystem::addBody(PxRigidDynamic* p) {
	bodies.push_back(p);
}
void SolidSystem::addForce(ForceGenerator* g) {
	forces.push_back(g);
}