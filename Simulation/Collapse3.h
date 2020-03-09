#pragma once

#include "Simulation.h"

class Collapse3
{
public:
	static Collapse3& GetInstance();
	void Init();

private:
	Collapse3() {};
	~Collapse3() {};
};
