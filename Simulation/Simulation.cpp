
#include <glad/glad.h>
#include <iostream>
#include "Simulation.h"
#include "../input.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../Components/Model.h"
#include "../Mesh/Geometry.h"
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
	Graphics::GetInstance().P = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 1000.0f);
	camera = Camera(glm::vec3(0.0f, 3.0f, 20.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	Graphics::GetInstance().worldShader = Shader::CreateShader("Resources/WorldVertexShader.vert",                                                                       "Resources/WorldFragmentShader.frag");

	Texture::CreateTexture(Texture::TextureType::WOOD, "resources/textures/container.jpg", false, textureData);
	Texture::CreateTexture(Texture::TextureType::SMILEY, "resources/textures/awesomeface.png", true, textureData);
	Texture::AddUniformLoc("woodTexture", textureData);
	Texture::AddUniformLoc("smileyTexture", textureData);
	Texture::SetUniforms(Graphics::GetInstance().worldShader, textureData);

	ModelDef box, sphere, line;
	HMesh mesh;
	ParseObj("Resources/Models/Box.obj", mesh);
	mesh.GetModelData(box);
	unsigned int boxModel = Graphics::GetInstance().CreateModel(box);
	glm::vec3 latte(1.0f, 0.97f, 0.9f);

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;

	tx = Transform(glm::vec3(0.0f), glm::identity<glm::quat>());
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(glm::vec3(50.0f, 0.5f, 50.0f));
	Physics::GetInstance().AddCollider(bID, boxCollider);
	Graphics::GetInstance().scales.push_back(glm::vec3(50.0f, 0.5f, 50.0f));
	gameObjects.push_back(GameObject(boxModel, bID, latte));

	CreateLine(line);
	unsigned int lineModel = Graphics::GetInstance().CreateModel(line);
	Graphics::GetInstance().lineModelID = lineModel;

	Graphics::GetInstance().lightShader = Shader::CreateShader("Resources/WorldVertexShader.vert",																 "Resources/FragmentShader.frag");
	Graphics::GetInstance().AddPointLight(glm::vec3(0.0f, 50.0f, 25.0f));
	Graphics::GetInstance().cubeModelID = boxModel;

	Graphics::GetInstance().Initialize();
	Physics::GetInstance().Initialize();
}

void Simulation::OnKeyPress(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		Physics::GetInstance().pause = !(Physics::GetInstance().pause);
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
		Physics::GetInstance().singleStep = true;
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		Physics::GetInstance().debugDraw = !(Physics::GetInstance().debugDraw);
		Graphics::GetInstance().points.clear();
		Graphics::GetInstance().lines.clear();
		Graphics::GetInstance().aabbs.clear();
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		Transform tx = Transform(camera.position + 3.0f*camera.fwd);
		ModelDef sphere;
		BodyDef bd;
		bd.tx = tx;
		bd.isStatic = false;
		bd.velocity = 20.f * camera.fwd;
		unsigned int bID = Physics::GetInstance().AddBody(bd);
		SphereCollider* sphereCollider = new SphereCollider();
		sphereCollider->Scale(0.5f);
		sphereCollider->massData->density = 10.0f;
		Physics::GetInstance().AddCollider(bID, sphereCollider);
		Graphics::GetInstance().scales.push_back(glm::vec3(0.5f));
		CreateSphere(sphere);
		unsigned int sphereModel = Graphics::GetInstance().CreateModel(sphere);
		gameObjects.push_back(GameObject(sphereModel, bID, glm::vec3(0.5f, 1.0f, 0.3f)));
	}
	if (key == GLFW_KEY_T)
	{
		Body* b = Physics::GetInstance().bodies[0];
		glm::vec3 v = b->GetVelocity();
		v = 25.0f * glm::vec3(1.0f, 0.0f, 0.0f);
		b->ApplyForce(glm::vec3(0.0f, 0.0f, -250.0f), glm::vec3(0.0f, 14.5f, 0.0f));
	}
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
}

void Simulation::OnWindowResize(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	Graphics::GetInstance().P = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f,1000.0f);
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

	static const float dt = 1.0f / hertz;

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