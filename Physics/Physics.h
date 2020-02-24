#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../Components/Body.h"
#include "ContactManager.h"
#include "../Physics/PositionJoint.h"

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

	std::vector<Body*> bodies;
	std::vector<Position> positions;
	std::vector<Velocity> velocities;
	std::vector<Collider*> colliders;
	std::vector<PositionJoint> posJoints;

	ContactManager contactManager;
};