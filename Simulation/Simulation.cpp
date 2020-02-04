#include <glad/glad.h>
#include <iostream>
#include "Simulation.h"
#include "../input.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../Components/Model.h"
#include "../Mesh/SphereModel.h"
#include "../Mesh/ObjParser.h"
#include "../Physics/Physics.h"
#include "../Components/Body.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"

#define MARGIN 30

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

void Simulation::Init(GLFWwindow* window, int w, int h)
{
	glfwSetFramebufferSizeCallback(window, InputEvent::WindowResizeCallback);
	glfwSetKeyCallback(window, InputEvent::KeyboardCallback);
	glfwSetCursorPosCallback(window, InputEvent::MouseMoveCallback);

	width = w;
	height = h;
	glViewport(0, 0, width, height);
	Graphics::GetInstance().P = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	camera = Camera(glm::vec3(30.0f, 3.0f, 20.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	Graphics::GetInstance().worldShader = Shader::CreateShader("Resources/VertexShader.vert",                                                                                        "Resources/FragmentShader.frag");

	Texture::CreateTexture(Texture::TextureType::WOOD, "resources/textures/container.jpg", false, textureData);
	Texture::CreateTexture(Texture::TextureType::SMILEY, "resources/textures/awesomeface.png", true, textureData);
	Texture::AddUniformLoc("woodTexture", textureData);
	Texture::AddUniformLoc("smileyTexture", textureData);
	Texture::SetUniforms(Graphics::GetInstance().worldShader, textureData);

	ModelDef box, sphere;
	HMesh mesh;
	ParseObj("Resources/Models/Box.obj", mesh);
	mesh.GetModelData(box);
	unsigned int boxModel = Graphics::GetInstance().CreateModel(box);

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;

	tx = Transform(glm::vec3(-5.0f, 3.0f, 0.0f));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	HullCollider* boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(tx.scale);
	Physics::GetInstance().bodies.back()->AddCollider(boxCollider);
	Graphics::GetInstance().scales.push_back(tx.scale);
	gameObjects.push_back(GameObject(boxModel, bID));

	tx = Transform(glm::vec3(5.0f, 3.0f, 0.0f));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(tx.scale);
	Physics::GetInstance().bodies.back()->AddCollider(boxCollider);
	Graphics::GetInstance().scales.push_back(tx.scale);
	gameObjects.push_back(GameObject(boxModel, bID));

	tx = Transform(glm::vec3(0.0f), glm::identity<glm::quat>(), glm::vec3(10.0f, 0.5f, 10.0f));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(tx.scale);
	Physics::GetInstance().bodies.back()->AddCollider(boxCollider);
	Graphics::GetInstance().scales.push_back(tx.scale);
	gameObjects.push_back(GameObject(boxModel, bID));

	CreateSphere(sphere);
	unsigned int sphereModel = Graphics::GetInstance().CreateModel(sphere);

	for (int i = 0; i < 9; ++i)
	{
		tx = Transform(glm::vec3(30.0f, 7.0f + (float)i*3.0f, 0.0f));
		bd.tx = tx;
		bd.isStatic = false;
		bID = Physics::GetInstance().AddBody(bd);
		SphereCollider* sphereCollider = new SphereCollider();
		sphereCollider->Scale(1.0f);
		Physics::GetInstance().bodies.back()->AddCollider(sphereCollider);
		Graphics::GetInstance().scales.push_back(glm::vec3(1.0f));
		gameObjects.push_back(GameObject(sphereModel, bID));
	}

	tx = Transform(glm::vec3(30.0f, 0.0f, 0.0f));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	SphereCollider* sphereCollider = new SphereCollider();
	sphereCollider->Scale(5.0f);
	Physics::GetInstance().bodies.back()->AddCollider(sphereCollider);
	Graphics::GetInstance().scales.push_back(glm::vec3(5.0f));
	gameObjects.push_back(GameObject(sphereModel, bID));

	glUseProgram(Graphics::GetInstance().worldShader);
	glUniform3f(glGetUniformLocation(Graphics::GetInstance().worldShader, "objColor"), 0.9f, 0.5f, 0.3f);
	glUniform3f(glGetUniformLocation(Graphics::GetInstance().worldShader, "lightColor"), 1.0f, 1.0f, 1.0f);
	glUseProgram(0);

	Graphics::GetInstance().lightShader = Shader::CreateShader("Resources/VertexShader.vert",
															   "Resources/PointLight.frag");
	Graphics::GetInstance().AddPointLight(glm::vec3(0.0f, 10.0f, 10.0f));
	Graphics::GetInstance().lightModelID = boxModel;

	Physics::GetInstance().Initialize();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Simulation::OnKeyPress(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		Physics::GetInstance().pause = !(Physics::GetInstance().pause);
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
		Physics::GetInstance().singleStep = true;
}

void Simulation::OnKeyPressHold(GLFWwindow* window)
{
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Simulation::GetInstance().camera.Translate(Camera::FWD);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Simulation::GetInstance().camera.Translate(Camera::REV);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Simulation::GetInstance().camera.Translate(Camera::LEFT);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Simulation::GetInstance().camera.Translate(Camera::RIGHT);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		Simulation::GetInstance().camera.Translate(Camera::UP);
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		Simulation::GetInstance().camera.Translate(Camera::DOWN);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		Body* b = Physics::GetInstance().bodies[0];
		glm::vec3 v = b->GetVelocity();
		v += 0.2f * glm::vec3(1.0f, 0.0f, 0.0f);
		b->SetVelocity(v);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS)
	{
		Body* b = Physics::GetInstance().bodies[1];
		glm::vec3 w = b->GetAngularVelocity();
		w += 0.2f * glm::vec3(1.0f, 0.0f, 0.0f);
		b->SetAngularVelocity(w);
	}
}

void Simulation::OnWindowResize(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	Graphics::GetInstance().P = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f,100.0f);
}

void Simulation::OnMouseMove(GLFWwindow* window, double x, double y)
{
	static bool firstMouse = true;
	if (firstMouse)
	{
		mouseData.prevX = x;
		mouseData.prevY = y;
		firstMouse = false;
	}

	float dx = x - mouseData.prevX;
	float dy = mouseData.prevY - y;
	mouseData.prevX = x;
	mouseData.prevY = y;

	dx *= mouseData.sensitivity;
	dy *= mouseData.sensitivity;

	mouseData.lateral += dx;
	mouseData.vertical += dy;

	if (mouseData.vertical > 89.0f)
		mouseData.vertical = 89.0f;
	if (mouseData.vertical < -89.0f)
		mouseData.vertical = -89.0f;

	if (x < MARGIN)
		mouseData.pan = MouseInfo::PAN_LEFT;
	else if (x > width - MARGIN)
		mouseData.pan = MouseInfo::PAN_RIGHT;
	else if (y < MARGIN)
		mouseData.pan = MouseInfo::PAN_DOWN;
	else if (y > height - MARGIN)
		mouseData.pan = MouseInfo::PAN_UP;
	else
		mouseData.pan = MouseInfo::NORMAL;

	camera.Rotate(mouseData.lateral, mouseData.vertical);
}

void Simulation::Update(GLFWwindow* window)
{
	double currFrame = glfwGetTime();
	double delta = currFrame - prevFrame;
	prevFrame = currFrame;

	static const float dt = 1.0f / 60.0f;

	OnKeyPressHold(window);

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

	Graphics::GetInstance().Update(gameObjects);

	glfwSwapBuffers(window);
	glfwPollEvents();
}