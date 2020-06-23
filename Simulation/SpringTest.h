#pragma once

#include "Simulation.h"

class Body;
class HingeJoint;

class SpringTest : public Simulation
{
public:
	void Init(GLFWwindow* window, int width, int height);
	void OnKeyPress(GLFWwindow* window);

	SpringTest() {};
	~SpringTest() {};
};
