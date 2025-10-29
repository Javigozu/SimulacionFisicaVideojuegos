#include "Particle.h"
#include "Vector3D.h"
using namespace physx;

Particle::Particle(PxGeometry* shape, Vector4 col, Vector3D p, Vector3D v, Vector3D a, double d, double m, double g)
	:vel(v), acc(a), pose(p.getX(), p.getY(), p.getZ()), damping(d), mass(m), gravity(g), geo(shape),color(col), time(0)
{
	renderItem = new RenderItem(CreateShape(*shape), &pose, color);
	acc.setY(acc.getY() - gravity);
}
Particle::~Particle() {
	DeregisterRenderItem(renderItem);
	renderItem = nullptr;
}

void Particle::integrate(double t) {
	//Euler
	/*pose.p += Vector3(vel.getX(), vel.getY(), vel.getZ()) * t;
	vel = vel * pow(damping, t) + (acc * t);*/
	
	//Euler semi implicito
	vel = vel * pow(damping, t) + (acc * t);
	pose.p += Vector3(vel.getX(), vel.getY(), vel.getZ()) * t;
}
void Particle::updateForces(Vector3D f) {
	setAcc(f / mass);
}
