#pragma once

#include "Simulation.h"

class Collapse1 : public Simulation
{
public:
	static Collapse1& GetInstance();
	void Init(GLFWwindow* window, int width, int height);
	void OnKeyTap(GLFWwindow* window, int key, int scanCode, int action, int mods);
	void OnKeyPress(GLFWwindow* window);
	void Update(GLFWwindow* window);

	Collapse1() {};
	~Collapse1() {};
};
