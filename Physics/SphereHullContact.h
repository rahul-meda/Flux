
#pragma once

#include "Contact.h"

class SphereHullContact : public Contact
{
public:
	SphereHullContact(Collider* colliderA, Collider* colliderB);

	~SphereHullContact() {}

	static Contact* Create(Collider* colliderA, Collider* colliderB);

	static void Destroy(Contact* contact);

	void Evaluate(Manifold* manifold, const Transform& txA, const Transform& txB) override;
};
