#pragma once

#include <glm/glm.hpp>

class Body;
struct AABB;
class Collider
{
public:
	enum Type
	{
		Sphere = 0,
		Cylinder,
		Capsule,
		Cone,
		ConvexHull,
		nShapes
	};

	virtual ~Collider() {}

	virtual Collider* Clone() const = 0;

	virtual void ComputeAABB(AABB* aabb) const = 0;

	virtual void ComputeMass() = 0;

	Type GetType() const;

	Type type;

	float mass;
	float density;
	glm::vec3 com;	// body space
	glm::mat3 inertia;
	Body* body;
};

inline Collider::Type Collider::GetType() const
{
	return type;
}