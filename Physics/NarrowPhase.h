#pragma once

#include "Collider.h"

class SphereCollider;
class HullCollider;
class Manifold;

typedef Contact* ContactCreateFn(Collider* colliderA, Collider* colliderB);

typedef void ContactDestroyFn(Contact* contact);

struct CollisionTable
{
	ContactCreateFn* createFn;
	ContactDestroyFn* destroyFn;
	bool flip;
};

class NarrowPhase
{
public:
	static void AddType(ContactCreateFn* createFn, ContactDestroyFn* destroyFn, Collider::Type typeA, Collider::Type typeB);

	static void InitializeTable();

	static CollisionTable contactTable[Collider::nShapes][Collider::nShapes];
};

void CollideSpheres(Manifold* manifold, SphereCollider* sphereA, SphereCollider* sphereB);

void CollideHulls(Manifold* manifold, HullCollider* hullA, HullCollider* hullB,
								      const Transform& txA, const Transform& txB);