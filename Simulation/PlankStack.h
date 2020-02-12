#pragma once

#include "Simulation.h"

class PlankStack
{
public:
	static PlankStack& GetInstance();
	void Init(std::vector<GameObject>& gameObjects);

private:
	PlankStack() {};
	~PlankStack() {};
};
