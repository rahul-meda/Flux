
#include "NarrowPhase.h"
#include "Contact.h"
#include "SphereCollider.h"
#include "HullCollider.h"

void CollideSpheres(Manifold* manifold, SphereCollider* sphereA, SphereCollider* sphereB,
										const Transform& txA, const Transform& txB)
{
	manifold->nPoints = 0;

	glm::vec3 cA = txA.position + txA.R * sphereA->com;
	glm::vec3 cB = txB.position + txB.R * sphereB->com;

	glm::vec3 d = cB - cA;
	float dist2 = glm::dot(d, d);
	float r = sphereA->radius + sphereB->radius;
	if (dist2 > r * r)
		return;

	manifold->type = Manifold::sphere;
	manifold->localPoint = sphereA->com;
	manifold->localNormal = glm::vec3(0.0f);
	manifold->nPoints = 1;

	manifold->points[0].localPoint = sphereB->com;
	manifold->points[0].id.key = 0;
}

void CollideSphereHull(Manifold* manifold, SphereCollider* sphereA, HullCollider* hullB,
										   const Transform& txA, const Transform& txB)
{
	manifold->nPoints = 0;

	DistanceOutput output;
	DistanceInput input;
	input.proxyA = DistanceProxy(sphereA);
	input.proxyB = DistanceProxy(hullB);
	input.txA = txA;
	input.txB = txB;

	GJK_Distance(&input, &output);

	float r = sphereA->radius;

	if (output.distance > r + hullB->radius)
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
		float d2 = glm::length2(output.CB - output.CA);
		if (d2 > r * r)
			return;

		manifold->type = Manifold::sphere;
		manifold->localPoint = sphereA->com;// glm::transpose(txA.R) * (output.CA - txA.position);
		manifold->localNormal = glm::vec3(0.0f);
		manifold->nPoints = 1;
		manifold->points[0].localPoint = glm::transpose(txB.R) * (output.CB - txB.position);
		manifold->points[0].id.key = 0;
	}
}