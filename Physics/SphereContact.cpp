#include "SphereContact.h"
#include "NarrowPhase.h"

SphereContact::SphereContact(Collider* colliderA, Collider* colliderB)
	: Contact(colliderA, colliderB)
{}

Contact* SphereContact::Create(Collider* colliderA, Collider* colliderB)
{
	return new SphereContact(colliderA, colliderB);
}

void SphereContact::Destroy(Contact* contact)
{
	((SphereContact*)contact)->~SphereContact();
}

void SphereContact::Evaluate(Manifold* manifold, const Transform& txA, const Transform& txB)
{
	CollideSpheres(manifold, (SphereCollider*)colliderA, (SphereCollider*)colliderB);
}