
#include "NarrowPhase.h"
#include "Contact.h"
#include "SphereCollider.h"

void CollideSpheres(Manifold* manifold, SphereCollider* sphereA, SphereCollider* sphereB)
{
	manifold->nPoints = 0;

	Body* bodyA = sphereA->GetBody();
	Body* bodyB = sphereB->GetBody();
	Transform txA = bodyA->GetTransform();
	Transform txB = bodyB->GetTransform();

	glm::vec3 cA = txA.position + txA.R * sphereA->position;
	glm::vec3 cB = txB.position + txB.R * sphereB->position;

	glm::vec3 d = cB - cA;
	float dist2 = glm::dot(d, d);
	float rA = sphereA->radius;
	float rB = sphereB->radius;
	float r = rA + rB;
	if (dist2 > r * r)
		return;

	manifold->type = Manifold::spheres;
	manifold->localPoint = sphereA->position;
	manifold->localNormal = glm::vec3(0.0f);
	manifold->nPoints = 1;

	manifold->points[0].localPoint = sphereB->position;
	manifold->points[0].id.key = 0;
}