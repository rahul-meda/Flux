
#include "CapsuleHullContact.h"
#include "NarrowPhase.h"

CapsuleHullContact::CapsuleHullContact(Collider* colliderA, Collider* colliderB)
	: Contact(colliderA, colliderB)
{}

Contact* CapsuleHullContact::Create(Collider* colliderA, Collider* colliderB)
{
	return new CapsuleHullContact(colliderA, colliderB);
}

void CapsuleHullContact::Destroy(Contact* contact)
{
	((CapsuleHullContact*)contact)->~CapsuleHullContact();
}

void CapsuleHullContact::Evaluate(Manifold* manifold, const Transform& txA, const Transform& txB)
{
	CollideCapsuleHull(manifold, (CapsuleCollider*)colliderA, (HullCollider*)colliderB, txA, txB);
}