#pragma once
#include "Particle.h"

class Projectile : public Particle {
private:
	double massSimulated(double mR, double vR, double vS);
	double gravitySimulated(double gR, double vR, double vS);
	double speed;
public:
	Projectile(Vector3D p, Vector3D vDir, double spR, double spS, double massR, double d = 1.0, double g = 9.8);
};