
#include "Collision.h"
#include "../Utils.h"

bool Overlap(AABB* A, AABB* B)
{
	if (A->max.x < B->min.x || A->min.x > B->max.x)
		return false;
	if (A->max.y < B->min.y || A->min.y > B->max.y)
		return false;
	if (A->max.z < B->min.z || A->min.z > B->max.z)
		return false;

	return true;
}

void WorldManifold::Initialize(const Manifold* manifold, const Transform & txA, const Transform & txB,
														 float radiusA, float radiusB)
{
	switch (manifold->type)
	{
	case Manifold::spheres:
		{
			normal = glm::vec3(1.0f, 0.0f, 0.0f);
			glm::vec3 pointA = txA.position + (txA.R * manifold->localPoint);
			glm::vec3 pointB = txB.position + (txB.R * manifold->points[0].localPoint);
			if (glm::distance2(pointA, pointB) > fxEpsilon * fxEpsilon)
			{
				normal = pointB - pointA;
				normal = glm::normalize(normal);
			}

			glm::vec3 cA = pointA + radiusA * normal;
			glm::vec3 cB = pointB - radiusB * normal;
			points[0] = 0.5f * (cA + cB);
			separations[0] = glm::dot(cB - cA, normal);

			ComputeBasis(normal, &tangent[0], &tangent[1]);
		}
	break;

	case Manifold::faceA:
		{
			normal = txA.R * manifold->localNormal;
			glm::vec3 point = txA.R * manifold->localPoint + txA.position;

			for (int i = 0; i < manifold->nPoints; ++i)
			{
				glm::vec3 clipPoint = txB.R * (manifold->points[i].localPoint) + txB.position;
				glm::vec3 cA = clipPoint + normal * (radiusA - glm::dot(clipPoint - point, normal));
				glm::vec3 cB = clipPoint - normal * radiusB;
				points[i] = (cA + cB) * 0.5f;
				separations[i] = glm::dot(cB - cA, normal);
			}

			ComputeBasis(normal, &tangent[0], &tangent[1]);
		}
		break;

	case Manifold::faceB:
		{
			normal = txB.R * manifold->localNormal;
			glm::vec3 point = txB.R * manifold->localPoint + txB.position;

			for (int i = 0; i < manifold->nPoints; ++i)
			{
				glm::vec3 clipPoint = txA.R * (manifold->points[i].localPoint) + txA.position;
				glm::vec3 cB = clipPoint + normal * (radiusB - glm::dot(clipPoint - point, normal));
				glm::vec3 cA = clipPoint - normal * radiusA;
				points[i] = (cA + cB) * 0.5f;
				separations[i] = glm::dot(cA - cB, normal);
			}
			normal = -normal;

			ComputeBasis(normal, &tangent[0], &tangent[1]);
		}
		break;

	case Manifold::edges:
		{
			glm::vec3 PA = txA.R * manifold->localPoint + txA.position;
			glm::vec3 PB = txB.R * manifold->points[0].localPoint + txB.position;
			normal = txB.R * manifold->localNormal;
			glm::vec3 CA = PA - radiusA * normal; // TODO: check sign
			glm::vec3 CB = PB + radiusB * normal;
			points[0] = (CA + CB) * 0.5f;
			separations[0] = glm::dot(CB - CA, normal);
			
			ComputeBasis(normal, &tangent[0], &tangent[1]);
		}
		break;
	}
}
