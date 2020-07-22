
#include "Physics.h"
#include "../Graphics/Graphics.h"
#include "../Components/Body.h"
#include "NarrowPhase.h"
#include "ContactSolver.h"
#include <ctime>
#include <iostream>

#define GRAVITY 9.8f

Physics::Physics()
	: singleStep(false), pause(true), debugDraw(false), slowMo(false)
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

void Physics::AddCollider(unsigned int bID, Collider* collider)
{
	bodies[bID]->AddCollider(collider);

	colliders.push_back(collider);

	contactManager.bp.Add(collider);
}

void Physics::Initialize()
{
	NarrowPhase::InitializeTable();
}

void Physics::Step(float dt)
{
	// detect collisions
	contactManager.bp.Update();

	contactManager.FindNewContacts();

	contactManager.Collide();

	int nBodies = bodies.size();

	int NS = springs.size();
	for (int i = 0; i < NS; ++i)
	{
		//springs[i].Solve();
	}

	// integrate velocities
	for (int i = 0; i < nBodies; ++i)
	{
		Body* b = bodies[i];
		if (b->isStatic) continue;

		glm::vec3 v = b->velocity;
		glm::vec3 w = b->angularVelocity;

		glm::mat3 R = b->tx.R;
		b->iitW = (R) * b->iitL * glm::transpose(R);
		
		v += dt * (GRAVITY * glm::vec3(0.0f, -1.0f, 0.0f) + b->invMass * b->force);
		w += dt * b->iitW * b->torque;
		v *= 0.9995f;
		w *= 0.995f;

		positions[i].c = b->GetCentroid();
		positions[i].q = b->GetOrientation();
		velocities[i].v = v;
		velocities[i].w = w;
	}

	// solve constraints
	SolverDef solverData;
	solverData.positions = &positions;
	solverData.velocities = &velocities;

	ContactSolverDef contactSolverDef;
	contactSolverDef.contacts = &(contactManager.contacts);
	contactSolverDef.positions = &positions;
	contactSolverDef.velocities = &velocities;

	ContactSolver contactSolver(&contactSolverDef);
	contactSolver.InitializeVelocityConstraints();

	contactSolver.WarmStart();

	int NP = posJoints.size();
	for (int i = 0; i < NP; ++i)
	{
		posJoints[i].InitVelocityConstraints(solverData);
	}

	int NH = hingeJoints.size();
	for (int i = 0; i < NH; ++i)
	{
		hingeJoints[i].InitVelocityConstraints(solverData);
	}

	int NU = uniJoints.size();
	for (int i = 0; i < NU; ++i)
	{
		uniJoints[i].InitVelocityConstraints(solverData);
	}

	int NW = wheelJoints.size();
	for (int i = 0; i < NW; ++i)
	{
		wheelJoints[i].InitVelocityConstraints(solverData);
	}

	int velocityIters = 8;
	for (int i = 0; i < velocityIters; ++i)
	{
		contactSolver.SolveVelocityConstraints();
	}

	contactSolver.StoreImpulses();

	velocityIters = 10;
	for (int i = 0; i < velocityIters; ++i)
	{
		for (int j = 0; j < NP; ++j)
		{
			posJoints[j].SolveVelocityConstraints(solverData);
		}

		for (int ih = 0; ih < NH; ++ih)
		{
			hingeJoints[ih].SolveVelocityConstraints(solverData);
		}

		for (int iu = 0; iu < NU; ++iu)
		{
			uniJoints[iu].SolveVelocityConstraints(solverData);
		}

		for (int iw = 0; iw < NW; ++iw)
		{
			wheelJoints[iw].SolveVelocityConstraints(solverData);
		}
	}


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

	int positionIters = 3;
	bool contactsOkay = false;
	for (int i = 0; i < positionIters; ++i)
	{
		for (int j = 0; j < NP; ++j)
		{
			posJoints[j].SolvePositionConstraints(solverData);
		}

		for (int ih = 0; ih < NH; ++ih)
		{
			hingeJoints[ih].SolvePositionConstraints(solverData);
		}

		for (int iu = 0; iu < NU; ++iu)
		{
			uniJoints[iu].SolvePositionConstraints(solverData);
		}

		for (int iw = 0; iw < NW; ++iw)
		{
			wheelJoints[iw].SolvePositionConstraints(solverData);
		}
	}

	positionIters = 5;
	for (int i = 0; i < positionIters; ++i)
	{
		minSep = contactSolver.SolvePositionConstraints();
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

		bodies[i]->SynchronizeTransform(i);
	}
}

void Physics::Update(float dt)
{
	dt = slowMo ? 0.1f * dt : dt;

	if (!pause)
	{
		Step(dt);
	}
	else if (singleStep)
	{
		Step(dt);
		singleStep = false;
	}

	if (debugDraw)
		contactManager.DebugDraw();

	int NP = posJoints.size();
	for (int i = 0; i < NP; ++i)
	{
		posJoints[i].Render();
	}

	int NH = hingeJoints.size();
	for (int i = 0; i < NH; ++i)
	{
		hingeJoints[i].Render();
	}

	int NU = uniJoints.size();
	for (int i = 0; i < NU; ++i)
	{
		//uniJoints[i].Render();
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