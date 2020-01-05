#include <glad/glad.h>
#include "Input.h"

void InputEvent::HandleInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void InputEvent::OnWindowResize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}