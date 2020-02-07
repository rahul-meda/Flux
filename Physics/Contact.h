#pragma once

#include "Collision.h"
#include "Collider.h"

// A contact edge connects bodies and contacts together in a contact graph
// bodies are nodes, and contacts are edges
struct ContactEdge
{
	Body* other;
	Contact* contact;
	ContactEdge* prev;
	ContactEdge* next;
};

class Contact
{
public:
	Contact()
		: colliderA(nullptr), colliderB(nullptr) {}

	Contact(Collider* colliderA, Collider* colliderB);

	virtual ~Contact() {}

	static Contact* Create(Collider* colliderA, Collider* colliderB);

	static void Destroy(Contact* contact);

	void Update();

	virtual void Evaluate(Manifold* manifold, const Transform& txA, const Transform& txB) = 0;

	Manifold* GetManifold();
	const Manifold*GetManifold() const;

	void GetWorldManifold(WorldManifold* wManifold) const;

	Collider* GetColliderA();
	const Collider* GetColliderA() const;

	Collider* GetColliderB();
	const Collider* GetColliderB() const;

	float GetRestitution() const;

	float GetFriction() const;

protected:
	bool touching;

	ContactEdge edgeA;
	ContactEdge edgeB;

	Collider* colliderA;
	Collider* colliderB;

	Manifold manifold;

	friend class ContactManager;
	friend class ContactSolver;
};

inline Manifold* Contact::GetManifold()
{
	return &manifold;
}

inline const Manifold* Contact::GetManifold() const 
{
	return &manifold;
}

inline void Contact::GetWorldManifold(WorldManifold* wManifold) const
{
	Body* bodyA = colliderA->GetBody();
	Body* bodyB = colliderB->GetBody();
	float radiusA = colliderA->radius;
	float radiusB = colliderB->radius;

	wManifold->Initialize(&manifold, bodyA->GetTransform(), bodyB->GetTransform(), radiusA, radiusB);
}

inline Collider* Contact::GetColliderA()
{
	return colliderA;
}

inline const Collider* Contact::GetColliderA() const
{
	return colliderA;
}

inline Collider* Contact::GetColliderB()
{
	return colliderB;
}

inline const Collider* Contact::GetColliderB() const
{
	return colliderB;
}

inline float Contact::GetRestitution() const
{
	return (colliderA->restitution > colliderB->restitution ? colliderA->restitution : colliderB->restitution);
}

inline float Contact::GetFriction() const
{
	return std::sqrtf(colliderA->friction * colliderB->friction);
}

