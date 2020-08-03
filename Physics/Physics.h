#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../Components/Body.h"
#include "ContactManager.h"
#include "PositionJoint.h"
#include "HingeJoint.h"
#include "UniversalJoint.h"
#include "WheelJoint.h"
#include "SpringConstraint.h"

class Physics
{
private:
	Physics();

	~Physics();

public:
	static Physics& GetInstance();

	unsigned int AddBody(const BodyDef& bd);

	void AddCollider(unsigned int bID, Collider* collider);

	void Initialize();

	void Step(float dt);

	void Update(float dt);

	bool pause;
	bool singleStep;
	bool debugDraw;
	bool slowMo;
	float minSep;

	std::vector<Body*> bodies;
	std::vector<Transform> transforms;
	std::vector<Position> positions;
	std::vector<Velocity> velocities;
	std::vector<Collider*> colliders;
	std::vector<PositionJoint> posJoints;
	std::vector<HingeJoint> hingeJoints;
	std::vector<UniversalJoint> uniJoints;
	std::vector<WheelJoint> wheelJoints;
	std::vector<Spring> springs;

	ContactManager contactManager;
};