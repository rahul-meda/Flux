
#include "Geometry.h"

#define EPSILON 0.005f

HalfSpace::HalfSpace()
{}

HalfSpace::HalfSpace(const glm::vec3& normal, float distance)
	: normal(normal), distance(distance)
{}

HalfSpace::HalfSpace(const glm::vec3& normal, const glm::vec3& point)
	: normal(normal), distance(glm::dot(point, normal))
{}

glm::vec3 HalfSpace::Origin() const
{
	return distance * normal;
}

float HalfSpace::Distance(const glm::vec3& point) const
{
	return glm::dot(point, normal) - distance;
}

glm::vec3 HalfSpace::Projection(const glm::vec3& point) const
{
	return point - Distance(point) * normal;
}

inline bool HalfSpace::Infront(const glm::vec3& point) const
{
	return Distance(point) > 0.0f;
}

inline bool HalfSpace::Behind(const glm::vec3& point) const
{
	return Distance(point) < 0.0f;
}

inline bool HalfSpace::On(const glm::vec3& point) const
{
	float dist = Distance(point);

	return (dist < EPSILON && dist > -EPSILON);
}