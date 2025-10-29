#include "ParticleSystem.h"
using namespace std;

ParticleSystem::ParticleSystem(double add, double time) : symTime(time), addTime(add), next(0.0) {}
ParticleSystem::~ParticleSystem() {
	for (auto& p : particle) {
		if (p != nullptr) {
			delete p;
			p = nullptr;
		}
	}
	for (auto& g : generators) {
		if (g != nullptr) {
			delete g;
			g = nullptr;
		}
	}
	/*for (auto& f : forces) {
		if (f != nullptr) {
			delete f;
			f = nullptr;
		}
	}*/
}

void ParticleSystem::update(double t) {
	next += t;
	list<Particle*>::iterator it = particle.begin();
	while (it != particle.end()) {
		Particle* p = *it;
		Vector3D fuerzas = { 0.0,0.0,0.0 };
		for (auto& f : forces) {
			fuerzas = fuerzas + f->applyForce(p);
		}
		p->updateForces(fuerzas);
		p->addTime(t);
		p->integrate(t);
		if (p->getTime() > symTime && symTime != 0.0) {
			it = particle.erase(it);
			delete p;
		}
		else ++it;
	}
	for (auto& g : generators) {
		if (next > addTime) {
			particle.push_back(g->addParticle());
		}
	}
	if (next > addTime) next = 0.0;
}
void ParticleSystem::addParticle(Particle* p) {
	particle.push_back(p);
}

void ParticleSystem::addGen(ParticleGenerator* g) {
	generators.push_back(g);
}
void ParticleSystem::addForce(ForceGenerator* g) {
	forces.push_back(g);
}