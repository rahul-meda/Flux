#pragma once

class GameObject
{
public:
	GameObject(unsigned int modelID, unsigned int bodyID)
		: modelID(modelID), bodyID(bodyID)
	{}

	unsigned int modelID;

	unsigned int bodyID;	// is it better to store pointers?
};