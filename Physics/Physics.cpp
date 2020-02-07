
#include "Physics.h"
#include "../Components/Body.h"
#include "NarrowPhase.h"
#include "ContactSolver.h"

#define GRAVITY 3.0f

Physics::Physics()
	: singleStep(false), pause(true)
{}

Physics& Physics::GetInstance()
{
	static Physics instance;
	return instance;
}

unsigned int Physics::AddBody(const BodyDef& bd)
{
	Body* b = new Body(bd);
	bodies.push_back(b);
	b->index = bodies.size() - 1;
	positions.push_back(Position(b->comW, b->orientation));
	velocities.push_back(Velocity(b->velocity, b->angularVelocity));

	return b->index;
}

void Physics::Initialize()
{
	for (Body* b : bodies)
	{
		for (Collider* c : b->colliders)
			colliders.push_back(c);
	}

	for (Collider* c : colliders)
		contactManager.bp.Add(c);

	NarrowPhase::InitializeTable();
}

void Physics::Step(float dt)
{
	// detect collisions
	contactManager.bp.Update();

	contactManager.FindNewContacts();

	contactManager.Collide();

	int nBodies = bodies.size();

	// integrate velocities
	for (int i = 0; i < nBodies; ++i)
	{
		Body* b = bodies[i];
		if (b->isStatic) continue;

		glm::vec3 v = b->velocity;
		glm::vec3 w = b->angularVelocity;
		
		v += dt * (GRAVITY * glm::vec3(0.0f, -1.0f, 0.0f) + b->invMass * b->force);
		w += dt * b->iitW * b->torque;

		positions[i].c = b->GetCentroid();
		positions[i].q = b->GetOrientation();
		velocities[i].v = v;
		velocities[i].w = w;
	}

	// solve constraints
	ContactSolverDef contactSolverDef;
	contactSolverDef.contacts = &(contactManager.contacts);
	contactSolverDef.positions = &positions;
	contactSolverDef.velocities = &velocities;

	ContactSolver contactSolver(&contactSolverDef);
	contactSolver.InitializeVelocityConstraints();

	int velocityIters = 8;
	for (int i = 0; i < velocityIters; ++i)
	{
		contactSolver.SolveVelocityConstraints();
	}

	// post stabilization error correction

	// integrate positions
	for (int i = 0; i < nBodies; ++i)
	{
		if (bodies[i]->isStatic) continue;
		glm::vec3 c = positions[i].c;
		glm::quat q = positions[i].q;
		glm::vec3 v = velocities[i].v;
		glm::vec3 w = velocities[i].w;

		// check for large velocities and adjust

		c += dt * v;
		q += dt * 0.5f * glm::quat(0.0f, w) * q;

		positions[i].c = c;
		positions[i].q = q;
	}

	for (int i = 0; i < nBodies; ++i)
	{
		if (bodies[i]->isStatic) continue;
		bodies[i]->comW = positions[i].c;
		bodies[i]->orientation = positions[i].q;
		bodies[i]->velocity = velocities[i].v;
		bodies[i]->angularVelocity = velocities[i].w;

		bodies[i]->force = glm::vec3(0.0f);
		bodies[i]->torque = glm::vec3(0.0f);

		bodies[i]->SynchronizeTransform();
	}
}

void Physics::Update(float dt)
{
	if (!pause)
	{
		Step(dt);
	}
	else if (singleStep)
	{
		Step(dt);
		singleStep = false;
	}
}

Physics::~Physics()
{

	for (Body* b : bodies)
	{
		for (Collider* c : b->colliders)
			c = nullptr;

		delete b;
	}
}