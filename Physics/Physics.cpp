
#include "Physics.h"
#include "../Graphics/Graphics.h"
#include "../Components/Body.h"
#include "NarrowPhase.h"
#include "ContactSolver.h"
#include <ctime>

#define GRAVITY 9.8f

Physics::Physics()
	: singleStep(false), pause(true), debugDraw(false)
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
	DistanceOutput output;
	DistanceInput input;
	input.proxyA = DistanceProxy(bodies[0]->colliders[0]);
	input.proxyB = DistanceProxy(bodies[1]->colliders[0]);
	input.txA = bodies[0]->tx;
	input.txB = bodies[1]->tx;

	GJK_Distance(&input, &output);

	// detect collisions
	contactManager.bp.Update();

	contactManager.FindNewContacts();

	std::clock_t t0 = std::clock();
	contactManager.Collide();
	std::clock_t t1 = std::clock();
	double elapsed = (t1 - t0) / (double)CLOCKS_PER_SEC;

	int nBodies = bodies.size();

	// integrate velocities
	for (int i = 0; i < nBodies; ++i)
	{
		Body* b = bodies[i];
		if (b->isStatic) continue;

		glm::vec3 v = b->velocity;
		glm::vec3 w = b->angularVelocity;

		b->iitW = b->tx.R * b->iitL * glm::transpose(b->tx.R);
		
		v += dt * (GRAVITY * glm::vec3(0.0f, -1.0f, 0.0f) + b->invMass * b->force);
		w += dt * b->iitW * b->torque;
		v *= 0.995f;
		w *= 0.995f;

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

	contactSolver.WarmStart();

	int velocityIters = 25;
	for (int i = 0; i < velocityIters; ++i)
	{
		contactSolver.SolveVelocityConstraints();
	}

	contactSolver.StoreImpulses();

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
		glm::vec3 translation = v * dt;
		if (glm::dot(translation, translation) > maxTranslation2)
		{
			float ratio = maxTranslation / glm::length(translation);
			v *= ratio;
		}

		glm::vec3 rotation = w * dt;
		if (glm::dot(rotation, rotation) > maxRotation2)
		{
			float ratio = maxRotation / glm::length(rotation);
			w *= ratio;
		}

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

	Graphics::GetInstance().points.clear();
	Graphics::GetInstance().lines.clear();
	if (debugDraw)
	{
		contactManager.DebugDraw();
	}

	Graphics::GetInstance().points.push_back(R_Point(output.CA));
	Graphics::GetInstance().points.push_back(R_Point(output.CB));
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