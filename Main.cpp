#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Window.h"
#include "Simulation/Simulations.h"

int main()
{
	int width, height;
	GLFWwindow* window =  Window::CreateWindow(width, height);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cout << "Failed to initialize GLAD" << std::endl;

	glEnable(GL_DEPTH_TEST);

	BoxStack::GetInstance().Init(Simulation::GetInstance().gameObjects);

	Simulation::GetInstance().Init(window, width, height);

	while (!glfwWindowShouldClose(window))
	{
		Simulation::GetInstance().Update(window);
	}

	return 0;
}