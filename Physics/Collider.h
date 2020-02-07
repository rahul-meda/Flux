#pragma once

#include <glm/glm.hpp>
#include "../Components/Body.h"

struct MassData
{
	MassData()
		: inertia(0.0f), com(0.0f), mass(0.0f){}
	glm::mat3 inertia;
	glm::vec3 com;	// body space
	float mass;
};

struct AABB;
// ToDo: Collider has a lot of 'cold' data which is never accessed
// after initilization. Separate the 'hot' and 'cold' parts to optimize cache
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

	virtual ~Collider() { delete massData; }

	virtual void ComputeAABB(AABB* aabb) const = 0;

	virtual void ComputeMass() = 0;

	Type GetType() const;

	Body* GetBody();
	const Body* GetBody() const;

	MassData* massData;
	float density;
	float restitution;
	float friction;
	Body* body;
	Type type;
	float radius;
	int aabbIndex;	// index of corresponding AABB in broad-phase	

	friend class Contact;
	friend class Body;
	friend class ContactSolver;
};

inline Collider::Type Collider::GetType() const
{
	return type;
}

inline Body* Collider::GetBody()
{
	return body;
}

inline const Body* Collider::GetBody() const
{
	return body;
}