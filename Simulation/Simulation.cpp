
#include <glad/glad.h>
#include <iostream>
#include "Simulation.h"
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

void Simulation::Init(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	Graphics::GetInstance().P = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
	camera = Camera(glm::vec3(0.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	Graphics::GetInstance().Initialize();
	Physics::GetInstance().Initialize();

	Graphics::GetInstance().worldShader = Shader::CreateShader("Resources/WorldVertexShader.vert", "Resources/WorldFragmentShader.frag");

	ModelDef floor;
	HMesh mesh;

	ParseObj("Resources/Models/Box.obj", mesh);
	unsigned int boxModel = Graphics::GetInstance().cubeModelID;
	mesh.GetModelData(floor);
	int N = floor.textureCoords.size();
	for (int i = 0; i < N; ++i)
	{
		floor.textureCoords[i] *= 32.0f;
	}
	unsigned int floorModel = Graphics::GetInstance().CreateModel(floor);

	unsigned int floorTexture = Graphics::GetInstance().CreateTexture("resources/textures/sci_fi1.jpg");
	Material floorMaterial;
	floorMaterial.diffuseMap = floorTexture;
	floorMaterial.specularMap = floorTexture;
	floorMaterial.count = 2;

	unsigned int ballDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/leather1.jpg");
	unsigned int ballEmTxt = Graphics::GetInstance().CreateTexture("resources/textures/matrix.jpg");
	material.diffuseMap = ballDfTxt;
	material.specularMap = ballDfTxt;
	material.emissionMap = ballEmTxt;
	material.count = 2;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;
	R_Object obj;
	glm::vec3 s1(100.0f, 0.5f, 100.0f);

	tx = Transform(glm::vec3(0.0f, -0.5f, 0.0f), glm::identity<glm::quat>());
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	obj.pos = tx.position;
	obj.rot = tx.R;
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s1);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(s1);
	obj.modelIDs.push_back(floorModel);
	obj.materials.push_back(floorMaterial);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	unsigned int wallTexture = Graphics::GetInstance().CreateTexture("resources/textures/batman1.jpg");
	Material wallMaterial;
	wallMaterial.diffuseMap = wallTexture;
	wallMaterial.specularMap = wallTexture;
	wallMaterial.emissionMap = ballEmTxt;
	wallMaterial.count = 3;

	glm::vec3 s2(0.5f, 100.0f, 50.0f);
	tx = Transform(glm::vec3(-s1.x, s2.z, 0.0f), glm::angleAxis(PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s2);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(s2);
	obj.modelIDs.push_back(boxModel);
	obj.materials.push_back(wallMaterial);
	obj.scale = s2;
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	tx = Transform(glm::vec3(s1.x, s2.z, 0.0f), glm::angleAxis(PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s2);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(s2);
	obj.modelIDs.push_back(boxModel);
	obj.materials.push_back(wallMaterial);
	obj.scale = s2;
	Graphics::GetInstance().objects.push_back(obj);
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
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(s2);
	obj.modelIDs.push_back(boxModel);
	obj.materials.push_back(wallMaterial);
	obj.scale = s2;
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	tx = Transform(glm::vec3(0.0f, s2.z, -s1.z), q * glm::angleAxis(PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s2);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(s2);
	obj.modelIDs.push_back(boxModel);
	obj.materials.push_back(wallMaterial);
	obj.scale = s2;
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	Graphics::GetInstance().lightShader = Shader::CreateShader("Resources/WorldVertexShader.vert",																									 "Resources/FragmentShader.frag");

	Graphics::GetInstance().AddPointLight(glm::vec3(-50.0f, 20.0f, 50.0f));
	Graphics::GetInstance().AddPointLight(glm::vec3(50.0f, 20.0f, 50.0f));
	Graphics::GetInstance().AddPointLight(glm::vec3(-50.0f, 20.0f, -50.0f));
	Graphics::GetInstance().AddPointLight(glm::vec3(50.0f, 20.0f, -50.0f));
}

void Simulation::OnKeyTap(GLFWwindow* window, int key, int scanCode, int action, int mods)
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
		bd.velocity = 20.0f * camera.fwd;
		unsigned int bID = Physics::GetInstance().AddBody(bd);
		SphereCollider* sphereCollider = new SphereCollider();
		sphereCollider->Scale(0.25f);
		sphereCollider->massData->density = 1.0f;
		Physics::GetInstance().AddCollider(bID, sphereCollider);
		unsigned int sphereModel = Graphics::GetInstance().sphereModelID;
		R_Object obj;
		obj.pos = tx.position;
		obj.rot = tx.R;
		obj.scales.push_back(glm::vec3(0.25f));
		obj.posOffsets.push_back(glm::vec3(0.0f));
		obj.rotOffsets.push_back(glm::mat3(1.0f));
		obj.modelIDs.push_back(sphereModel);
		obj.materials.push_back(material);
		obj.scale = glm::vec3(1.0f);
		Graphics::GetInstance().objects.push_back(obj);
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		HingeJoint* hj = &Physics::GetInstance().hingeJoints[0];
		float speed = hj->GetMotorSpeed();

		if (speed <= 4.0f)
			speed += 0.1f;

		hj->SetMotorSpeed(speed);
	}
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		HingeJoint* hj = &Physics::GetInstance().hingeJoints[0];
		float speed = hj->GetMotorSpeed();

		if (speed > 0.0f)
			speed -= 0.1f;
		if (speed < 0.0f)
			speed = 0.0f;

		hj->SetMotorSpeed(speed);
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		camera.follow = true;
		int n = Physics::GetInstance().bodies.size();
		camera.body = Physics::GetInstance().bodies[n - 1];
	}
	if (key == GLFW_KEY_U && action == GLFW_PRESS)
	{
		camera.follow = false;
	}
}

void Simulation::OnKeyPress(GLFWwindow* window)
{
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.Translate(Camera::FWD);
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.Translate(Camera::REV);
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.Translate(Camera::LEFT);
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.Translate(Camera::RIGHT);
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.Translate(Camera::UP);
	else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		camera.Translate(Camera::DOWN);
	else
		camera.velocity = glm::vec3(0.0f);
}

void Simulation::OnWindowResize(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	Graphics::GetInstance().P = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 1000.0f);
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

	/*static const float dt = 1.0f / hertz;

	Physics::GetInstance().Update(dt);*/

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

	if (camera.follow)
	{
		Transform tx = camera.body->GetTransform();
		camera.target = tx.position;
		camera.position = tx.R * glm::vec3(-10.0f, 2.0f, 0.0f) + tx.position;
	}

	Graphics::GetInstance().Update(camera);

	glfwSwapBuffers(window);
	glfwPollEvents();
}