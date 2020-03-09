#pragma once

#include "Simulation.h"

class PositionJointTest
{
public:
	static PositionJointTest& GetInstance();
	void Init();

private:
	PositionJointTest() {};
	~PositionJointTest() {};
};
