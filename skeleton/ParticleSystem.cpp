#include "ParticleSystem.h"
using namespace std;

ParticleSystem::ParticleSystem(double r, Vector3D o, double add, double time) : particle(), generators(), forces(), R(r), origen(o), symTime(time), addTime(add), next(0.0)
{	}
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
	//for (auto& f : forces) {
	//	if (f != nullptr) {
	//		delete f;
	//		f = nullptr;
	//	}
	//}
}
void ParticleSystem::activate(bool a) {
	for (auto& gen : generators) {
		gen->activate(a);
	}
}


void ParticleSystem::update(double t) {
	next += t;
	for (auto& g : generators) {
		if (next > addTime && g->getActive()) {
			Particle* p = g->addParticle();
			particle.push_back(p);
		}
	}
	if (next > addTime) next = 0.0;

	for (auto& it = particle.begin(); it != particle.end();) {
		Particle* p = *it;
		Vector3D fuerzas = { 0.0,0.0,0.0 };
		for (auto& f : forces) {
			if (f->getActive()) {
				f->updateTime(t);
				fuerzas = fuerzas + f->applyForce(p);
			}
		}
		p->updateForces(fuerzas);
		p->addTime(t);
		p->integrate(t);
		if ((p->getTime() > symTime && symTime != 0.0) || (p->getPos() - origen).magnitude() > R) {
			delete p;
			p = nullptr;
			it = particle.erase(it);
		}
		else ++it;
	}
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