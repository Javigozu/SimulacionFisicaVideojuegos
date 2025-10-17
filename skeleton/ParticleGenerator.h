#pragma once
#include "Particle.h"
#include <random>
class ParticleGenerator {
private:
	Particle* model;
	Vector3D distPos;
	Vector3D distVel;
	std::mt19937 mt;
	std::uniform_real_distribution<double> u;
	std::normal_distribution<double> n;
public:
	ParticleGenerator(Particle* p, Vector3D dpos, Vector3D dvel) : model(p), distPos(dpos), distVel(dvel), mt(), u(0, 1), n(0, 1) {
	}
	~ParticleGenerator() { delete model; }
	Particle* addParticle() {
		Vector3D pos = model->getPos() + Vector3D(n(mt) * distPos.getX(), n(mt) * distPos.getY(), n(mt) * distPos.getZ());
		Vector3D vel = model->getVel() + Vector3D(n(mt) * distVel.getX(), n(mt) * distVel.getY(), n(mt) * distVel.getZ());
		return new Particle(model->getGeo(), model->getColor(), pos, vel, model->getAcc(), model->getDamp(), model->getMass(), model->getGrav());
	}
};