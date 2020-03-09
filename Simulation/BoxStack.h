#pragma once

#include "Simulation.h"

class BoxStack
{
public:
	static BoxStack& GetInstance();
	void Init();

private:
	BoxStack() {};
	~BoxStack() {};
};
