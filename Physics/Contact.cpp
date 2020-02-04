#include "Contact.h"
#include "NarrowPhase.h"

Contact* Contact::Create(Collider* colliderA, Collider* colliderB)
{
	Collider::Type typeA = colliderA->type;
	Collider::Type typeB = colliderB->type;

	ContactCreateFn* createFn = NarrowPhase::collisionTable[typeA][typeB].createFn;

	return createFn(colliderA, colliderB);
}

void Contact::Destroy(Contact* contact)
{
	Collider::Type typeA = contact->GetColliderA()->GetType();
	Collider::Type typeB = contact->GetColliderB()->GetType();
	ContactDestroyFn* destroyFn = NarrowPhase::collisionTable[typeA][typeB].destroyFn;
	destroyFn(contact);
}

void Contact::Update()
{
	// get touching - set was touching

	Transform txA = colliderA->body->GetTransform();
	Transform txB = colliderB->body->GetTransform();
	Evaluate(&manifold, txA, txB);	// virtual call!! (why is box2D using virtual call?)

	// set touching
	bool touching = manifold.nPoints > 0;
	if (!touching)
	{
		nuke = true;
		Destroy(this);
	}

	// match old and new contacts

	// begin, end, pre-solve
}