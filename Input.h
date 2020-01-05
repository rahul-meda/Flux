#pragma once

#include <GLFW/glfw3.h>

namespace InputEvent
{
	void HandleInput(GLFWwindow* window);
	
	void OnWindowResize(GLFWwindow* window, int width, int height);
}
