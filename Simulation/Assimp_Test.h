#pragma once

#include "Simulation.h"

class Assimp_Test : public Simulation
{
public:
	static Assimp_Test& GetInstance();
	void Init(GLFWwindow* window, int width, int height);
	void OnKeyTap(GLFWwindow* window, int key, int scanCode, int action, int mods);
	void OnKeyPress(GLFWwindow* window);
	void Update(GLFWwindow* window);

	Assimp_Test() {};
	~Assimp_Test() {};
};
