
#include "CapsuleCollider.h"
#include "Collision.h"

CapsuleCollider::CapsuleCollider()
{
	type = Capsule;
	massData = new MassData();
	com = glm::vec3(0.0f);
	upB = glm::vec3(0.0f, 1.0f, 0.0f);
	H = 1.0f;
	restitution = 0.2f;
	friction = 0.7f;
	radius = 1.0f;
	aabbIndex = 0;
}

void CapsuleCollider::ComputeAABB(AABB* aabb) const
{
	Transform tx = body->GetTransform();	// todo: store body transform?
	glm::vec3 center = tx.R * com + tx.position;
	glm::vec3 upW = tx.R * upB;
	glm::vec3 d = H * upW;
	glm::vec3 P = center - d;
	glm::vec3 Q = center + d;

	aabb->min = glm::vec3(glm::min(P.x, Q.x), glm::min(P.y, Q.y), glm::min(P.z, Q.z));
	aabb->max = glm::vec3(glm::max(P.x, Q.x), glm::max(P.y, Q.y), glm::max(P.z, Q.z));

	aabb->min -= glm::vec3(radius);
	aabb->max += glm::vec3(radius);
}

void CapsuleCollider::ComputeMass()
{
	float r2 = radius * radius;
	float h2 = 4.0f * H * H;
	float h = 2.0f * H;

	float VC = PI * r2 * h;
	float VS = (4.0f / 3.0f) * PI * r2 * radius;

	float d = massData->density;
	float MC = d * VC;
	float MS = d * VS;

	float Ixx = (1.0f / 12.0f) * MC * (3 * r2 + h2) + (2.0f / 5.0f) * MS * r2 +  (1.0f / 8.0f) * MS * (3.0f * radius + 2.0f * h) * h;
	float Iyy = MC * r2 * 0.5f + (2.0f / 5.0f) * MS * r2;

	massData->mass = MC + MS;

	massData->inertia[0] = glm::vec3(Ixx, 0.0f, 0.0f);
	massData->inertia[1] = glm::vec3(0.0f, Iyy, 0.0f);
	massData->inertia[2] = glm::vec3(0.0f, 0.0f, Ixx);
}

void CapsuleCollider::Scale(const float r)
{
	radius = r;
}