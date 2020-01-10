#include <glad/glad.h>
#include "Simulation.h"
#include "../input.h"
#include "../Graphics/Graphics.h"
#include "../Components/Model.h"
#include "../Graphics/Shader.h"

Simulation::Simulation()
	: prevFrame(0.0)
{}

Simulation::~Simulation()
{
	glfwTerminate();
}

Simulation& Simulation::GetInstance()
{
	static Simulation instance;
	return instance;
}

void Simulation::Init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, InputEvent::OnWindowResize);
	glfwSetCursorPosCallback(window, InputEvent::OnMouseMove);

	camera = Camera(glm::vec3(0.0f, 3.0f, 20.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	ModelDef box;

	// front, back, right, left, bot, top
	box.vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(0.5f, -0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(0.5f, -0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(0.5f, -0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(0.5f, -0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(0.5f, -0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(0.5f, -0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(0.5f, -0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(0.5f, -0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
	box.vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));
	box.vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));

	for (int i = 0; i < 6; i++)
		box.normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	for (int i = 0; i < 6; i++)
		box.normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	for (int i = 0; i < 6; i++)
		box.normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	for (int i = 0; i < 6; i++)
		box.normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	for (int i = 0; i < 6; i++)
		box.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	for (int i = 0; i < 6; i++)
		box.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	unsigned int boxModel = Graphics::GetInstance().CreateModel(box);

	Graphics::GetInstance().worldShader = Shader::CreateShader("Resources/VertexShader.vert", 
															   "Resources/FragmentShader.frag");

	Texture::CreateTexture(Texture::TextureType::WOOD, "resources/textures/container.jpg", false,
						   Simulation::GetInstance().textureData);
	Texture::CreateTexture(Texture::TextureType::SMILEY, "resources/textures/awesomeface.png", true,
						   Simulation::GetInstance().textureData);

	Texture::AddUniformLoc("woodTexture", Simulation::GetInstance().textureData);
	Texture::AddUniformLoc("smileyTexture", Simulation::GetInstance().textureData);

	Texture::SetUniforms(Graphics::GetInstance().worldShader, Simulation::GetInstance().textureData);

	transforms.push_back(Transform(glm::vec3(-5.0f, 0.0f, 0.0f), glm::identity<glm::quat>()));
	gameObjects.push_back(GameObject(boxModel, transforms.size() - 1));
	transforms.push_back(Transform(glm::vec3(5.0f, 0.0f, 0.0f), glm::identity<glm::quat>()));
	gameObjects.push_back(GameObject(boxModel, transforms.size() - 1));

	glUseProgram(Graphics::GetInstance().worldShader);
	glUniform3f(glGetUniformLocation(Graphics::GetInstance().worldShader, "objColor"), 1.0f, 0.5f, 0.3f);
	glUniform3f(glGetUniformLocation(Graphics::GetInstance().worldShader, "lightColor"), 1.0f, 1.0f, 1.0f);
	glUseProgram(0);

	Graphics::GetInstance().lightShader = Shader::CreateShader("Resources/VertexShader.vert",
															   "Resources/PointLight.frag");
	Graphics::GetInstance().AddPointLight(glm::vec3(10.0f, 0.0f, 0.0f));
	Graphics::GetInstance().lightModelID = boxModel;
}

void Simulation::Update(GLFWwindow* window)
{
	double currFrame = glfwGetTime();
	double dt = currFrame - prevFrame;
	prevFrame = currFrame;

	InputEvent::HandleKeyInput(window, dt);

	Graphics::GetInstance().Update(gameObjects);

	switch (mouseData.pan)
	{
	case MouseInfo::PAN_LEFT:
		mouseData.lateral -= mouseData.sensitivity;
		camera.Rotate(mouseData.lateral, mouseData.vertical);
		break;
	case MouseInfo::PAN_RIGHT:
		mouseData.lateral += mouseData.sensitivity;
		camera.Rotate(mouseData.lateral, mouseData.vertical);
		break;
	case MouseInfo::PAN_UP:
		mouseData.vertical -= mouseData.sensitivity;
		camera.Rotate(mouseData.lateral, mouseData.vertical);
		break;
	case MouseInfo::PAN_DOWN:
		mouseData.vertical += mouseData.sensitivity;
		camera.Rotate(mouseData.lateral, mouseData.vertical);
		break;
	}

	glfwSwapBuffers(window);
	glfwPollEvents();
}