#pragma once

#include "Simulation.h"

class Bike
{
public:
	static Bike& GetInstance();
	void Init();

private:
	Bike() {};
	~Bike() {};
};
