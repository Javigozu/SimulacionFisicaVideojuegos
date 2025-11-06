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

//--------------------------------------!
//Ejes
RenderItem* _zero = NULL;
RenderItem* _xAxes = NULL;
RenderItem* _yAxes = NULL;
RenderItem* _zAxes = NULL;
//Escenario
std::vector<RenderItem*> walls;
//Particulas
PxGeometry* sphereSmall = NULL;
PxGeometry* sphereBig = NULL;
PxGeometry* cube = NULL;
//Proyectiles
std::vector<Projectile*> gun;
Particle* ball = nullptr; //pelota
Particle* px1 = nullptr; //particula explosion 1
Particle* px2 = nullptr; //particula explosion 1
Particle* smoke = nullptr; //particula Humo
//Parametros para la bola y la explosion
bool ballActive = false; Vector3D ballPos(0.0, 70.0, 30.0);
double expTime = 0.0, maxTime = 3.0;
//Sistemas
ParticleSystem* ballSys = NULL; //Sistema que guarda la pelota y gestiona las fuerzas
ParticleSystem* explosion = NULL; //Sistemas para la explosion
ParticleSystem* wind = NULL; //Sistema para el viento
//Fuerzas
WindGenerator* w = NULL;
GravityGenerator* g = NULL;
ExplosionGenerator* e = NULL;

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
void resetExp() {
	px1->setPos({ 0,0,0 });
	px2->setPos({ 0,0,0 });
	smoke->setPos({ 0,0,0 });
	e->setPos({ 0,0,0 });
	e->activate(false);
	explosion->activate(false);
}
void createBall() {
	ball->setPos(ballPos);
	ball->setAcc({ 0,0,0 });
	ball->setVel({ 0,0,0 });
}
void explode() {
	px1->setPos(ball->getPos());
	px2->setPos(ball->getPos());
	smoke->setPos(ball->getPos());
	e->setPos(ball->getPos());
	e->reset();
	e->activate(true);
	explosion->activate(true);
	explosion->setOrigen(ball->getPos());
	expTime = 0.0;
	createBall();
	ballActive = false;
}
void buildWalls() {
	Vector3D walltam(2.0, 70.0, 2.0);
	Vector3D wallpos(0.0, 0.0, 0.0);
	double offset = 100.0, windheight = 45, fantam = 10;
	//Side Walls
	RenderItem* LeftWall = new RenderItem(CreateShape(PxBoxGeometry(walltam.getX(), walltam.getY(), walltam.getZ())), new PxTransform(wallpos.getX(), wallpos.getY(), wallpos.getZ()), { 1,1,1,1 });
	RegisterRenderItem(LeftWall);
	walls.push_back(LeftWall);
	RenderItem* RightWall = new RenderItem(CreateShape(PxBoxGeometry(walltam.getX(), walltam.getY(), walltam.getZ())), new PxTransform(wallpos.getX(), wallpos.getY(), wallpos.getZ() + offset), { 1,1,1,1 });
	RegisterRenderItem(RightWall);
	walls.push_back(RightWall);
	//wind1
	wind = new ParticleSystem(offset, { wallpos.getX(),windheight, wallpos.getZ() }, 0.1, 2.0);
	w = new WindGenerator({ wallpos.getX() - fantam / 2,windheight - fantam / 2, wallpos.getZ() - fantam / 2 }, { fantam,fantam,offset }, { 0,0,offset });
	Particle* pw = new Particle(sphereSmall, { 0.8,1,1,1 }, { wallpos.getX(),windheight, wallpos.getZ() }, { 0, 0, 0 }, { 0.0, 0.0, 0.0 }, 1.0, 1.0, 0.0);
	ParticleGenerator* u = new UniformGenerator(pw, { fantam / 2,fantam / 2,0 }, { 0,0,0 }); //Generador uniforme
	wind->addGen(u);
	wind->addForce(w);
	//Suelo
	Vector3D floortam(2.0, 2.0, 30.0);
	RenderItem* floor1 = new RenderItem(CreateShape(PxBoxGeometry(floortam.getX(), floortam.getY(), floortam.getZ())), new PxTransform(wallpos.getX(), 30, wallpos.getZ() + floortam.getZ()), { 0,1,0,1 });
	RegisterRenderItem(floor1);
	walls.push_back(floor1);
	RenderItem* floor2 = new RenderItem(CreateShape(PxBoxGeometry(floortam.getX(), floortam.getY(), 5.0)), new PxTransform(wallpos.getX(), 30, offset - 5.0), { 0,1,0,1 });
	RegisterRenderItem(floor2);
	walls.push_back(floor2);
	//Obstaculos
	RenderItem* obstacle1 = new RenderItem(CreateShape(PxBoxGeometry(walltam.getX(), 6.0, walltam.getZ())), new PxTransform(wallpos.getX(), 20, wallpos.getZ() + 50), { 1,0.2,0.5,1 });
	RegisterRenderItem(obstacle1);
	walls.push_back(obstacle1);
	RenderItem* obstacle2 = new RenderItem(CreateShape(PxBoxGeometry(walltam.getX(), 6.0, walltam.getZ())), new PxTransform(wallpos.getX(), 20, wallpos.getZ() + 30), { 0.5,0.0,0.2,1 });
	RegisterRenderItem(obstacle2);
	walls.push_back(obstacle2);
	RenderItem* floor3 = new RenderItem(CreateShape(PxBoxGeometry(floortam.getX(), floortam.getY(), offset / 2 - 10.0)), new PxTransform(wallpos.getX(), 10, wallpos.getZ() + offset / 2 + 10.0), { 1,1,1,1 });
	RegisterRenderItem(floor3);
	walls.push_back(floor3);
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
	//Geometrias a usar
	sphereSmall = new PxSphereGeometry(0.3);
	sphereBig = new PxSphereGeometry(2.0);
	cube = new PxBoxGeometry(1.0, 1.0, 1.0);
	//Fuerza de Gravedad
	g = new GravityGenerator();
	//axes();
	//Viento y generador de escenario
	buildWalls();
	//Pelota y su sistema
	ball = new Particle(sphereBig, { 0.5,0,1,1 }, ballPos, { 0,0,0 }, { 0,0,0 }, 1.0, 0.7);
	ballSys = new ParticleSystem(2000, { 0,0,0 }, 0.0, 0.0);
	ballSys->addParticle(ball);
	ballSys->addForce(w);
	ballSys->addForce(g);
	//Explosion
	e = new ExplosionGenerator({ 0.0,0.0,0.0 }, 40, 800, 10, 5); //Fuerza
	px1 = new Particle(sphereSmall, { 1,0.8,0,1 }, { 0,0,0 }, { 0,0,0 }, { 0.0, 0.0, 0.0 }, 1.0, 0.8); //Particula ligera
	ParticleGenerator* x1 = new NormalGenerator(px1, { 1,1,1 }, { 0,0,0 }); //Generador normal 1
	px2 = new Particle(sphereSmall, { 1,0.2,0,1 }, { 0,0,0 }, { 0,0,0 }, { 0.0, 0.0, 0.0 }, 1.0, 2.0); //particula pesada
	ParticleGenerator* x2 = new NormalGenerator(px2, { 3,3,3 }, { 0,0,0 }); //Generador normal 2
	smoke = new Particle(sphereBig, { 0.2,0.2,0.2,1.0 }, { 0,0,0 }, { 0,10,0 }, { 0.0, 0.0, 0.0 }, 1.0, 2.0); //particula pesada
	ParticleGenerator* smk = new FountainGenerator(smoke, { 1,0,1 }, { 1,5,1 }); //Generador fuente
	explosion = new ParticleSystem(15.0, {0,0,0}, 0.1, 2.0); //Sistema
	explosion->addForce(e);
	explosion->addGen(x1);
	explosion->addGen(x2);
	explosion->addGen(smk);

	explosion->activate(false);
}

// Function to configure what happens in each step of physics
// interactive: true if the game is rendering, false if it offline
// t: time passed since last call in milliseconds
void stepPhysics(bool interactive, double t)
{
	PX_UNUSED(interactive);

	gScene->simulate(t);
	//----------------------------------!
	explosion->update(t);
	if (!ballActive && expTime < maxTime) {
		expTime += t;
	}
	if (expTime >= maxTime) resetExp();
	if (ballActive)
		ballSys->update(t);
	for (auto& g : gun) g->integrate(t);
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

	//DeregisterAxes();
	for (auto& w : walls) DeregisterRenderItem(w);
	for (auto& g : gun) delete g;
	delete explosion;
	delete ballSys;
	delete wind;
	delete sphereSmall;
	delete sphereBig;
	delete cube;
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
		createBall();
		ballActive = true;
		resetExp();
		break;
	case 'N':
		explode();
		break;
	case 'V':
		gun.push_back(new Projectile(cube, { 1,1,0,1 }, GetCamera()->getEye(), GetCamera()->getDir(), 800.0, 250.0, 1.0));
		break;
	case 'B':
		gun.push_back(new Projectile(sphereBig, { 1,0.3,0,1 }, GetCamera()->getEye(), GetCamera()->getDir(), 100.0, 70.0, 120.0));
		break;
	case 'X':
		wind->activate(!w->getActive());
		w->activate(!w->getActive());
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