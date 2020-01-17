#include "Window.h"
#include <iostream>

#define WIDTH 1024
#define HEIGHT 768

GLFWwindow * Window::CreateWindow(int&width, int& height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	width = WIDTH;
	height = HEIGHT;
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Flux", nullptr, nullptr);

	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);

	return window;
}
