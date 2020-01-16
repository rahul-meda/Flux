#pragma once

#include "../Simulation/Simulation.h"

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

// ToDo: store IDs which will be indices into containers?
class Body
{
private:
	float invMass;
	glm::mat3 iitL;
	glm::mat3 iitW;

	Transform tx;	

	glm::vec3 velocity;
	glm::vec3 angularVelocity;

	glm::vec3 force;
	glm::vec3 torque;

public:
	Body(const BodyDef& bd);

	const glm::vec3& GetPosition() const;
	const glm::quat& GetOrientation() const;
	void SetVelocity(const glm::vec3& v);
	const glm::vec3& GetVelocity() const;
	void SetAngularVelocity(const glm::vec3& w);
	const glm::vec3& GetAngularVelocity() const;

	friend class Physics;
};

inline const glm::vec3& Body::GetPosition() const
{
	return tx.position;
}

inline const glm::quat& Body::GetOrientation() const
{
	return tx.orientation;
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