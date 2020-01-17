#pragma once

#include <GLFW/glfw3.h>

struct MouseInfo;

namespace InputEvent
{
	void KeyboardCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);
	
	void WindowResizeCallback(GLFWwindow* window, int width, int height);

	void MouseMoveCallback(GLFWwindow* window, double x, double y);
}