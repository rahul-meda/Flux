#pragma once

#include "Simulation.h"

class Domino1
{
public:
	static Domino1& GetInstance();
	void Init();

private:
	Domino1() {};
	~Domino1() {};
};
