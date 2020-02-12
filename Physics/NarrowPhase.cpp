
#include "NarrowPhase.h"
#include "SphereContact.h"
#include "HullContact.h"

CollisionTable NarrowPhase::contactTable[Collider::nShapes][Collider::nShapes];

void NarrowPhase::InitializeTable()
{
	AddType(SphereContact::Create, SphereContact::Destroy, Collider::Sphere, Collider::Sphere);
	AddType(HullContact::Create, HullContact::Destroy, Collider::ConvexHull, Collider::ConvexHull);
}

void NarrowPhase::AddType(ContactCreateFn* createFn, ContactDestroyFn* destroyFn, 
						  Collider::Type type1, Collider::Type type2)
{
	contactTable[type1][type2].createFn = createFn;
	contactTable[type1][type2].destroyFn = destroyFn;
	contactTable[type1][type2].flip = false;

	if (type1 != type2)
	{
		contactTable[type2][type1].createFn = createFn;
		contactTable[type2][type1].destroyFn = destroyFn;
		contactTable[type2][type1].flip = true;
	}
}

