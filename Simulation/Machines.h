#pragma once

#include "Simulation.h"

class Machines
{
public:
	static Machines& GetInstance();
	void Init();

private:
	Machines() {};
	~Machines() {};
};
