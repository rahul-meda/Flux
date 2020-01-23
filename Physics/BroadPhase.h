#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <utility>

class Collider;
typedef std::pair<Collider*, Collider*> ColliderPair;

struct AABB;
class BroadPhase
{
public:
	void Add(AABB* aabb);

	void Add(Collider* collider);

	void ComputePairs();

	void Update();

	std::vector<AABB*> aabbs;
	std::vector<ColliderPair> pairs;
};