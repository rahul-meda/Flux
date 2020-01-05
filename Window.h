#pragma once
#pragma once

#include <GLFW/glfw3.h>

namespace Window
{
	GLFWwindow* CreateWindow(const int width, const int height, const char* title);
}