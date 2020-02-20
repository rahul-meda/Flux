#pragma once

#include "Simulation.h"

class GJK_Test
{
public:
	static GJK_Test& GetInstance();
	void Init(std::vector<GameObject>& gameObjects);

private:
	GJK_Test() {};
	~GJK_Test() {};
};
