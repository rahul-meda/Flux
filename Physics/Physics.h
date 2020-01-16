#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../Components/Body.h"

struct Position
{
	Position(glm::vec3 p, glm::quat q)
		: p(p), q(q)
	{}
	glm::vec3 p;
	glm::quat q;
};

struct Velocity
{
	Velocity(glm::vec3 v, glm::vec3 w)
		: v(v), w(w)
	{}
	glm::vec3 v;
	glm::vec3 w;
};

class Physics
{
private:
	Physics();

public:
	static Physics& GetInstance();

	unsigned int AddBody(const BodyDef& bd);

	bool pause;

	bool singleStep;

	std::vector<Body> bodies;

	std::vector<Position> positions;

	std::vector<Velocity> velocities;

	void Step(float dt);

	void Update(float dt);
};