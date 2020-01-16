#pragma once

#include <GLFW/glfw3.h>

struct MouseInfo;

namespace InputEvent
{
	void HandleKeyInput(GLFWwindow* window, bool displayDebug, double dt);
	
	void OnWindowResize(GLFWwindow* window, int width, int height);

	void OnMouseMove(GLFWwindow* window, double x, double y);
}