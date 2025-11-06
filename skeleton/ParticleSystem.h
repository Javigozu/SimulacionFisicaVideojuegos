#pragma once
#include "Particle.h"
#include "ParticleGenerator.h"
#include "ForceGenerator.h"
#include <vector>

class ParticleSystem {
private:
	//std::list<Particle*> particle;
	std::vector<Particle*> particle;
	std::vector<ParticleGenerator*> generators;
	std::vector<ForceGenerator*> forces;
	double symTime;
	double addTime;
	double next;
	double R;
	Vector3D origen;
public:
	ParticleSystem(double r, Vector3D o, double addTime = 0.0, double time = 0.0);
	~ParticleSystem();
	void update(double t);
	void addParticle(Particle* p);
	void addGen(ParticleGenerator* g);
	void activate(bool a);
	void addForce(ForceGenerator* g);
	inline void setOrigen(Vector3D o) { origen = o; }
};