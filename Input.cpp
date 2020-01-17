#include <glad/glad.h>
#include "Input.h"
#include "Graphics/Camera.h"
#include "Simulation/Simulation.h"
#include "Physics/Physics.h"

void InputEvent::KeyboardCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	Simulation::GetInstance().OnKeyPress(window, key, scanCode, action, mods);
}

void InputEvent::WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	Simulation::GetInstance().OnWindowResize(window, width, height);
}

void InputEvent::MouseMoveCallback(GLFWwindow* window, double x, double y)
{
	Simulation::GetInstance().OnMouseMove(window, x, y);
}