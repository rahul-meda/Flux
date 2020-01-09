#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Window.h"
#include "Simulation/Simulation.h"

int main()
{
	GLFWwindow* window =  Window::CreateWindow(1920, 1080, "Flux");

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cout << "Failed to initialize GLAD" << std::endl;

	glEnable(GL_DEPTH_TEST);

	Simulation::GetInstance().Init(window);

	while (!glfwWindowShouldClose(window))
	{
		Simulation::GetInstance().Update(window);
	}

	return 0;
}