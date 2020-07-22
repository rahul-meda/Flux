
#include "Body.h"
#include "../Physics/Collider.h"
#include "../Graphics/Graphics.h"

Body::Body(const BodyDef& bd)
{
	isStatic = bd.isStatic;
	invMass = 0.0f;
	iitL = glm::mat3(0.0f);
	iitW = glm::mat3(0.0f);

	tx = bd.tx;
	comL = glm::vec3(0.0f);
	comW = tx.position;
	orientation = glm::quat_cast(tx.R);
	velocity = bd.velocity;
	angularVelocity = bd.angularVelocity;

	force = glm::vec3(0.0f);
	torque = glm::vec3(0.0f);

	filterID = bd.filterID;
}

void Body::SynchronizeTransform(int i)
{
	orientation = glm::normalize(orientation);	// needed every frame?
	tx.R = glm::toMat3(orientation);
	tx.position = comW - (tx.R * comL);

	R_Mesh* obj = &Graphics::GetInstance().objects[i];
	obj->pos = tx.position;
	obj->rot = tx.R;
}

void Body::AddCollider(Collider* collider)
{
	collider->body = this;
	colliders.push_back(collider);

	if (isStatic)
		return;

	collider->ComputeMass();

	float mass = 0.0f;
	comL = glm::vec3(0.0f);
	invMass = 0.0f;

	for (Collider* c : colliders)
	{
		MassData* md = c->massData;
		mass += md->mass;
		comL += md->mass * c->com;
	}

	assert(mass != 0, "ERROR::Body has invalid mass");

	invMass = 1.0f / mass;
	comL *= invMass;
	comW = tx.position + (tx.R * comL);

	iitL = glm::mat3(0.0f);
	for (Collider* c : colliders)
	{
		MassData* md = c->massData;
		glm::vec3 r = comL - c->com;
		float rDotr = glm::dot(r, r);
		glm::mat3 rOutr = glm::outerProduct(r, r);

		// PAT
		iitL += md->inertia + md->mass * (rDotr * glm::mat3(1.0f) - rOutr);
	}
	iitL = glm::inverse(iitL);
	iitW = tx.R * iitL * glm::transpose(tx.R);
}

bool Body::ShouldCollide(Body* other) const
{
	if (isStatic && other->isStatic)
		return false;

	if ((filterID != 0 || other->filterID != 0) && (filterID == other->filterID))
		return false;

	return true;
}

void Body::FixRotation()
{
	iitL = glm::mat3(0.0f);
	iitW = glm::mat3(0.0f);
}
