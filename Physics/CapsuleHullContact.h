
#pragma once

#include "Contact.h"

class CapsuleHullContact : public Contact
{
public:
	CapsuleHullContact(Collider* colliderA, Collider* colliderB);

	~CapsuleHullContact() {}

	static Contact* Create(Collider* colliderA, Collider* colliderB);

	static void Destroy(Contact* contact);

	void Evaluate(Manifold* manifold, const Transform& txA, const Transform& txB) override;
};
