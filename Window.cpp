#include "Window.h"
#include <iostream>

#define WIDTH 1920
#define HEIGHT 1080

GLFWwindow * Window::CreateWindow(int& width, int& height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	width = WIDTH;
	height = HEIGHT;
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Flux", nullptr, nullptr);

	//GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Flux", glfwGetPrimaryMonitor(), nullptr);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);

	return window;
}
