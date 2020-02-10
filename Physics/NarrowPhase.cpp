
#include "NarrowPhase.h"
#include "SphereContact.h"
#include "HullContact.h"

CollisionTable NarrowPhase::collisionTable[Collider::nShapes][Collider::nShapes];

void NarrowPhase::InitializeTable()
{
	AddType(SphereContact::Create, SphereContact::Destroy, Collider::Sphere, Collider::Sphere);
	AddType(HullContact::Create, HullContact::Destroy, Collider::ConvexHull, Collider::ConvexHull);
}

void NarrowPhase::AddType(ContactCreateFn* createFn, ContactDestroyFn* destroyFn, 
						  Collider::Type type1, Collider::Type type2)
{
	collisionTable[type1][type2].createFn = createFn;
	collisionTable[type1][type2].destroyFn = destroyFn;
	collisionTable[type1][type2].flip = false;

	if (type1 != type2)
	{
		collisionTable[type2][type1].createFn = createFn;
		collisionTable[type2][type1].destroyFn = destroyFn;
		collisionTable[type2][type1].flip = true;
	}
}

