
#include <glad/glad.h>
#include <iostream>
#include "Simulation.h"
#include "../Graphics/Shader.h"
#include "../Mesh/Geometry.h"
#include "../Mesh/ObjParser.h"
#include "../Physics/Physics.h"
#include "../Components/Body.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"

#define MARGIN 30

Simulation::Simulation()
{}

Simulation::~Simulation()
{
	glfwTerminate();
}

void Simulation::Init(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	Graphics::GetInstance().P = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 10000.0f);
	camera = Camera(glm::vec3(0.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	Graphics::GetInstance().Initialize();
	Physics::GetInstance().Initialize();

	Graphics::GetInstance().worldShader = Shader::CreateShader("Resources/WorldVertexShader.vert", 
										  "Resources/WorldFragmentShader.frag");
	Graphics::GetInstance().animShader = Shader::CreateShader("Resources/AnimVertexShader.vert",
										 "Resources/WorldFragmentShader.frag");
	Graphics::GetInstance().instanceShader = Shader::CreateShader("Resources/InstanceVertexShader.vert",
										 "Resources/InstanceFragmentShader.frag");
	Graphics::GetInstance().skyboxShader = Shader::CreateShader("Resources/Skybox.vert", "Resources/Skybox.frag");

	HMesh mesh;
	ParseObj("Resources/Models/Box.obj", mesh);

	unsigned int floorDfTexture = Graphics::GetInstance().CreateTexture("resources/textures/chess2.jpg");
	unsigned int floorSpTexture = Graphics::GetInstance().CreateTexture("resources/textures/carbon_fiber3_sp.jpg");
	unsigned int floorEmTexture = Graphics::GetInstance().CreateTexture("resources/textures/lightning2_em.jpg");
	Material floorMaterial;
	floorMaterial.diffuseMap = floorDfTexture;
	floorMaterial.specularMap = floorDfTexture;
	floorMaterial.emissionMap = floorEmTexture;
	floorMaterial.nMaps = 3;

	unsigned int ballDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/leather1.jpg");
	unsigned int ballEmTxt = Graphics::GetInstance().CreateTexture("resources/textures/matrix.jpg");
	material.diffuseMap = ballDfTxt;
	material.specularMap = ballDfTxt;
	material.emissionMap = ballEmTxt;
	material.nMaps = 2;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;
	R_Mesh obj;
	glm::vec3 s1(50.0f, 0.5f, 50.0f);

	tx = Transform(glm::vec3(0.0f, -0.5f, 0.0f), glm::angleAxis(PI, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	obj.txID = bID;
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s1);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scale = s1;
	obj.LoadModel("resources/models/floor/floor.obj");
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	unsigned int wallTexture = Graphics::GetInstance().CreateTexture("resources/textures/batman1.jpg");
	Material wallMaterial;
	wallMaterial.diffuseMap = wallTexture;
	wallMaterial.specularMap = wallTexture;
	wallMaterial.emissionMap = ballEmTxt;
	wallMaterial.nMaps = 3;

	glm::vec3 s2(0.5f, 100.0f, 50.0f);
	tx = Transform(glm::vec3(-s1.x, s2.z, 0.0f), glm::angleAxis(PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s2);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.txID = bID;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj. scale = (s2);
	obj.LoadModel("resources/models/box/box.obj");
	obj.scale = s2;
	//Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	tx = Transform(glm::vec3(s1.x, s2.z, 0.0f), glm::angleAxis(PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s2);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.txID = bID;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj. scale = (s2);
	obj.LoadModel("resources/models/box/box.obj");
	obj.scale = s2;
	//Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	glm::quat q = glm::angleAxis(PI * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
	tx = Transform(glm::vec3(0.0f, s2.z, s1.z), q * glm::angleAxis(PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s2);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.txID = bID;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj. scale = (s2);
	obj.LoadModel("resources/models/box/box.obj");
	obj.scale = s2;
	//Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	tx = Transform(glm::vec3(0.0f, s2.z, -s1.z), q * glm::angleAxis(PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s2);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.txID = bID;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj. scale = (s2);
	obj.LoadModel("resources/models/box/box.obj");
	obj.scale = s2;
	//Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	Graphics::GetInstance().lightShader = Shader::CreateShader("Resources/WorldVertexShader.vert",																									 "Resources/FragmentShader.frag");

	Graphics::GetInstance().AddPointLight(glm::vec3(-25.0f, 2.5f,  25.0f));
	Graphics::GetInstance().AddPointLight(glm::vec3( 25.0f, 0.5f,  25.0f));
	Graphics::GetInstance().AddPointLight(glm::vec3(-25.0f, 0.5f, -25.0f));
	Graphics::GetInstance().AddPointLight(glm::vec3( 25.0f, 0.5f, -25.0f));
}

void Simulation::OnKeyTap(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		Physics::GetInstance().pause = !(Physics::GetInstance().pause);
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
		Physics::GetInstance().singleStep = true;
	if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		Physics::GetInstance().slowMo = !(Physics::GetInstance().slowMo);
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		Physics::GetInstance().debugDraw = !(Physics::GetInstance().debugDraw);
		Graphics::GetInstance().points.clear();
		Graphics::GetInstance().lines.clear();
		Graphics::GetInstance().aabbs.clear();
	}
	if (keyboard.IsKeyDown(GLFW_KEY_R))
	{
		Transform tx = Transform(camera.position + 3.0f*camera.fwd);
		BodyDef bd;
		bd.tx = tx;
		bd.isStatic = false;
		bd.velocity = 20.0f * camera.fwd;
		unsigned int bID = Physics::GetInstance().AddBody(bd);
		SphereCollider* sphereCollider = new SphereCollider();
		sphereCollider->Scale(0.5f);
		sphereCollider->massData->density = 1.0f;
		Physics::GetInstance().AddCollider(bID, sphereCollider);
		R_Mesh obj;
		obj.txID = bID;
		obj.scale = (glm::vec3(0.5f));
		obj.posOffsets.push_back(glm::vec3(0.0f));
		obj.rotOffsets.push_back(glm::mat3(1.0f));
		obj.LoadModel(Graphics::GetInstance().dSphere);
		Graphics::GetInstance().objects.push_back(obj);
	}
}

void Simulation::OnKeyPress(GLFWwindow* window)
{
	if(glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
		camera.Translate(Camera::FWD);
	else if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
		camera.Translate(Camera::REV);
	else if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
		camera.Translate(Camera::LEFT);
	else if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
		camera.Translate(Camera::RIGHT);
	else if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS)
		camera.Translate(Camera::UP);
	else if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
		camera.Translate(Camera::DOWN);
	else
		camera.velocity = glm::vec3(0.0f);
}

void Simulation::OnWindowResize(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	Graphics::GetInstance().P = glm::perspective(glm::radians(60.0f), (float)width/(float)height, 0.1f, 10000.0f);
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

	mouseData.sensitivity = 0.25f;

	mouseData.dx = x - mouseData.prevX;
	mouseData.dy = mouseData.prevY - y;
	mouseData.prevX = x;
	mouseData.prevY = y;

	mouseData.dx *= mouseData.sensitivity;
	mouseData.dy *= mouseData.sensitivity;

	mouseData.lateral += mouseData.dx;
	mouseData.vertical += mouseData.dy;

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
		mouseData.pan = MouseInfo::DEFAULT;
}

void Simulation::OnMouseScroll(GLFWwindow * window, double xoffset, double yoffset)
{
	camera.radius -= yoffset;
	camera.radius = glm::clamp(camera.radius, 2.0f, 25.0f);
}

void Simulation::Update(GLFWwindow* window)
{
	keyboard.Update(window);

	if (mouseData.pan == MouseInfo::PAN_LEFT)
	{
		camera.isIdle = false;
		mouseData.dx = -1.0f;
	}
	else if (mouseData.pan == MouseInfo::PAN_RIGHT)
	{
		camera.isIdle = false;
		mouseData.dx = 1.0f;
	}

	Graphics::GetInstance().V = camera.ViewSpace();
	Graphics::GetInstance().eye = camera.position;
}