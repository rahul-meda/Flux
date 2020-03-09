
#include "NarrowPhase.h"
#include "Contact.h"
#include "HullCollider.h"
#include "CapsuleCollider.h"
#include "../Mesh/Geometry.h"

void CollideCapsules(Manifold* m, CapsuleCollider* A, CapsuleCollider* B, const Transform& txA, const Transform& txB)
{
	m->nPoints = 0;

	DistanceOutput output;
	DistanceInput input;
	input.proxyA = DistanceProxy(A);
	input.proxyB = DistanceProxy(B);
	input.txA = txA;
	input.txB = txB;

	GJK_Distance(&input, &output);

	float radius = A->radius + B->radius;
	if (output.distance < radius)
	{
		m->type = Manifold::sphere;
		m->localPoint = glm::transpose(txA.R) * (output.CA - txA.position);
		m->nPoints = 1;
		m->points[0].localPoint = glm::transpose(txB.R) * (output.CB - txB.position);
		m->points[0].id.key = 0;
	}
}

void CollideCapsuleHull(Manifold* m, CapsuleCollider* A, HullCollider* B, const Transform& txA, const Transform& txB)
{
	m->nPoints = 0;

	Transform btxB = B->txB;
	Transform btx = FxMul(txB, btxB);

	DistanceOutput output;
	DistanceInput input;
	input.proxyA = DistanceProxy(A);
	input.proxyB = DistanceProxy(B);
	input.txA = txA;
	input.txB = btx;

	GJK_Distance(&input, &output);

	float radius = A->radius + B->radius;

	if (output.distance == 0.0f)
	{
		// deep contact
		assert(false, "Capsule-Hull deep contact");
	}
	else
	{
		// shallow contact
		glm::vec3 n = output.CA - output.CB;
		float l2 = glm::length2(n);
		
		if (l2 > radius * radius)
			return;

		n *= 1.0f / std::sqrtf(l2);

		int refIndex = -1;
		int nFaces = B->faces.size();
		for (int i = 0; i < nFaces; ++i)
		{
			glm::vec3 nB = btx.R * B->faces[i]->normal;

			if (FxParallel(n, nB, linearSlop))
			{
				refIndex = i;
				break;
			}
		}

		if (refIndex == -1)
		{
			m->type = Manifold::sphere;
			m->localPoint = glm::transpose(txA.R) * (output.CA - txA.position);
			m->nPoints = 1;
			m->points[0].localPoint = glm::transpose(btx.R) * (output.CB - btx.position);
			m->points[0].id.key = 0;
		}
		else
		{
			Transform bLocal = FxMulT(btx, txA);
			glm::vec3 P = bLocal.R * input.proxyA.vertices[0] + bLocal.position;
			glm::vec3 Q = bLocal.R * input.proxyA.vertices[1] + bLocal.position;
			Face* ref = B->faces[refIndex];
			Edge* start = ref->edge;
			Edge* edge = start;
			do {
				Face* sideFace = edge->twin->face;
				glm::vec3 A = sideFace->edge->tail->position;
				glm::vec3 n = sideFace->normal;
				float dp = glm::dot(P - A, n) - hullRadius;
				float dq = glm::dot(Q - A, n) - hullRadius;

				if (dp * dq <= 0.0f)
				{
					if (dp <= 0.0f)
						Q = FxIntersect(P, Q, dp, dq);
					else
						P = FxIntersect(Q, P, dq, dp);
				}
			} while (edge != start);

			float d;
			int cp = 0;
			glm::vec3 out[2];
			ContactID ids[2];
			glm::vec3 A = start->tail->position;
			glm::vec3 n = ref->normal;
			Transform aLocal = FxMulT(txA, btx);

			d = glm::dot(P - A, n) - radius;
			if (d < 0.0f)
			{
				ContactID id;
				id.key = 0;

				ids[cp] = id;
				out[cp++] = aLocal.R * P + aLocal.position;
			}

			d = glm::dot(Q - A, n) - radius;
			if (d < 0.0f)
			{
				ContactID id;
				id.key = 0;
				id.featurePair.edgeA = 1;
				ids[cp] = id;
				out[cp++] = aLocal.R * Q + aLocal.position;
			}

			if (!cp)
				return;

			m->type = Manifold::faceB;
			m->localPoint = btxB.R * A + btxB.position;
			m->localNormal = btxB.R * n;
			m->nPoints = cp;

			for (int i = 0; i < cp; ++i)
			{
				m->points[i].localPoint = out[i];
				m->points[i].id = ids[i];
			}
		}
	}
}