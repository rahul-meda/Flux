#pragma once

#include "Simulation.h"

class CRB_Test
{
public:
	static CRB_Test& GetInstance();
	void Init();

private:
	CRB_Test() {};
	~CRB_Test() {};
};
