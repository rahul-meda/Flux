#include <glad/glad.h>
#include <iostream>
#include "Simulation.h"
#include "../input.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../Physics/Physics.h"
#include "../Components/Model.h"
#include "../Components/Body.h"

Simulation::Simulation()
	: prevFrame(0.0), displayDebug(false)
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

	Texture::CreateTexture(Texture::TextureType::WOOD, "resources/textures/container.jpg", false, textureData);
	Texture::CreateTexture(Texture::TextureType::SMILEY, "resources/textures/awesomeface.png", true, textureData);

	Texture::AddUniformLoc("woodTexture", textureData);
	Texture::AddUniformLoc("smileyTexture", textureData);

	Texture::SetUniforms(Graphics::GetInstance().worldShader, textureData);

	Transform tx;
	BodyDef bd;
	unsigned int bID;
	unsigned int tID = 0;

	tx = Transform(glm::vec3(-5.0f, 3.0f, 0.0f));
	bd.tx = tx;
	bID = Physics::GetInstance().AddBody(bd);
	Graphics::GetInstance().scales.push_back(tx.scale);
	gameObjects.push_back(GameObject(boxModel, bID));

	tx = Transform(glm::vec3(5.0f, 3.0f, 0.0f));
	bd.tx = tx;
	bID = Physics::GetInstance().AddBody(bd);
	Graphics::GetInstance().scales.push_back(tx.scale);
	gameObjects.push_back(GameObject(boxModel, bID));

	tx = Transform(glm::vec3(0.0f), glm::identity<glm::quat>(), glm::vec3(10.0f, 0.5f, 10.0f));
	bd.tx = tx;
	bID = Physics::GetInstance().AddBody(bd);
	Graphics::GetInstance().scales.push_back(tx.scale);
	gameObjects.push_back(GameObject(boxModel, bID));

	glUseProgram(Graphics::GetInstance().worldShader);
	glUniform3f(glGetUniformLocation(Graphics::GetInstance().worldShader, "objColor"), 1.0f, 0.5f, 0.3f);
	glUniform3f(glGetUniformLocation(Graphics::GetInstance().worldShader, "lightColor"), 1.0f, 1.0f, 1.0f);
	glUseProgram(0);

	Graphics::GetInstance().lightShader = Shader::CreateShader("Resources/VertexShader.vert",
															   "Resources/PointLight.frag");
	Graphics::GetInstance().AddPointLight(glm::vec3(0.0f, 10.0f, 10.0f));
	Graphics::GetInstance().lightModelID = boxModel;
}

void Simulation::Update(GLFWwindow* window)
{
	double currFrame = glfwGetTime();
	double delta = currFrame - prevFrame;
	prevFrame = currFrame;

	if (displayDebug)
		std::cout << delta << std::endl;

	static const float dt = 1.0f / 60.0f;

	InputEvent::HandleKeyInput(window, displayDebug, dt);

	Graphics::GetInstance().Update(gameObjects);

	Physics::GetInstance().Update(dt);

	switch (mouseData.pan)
	{
	case MouseInfo::PAN_LEFT:
		mouseData.lateral -= mouseData.sensitivity * 5.0;
		camera.Rotate(mouseData.lateral, mouseData.vertical);
		break;
	case MouseInfo::PAN_RIGHT:
		mouseData.lateral += mouseData.sensitivity * 5.0;
		camera.Rotate(mouseData.lateral, mouseData.vertical);
		break;
	case MouseInfo::PAN_UP:
		mouseData.vertical -= mouseData.sensitivity * 5.0;
		camera.Rotate(mouseData.lateral, mouseData.vertical);
		break;
	case MouseInfo::PAN_DOWN:
		mouseData.vertical += mouseData.sensitivity * 5.0;
		camera.Rotate(mouseData.lateral, mouseData.vertical);
		break;
	}

	glfwSwapBuffers(window);
	glfwPollEvents();
}