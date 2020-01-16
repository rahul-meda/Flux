#include "Physics.h"
#include "../Components/Body.h"

#define GRAVITY 0.0f

Physics::Physics()
	: singleStep(false)
{}

Physics& Physics::GetInstance()
{
	static Physics instance;
	return instance;
}

unsigned int Physics::AddBody(const BodyDef& bd)
{
	bodies.push_back(Body(bd));
	positions.push_back(Position(bd.tx.position, bd.tx.orientation));
	velocities.push_back(Velocity(bd.velocity, bd.angularVelocity));

	return bodies.size() - 1;
}

void Physics::Step(float dt)
{
	int nBodies = bodies.size();

	// integrate velocities
	for (int i = 0; i < nBodies; ++i)
	{
		Body b = bodies[i];
		glm::vec3 v = b.velocity;
		glm::vec3 w = b.angularVelocity;
		
		v += dt * (GRAVITY * glm::vec3(0.0f, -1.0f, 0.0f) + b.invMass * b.force);
		w += dt * b.iitW * b.torque;

		positions[i].p = b.GetPosition();
		positions[i].q = b.GetOrientation();
		velocities[i].v = v;
		velocities[i].w = w;
	}

	// detect collisions

	// solve constraints

	// post stabilization error correction

	// integrate positions
	for (int i = 0; i < nBodies; ++i)
	{
		glm::vec3 p = positions[i].p;
		glm::quat q = positions[i].q;
		glm::vec3 v = velocities[i].v;
		glm::vec3 w = velocities[i].w;

		// check for large velocities and adjust

		p += dt * v;
		q += dt * 0.5f * glm::quat(0.0f, w) * q;

		positions[i].p = p;
		positions[i].q = q;
	}

	for (int i = 0; i < nBodies; ++i)
	{
		bodies[i].tx.position = positions[i].p;
		bodies[i].tx.orientation = positions[i].q;
		bodies[i].velocity = velocities[i].v;
		bodies[i].angularVelocity = velocities[i].w;

		// calculate inertia and rotation
	}

	// reset forces
}

void Physics::Update(float dt)
{
	Step(dt);
}