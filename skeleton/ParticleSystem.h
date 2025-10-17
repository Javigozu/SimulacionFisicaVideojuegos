#pragma once
#include "Particle.h"
#include "ParticleGenerator.h"
#include <list>

class ParticleSystem {
private:
	std::list<Particle*> particle;
	std::list<ParticleGenerator*> generators;
	double symTime;
public:
	ParticleSystem(double time = 20);
	~ParticleSystem();
	void update(double t);
	void addGen(ParticleGenerator* g);
};