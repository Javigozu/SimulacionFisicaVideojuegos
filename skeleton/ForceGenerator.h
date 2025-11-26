#pragma once
#include "Particle.h"
class ForceGenerator {
protected:
	bool active;
public:
	ForceGenerator() : active(true) {}
	virtual ~ForceGenerator() {}
	virtual void updateTime(double t) {}
	virtual Vector3D applyForce(Particle* p) = 0;
	void activate(bool a) { active = a; }
	bool getActive() { return active; }
};

class GravityGenerator :public ForceGenerator {
private:
	const Vector3D G = { 0.0,-9.8,0.0 };
public:
	GravityGenerator() {}
	virtual Vector3D applyForce(Particle* p) override {
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
	virtual Vector3D applyForce(Particle* p) override {
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
	double R, newR, K, T, time, vel;
public:
	ExplosionGenerator(Vector3D pos, double r, double k, double t, double v = 0.0) : Pos(pos), R(r), K(k), T(t), time(0.0), vel(v) { active = false; }
	void reset() {
		time = 0.0;
		newR = R;
	}
	void setPos(Vector3D p) { Pos = p; }
	virtual void updateTime(double t) override {
		time += t;
		newR = newR + vel * t;
	}
	virtual Vector3D applyForce(Particle* p) override {
		double r = (p->getPos() - Pos).magnitude();
		if (r < newR) return (p->getPos() - Pos) * (K / (r * r)) * physx::PxExp(-time / T);
		else return { 0.0,0.0,0.0 };
	}
};

class SpringAnchorGenerator : public ForceGenerator
{
private:
	Vector3D Pos;
	double K;
	double lon;
	RenderItem* anchor;
public:
	SpringAnchorGenerator(Vector3D pos, double k, double l)
		: Pos(pos), K(k), lon(l)
	{
		anchor = new RenderItem(CreateShape(physx::PxBoxGeometry(0.3, 0.3, 0.3)), new physx::PxTransform(pos.getX(), pos.getY(), pos.getZ()), { 1,1,1,1 });
		RegisterRenderItem(anchor);
	}
	virtual ~SpringAnchorGenerator() {
		DeregisterRenderItem(anchor);
	}
	double getK() { return K; }
	void setK(double k) { K = max(0, k); }
	virtual Vector3D applyForce(Particle* p) override {
		Vector3D dist = p->getPos() - Pos;
		return dist * ((1 / dist.magnitude()) * (dist.magnitude() - lon) * -K);
	}
};

class SpringGenerator : public ForceGenerator
{
private:
	Particle* other;
	double K;
	double lon;
public:
	SpringGenerator(Particle* p, double k, double l) : other(p), K(k), lon(l)
	{
	}
	double getK() { return K; }
	void setK(double k) { K = max(0, k); }
	virtual Vector3D applyForce(Particle* p) override {
		if (p != other) {
			Vector3D dist = p->getPos() - other->getPos();
			return dist * ((1 / dist.magnitude()) * (dist.magnitude() - lon) * -K);
		}
		else return { 0.0,0.0,0.0 };
	}
};
class BuoyancyGenerator : public ForceGenerator {
private:
	float  liquid_H, H, V, D;
	const float g = 9.8;
	RenderItem* liquid;
public:
	BuoyancyGenerator(float lh, float h, float v, float d) {
		liquid_H = lh;
		H = h;
		V = v;
		D = d;
		liquid = new RenderItem(CreateShape(physx::PxBoxGeometry(50, 0.1, 50)),
			new physx::PxTransform(0.0, lh, 0.0), { 0.0,0.6,1.0,0.5 });
	}
	virtual ~BuoyancyGenerator() {
		DeregisterRenderItem(liquid);
	}
	virtual Vector3D applyForce(Particle* p) override {
		float h = p->getPos().getY();
		float h0 = liquid_H;
			float inmersed;
			if (h - h0 > H / 2) inmersed = 0.0;
			else if (h0 - h > H / 2) inmersed = 1.0;
			else inmersed = (h0 - h) / H + 0.5;
			return { 0.0,D * V * inmersed * g,0.0 };
	}
};