#include "ParticleSystem.h"
using namespace std;

ParticleSystem::ParticleSystem(double time) : symTime(time) {}
ParticleSystem::~ParticleSystem() {
	for (auto p : particle) { delete p; }
	for (auto g : generators) { delete g; }
}

void ParticleSystem::update(double t) {
	for (list<Particle*>::iterator it = particle.begin(); it != particle.end(); ++it) {
		Particle* p = *it;
		p->addTime(t);
		p->integrate(t);
		if (p->getTime() > symTime) {
			it = particle.erase(it);
			delete p;
		}
	}
	for (auto g : generators) {
		particle.push_back(g->addParticle());
	}
}
void ParticleSystem::addGen(ParticleGenerator* g) {
	generators.push_back(g);
}