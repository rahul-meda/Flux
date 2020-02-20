#pragma once

#include "Simulation.h"

class Collapse1
{
public:
	static Collapse1& GetInstance();
	void Init(std::vector<GameObject>& gameObjects);

private:
	Collapse1() {};
	~Collapse1() {};
};
