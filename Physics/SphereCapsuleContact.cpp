
#include "HullCollider.h"
#include "SphereCapsuleContact.h"
#include "NarrowPhase.h"

SphereCapsuleContact::SphereCapsuleContact(Collider* colliderA, Collider* colliderB)
	: Contact(colliderA, colliderB)
{}

Contact* SphereCapsuleContact::Create(Collider* colliderA, Collider* colliderB)
{
	return new SphereCapsuleContact(colliderA, colliderB);
}

void SphereCapsuleContact::Destroy(Contact* contact)
{
	((SphereCapsuleContact*)contact)->~SphereCapsuleContact();
}

void SphereCapsuleContact::Evaluate(Manifold* manifold, const Transform& txA, const Transform& txB)
{
	CollideSphereCapsule(manifold, (SphereCollider*)colliderA, (CapsuleCollider*)colliderB, txA, txB);
}