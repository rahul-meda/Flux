#pragma once

#include <vector>
#include <glm/glm.hpp>

#define PI 3.14159265359f

struct HalfSpace
{
	glm::vec3 normal;
	float distance;

	HalfSpace();
	HalfSpace(const glm::vec3& normal, const float distance);
	HalfSpace(const glm::vec3& normal, const glm::vec3& point);

	glm::vec3 Origin() const;
	float Distance(const glm::vec3& point) const;
	glm::vec3 Projection(const glm::vec3& point) const;

	bool Infront(const glm::vec3& point) const;
	bool Behind(const glm::vec3& point) const;
	bool On(const glm::vec3& point) const;
};

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

void ComputeBasis(const glm::vec3& v1, glm::vec3* v2, glm::vec3* v3);

#include "Geometry.inl"
