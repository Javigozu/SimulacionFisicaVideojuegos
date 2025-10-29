#pragma once
#include "Particle.h"
#include "ParticleGenerator.h"
#include "ForceGenerator.h"
#include <list>

class ParticleSystem {
private:
	std::list<Particle*> particle;
	std::list<ParticleGenerator*> generators;
	std::list<ForceGenerator*> forces;
	double symTime;
	double addTime;
	double next;
public:
	ParticleSystem(double addTime = 0.0, double time = 0.0);
	~ParticleSystem();
	void update(double t);
	void addParticle(Particle* p);
	void addGen(ParticleGenerator* g);
	void addForce(ForceGenerator* g);
};