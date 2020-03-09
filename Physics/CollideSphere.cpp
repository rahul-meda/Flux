
#include "NarrowPhase.h"
#include "Contact.h"
#include "SphereCollider.h"
#include "HullCollider.h"
#include "CapsuleCollider.h"

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
	input.txB = FxMul(txB, hullB->txB);

	GJK_Distance(&input, &output);

	float r = sphereA->radius;

	if (output.distance > r + hullB->radius)
		return;

	if (output.distance <= 0.0f)	// deep contact // todo: fix it!
	{
		int x = 1;
		return;

		glm::vec3 C = txB.R * sphereA->com + txB.position;
		glm::vec3 CL = glm::transpose(txA.R) * (C - txA.position);

		float maxSep = -FLT_MAX;
		int bestFace = -1;

		int N = hullB->faces.size();
		for (int i = 0; i < N; ++i)
		{
			Face* face = hullB->faces[i];
			float sep = glm::dot(CL - face->edge->tail->position, face->normal);

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
		manifold->localPoint = sphereA->com;
		manifold->localNormal = glm::vec3(0.0f);
		manifold->nPoints = 1;
		manifold->points[0].localPoint = glm::transpose(txB.R) * (output.CB - txB.position);
		manifold->points[0].id.key = 0;
	}
}

void CollideSphereCapsule(Manifold* m, SphereCollider* A, CapsuleCollider* B, const Transform& txA, const Transform& txB)
{
	m->nPoints = 0;

	SphereCollider* cA = (SphereCollider*)A;
	CapsuleCollider* cB = (CapsuleCollider*)B;
	float radius = cA->radius + cB->radius;
	glm::vec3 CA = txA.R * A->com + txA.position;
	glm::vec3 CB = txB.R * B->com + txB.position;
	glm::vec3 upW = txB.R * cB->upB;
	glm::vec3 P = CB - B->H * upW;
	glm::vec3 Q = CB + B->H * upW;
	glm::vec3 PC = CA - P;
	glm::vec3 CQ = Q - CA;
	float d1 = glm::dot(PC, upW);
	float d2 = glm::dot(CQ, upW);

	if (d1 <= 0.0f)
	{
		float l2 = glm::length2(PC);

		if (l2 > radius * radius)
			return;

		m->type = Manifold::sphere;
		m->localPoint = A->com;
		m->nPoints = 1;
		m->points[0].localPoint = glm::transpose(txB.R) * (P - txB.position);
		m->points[0].id.key = 0;
	}
	else if (d2 <= 0.0f)
	{
		float l2 = glm::length2(CQ);

		if (l2 > radius * radius)
			return;

		m->type = Manifold::sphere;
		m->localPoint = A->com;
		m->nPoints = 1;
		m->points[0].localPoint = glm::transpose(txB.R) * (Q - txB.position);
		m->points[0].id.key = 0;
	}
	else
	{
		upW = glm::normalize(upW);
		glm::vec3 d = PC - upW * glm::dot(PC, upW);
		float l2 = glm::dot(d, d);

		if (l2 > radius * radius)
			return;

		m->type = Manifold::sphere;
		m->localPoint = A->com;
		m->nPoints = 1;
		m->points[0].localPoint = glm::transpose(txB.R) * (CA - d - txB.position);
		m->points[0].id.key = 0;
	}
}