#pragma once

#include "Simulation.h"

class BoxStack
{
public:
	static BoxStack& GetInstance();
	void Init(std::vector<GameObject>& gameObjects);

private:
	BoxStack() {};
	~BoxStack() {};
};
