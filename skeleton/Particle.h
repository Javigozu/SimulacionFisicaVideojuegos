#include "Vector3D.h"
#include "RenderUtils.hpp"

class Particle {
public:
	Particle(Vector3D p, Vector3D v, Vector3D a, double d);
	~Particle();

	void integrate(double t);
	inline void setAcc(Vector3D newAcc) { acc = newAcc; }
private:
	Vector3D vel;
	Vector3D acc;
	double damping;
	physx::PxTransform pose;
	RenderItem* renderItem;
};