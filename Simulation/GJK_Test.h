#pragma once

#include "Simulation.h"

class GJK_Test
{
public:
	static GJK_Test& GetInstance();
	void Init();

private:
	GJK_Test() {};
	~GJK_Test() {};
};
