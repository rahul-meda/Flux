#pragma once

#include "Simulation.h"

class HingeJointTest
{
public:
	static HingeJointTest& GetInstance();
	void Init(std::vector<GameObject>& gameObjects);

private:
	HingeJointTest() {};
	~HingeJointTest() {};
};
