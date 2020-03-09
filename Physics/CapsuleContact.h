#pragma once

#include "Contact.h"

class CapsuleContact : public Contact
{
public:
	CapsuleContact(Collider* colliderA, Collider* colliderB);

	~CapsuleContact() {}

	static Contact* Create(Collider* colliderA, Collider* colliderB);

	static void Destroy(Contact* contact);

	void Evaluate(Manifold* manifold, const Transform& txA, const Transform& txB) override;
};