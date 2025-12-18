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
#include "SolidSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

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
std::vector<RenderItem*> items;
std::vector<physx::PxRigidActor*> obstacles;
//Geometría partículas
PxGeometry* sphereSmall = NULL;
PxGeometry* sphereBig = NULL;
PxGeometry* cube = NULL;
//Proyectiles
std::vector<Projectile*> gun;
//Explosion
Particle* px1 = nullptr; //particula explosion 1
Particle* px2 = nullptr; //particula explosion 1
Particle* smoke = nullptr; //particula Humo
//Parametros para la bola y la explosion
bool ballActive = true; Vector3D ballPos;
double expTime = 0.0, maxTime = 3.0;
bool dead = false; bool resetPos = false;
//Sistemas
SolidSystem* ballSys = NULL; //Sistema que guarda la pelota y gestiona las fuerzas
PxRigidDynamic* ball = NULL;
PxRigidStatic* meta = NULL;
ParticleSystem* deadExp = NULL; //Sistemas para la explosion
ParticleSystem* festive = NULL;
bool win = false;
ParticleSystem* expSys = NULL;
std::vector<SolidSystem*> springSys;
ParticleSystem* windSys = NULL; //Sistema para el viento
bool windActive = true;
//Fuerzas
std::vector<WindGenerator*> windForce;
std::vector<ExplosionGenerator*> expForce;
std::vector<ForceGenerator*> flotForce;
GravityGenerator* g = NULL;
ExplosionGenerator* e = NULL;

PxRigidStatic* createRBStat(PxGeometry* geo, const PxTransform tr, Vector4 color, const PxMaterial* mat = nullptr) {
	PxRigidStatic* rb;
	rb = gPhysics->createRigidStatic(tr);
	PxShape* shape = CreateShape(*geo, mat);
	rb->attachShape(*shape);
	gScene->addActor(*rb);

	RenderItem* item;
	item = new RenderItem(shape, rb, color);
	RegisterRenderItem(item);
	items.push_back(item);
	return rb;
}
PxRigidDynamic* createRBDin(PxGeometry* geo, const PxTransform tr, double density, Vector4 color, const PxMaterial* mat = nullptr) {
	PxRigidDynamic* rb;
	rb = gPhysics->createRigidDynamic(tr);
	PxShape* shape = CreateShape(*geo, mat);
	rb->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*rb, density);
	gScene->addActor(*rb);

	rb->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_LINEAR_X);

	RenderItem* item;
	item = new RenderItem(shape, rb, color);
	RegisterRenderItem(item);
	items.push_back(item);
	return rb;
}
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
	px1->setPos({ -100,0,0 });
	px2->setPos({ -100,0,0 });
	smoke->setPos({ -100,0,0 });
	e->setPos({ -100,0,0 });
	e->activate(false);
	deadExp->activate(false);
}
void resetBall() {
	ball->setGlobalPose(PxTransform(PxVec3(ballPos.getX(), ballPos.getY(), ballPos.getZ())));
	ball->clearForce();
	ball->setLinearVelocity(PxVec3(0, 0, 0));
	ballActive = true;
	resetExp();
}
void explode() {
	px1->setPos(ball->getGlobalPose().p);
	px2->setPos(ball->getGlobalPose().p);
	smoke->setPos(ball->getGlobalPose().p);
	deadExp->setOrigen(ball->getGlobalPose().p);
	e->setPos(ball->getGlobalPose().p);
	e->reset();
	e->activate(true);
	deadExp->activate(true);
	expTime = 0.0;
	ballActive = false;
}
void walls() {
	g = new GravityGenerator();
	windSys = new ParticleSystem(10000, { 0,0,0 }, 0.1, 2.0);
	expSys = new ParticleSystem(10000, { 0,0,0 }, 0.1, 1.0);
	festive = new ParticleSystem(10000, { 0,0,0 }, 0.05, 2.5);
	festive->addForce(g);

	Vector3D zero(0, 70, 0);
	Vector3D tam(2.0, 2.0, 2.0);
	std::ifstream file;
	file.open("map.txt");
	std::string s;
	int j = 0;
	while (getline(file, s)) {
		Vector3D ini = zero;
		ini.setY(ini.getY() - tam.getY() * j * 2);

		std::istringstream line(s);
		for (int i = 0; i < s.size(); ++i) {
			char c; line >> c;
			Vector3D pos = ini;
			pos.setZ(tam.getZ() * i * 2);

			Particle* pw;
			WindGenerator* w;
			ParticleGenerator* u;

			Particle* exp;
			ParticleGenerator* n;
			ExplosionGenerator* gen;

			SolidSystem* sSys;
			SpringAnchorGenerator* spring;
			PxRigidDynamic* rb;

			PxRigidActor* actor;

			BuoyancyGenerator* flot;

			physx::PxVec4 color;
			switch (c) {
			case 'x':
				createRBStat(new PxBoxGeometry(tam.getX(), tam.getY(), tam.getZ()), PxTransform(pos.getX(), pos.getY(), pos.getZ()), { 1,1,1,1 });
				break;
			case 'b':
				ballPos = pos;
				break;
			case '<':
				w = new WindGenerator({ pos.getX() - tam.getX(),pos.getY() - tam.getY(), zero.getZ() - tam.getZ() }, { tam.getX() * 2,tam.getY() * 2,s.size() * 2 * tam.getZ() }, { 0,0,-100 });
				windForce.push_back(w);
				pw = new Particle(sphereSmall, { 0.8,1,1,1 }, { pos.getX(),pos.getY(), pos.getZ() }, { 0, 0, 0 }, { 0.0, 0.0, 0.0 }, 1.0, 1.0, 0.0);
				u = new UniformGenerator(pw, { tam.getX(),tam.getY(),0 }, { 0,0,0 }); //Generador uniforme
				windSys->addGen(u);
				windSys->addForce(w);

				createRBStat(new PxBoxGeometry(tam.getX(), tam.getY(), tam.getZ()), PxTransform(pos.getX(), pos.getY(), pos.getZ()), { 0.8,0.7,1.0,1 });
				break;
			case '>':
				w = new WindGenerator({ pos.getX() - tam.getX(),pos.getY() - tam.getY(), pos.getZ() - tam.getZ() }, { tam.getX() * 2,tam.getY() * 2,s.size() * 2 * tam.getZ() }, { 0,0,100 });
				windForce.push_back(w);
				pw = new Particle(sphereSmall, { 0.8,1,1,1 }, { pos.getX(),pos.getY(), pos.getZ() }, { 0, 0, 0 }, { 0.0, 0.0, 0.0 }, 1.0, 1.0, 0.0);
				u = new UniformGenerator(pw, { tam.getX(),tam.getY(),0 }, { 0,0,0 }); //Generador uniforme
				windSys->addGen(u);
				windSys->addForce(w);

				createRBStat(new PxBoxGeometry(tam.getX(), tam.getY(), tam.getZ()), PxTransform(pos.getX(), pos.getY(), pos.getZ()), { 0.8,0.7,1.0,1 });
				break;
			case 'a':
				actor = createRBStat(new PxBoxGeometry(tam.getX(), tam.getY(), tam.getZ()), PxTransform(pos.getX(), pos.getY(), pos.getZ()), { 0,1,0,1 });
				obstacles.push_back(actor);
				break;
			case 'm':
				sSys = new SolidSystem();
				rb = createRBDin(new PxBoxGeometry(tam.getX(), tam.getY() * 2, tam.getZ()), PxTransform(pos.getX(), pos.getY() - tam.getY() * 4, pos.getZ()), 4, { 0,0.7,0,1 });
				rb->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z);
				sSys->addBody(rb);
				spring = new SpringAnchorGenerator(pos, 1300, tam.getY() * 4);
				sSys->addForce(spring);
				springSys.push_back(sSys);
				obstacles.push_back(rb);
				break;
			case 'e':
				createRBStat(new PxBoxGeometry(tam.getX(), tam.getY(), tam.getZ()), PxTransform(pos.getX(), pos.getY(), pos.getZ()), { 1,0.8,0,1 });
				exp = new Particle(sphereSmall, { 1,0.8,0,1 }, { pos.getX(),pos.getY(), pos.getZ() }, { 0, 0, 0 }, { 0.0, 0.0, 0.0 }, 1.0, 1.0, 0.0);
				n = new NormalGenerator(exp, tam / 2, { 0,0,0 });
				gen = new ExplosionGenerator(pos, 25, 400, 4, 5);
				expSys->addGen(n); expSys->addForce(gen);
				expForce.push_back(gen);
				break;
			case 'l':
				flot = new BuoyancyGenerator({ pos.getX() - tam.getX(),pos.getY() - tam.getY(), pos.getZ() - tam.getZ() }, { tam.getX() * 2,tam.getY() * 2, tam.getZ() * 2 }, 4, 33.5, 10);
				w = new WindGenerator({ pos.getX() - tam.getX(),pos.getY() - tam.getY(), pos.getZ() - tam.getZ() }, { tam.getX() * 2,tam.getY() * 2,tam.getZ() * 2 }, { 0,0,-10 });
				flotForce.push_back(flot);
				flotForce.push_back(w);
				break;
			case 'r':
				meta = createRBStat(new PxBoxGeometry(tam.getX(), tam.getY(), tam.getZ()), PxTransform(pos.getX(), pos.getY(), pos.getZ()), { 0.9,0.2,0,3 });
				break;
			case 'f':
			case 'g':
			case 'h':
			case 'j':
				if (c == 'f') color = { 1,0.1,0.4,1 };
				else if (c == 'g') color = { 1.0,0.75,0.1,1 };
				else if (c == 'h') color = { 0.1,0.9,0.7,1 };
				else if (c == 'j') color = { 0.3,0.1,1,1 };
				pw = new Particle(sphereSmall, color, { pos.getX(),pos.getY(), pos.getZ() }, { 0,10, 0 }, { 0.0, 0.0, 0.0 }, 1.0, 0.3);
				u = new FountainGenerator(pw, { 1,1,1 }, { 5,20,5 });
				festive->addGen(u); festive->activate(false);
				break;
			case'1':
			case'2':
			case'3':
			case'4':
			case'5':
			case'6':
			case'7':
			case'8':
			case'9':
				physx::PxQuat q(0.44, physx::PxVec3(1, 0, 0));
				createRBStat(new PxBoxGeometry(tam.getX(), tam.getY(), tam.getZ() * (c - '0')), PxTransform(pos.getX(), pos.getY(), pos.getZ(), q), { 1,1,1,1 });
				break;
			}
		}
		++j;
	}
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
	//axes();
	//Generador de escenario
	walls();
	//Pelota y su sistema
	ball = createRBDin(new PxSphereGeometry(2.0), PxTransform(ballPos.getX(), ballPos.getY(), ballPos.getZ()), 0.01, { 0.5,0,1,1 });
	ballSys = new SolidSystem();
	ballSys->addBody(ball);
	for (auto& w : windForce)
		ballSys->addForce(w);
	for (auto& e : expForce)
		ballSys->addForce(e);
	for (auto& f : flotForce)
		ballSys->addForce(f);

	//Explosion
	px1 = new Particle(sphereSmall, { 1,0.8,0,1 }, { 100,0,0 }, { 0,0,0 }, { 0.0, 0.0, 0.0 }, 1.0, 0.7); //Particula ligera
	ParticleGenerator* x1 = new NormalGenerator(px1, { 1,1,1 }, { 0,0,0 }); //Generador normal 1
	px2 = new Particle(sphereSmall, { 1,0.2,0,1 }, { 100,0,0 }, { 0,0,0 }, { 0.0, 0.0, 0.0 }, 1.0, 3.0); //particula pesada
	ParticleGenerator* x2 = new NormalGenerator(px2, { 3,3,3 }, { 0,0,0 }); //Generador normal 2
	smoke = new Particle(sphereBig, { 0.2,0.2,0.2,1.0 }, { 100,0,0 }, { 0,10,0 }, { 0.0, 0.0, 0.0 }, 1.0, 2.0); //humo
	ParticleGenerator* smk = new FountainGenerator(smoke, { 1,0,1 }, { 1,5,1 }); //Generador fuente
	deadExp = new ParticleSystem(15.0, { 0,0,0 }, 0.1, 2.0); //Sistema
	deadExp->addGen(x1);
	deadExp->addGen(x2);
	deadExp->addGen(smk);
	e = new ExplosionGenerator({ 0.0,0.0,0.0 }, 10, 800, 10, 5); //Fuerza
	deadExp->addForce(e);
	deadExp->activate(false);
	resetExp();
}

// Function to configure what happens in each step of physics
// interactive: true if the game is rendering, false if it offline
// t: time passed since last call in milliseconds
void stepPhysics(bool interactive, double t)
{
	PX_UNUSED(interactive);

	if (resetPos) {
		resetPos = false;
		//ball->setGlobalPose(PxTransform(PxVec3(-100, ballPos.getY(), ballPos.getZ())));
	}

	gScene->simulate(t);
	//----------------------------------!
	deadExp->update(t);
	if (!ballActive && expTime < maxTime) {
		expTime += t;
		if (expTime >= maxTime) resetBall();
	}

	//for (auto& g : gun) g->integrate(t);
	windSys->update(t);
	for (auto& e : expForce) e->updateTime(t);
	expSys->update(t);
	for (auto& s : springSys)s->update(t);

	ballSys->update(t);

	if (dead) {
		dead = false;
		resetPos = true;
		//explode();
	}
	if (win) {
		festive->update(t);
	}

	if (ballActive) GetCamera()->setCamera(PxVec3(GetCamera()->getEye().x, ball->getGlobalPose().p.y, GetCamera()->getEye().z));

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

	for (auto& w : items) DeregisterRenderItem(w);
	for (auto& g : gun) delete g;
	delete deadExp;
	delete windSys;
	delete sphereSmall;
	delete sphereBig;
	delete cube;
	delete g;
	delete e;
}
// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);

	switch (toupper(key))
	{
	case 'Z':
		windActive = !windActive;
		windSys->activate(windActive);
		for (auto& w : windForce) w->activate(windActive);
		break;
	case 'X':
		for (auto& e : expForce) { e->activate(true); e->reset(); }
		break;
	default:
		break;
	}
}

void onCollision(physx::PxActor* actor1, physx::PxActor* actor2)
{
	PX_UNUSED(actor1);
	PX_UNUSED(actor2);
	if (actor1 == ball) {
		if (actor2 == meta) { win = true; festive->activate(true); }
		for (auto& o : obstacles) {
			if (actor2 == o) {
				std::cout << "MUERTO\n";
				dead = true;
				break;
			}
		}
	}
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