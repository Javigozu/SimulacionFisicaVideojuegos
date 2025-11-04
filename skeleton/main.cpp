#include <ctype.h>

#include <PxPhysicsAPI.h>

#include <vector>

#include "core.hpp"
#include "RenderUtils.hpp"
#include "callbacks.hpp"

#include "Vector3D.h"
#include "Particle.h"
#include "Projectile.h"
#include "ParticleSystem.h"

#include <iostream>

std::string display_text = "This is a test";


using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;


PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;
ContactReportCallback gContactReportCallback;

RenderItem* _zero = NULL;
RenderItem* _xAxes = NULL;
RenderItem* _yAxes = NULL;
RenderItem* _zAxes = NULL;

//--------------------------------------!
//Particulas
PxGeometry* bullet1;
PxGeometry* bullet2;

ParticleSystem* gun;
ParticleSystem* system1;
ParticleSystem* wind;

WindGenerator* w;
GravityGenerator* g;
ExplosionGenerator* e;

void axes() {
	Vector3D xAxes(10.0f, 0.0f, 0.0f);
	Vector3D yAxes(0.0f, 10.0f, 0.0f);
	Vector3D zAxes(0.0f, 0.0f, 10.0f);
	Vector3D zero;
	float radio = 2.0f;

	_zero = new RenderItem(CreateShape(PxSphereGeometry(radio)), new PxTransform(zero.getX(), zero.getY(), zero.getZ()), { 1,1,1,1 });
	RegisterRenderItem(_zero);
	_xAxes = new RenderItem(CreateShape(PxSphereGeometry(radio)), new PxTransform(xAxes.getX(), xAxes.getY(), yAxes.getZ()), { 1,0,0,1 });
	RegisterRenderItem(_xAxes);
	_yAxes = new RenderItem(CreateShape(PxSphereGeometry(radio)), new PxTransform(yAxes.getX(), yAxes.getY(), yAxes.getZ()), { 0,1,0,1 });
	RegisterRenderItem(_yAxes);
	_zAxes = new RenderItem(CreateShape(PxSphereGeometry(radio)), new PxTransform(zAxes.getX(), zAxes.getY(), zAxes.getZ()), { 0,0,1,1 });
	RegisterRenderItem(_zAxes);
}
void DeregisterAxes() {
	DeregisterRenderItem(_zero);
	DeregisterRenderItem(_xAxes);
	DeregisterRenderItem(_yAxes);
	DeregisterRenderItem(_zAxes);
}

// Initialize physics engine
void initPhysics(bool interactive)
{
	PX_UNUSED(interactive);

	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// For Solid Rigids +++++++++++++++++++++++++++++++++++++
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = &gContactReportCallback;
	gScene = gPhysics->createScene(sceneDesc);

	//-------------------------------------!
	axes();
	bullet1 = new PxSphereGeometry(1.0);
	bullet2 = new PxSphereGeometry(3.0);

	gun = new ParticleSystem();
	g = new GravityGenerator();
	gun->addForce(g);

	wind = new ParticleSystem(0.2, 10.0);
	w = new WindGenerator({ -10,20,-5 }, { 20,40,60 }, { 0,3,100 });
	Particle* pw = new Particle(bullet1, { 0,1,1,1 }, { 0, 40, 10 }, { 0, 0, 0 }, { 0.0, 0.0, 0.0 }, 1.0, 1.0, 0.0);
	ParticleGenerator* u = new UniformGenerator(pw, { 10,5,10 }, { 0,0,0 });
	wind->addGen(u);
	wind->addForce(w);

	system1 = new ParticleSystem(0.3,10);
	e = new ExplosionGenerator({ 0.0,0.0,0.0 }, 20, 1000, 5, 4);
	system1->addForce(g);
	system1->addForce(e);
	system1->addForce(w);
	Particle* px = new Particle(bullet1, { 1,0,0,1 }, { 0,0,0 }, { 0,10,0 }, { 0.0, 0.0, 0.0 }, 0.7, 1.0, 0.0);
	ParticleGenerator* x = new UniformGenerator(px, { 2,2,2 }, { 0,0,0 });
	system1->addGen(x);
}

// Function to configure what happens in each step of physics
// interactive: true if the game is rendering, false if it offline
// t: time passed since last call in milliseconds
void stepPhysics(bool interactive, double t)
{
	PX_UNUSED(interactive);

	gScene->simulate(t);
	//----------------------------------!
	gun->update(t);
	system1->update(t);
	wind->update(t);

	//
	gScene->fetchResults(true);
}

// Function to clean data
// Add custom code to the begining of the function
void cleanupPhysics(bool interactive)
{
	PX_UNUSED(interactive);

	// Rigid Body ++++++++++++++++++++++++++++++++++++++++++
	gScene->release();
	gDispatcher->release();
	// -----------------------------------------------------
	gPhysics->release();
	PxPvdTransport* transport = gPvd->getTransport();
	gPvd->release();
	transport->release();

	gFoundation->release();

	DeregisterAxes();
	delete gun;
	delete system1;
	delete wind;
	delete bullet1;
	delete bullet2;
	delete g;
	delete w;
	delete e;
}

// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);

	switch (toupper(key))
	{
	case ' ':
		gun->addParticle(new Projectile(bullet1, { 1,1,0,1 }, GetCamera()->getEye(), GetCamera()->getDir(), 400.0, 40.0, 0.5));
		break;
	case 'B':
		gun->addParticle(new Projectile(bullet2, { 1,0.3,0,1 }, GetCamera()->getEye(), GetCamera()->getDir(), 10.0, 15.0, 0.65));
		break;
	case 'H':
		e->activate(true);//!e->getActive());
		e->reset();
		break;
	default:
		break;
	}
}

void onCollision(physx::PxActor* actor1, physx::PxActor* actor2)
{
	PX_UNUSED(actor1);
	PX_UNUSED(actor2);
}


int main(int, const char* const*)
{
#ifndef OFFLINE_EXECUTION 
	extern void renderLoop();
	renderLoop();
#else
	static const PxU32 frameCount = 100;
	initPhysics(false);
	for (PxU32 i = 0; i < frameCount; i++)
		stepPhysics(false);
	cleanupPhysics(false);
#endif
	return 0;
}