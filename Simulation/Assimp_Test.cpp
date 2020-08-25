
#include <glm/gtx/matrix_decompose.hpp>
#include "Assimp_Test.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/World.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"
#include "../Physics/CapsuleCollider.h"

Assimp_Test& Assimp_Test::GetInstance()
{
	static Assimp_Test instance;
	return instance;
}

void Assimp_Test::Init(GLFWwindow* window, int width, int height)
{
	Simulation::Init(window, width, height);

	alwaysRun = true;

	HMesh mesh;
	ParseObj("Resources/Models/Box.obj", mesh);

	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal11_sp.jpg");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal11_sp.jpg");
	unsigned int boxEmTxt = Graphics::GetInstance().CreateTexture("resources/textures/border2_em.jpg");
	unsigned int ballTxt = Graphics::GetInstance().CreateTexture("resources/textures/sci_fi1.jpg");
	unsigned int carTxt = Graphics::GetInstance().CreateTexture("resources/textures/batman1.jpg");

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;
	R_Mesh obj;

	glm::vec3 p(0.0f, 0.0f, 0.0f);
	tx = Transform(p, glm::angleAxis(PI * 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bd.filterID = 1;
	bd.lockRotation = true;
	bID = Physics::GetInstance().AddBody(bd);
	CapsuleCollider* capsuleCollider = new CapsuleCollider();
	float h = 1.25f;
	capsuleCollider->com = glm::vec3(0.0f, 2.0f, 0.0f);
	capsuleCollider->Scale(1.0f);
	capsuleCollider->H = h;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	capsuleCollider->friction = 0.9f;
	capsuleCollider->restitution = 0.0f;
	capsuleCollider->massData->density = 9999999.0f;
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.txID = bID;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.025f));
	obj.scale = glm::vec3(1.0f);
	std::string path = "Resources/Models/morak/morak.fbx";
	obj.LoadModel(path);
	Graphics::GetInstance().animModels.push_back(obj);
	obj.Clear();

	objID = Graphics::GetInstance().animModels.size() - 1;
	txID = bID;

	animation.Init(path, obj.boneOffsets, obj.boneMap, obj.invBindTx);
	bPlayer = Physics::GetInstance().bodies[bID];
	animFSM.Init(bPlayer);
	camera.body = bPlayer;
	camera.follow = true;
	camera.position = glm::vec3(0.0f, 7.0f, -5.0f);
	camera.up = glm::vec3(0.0f, sinf(PI/4.0f), cosf(PI/4.0f));
	camera.target = glm::vec3(0.0f, 2.0f, 0.0f);
	camera.offset = glm::vec3(0.0f, 2.0f, 0.0f);
	camera.radius = glm::length(camera.position - camera.target);
	camera.minRadius = 2.0f;
	camera.maxRadius = 7.0f;

	const unsigned int asteroidCount = 100000;
	CreateAsteroidBelt(Graphics::GetInstance().instTransforms, asteroidCount);

	I_Mesh iMesh(asteroidCount);
	iMesh.LoadModel("resources/models/asteroid/asteroid.obj");
	//Graphics::GetInstance().instModels.push_back(iMesh);

	p = glm::vec3(0.0f, 10.0f, -50.0f);
	tx = Transform(p, glm::angleAxis(PI * 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = true;
	bd.angularVelocity = glm::vec3(0.0f, 0.1f, 0.0f);
	bID = Physics::GetInstance().AddBody(bd);
	SphereCollider* sphereCollider = new SphereCollider();
	sphereCollider->Scale(10.0f);
	Physics::GetInstance().AddCollider(bID, sphereCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.txID = bID;
	obj.scale = glm::vec3(10.0f, 10.0f, 1.0f);
	obj.LoadModel("resources/models/wall/box.obj");
	//Graphics::GetInstance().objects.push_back(obj);
}

void Assimp_Test::OnKeyTap(GLFWwindow * window, int key, int scanCode, int action, int mods)
{
	Simulation::OnKeyTap(window, key, scanCode, action, mods);

	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		int i = animation.GetAnimIndex();
		++i;
		if (i > 11)
		{
			i = 0;
		}
		animation.SetAnimIndex(i);
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		camera.follow = !camera.follow;
	}
	if (keyboard.IsKeyDown(GLFW_KEY_CAPS_LOCK))
	{
		alwaysRun = !alwaysRun;
	}
	if (keyboard.IsKeyDown(GLFW_KEY_SPACE))
	{
		animFSM.transition = T_IDLE_JUMP;
		//animation.SetAnimIndex(IDLE_JUMP + 1);
	}
}

void Assimp_Test::OnKeyPress(GLFWwindow * window)
{
	Simulation::OnKeyPress(window);
	
	if (keyboard.IsKeyDown(GLFW_KEY_W) && keyboard.IsKeyDown(GLFW_KEY_LEFT_SHIFT))
	{
		animFSM.transition = T_SPRINT;
	}
	else if (keyboard.IsKeyDown(GLFW_KEY_W))
	{
		animFSM.transition = alwaysRun ? T_RUN : T_WALK;
	}
	if (keyboard.IsKeyReleased(GLFW_KEY_W))
	{
		animFSM.transition = T_IDLE;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		glm::quat q = bPlayer->GetOrientation();
		q += deltaT * 0.5f * glm::quat(0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * q;
		bPlayer->SetOrientation(q);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		glm::quat q = bPlayer->GetOrientation();
		q += deltaT * 0.5f * glm::quat(0.0f, glm::vec3(0.0f, -1.0f, 0.0f)) * q;
		bPlayer->SetOrientation(q);
	}
}

void Assimp_Test::OnMouseMove(GLFWwindow* window, double x, double y)
{
	Simulation::OnMouseMove(window, x, y);

	if (!camera.follow)
		camera.Rotate(mouseData.lateral, mouseData.vertical, mouseData.dx, mouseData.dy);

	camera.isIdle = false;
}

void Assimp_Test::Update(GLFWwindow* window)
{
	Simulation::Update(window);

	if (camera.follow)
	{
		camera.Follow(mouseData.dx, mouseData.dy);
	}
	camera.isIdle = true;

	if (animFSM.Update())
	{
		animation.SetAnimIndex(animFSM.animID);
	}

	animation.Update();
}