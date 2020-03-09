#pragma once

#include "Simulation.h"

class BallStack
{
public:
	static BallStack& GetInstance();
	void Init();

private:
	BallStack() {};
	~BallStack() {};
};
