#include "Particle.h"
using namespace physx;

Particle::Particle(Vector3D p, Vector3D v, Vector3D a, double d = 1)
	:vel(v), acc(a), pose(p.getX(), p.getY(), p.getZ()), damping(d)
{
	renderItem = new RenderItem(CreateShape(PxBoxGeometry(1.0,1.0,1.0)), &pose, { 1,1,0,1});
}
Particle::~Particle() {
	DeregisterRenderItem(renderItem);
	renderItem = nullptr;
}

void Particle::integrate(double t) {
	vel = vel * pow(damping, t) + (acc * t);

	pose.p += Vector3(vel.getX(), vel.getY(), vel.getZ()) * t;
}