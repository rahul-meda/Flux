#pragma once

#include <vector>
#include "BroadPhase.h"

class Contact;
class Collider;

class ContactManager
{
public:
	ContactManager();

	void AddPair(Collider* colliderA, Collider* colliderB);

	void FindNewContacts(const std::vector<ColliderPair>& pairs);

	void Collide();

	void Destroy(Contact* contact);

	std::vector<Contact*> contacts;
};