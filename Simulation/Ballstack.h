#pragma once

#include "Simulation.h"

class BallStack
{
public:
	static BallStack& GetInstance();
	void Init(std::vector<GameObject>& gameObjects);

private:
	BallStack() {};
	~BallStack() {};
};
