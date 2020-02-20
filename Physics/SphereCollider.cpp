#include "SphereCollider.h"
#include "Collision.h"

SphereCollider::SphereCollider()
{
	type = Sphere;
	massData = new MassData();
	com = glm::vec3(0.0f);
	restitution = 0.3f;
	friction = 0.4f;
	radius = 1.0f;
	aabbIndex = 0;
}

void SphereCollider::ComputeAABB(AABB* aabb) const
{
	Transform tx = body->GetTransform();
	glm::vec3 center = tx.R * com + tx.position;
	aabb->min = center - glm::vec3(radius);
	aabb->max = center + glm::vec3(radius);
}

void SphereCollider::ComputeMass()
{
	float volume = (4.0f / 3.0f) * PI * radius * radius * radius;
	massData->mass = massData->density * volume;
	float I = (2.0f / 5.0f) * massData->mass * radius * radius;

	massData->inertia[0] = glm::vec3(I, 0.0f, 0.0f);
	massData->inertia[1] = glm::vec3(0.0f, I, 0.0f);
	massData->inertia[2] = glm::vec3(0.0f, 0.0f, I);
}

void SphereCollider::Scale(const float r)
{
	radius = r;
}