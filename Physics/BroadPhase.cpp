#include "BroadPhase.h"
#include "Collision.h"
#include "Collider.h"

void BroadPhase::Add(AABB* aabb)
{
	aabbs.push_back(aabb);
}

void BroadPhase::Add(Collider* collider)
{
	AABB* aabb = new AABB();
	aabb->collider = collider;
	aabb->collider->ComputeAABB(aabb);
	aabbs.push_back(aabb);
}

void BroadPhase::ComputePairs()
{
	pairs.clear();

	int N = aabbs.size();
	for (int iA = 0; iA < N; ++iA)
	{
		for (int iB = iA + 1; iB < N; ++iB)
		{
			AABB* aabbA = aabbs[iA];
			AABB* aabbB = aabbs[iB];
			Collider* cA = aabbA->collider;
			Collider* cB = aabbB->collider;

			if (Overlap(aabbA, aabbB))
				pairs.push_back(ColliderPair(cA, cB));
		}
	}
}

void BroadPhase::Update()
{
	int N = aabbs.size();
	for (int i = 0; i < N; ++i)
	{
		aabbs[i]->collider->ComputeAABB(aabbs[i]);
	}

	ComputePairs();
}