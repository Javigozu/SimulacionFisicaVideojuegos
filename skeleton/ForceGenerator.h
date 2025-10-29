#pragma once
#include <cmath>
#include "Particle.h"
class ForceGenerator {
public:
	virtual Vector3D applyForce(Particle* p, double t = 0.0) = 0;
};

class GravityGenerator :public ForceGenerator {
private:
	const Vector3D G = { 0.0,-9.8,0.0 };
public:
	GravityGenerator() {}
	virtual Vector3D applyForce(Particle* p, double t) override {
		return G * p->getMass();
	}
};

class WindGenerator :public ForceGenerator {
private:
	Vector3D Pos;
	Vector3D Volume;
	Vector3D Vel;
	double K1, K2;
public:
	WindGenerator(Vector3D pos, Vector3D vol, Vector3D v, double k1 = 1.0, double k2 = 0.0) : Pos(pos), Volume(vol), Vel(v), K1(k1), K2(k2) {}
	virtual Vector3D applyForce(Particle* p, double t) override {
		if ((p->getPos().getX() >= Pos.getX() && p->getPos().getY() >= Pos.getY() && p->getPos().getZ() >= Pos.getZ())
			&& (p->getPos().getX() <= Pos.getX() + Volume.getX() && p->getPos().getY() <= Pos.getY() + Volume.getY() && p->getPos().getZ() <= Pos.getZ() + Volume.getZ()))
			return (Vel - p->getVel()) * K1 + ((Vel - p->getVel()) * (Vel - p->getVel()).magnitude() * K2);
		else return { 0.0,0.0,0.0 };
	}
};
/*class WindGenerator :public ForceGenerator {
private:
	Vector3D Pos;
	Vector3D Volume;
	Vector3D Vel;
	double K1, K2;
public:
	WindGenerator(Vector3D pos, Vector3D vol, Vector3D v, double k1, double k2) : Pos(pos), Volume(vol), Vel(v), K1(k1), K2(k2) {}
	virtual Vector3D applyForce(Particle* p, double t) override {
		if ((p->getPos().getX() >= Pos.getX() && p->getPos().getY() >= Pos.getY() && p->getPos().getZ() >= Pos.getZ())
			&& (p->getPos().getX() <= Pos.getX() + Volume.getX() && p->getPos().getY() <= Pos.getY() + Volume.getY() && p->getPos().getZ() <= Pos.getZ() + Volume.getZ()))
			return (Vel - p->getVel()) * K1 + ((Vel - p->getVel()) * (Vel - p->getVel()).magnitude() * K2);
		else return { 0.0,0.0,0.0 };
	}
};*/

class ExplosionGenerator :public ForceGenerator {
private:
	Vector3D Pos;
	double R, K, T;
public:
	ExplosionGenerator(Vector3D pos, double r, double k, double t) : Pos(pos), R(r), K(k), T(t){}
	virtual Vector3D applyForce(Particle* p, double t) override {
		double r = (p->getPos() - Pos).magnitude();
		if (r < R) return p->getPos() - Pos * (K / (r * r))* exp(-t/T);
		else return { 0.0,0.0,0.0 };
	}
};