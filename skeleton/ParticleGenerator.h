#pragma once
#include "Particle.h"
#include <random>
class ParticleGenerator {
protected:
	Particle* model;
	Vector3D distPos;
	Vector3D distVel;
	bool generate;
	std::mt19937 mt;
	std::uniform_real_distribution<double> u;
	std::normal_distribution<double> n;
public:
	ParticleGenerator(Particle* p, Vector3D dpos, Vector3D dvel) : model(p), distPos(dpos), distVel(dvel), generate(true), mt(){
	}
	virtual ~ParticleGenerator() { delete model; }
	void activate(bool a) { generate = a; }
	bool getActive() const { return generate; }
	virtual Particle* addParticle() = 0;
};

class UniformGenerator : public ParticleGenerator {
public:
	UniformGenerator(Particle* p, Vector3D dpos, Vector3D dvel, double min = -1.0, double max = 1.0) : ParticleGenerator(p, dpos, dvel) {
		u = std::uniform_real_distribution<double>(min, max);
	}
	virtual Particle* addParticle() override {
		Vector3D pos = model->getPos() + Vector3D(u(mt) * distPos.getX(), u(mt) * distPos.getY(), u(mt) * distPos.getZ());
		Vector3D vel = model->getVel() + Vector3D(u(mt) * distVel.getX(), u(mt) * distVel.getY(), u(mt) * distVel.getZ());
		return new Particle(model->getGeo(), model->getColor(), pos, vel, model->getAcc(), model->getDamp(), model->getMass(), model->getGrav());
	}
};
class NormalGenerator : public ParticleGenerator {
public:
	NormalGenerator(Particle* p, Vector3D dpos, Vector3D dvel, double m = 0.0, double v = 1.0) : ParticleGenerator(p, dpos, dvel) {
		n = std::normal_distribution<double>(m, v);
	}
	virtual Particle* addParticle() override {
		Vector3D pos = model->getPos() + Vector3D(n(mt) * distPos.getX(), n(mt) * distPos.getY(), n(mt) * distPos.getZ());
		Vector3D vel = model->getVel() + Vector3D(n(mt) * distVel.getX(), n(mt) * distVel.getY(), n(mt) * distVel.getZ());
		return new Particle(model->getGeo(), model->getColor(), pos, vel, model->getAcc(), model->getDamp(), model->getMass(), model->getGrav());
	}
};

class FountainGenerator : public ParticleGenerator {
public:
	FountainGenerator(Particle* p, Vector3D dpos, Vector3D dvel) : ParticleGenerator(p, dpos, dvel) {
		u = std::uniform_real_distribution<double>(0, 1);
		n = std::normal_distribution<double>(0,1);
	}
	virtual Particle* addParticle() override {
		Vector3D pos = model->getPos() + Vector3D(n(mt) * distPos.getX(), n(mt) * distPos.getY(), n(mt) * distPos.getZ());
		Vector3D vel = model->getVel() + Vector3D(n(mt) * distVel.getX(), u(mt) * distVel.getY(), n(mt) * distVel.getZ());
		return new Particle(model->getGeo(), model->getColor(), pos, vel, model->getAcc(), model->getDamp(), model->getMass(), model->getGrav());
	}
};