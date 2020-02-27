
#pragma once

#include "Simulation.h"

class DoorHinge
{
public:
	static DoorHinge& GetInstance();
	void Init(std::vector<GameObject>& gameObjects);

private:
	DoorHinge() {};
	~DoorHinge() {};
};
