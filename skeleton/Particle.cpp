#include "Particle.h"
using namespace physx;

Particle::Particle(Vector3D v, Vector3D p) :vel(v), pose(p.getX(), p.getY(), p.getZ())
{
	renderItem = new RenderItem(CreateShape(PxBoxGeometry(1.0,1.0,1.0)), &pose, { 1,1,0,1});
}
Particle::~Particle() {
	DeregisterRenderItem(renderItem);
	renderItem = nullptr;
}

void Particle::integrate(double t) {
	pose.p += Vector3(vel.getX(), vel.getY(), vel.getZ()) * t;
}