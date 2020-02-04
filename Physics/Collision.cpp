#include "Collision.h"

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
			if (glm::distance2(pointA, pointB) > fEpsilon * fEpsilon)
			{
				normal = pointB - pointA;
				normal = glm::normalize(normal);
			}

			glm::vec3 cA = pointA + radiusA * normal;
			glm::vec3 cB = pointB - radiusB * normal;
			points[0] = 0.5f * (cA + cB);
			separations[0] = glm::dot(cB - cA, normal);
		}
	break;
	}
}
