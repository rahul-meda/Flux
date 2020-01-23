#pragma once

#include <glm/glm.hpp>

class Collider;
struct AABB
{
	AABB()
	{}

	AABB(const glm::vec3& min, const glm::vec3& max, Collider* c)
		: min(min), max(max), collider(c)
	{}

	glm::vec3 min;
	glm::vec3 max;

	Collider* collider;
};

bool Overlap(AABB* A, AABB* B);