#pragma once

#include "Collider.h"

class SphereCollider : public Collider
{
public:
	SphereCollider();

	~SphereCollider() {}

	void ComputeAABB(AABB* aabb) const override;

	void ComputeMass() override;

	void Scale(const float r);

	glm::vec3 position;	// relative to owning body
};