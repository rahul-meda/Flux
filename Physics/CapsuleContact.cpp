
#include "CapsuleContact.h"
#include "NarrowPhase.h"

CapsuleContact::CapsuleContact(Collider* colliderA, Collider* colliderB)
	: Contact(colliderA, colliderB)
{}

Contact* CapsuleContact::Create(Collider* colliderA, Collider* colliderB)
{
	return new CapsuleContact(colliderA, colliderB);
}

void CapsuleContact::Destroy(Contact* contact)
{
	((CapsuleContact*)contact)->~CapsuleContact();
}

void CapsuleContact::Evaluate(Manifold* manifold, const Transform& txA, const Transform& txB)
{
	CollideCapsules(manifold, (CapsuleCollider*)colliderA, (CapsuleCollider*)colliderB, txA, txB);
}