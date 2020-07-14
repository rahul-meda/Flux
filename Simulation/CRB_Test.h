#pragma once

#include "Simulation.h"

class CRB_Test : public Simulation
{
public:
	static CRB_Test& GetInstance();
	void Init(GLFWwindow* window, int width, int height);
	void OnKeyTap(GLFWwindow* window, int key, int scanCode, int action, int mods);
	void OnKeyPress(GLFWwindow* window);

	CRB_Test() {};
	~CRB_Test() {};
};
