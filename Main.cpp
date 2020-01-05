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
#include "Graphics/Texture.h"

int main()
{
	GLFWwindow* window =  Window::CreateWindow(1024, 768, "Flux");

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cout << "Failed to initialize GLAD" << std::endl;

	unsigned int shaderProgram = Shader::CreateShader("Resources/VertexShader.vert", "Resources/FragmentShader.frag");

	Simulation::GetInstance().Init(window, shaderProgram);

	ModelDef modelDef;

	modelDef.vertices.push_back(glm::vec3(-0.5f, -0.5f ,0.0f));
	modelDef.vertices.push_back(glm::vec3(0.5f, -0.5f, 0.0f));
	modelDef.vertices.push_back(glm::vec3(0.5f, 0.5f, 0.0f));
	modelDef.vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.0f));

	modelDef.colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	modelDef.colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	modelDef.colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	modelDef.colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f));

	modelDef.textureCoords.push_back(glm::vec2(0.0f, 0.0f));
	modelDef.textureCoords.push_back(glm::vec2(1.0f, 0.0f));
	modelDef.textureCoords.push_back(glm::vec2(1.0f, 1.0f));
	modelDef.textureCoords.push_back(glm::vec2(0.0f, 1.0f));

	modelDef.indices = {0, 1, 2, 2, 3, 0};

	Graphics::GetInstance().models.push_back(modelDef);

	CreateTexture(Texture::TextureType::WOOD, "resources/textures/container.jpg", false, 
		Simulation::GetInstance().textureData);
	CreateTexture(Texture::TextureType::SMILEY, "resources/textures/awesomeface.png", true,
		Simulation::GetInstance().textureData);

	Texture::AddUniformLoc("woodTexture", Simulation::GetInstance().textureData);
	Texture::AddUniformLoc("smileyTexture", Simulation::GetInstance().textureData);

	Texture::SetUniforms(shaderProgram, Simulation::GetInstance().textureData);

	while (!glfwWindowShouldClose(window))
	{
		Simulation::GetInstance().Update();
	}

	return 0;
}