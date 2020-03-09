
#pragma once

#include "Simulation.h"

class DoorHinge
{
public:
	static DoorHinge& GetInstance();
	void Init();

private:
	DoorHinge() {};
	~DoorHinge() {};
};
