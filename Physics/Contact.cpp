
#include "Contact.h"
#include "NarrowPhase.h"

Contact::Contact(Collider* colliderA, Collider* colliderB)
	: colliderA(colliderA), colliderB(colliderB)
{
	touching = false;

	edgeA.contact = nullptr;
	edgeA.other = nullptr;
	edgeA.prev = nullptr;
	edgeA.next = nullptr;

	edgeB.contact = nullptr;
	edgeB.other = nullptr;
	edgeB.prev = nullptr;
	edgeB.next = nullptr;
}

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
	touching = manifold.nPoints > 0;

	// match old and new contacts

	// begin, end, pre-solve
}