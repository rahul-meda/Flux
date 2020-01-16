#include <glad/glad.h>
#include "Input.h"
#include "Graphics/Camera.h"
#include "Simulation/Simulation.h"
#include "Physics/Physics.h"

void InputEvent::HandleKeyInput(GLFWwindow* window, bool displayDebug, double dt)
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
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		Simulation::GetInstance().camera.Translate(Camera::DOWN, dt);

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		Simulation::GetInstance().displayDebug = !(Simulation::GetInstance().displayDebug);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		Body* b = &Physics::GetInstance().bodies[0];
		glm::vec3 v = b->GetVelocity();
		v += 0.2f * glm::vec3(1.0f, 0.0f, 0.0f);
		b->SetVelocity(v);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS)
	{
		Body* b = &Physics::GetInstance().bodies[1];
		glm::vec3 w = b->GetAngularVelocity();
		w += 0.2f * glm::vec3(1.0f, 0.0f, 0.0f);
		b->SetAngularVelocity(w);
	}
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
 
	dx *= md.sensitivity;
	dy *= md.sensitivity;

	md.lateral += dx;
	md.vertical += dy;

	if (md.vertical > 89.0f)
		md.vertical = 89.0f;
	if (md.vertical < -89.0f)
		md.vertical = -89.0f;

	if (x < 20.0f)
		md.pan = MouseInfo::PAN_LEFT;
	else if (x > 1000.0)
		md.pan = MouseInfo::PAN_RIGHT;
	else if (y < 20.0f)
		md.pan = MouseInfo::PAN_DOWN;
	else if (y > 750.0)
		md.pan = MouseInfo::PAN_UP;
	else
		md.pan = MouseInfo::NORMAL;

	Simulation::GetInstance().mouseData = md;
	Simulation::GetInstance().camera.Rotate(md.lateral, md.vertical);
}