
#pragma once

#include "Contact.h"

class SphereCapsuleContact : public Contact
{
public:
	SphereCapsuleContact(Collider* colliderA, Collider* colliderB);

	~SphereCapsuleContact() {}

	static Contact* Create(Collider* colliderA, Collider* colliderB);

	static void Destroy(Contact* contact);

	void Evaluate(Manifold* manifold, const Transform& txA, const Transform& txB) override;
};
