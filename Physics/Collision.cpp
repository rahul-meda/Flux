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