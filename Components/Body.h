#pragma once

#include <vector>
#include "Transform.h"

struct Position
{
	Position(glm::vec3 c, glm::quat q)
		: c(c), q(q)
	{}
	glm::vec3 c;
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

struct BodyDef
{
	BodyDef()
	{
		tx = Transform(glm::vec3(0.0f));
		velocity = glm::vec3(0.0f);
		angularVelocity = glm::vec3(0.0f);
		isStatic = false;
		filterID = 0;
	}

	Transform tx;
	glm::vec3 velocity;
	glm::vec3 angularVelocity;
	bool isStatic;
	unsigned int filterID;
};

class Collider;
struct ContactEdge;
// ToDo: padding, and separation into 'cold' and 'hot' parts for cache - ref Orange book
class Body
{
private:
	glm::mat3 iitL;
	glm::mat3 iitW;
	float invMass;

	glm::vec3 comL;
	glm::vec3 comW;
	glm::quat orientation;
	Transform tx;

	glm::vec3 velocity;
	glm::vec3 angularVelocity;

	glm::vec3 force;
	glm::vec3 torque;

	std::vector<Collider*> colliders;

	ContactEdge* edgeList;

	bool isStatic;
	unsigned int filterID;	// to filter collision groups	
	int index;				// index in world body list

public:
	Body(const BodyDef& bd);

	const float GetMass() const;
	const float GetInvMass() const;

	const glm::vec3& GetPosition() const;
	const glm::quat& GetOrientation() const;
	void SetOrientation(const glm::quat& q);
	const glm::vec3& GetCentroid() const;

	void SetVelocity(const glm::vec3& v);
	const glm::vec3& GetVelocity() const;
	void SetAngularVelocity(const glm::vec3& w);
	const glm::vec3& GetAngularVelocity() const;

	const Transform GetTransform() const;

	void ApplyForceCOM(const glm::vec3& f);
	void ApplyForce(const glm::vec3& f, const glm::vec3& p);
	void ApplyTorque(const glm::vec3& t);

	void AddCollider(Collider* collider);

	void SynchronizeTransform(int i);

	bool ShouldCollide(Body* other) const;

	void FixRotation();

	friend class Physics;
	friend class HullCollider;
	friend class ContactSolver;
	friend class ContactManager;

	friend class PositionJoint;
	friend class HingeJoint;
	friend class UniversalJoint;
};

inline const float Body::GetMass() const
{
	return (invMass == 0.0f ? 0.0f : 1.0f / invMass);
}

inline const float Body::GetInvMass() const
{
	return (invMass == 0.0f ? 0.0f : invMass);
}

inline const glm::vec3& Body::GetPosition() const
{
	return tx.position;
}

inline const glm::quat& Body::GetOrientation() const
{
	return orientation;
}

inline void Body::SetOrientation(const glm::quat& q)
{
	orientation = q;
}

inline const glm::vec3& Body::GetCentroid() const
{
	return comW;
}

inline void Body::SetVelocity(const glm::vec3& v)
{
	velocity = v;
}

inline const glm::vec3& Body::GetVelocity() const
{
	return velocity;
}

inline void Body::SetAngularVelocity(const glm::vec3& w)
{
	angularVelocity = w;
}

inline const glm::vec3& Body::GetAngularVelocity() const
{
	return angularVelocity;
}

inline const Transform Body::GetTransform() const
{
	return tx;
}

inline void Body::ApplyForceCOM(const glm::vec3& f)
{
	force += f;
}

inline void Body::ApplyForce(const glm::vec3& f, const glm::vec3& p)
{
	force += f;
	torque += glm::cross(p - comW, f);
}

inline void Body::ApplyTorque(const glm::vec3& t)
{
	torque += t;
}