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

	void FindNewContacts();

	void Collide();

	void Destroy(Contact* contact, int i);

	void DebugDraw();

	std::vector<Contact*> contacts;

	BroadPhase bp;
};