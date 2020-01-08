#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Window.h"
#include "Simulation/Simulation.h"
#include "Graphics/Shader.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Graphics/Model.h"
#include "Graphics/Graphics.h"
#include "Graphics/Texture.h"

int main()
{
	GLFWwindow* window =  Window::CreateWindow(1024, 768, "Flux");

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cout << "Failed to initialize GLAD" << std::endl;

	glEnable(GL_DEPTH_TEST);

	unsigned int shaderProgram = Shader::CreateShader("Resources/VertexShader.vert", "Resources/FragmentShader.frag");

	Simulation::GetInstance().Init(window, shaderProgram);

	ModelDef modelDef;

	modelDef.vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
	modelDef.vertices.push_back(glm::vec3(0.5f, -0.5f, 0.5f));
	modelDef.vertices.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
	modelDef.vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
	modelDef.vertices.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
	modelDef.vertices.push_back(glm::vec3(0.5f, -0.5f, -0.5f));
	modelDef.vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
	modelDef.vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));

	modelDef.indices = {0,1,2,2,3,0,4,5,6,6,7,4,1,5,6,6,2,1,0,4,7,7,3,0,0,1,5,5,4,0,3,2,6,6,7,3};

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

	glfwTerminate();
	return 0;
}