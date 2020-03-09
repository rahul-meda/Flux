#pragma once

#include "Simulation.h"

class Chain1
{
public:
	static Chain1& GetInstance();
	void Init();

private:
	Chain1() {};
	~Chain1() {};
};
