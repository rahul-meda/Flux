#pragma once

class GameObject
{
public:
	GameObject(unsigned int modelID, unsigned int txID)
		: modelID(modelID), txID(txID)
	{}

	unsigned int modelID;

	unsigned int txID;
};