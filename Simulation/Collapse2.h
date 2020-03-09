#pragma once

#include "Simulation.h"

class Collapse2
{
public:
	static Collapse2& GetInstance();
	void Init();

private:
	Collapse2() {};
	~Collapse2() {};
};
