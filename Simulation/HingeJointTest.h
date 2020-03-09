#pragma once

#include "Simulation.h"

class HingeJointTest
{
public:
	static HingeJointTest& GetInstance();
	void Init();

private:
	HingeJointTest() {};
	~HingeJointTest() {};
};
