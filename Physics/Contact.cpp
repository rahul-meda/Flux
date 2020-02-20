
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

	ContactCreateFn* createFn = NarrowPhase::contactTable[typeA][typeB].createFn;

	if (NarrowPhase::contactTable[typeA][typeB].flip == false)
	{
		return createFn(colliderA, colliderB);
	}
	else
	{
		return createFn(colliderB, colliderA);
	}
}

void Contact::Destroy(Contact* contact)
{
	Collider::Type typeA = contact->GetColliderA()->GetType();
	Collider::Type typeB = contact->GetColliderB()->GetType();
	ContactDestroyFn* destroyFn = NarrowPhase::contactTable[typeA][typeB].destroyFn;
	destroyFn(contact);
}

void Contact::Update()
{
	Manifold oldManifold = manifold;

	Transform txA = colliderA->body->GetTransform();
	Transform txB = colliderB->body->GetTransform();

	Evaluate(&manifold, txA, txB);	// virtual call!! (why is box2D using virtual call?)
	touching = manifold.nPoints > 0;

	for (int i = 0; i < manifold.nPoints; ++i)
	{
		ManifoldPoint* mp = manifold.points + i;
		mp->normalImpulse = 0.0f;
		mp->tangentImpulse[0] = 0.0f;
		mp->tangentImpulse[1] = 0.0f;
		ContactID id = mp->id;

		for (int j = 0; j < oldManifold.nPoints; ++j)
		{
			ManifoldPoint* mpOld = oldManifold.points + j;

			if (id.key == mpOld->id.key)
			{
				mp->normalImpulse = mpOld->normalImpulse;
				mp->tangentImpulse[0] = mpOld->tangentImpulse[0];
				mp->tangentImpulse[1] = mpOld->tangentImpulse[1];
				break;
			}
		}
	}

	// match old and new contacts

	// begin, end, pre-solve
}