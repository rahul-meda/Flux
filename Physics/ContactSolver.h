#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Settings.h"

class Contact;
class Body;
struct Position;
struct Velocity;
struct ContactPositionConstraint;

struct VelocityConstraintPoint
{
	glm::vec3 rA;
	glm::vec3 rB;
	float normalImpulse;
	float tangentImpulse[2];
	float normalMass;
	float tangentMass[2];
	float velocityBias;
};

struct ContactVelocityConstraint
{
	VelocityConstraintPoint points[maxManifoldPoints];
	glm::vec3 normal;
	glm::vec3 tangent[2];
	glm::mat3 normalMass;
	glm::mat3 K;
	int indexA, indexB;
	float invMassA, invMassB;
	glm::mat3 invIA, invIB;
	float friction;
	float restitution;
	int pointCount;
	int contactIndex;
};

struct ContactSolverDef
{
	std::vector<Contact*>* contacts;
	std::vector<Position>* positions;
	std::vector<Velocity>* velocities;
};

class ContactSolver
{
public:
	ContactSolver(ContactSolverDef* def);
	~ContactSolver();

	void InitializeVelocityConstraints();

	void WarmStart();

	void SolveVelocityConstraints();
	void StoreImpulses();

	float SolvePositionConstraints();

	std::vector<Contact*>* contacts;
	std::vector<Position>* positions;
	std::vector<Velocity>* velocities;
	std::vector<ContactPositionConstraint> positionConstraints;
	std::vector<ContactVelocityConstraint> velocityConstraints;
};

