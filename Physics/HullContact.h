
#pragma once

#include "Contact.h"

class HullContact : public Contact
{
public:
	HullContact(Collider* colliderA, Collider* colliderB);

	~HullContact() {}

	static Contact* Create(Collider* colliderA, Collider* colliderB);

	static void Destroy(Contact* contact);

	void Evaluate(Manifold* manifold, const Transform& txA, const Transform& txB) override;
};