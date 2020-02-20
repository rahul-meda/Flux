
#include <iterator>
#include "ContactManager.h"
#include "Contact.h"
#include "../Graphics/Graphics.h"
#include <iostream>

ContactManager::ContactManager()
{
	contacts.reserve(512);
}

void ContactManager::AddPair(Collider* colliderA, Collider* colliderB)
{
	Body* bodyA = colliderA->body;
	Body* bodyB = colliderB->body;

	if (bodyA == bodyB)
		return;

	// does a contact already exist?
	ContactEdge* edge = bodyB->edgeList;
	while (edge)
	{
		if (edge->other == bodyA)
		{
			Collider* cA = edge->contact->colliderA;
			Collider* cB = edge->contact->colliderB;

			if (cA == colliderA && cB == colliderB)
			{
				return;
			}

			if (cA == colliderB && cB == colliderA)
			{
				return;
			}
		}

		edge = edge->next;
	}

	// check if bodies should collide
	if (bodyB->ShouldCollide(bodyA) == false)
		return;

	// contact filtering - ToDo

	Contact* c = Contact::Create(colliderA, colliderB);
	assert(c, "ERROR::Invalid contact creation");

	// add to world
	contacts.push_back(c);

	// connect to island graph

	// contact creation may swap colliders
	bodyA = c->GetColliderA()->body;
	bodyB = c->GetColliderB()->body;

	// Connect to body A
	c->edgeA.contact = c;
	c->edgeA.other = bodyB;

	c->edgeA.prev = nullptr;
	c->edgeA.next = bodyA->edgeList;
	if (bodyA->edgeList != nullptr)
	{
		bodyA->edgeList->prev = &c->edgeA;
	}
	bodyA->edgeList = &c->edgeA;

	// Connect to body B
	c->edgeB.contact = c;
	c->edgeB.other = bodyA;

	c->edgeB.prev = nullptr;
	c->edgeB.next = bodyB->edgeList;
	if (bodyB->edgeList != nullptr)
	{
		bodyB->edgeList->prev = &c->edgeB;
	}
	bodyB->edgeList = &c->edgeB;
}

void ContactManager::FindNewContacts()
{
	//contacts.clear();	// temp
	int N = bp.pairs.size();
	for(int i = 0; i < N; ++i)
		AddPair(bp.pairs[i].first, bp.pairs[i].second);
}

void ContactManager::Collide()
{
	int N = contacts.size();
	for (int i = 0; i < N;)
	{
		if (i < 0)
		{
			int x = 1;
		}
		Contact* c = contacts[i];
		Collider* cA = c->colliderA;
		Collider* cB = c->colliderB;

		// destroy contacts that cease to overlap in braod-phase
		AABB* aabbA = bp.aabbs[cA->aabbIndex];
		AABB* aabbB = bp.aabbs[cB->aabbIndex];
		bool overlap = Overlap(aabbA, aabbB);

		if (!overlap)
		{
			Destroy(c, i);
			--N;
			continue;
		}

		// contact persists
		c->Update();
		++i;
	}
}

void ContactManager::Destroy(Contact* nuke, int i)
{
	int N = contacts.size();
	
	// remove from bodies
	Body* bodyA = nuke->colliderA->body;
	Body* bodyB = nuke->colliderB->body;

	// Remove from body A
	if (nuke->edgeA.prev)
	{
		nuke->edgeA.prev->next = nuke->edgeA.next;
	}

	if (nuke->edgeA.next)
	{
		nuke->edgeA.next->prev = nuke->edgeA.prev;
	}

	if (&nuke->edgeA == bodyA->edgeList)
	{
		bodyA->edgeList = nuke->edgeA.next;
	}

	// Remove from body B
	if (nuke->edgeB.prev)
	{
		nuke->edgeB.prev->next = nuke->edgeB.next;
	}

	if (nuke->edgeB.next)
	{
		nuke->edgeB.next->prev = nuke->edgeB.prev;
	}

	if (&nuke->edgeB == bodyB->edgeList)
	{
		bodyB->edgeList = nuke->edgeB.next;
	}

	// remove from world
	contacts[i] = contacts[N - 1];
	contacts.pop_back();

	Contact::Destroy(nuke);
}

void ContactManager::DebugDraw()
{
	Graphics::GetInstance().points.clear();
	Graphics::GetInstance().lines.clear();
	Graphics::GetInstance().aabbs.clear();

	int N = contacts.size();
	for (int i = 0; i < N; ++i)
	{
		Contact* c = contacts[i];
		Manifold m = c->manifold;
		Collider* cA = c->colliderA;
		Collider* cB = c->colliderB;
		Body* bA = cA->body;
		Body* bB = cB->body;
		Transform txA = bA->GetTransform();
		Transform txB = bB->GetTransform();
		WorldManifold wm;
		wm.Initialize(&m, txA, txB, cA->radius, cB->radius);

		for (int j = 0; j < m.nPoints; ++j)
		{
			Graphics::GetInstance().points.push_back(R_Point(wm.points[j]));

			float angle = acosf(glm::dot(wm.normal, glm::vec3(1.0f, 0.0f, 0.0f)));
			glm::vec3 axis = wm.tangent[0];
			glm::quat q = glm::angleAxis(angle, axis);
			Graphics::GetInstance().lines.push_back(R_Line(wm.points[j], q));
			angle = acosf(glm::dot(wm.tangent[0], glm::vec3(1.0f, 0.0f, 0.0f)));
			axis = wm.normal;
			q = glm::angleAxis(angle, axis);
			Graphics::GetInstance().lines.push_back(R_Line(wm.points[j], q, glm::vec3(0.5f, 0.9f, 0.9f)));
			angle = acosf(glm::dot(wm.tangent[1], glm::vec3(1.0f, 0.0f, 0.0f)));
			axis = wm.normal;
			q = glm::angleAxis(angle, axis);
			Graphics::GetInstance().lines.push_back(R_Line(wm.points[j], q, glm::vec3(0.5f, 0.9f, 0.9f)));
		}
	}

	/*N = bp.aabbs.size();
	for (int i = 0; i < N; ++i)
	{
		AABB* aabb = bp.aabbs[i];
		Graphics::GetInstance().aabbs.push_back(R_aabb(aabb->min, aabb->max));
	}*/
}