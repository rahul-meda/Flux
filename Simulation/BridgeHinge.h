
#pragma once

#include "Simulation.h"

class BridgeHinge
{
public:
	static BridgeHinge& GetInstance();
	void Init();

private:
	BridgeHinge() {};
	~BridgeHinge() {};
};
