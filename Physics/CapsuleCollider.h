
#pragma once

#include "Collider.h"

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider();

	~CapsuleCollider() {}

	void ComputeAABB(AABB* aabb) const override;

	void ComputeMass() override;

	void Scale(const float r);

	glm::vec3 upB;	// up in body space

	float H;			// half height of segment part
};