
#pragma once

#include "Simulation.h"

class BridgeHinge
{
public:
	static BridgeHinge& GetInstance();
	void Init(std::vector<GameObject>& gameObjects);

private:
	BridgeHinge() {};
	~BridgeHinge() {};
};
