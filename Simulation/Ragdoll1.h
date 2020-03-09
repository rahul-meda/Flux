#pragma once

#include "Simulation.h"

class Ragdoll1
{
public:
	static Ragdoll1& GetInstance();
	void Init();

private:
	Ragdoll1() {};
	~Ragdoll1() {};
};
