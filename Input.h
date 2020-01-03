#pragma once

#include <GLFW/glfw3.h>

namespace InputEvent
{
	void HandleInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}

	void OnWindowResize(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
}
