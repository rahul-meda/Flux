#include <glad/glad.h>
#include "Simulation.h"
#include "../input.h"
#include "../Graphics/Graphics.h"

Simulation::Simulation()
	: prevFrame(0.0)
{}

Simulation::~Simulation()
{
	delete m_window;

	glfwTerminate();
}

Simulation& Simulation::GetInstance()
{
	static Simulation instance;
	return instance;
}

void Simulation::Init(GLFWwindow* window, unsigned int shaderProgram)
{
	m_window = window;
	m_shaderID = shaderProgram;
	glfwSetFramebufferSizeCallback(window, InputEvent::OnWindowResize);
	glfwSetCursorPosCallback(window, InputEvent::OnMouseMove);

	camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Simulation::Update()
{
	double currFrame = glfwGetTime();
	double dt = currFrame - prevFrame;
	prevFrame = currFrame;

	InputEvent::HandleKeyInput(m_window, dt);

	Graphics::GetInstance().Update(m_shaderID);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}