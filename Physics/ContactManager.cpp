
#include <iterator>
#include "ContactManager.h"
#include "Contact.h"

ContactManager::ContactManager()
{
	contacts.reserve(512);
}

void ContactManager::AddPair(Collider* colliderA, Collider* colliderB)
{
	// if contact already exists - return

	// check if bodies should collide

	Contact* c = Contact::Create(colliderA, colliderB);
	c->nuke = false;
	contacts.push_back(c);

	// connect to island graph

	// connect to bodies
}

void ContactManager::FindNewContacts(const std::vector<ColliderPair>& pairs)
{
	contacts.clear();	// temp
	int N = pairs.size();
	for(int i = 0; i < N; ++i)
		AddPair(pairs[i].first, pairs[i].second);
}

void ContactManager::Collide()
{
	int N = contacts.size();
	for (int i = 0; i < N; ++i)
	{
		// should bodies collide?

		// fast broadphase test for overlap

		// contact persists
		contacts[i]->Update();
	}

	// temp
	std::vector<Contact*>::iterator it = contacts.begin();
	while (it != contacts.end())
	{
		if ((*it)->nuke)
			it = contacts.erase(it);
		else
			++it;
	}
}

void ContactManager::Destroy(Contact* c)
{
	int N = contacts.size();
	for (int i = 0; i < N; ++i)
	{
		if (contacts[i] == c)
		{
			contacts[i] = contacts[N - 1];
			contacts[N - 1] = nullptr;
			contacts.pop_back();
			break;
		}
	}
	
	// remove from bodies

	Contact::Destroy(c);
}