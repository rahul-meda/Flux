
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
	int i = 0;
	while(i < N)
	{
		Contact* c = contacts[i];
		Collider* cA = c->colliderA;
		Collider* cB = c->colliderB;

		// destroy contacts that cease to overlap in broad-phase
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
			glm::vec3 p = wm.points[j];

			Graphics::GetInstance().points.push_back(R_Point(p));

			glm::vec3 X(1.0f, 0.0f, 0.0f);
			glm::vec3 h = wm.normal + X;
			glm::quat q = glm::identity<glm::quat>();
			float angle = 0.0f;
			float lA = 0.0f, lB = 0.0f;
			if (glm::length2(h) == 0.0f)
			{
				q = glm::angleAxis(PI, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else
			{
				h = glm::normalize(h);
				glm::vec3 axis(0.0f, -h.z, h.y);
				if (glm::length2(axis) == 0.0f)
					axis = glm::vec3(0.0f, 1.0f, 0.0f);
				else
				{
					lA = glm::length(wm.normal);
					angle = acosf(wm.normal.x / lA);
					axis = glm::normalize(axis);
				}
				q = glm::angleAxis(angle, axis);
			}
			Graphics::GetInstance().lines.push_back(R_Line(p, q));

			for (int k = 0; k < 2; ++k)
			{
				h = wm.tangent[k] + X;
				if (glm::length2(h) == 0.0f)
				{
					q = glm::angleAxis(PI, glm::vec3(0.0f, 1.0f, 0.0f));
				}
				else
				{
					h = glm::normalize(h);
					glm::vec3 axis(0.0f, -h.z, h.y);
					if (glm::length2(axis) == 0.0f)
						axis = glm::vec3(0.0f, 1.0f, 0.0f);
					else
					{
						lB = glm::length(wm.tangent[k]);
						angle = acosf(wm.tangent[k].x / lB);
						axis = glm::normalize(axis);
					}
					q = glm::angleAxis(angle, axis);
				}
				Graphics::GetInstance().lines.push_back(R_Line(p, q, glm::vec3(0.3f, 0.5f, 0.9f)));
			}
		}
	}

	N = bp.aabbs.size();
	for (int i = 0; i < N; ++i)
	{
		AABB* aabb = bp.aabbs[i];
		Graphics::GetInstance().aabbs.push_back(R_aabb(aabb->min, aabb->max));
	}
}