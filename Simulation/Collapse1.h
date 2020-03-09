#pragma once

#include "Simulation.h"

class Collapse1
{
public:
	static Collapse1& GetInstance();
	void Init();

private:
	Collapse1() {};
	~Collapse1() {};
};
