
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Window.h"
#include "Simulation/Simulation.h"
#include "Graphics/Shader.h"
#include <vector>
#include <glm/vec3.hpp>
#include "Graphics/Model.h"
#include "Graphics/Graphics.h"

int main()
{
	GLFWwindow* window =  Window::CreateWindow(800, 600, "Flux");

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cout << "Failed to initialize GLAD" << std::endl;

	unsigned int shaderProgram = Shader::CreateShader("Resources/VertexShader.vs", "Resources/FragmentShader.fs");

	Simulation::GetInstance().Init(window, shaderProgram);

	std::vector<glm::vec3> vertices;
	vertices.push_back(glm::vec3(0.0f, 0.0f ,0.0f));
	vertices.push_back(glm::vec3(0.5f, 0.0f, 0.0f));
	vertices.push_back(glm::vec3(0.25f, 0.5f, 0.0f));

	std::vector<int> indices = {0, 1, 2};

	Graphics::GetInstance().models.push_back(Model(vertices, indices));

	while (!glfwWindowShouldClose(window))
	{
		Simulation::GetInstance().Update();
	}

	return 0;
}