
#include "HullContact.h"
#include "NarrowPhase.h"

HullContact::HullContact(Collider* colliderA, Collider* colliderB)
	: Contact(colliderA, colliderB)
{}

Contact* HullContact::Create(Collider* colliderA, Collider* colliderB)
{
	return new HullContact(colliderA, colliderB);
}

void HullContact::Destroy(Contact* contact)
{
	((HullContact*)contact)->~HullContact();
}

void HullContact::Evaluate(Manifold* manifold, const Transform& txA, const Transform& txB)
{
	CollideHulls(manifold, (HullCollider*)colliderA, (HullCollider*)colliderB, txA, txB);
}