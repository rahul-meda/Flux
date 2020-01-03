#include <glad/glad.h>
#include "Simulation.h"
#include "../input.h"
#include "../Graphics/Graphics.h"

Simulation::Simulation()
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
}

void Simulation::Update()
{
	InputEvent::HandleInput(m_window);

	Graphics::GetInstance().Update(m_shaderID);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}