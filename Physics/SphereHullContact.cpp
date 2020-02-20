
#include "SphereHullContact.h"
#include "NarrowPhase.h"

SphereHullContact::SphereHullContact(Collider* colliderA, Collider* colliderB)
	: Contact(colliderA, colliderB)
{}

Contact* SphereHullContact::Create(Collider* colliderA, Collider* colliderB)
{
	return new SphereHullContact(colliderA, colliderB);
}

void SphereHullContact::Destroy(Contact* contact)
{
	((SphereHullContact*)contact)->~SphereHullContact();
}

void SphereHullContact::Evaluate(Manifold* manifold, const Transform& txA, const Transform& txB)
{
	CollideSphereHull(manifold, (SphereCollider*)colliderA, (HullCollider*)colliderB, txA, txB);
}