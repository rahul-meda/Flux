#pragma once

#include "Simulation.h"

class Chain1
{
public:
	static Chain1& GetInstance();
	void Init(std::vector<GameObject>& gameObjects);

private:
	Chain1() {};
	~Chain1() {};
};
