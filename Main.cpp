
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Window.h"
#include "Simulation/Simulations.h"
#include "Physics/Physics.h"

Assimp_Test simulation;

void KeyboardCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	simulation.OnKeyTap(window, key, scanCode, action, mods);
}

void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	simulation.OnWindowResize(window, width, height);
}

void MouseMoveCallback(GLFWwindow* window, double x, double y)
{
	simulation.OnMouseMove(window, x, y);
}

void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	simulation.OnMouseScroll(window, xoffset, yoffset);
}

int main()
{
	int width, height;
	GLFWwindow* window = Window::CreateWindow(width, height);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cout << "Failed to initialize GLAD" << std::endl;

	glEnable(GL_DEPTH_TEST);

	glfwSetFramebufferSizeCallback(window, WindowResizeCallback);
	glfwSetKeyCallback(window, KeyboardCallback);
	glfwSetCursorPosCallback(window, MouseMoveCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);

	simulation.Init(window, width, height);
	Graphics::GetInstance().PostInit();

	while (!glfwWindowShouldClose(window))
	{
		static const float dt = 1.0f / hertz;

		Physics::GetInstance().Update(dt);
		simulation.OnKeyPress(window);
		simulation.Update(window);
		Graphics::GetInstance().Update();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}