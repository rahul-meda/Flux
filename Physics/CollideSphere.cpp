
#include "NarrowPhase.h"
#include "Contact.h"
#include "SphereCollider.h"
#include "HullCollider.h"

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

	manifold->type = Manifold::sphere;
	manifold->localPoint = sphereA->position;
	manifold->localNormal = glm::vec3(0.0f);
	manifold->nPoints = 1;

	manifold->points[0].localPoint = sphereB->position;
	manifold->points[0].id.key = 0;
}

void CollideSphereHull(Manifold* manifold, SphereCollider* sphereA, HullCollider* hullB,
	const Transform& txA, const Transform& txB)
{
	DistanceOutput output;
	DistanceInput input;
	input.proxyA = DistanceProxy(sphereA);
	input.proxyB = DistanceProxy(hullB);
	input.txA = txA;
	input.txB = txB;

	GJK_Distance(&input, &output);

	if (output.distance > sphereA->radius + hullB->radius)
		return;

	if (output.distance <= 0.0f)	// deep contact
	{
		glm::vec3 C = txB.R * sphereA->com + txB.position;
		glm::vec3 CL = glm::transpose(txA.R) * (C - txA.position);

		float maxSep = -FLT_MAX;
		int bestFace = -1;

		int N = hullB->faces.size();
		for (int i = 0; i < N; ++i)
		{
			Face* face = hullB->faces[i];
			float sep = glm::dot(CL - face->edge->tail->position, face->normal);
			if (sep > 0.0f)
			{
				assert(false);
				return;
			}
			if (sep > maxSep)
			{
				maxSep = sep;
				bestFace = i;
			}
		}

		Face* face = hullB->faces[bestFace];
		manifold->type = Manifold::faceB;
		manifold->localPoint = face->edge->tail->position;
		manifold->localNormal = face->normal;
		manifold->nPoints = 1;
		manifold->points[0].localPoint = C;
		manifold->points[0].id.key = 0;
	}
	else
	{
		manifold->type = Manifold::sphere;
		manifold->localPoint = glm::transpose(txA.R) * (output.CA - txA.position);
		manifold->localNormal = glm::vec3(0.0f);
		manifold->nPoints = 1;
		manifold->points[0].localPoint = glm::transpose(txB.R) * (output.CB - txB.position);
		manifold->points[0].id.key = 0;
	}
}