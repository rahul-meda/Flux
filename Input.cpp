#include <glad/glad.h>
#include "Input.h"
#include "Graphics/Camera.h"
#include "Simulation/Simulation.h"

void InputEvent::HandleKeyInput(GLFWwindow* window, double dt)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Simulation::GetInstance().camera.Translate(Camera::FWD, dt);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Simulation::GetInstance().camera.Translate(Camera::REV, dt);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Simulation::GetInstance().camera.Translate(Camera::LEFT, dt);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Simulation::GetInstance().camera.Translate(Camera::RIGHT, dt);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		Simulation::GetInstance().camera.Translate(Camera::UP, dt);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		Simulation::GetInstance().camera.Translate(Camera::DOWN, dt);
}

void InputEvent::OnWindowResize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void InputEvent::OnMouseMove(GLFWwindow* window, double x, double y)
{
	MouseInfo md = Simulation::GetInstance().mouseData;

	static bool firstMouse = true;
	if (firstMouse)
	{
		md.prevX = x;
		md.prevY = y;
		firstMouse = false;
	}

	float dx = x - md.prevX;
	float dy = md.prevY - y;
	md.prevX = x;
	md.prevY = y;

	float sensitivity = 0.1f; 
	dx *= sensitivity;
	dy *= sensitivity;

	md.lateral += dx;
	md.vertical += dy;

	if (md.vertical > 89.0f)
		md.vertical = 89.0f;
	if (md.vertical < -89.0f)
		md.vertical = -89.0f;

	Simulation::GetInstance().mouseData = md;
	Simulation::GetInstance().camera.Rotate(md.lateral, md.vertical);
}