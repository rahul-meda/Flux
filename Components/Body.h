#pragma once

#include <vector>
#include "Transform.h"

struct BodyDef
{
	BodyDef()
	{
		tx = Transform(glm::vec3(0.0f));
		velocity = glm::vec3(0.0f);
		angularVelocity = glm::vec3(0.0f);
	}

	Transform tx;
	glm::vec3 velocity;
	glm::vec3 angularVelocity;
};

class Collider;
// ToDo: store IDs which will be indices into containers?
class Body
{
private:
	float invMass;
	glm::mat3 iitL;
	glm::mat3 iitW;

	glm::vec3 comL;
	glm::vec3 comW;
	glm::quat orientation;
	Transform tx;

	glm::vec3 velocity;
	glm::vec3 angularVelocity;

	glm::vec3 force;
	glm::vec3 torque;

	std::vector<Collider*> colliders;

public:
	Body(const BodyDef& bd);

	const glm::vec3& GetPosition() const;
	const glm::quat& GetOrientation() const;
	const glm::vec3& GetCentroid() const;
	void SetVelocity(const glm::vec3& v);
	const glm::vec3& GetVelocity() const;
	void SetAngularVelocity(const glm::vec3& w);
	const glm::vec3& GetAngularVelocity() const;

	void AddCollider(Collider* collider);

	void SynchronizeTransform();

	friend class Physics;
	friend class HullCollider;
};

inline const glm::vec3& Body::GetPosition() const
{
	return tx.position;
}

inline const glm::quat& Body::GetOrientation() const
{
	return orientation;
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