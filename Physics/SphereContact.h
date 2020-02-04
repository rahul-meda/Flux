#pragma once

#include "Contact.h"

class SphereContact : public Contact
{
public:
	SphereContact(Collider* colliderA, Collider* colliderB);

	~SphereContact() {}

	static Contact* Create(Collider* colliderA, Collider* colliderB);

	static void Destroy(Contact* contact);

	void Evaluate(Manifold* manifold, const Transform& txA, const Transform& txB) override;
};