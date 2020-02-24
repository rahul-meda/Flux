#pragma once

#include "Simulation.h"

class PositionJointTest
{
public:
	static PositionJointTest& GetInstance();
	void Init(std::vector<GameObject>& gameObjects);

private:
	PositionJointTest() {};
	~PositionJointTest() {};
};
