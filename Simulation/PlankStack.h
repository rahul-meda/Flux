#pragma once

#include "Simulation.h"

class PlankStack
{
public:
	static PlankStack& GetInstance();
	void Init();

private:
	PlankStack() {};
	~PlankStack() {};
};
