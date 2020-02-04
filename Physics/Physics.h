#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../Components/Body.h"
#include "../Physics/BroadPhase.h"
#include "../Physics/ContactManager.h"

class Physics
{
private:
	Physics();

	~Physics();

public:
	static Physics& GetInstance();

	unsigned int AddBody(const BodyDef& bd);

	void Initialize();

	void Step(float dt);

	void Update(float dt);

	bool pause;
	bool singleStep;

	std::vector<Body*> bodies;
	std::vector<Position> positions;
	std::vector<Velocity> velocities;
	std::vector<Collider*> colliders;

	BroadPhase bp;
	ContactManager contactManager;
};