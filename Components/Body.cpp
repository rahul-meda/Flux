#include "Body.h"
#include "../Physics/Collider.h"

Body::Body(const BodyDef& bd)
{
	invMass = 0.0f;
	iitL = glm::mat3(0.0f);
	iitW = glm::mat3(0.0f);

	comL = glm::vec3(0.0f);
	comW = glm::vec3(0.0f);
	orientation = glm::quat_cast(tx.R);
	tx = bd.tx;
	velocity = bd.velocity;
	angularVelocity = bd.angularVelocity;

	force = glm::vec3(0.0f);
	torque = glm::vec3(0.0f);
}

void Body::SynchronizeTransform()
{
	orientation = glm::normalize(orientation);	// needed every frame?
	tx.R = glm::toMat3(orientation);
	tx.position = comW - (tx.R * comL);
}

void Body::AddCollider(Collider* collider)
{
	collider->body = this;
	colliders.push_back(collider);

	/*if (invMass == 0.0f)
		return;*/

	collider->ComputeMass();

	float mass = 0.0f;
	comL = glm::vec3(0.0f);
	invMass = 0.0f;

	for (Collider* c : colliders)
	{
		mass += c->mass;
		comL += c->mass * c->com;
	}

	assert(mass != 0, "ERROR::Body has invalid mass");

	invMass = 1.0f / mass;
	comL *= invMass;
	comW = tx.position + (tx.R * comL);

	iitL = glm::mat3(0.0f);
	for (Collider* c : colliders)
	{
		glm::vec3 r = comL - c->com;
		float rDotr = glm::dot(r, r);
		glm::mat3 rOutr = glm::outerProduct(r, r);

		// PAT
		iitL += c->inertia + c->mass * (rDotr * glm::mat3(1.0f) - rOutr);
	}
	iitL = glm::inverse(iitL);
	iitW = tx.R * iitL * glm::transpose(tx.R);
}