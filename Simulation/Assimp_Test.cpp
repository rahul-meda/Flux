
#include "Assimp_Test.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"
#include "../Physics/CapsuleCollider.h"
#include <glm/gtx/matrix_decompose.hpp>

Assimp_Test& Assimp_Test::GetInstance()
{
	static Assimp_Test instance;
	return instance;
}

void Assimp_Test::Init(GLFWwindow* window, int width, int height)
{
	Simulation::Init(window, width, height);

	HMesh mesh;
	ParseObj("Resources/Models/Box.obj", mesh);

	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal11_sp.jpg");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal11_sp.jpg");
	unsigned int boxEmTxt = Graphics::GetInstance().CreateTexture("resources/textures/border2_em.jpg");
	unsigned int ballTxt = Graphics::GetInstance().CreateTexture("resources/textures/sci_fi1.jpg");
	unsigned int carTxt = Graphics::GetInstance().CreateTexture("resources/textures/batman1.jpg");
	Material material;
	material.diffuseMap = ballTxt;
	material.specularMap = ballTxt;
	material.nMaps = 2;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;
	R_Mesh obj;

	material.diffuseMap = boxDfTxt;
	material.specularMap = boxSpTxt;
	material.emissionMap = boxEmTxt;
	material.nMaps = 3;

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
	obj.bodyID = bID;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.025f));
	obj.scale = glm::vec3(1.0f);
	std::string path = "Resources/Models/sandy/sandy.fbx";
	obj.LoadModel(path);
	Graphics::GetInstance().animModels.push_back(obj);
	obj.Clear();

	objID = Graphics::GetInstance().animModels.size() - 1;

	animation.Init(path, obj.boneOffsets, obj.boneMap, obj.invBindTx);
}

void Assimp_Test::OnKeyTap(GLFWwindow * window, int key, int scanCode, int action, int mods)
{
	Simulation::OnKeyTap(window, key, scanCode, action, mods);

	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		int i = animation.GetAnimIndex();
		++i;
		if (i > 4)
		{
			i = 0;
		}
		animation.SetAnimIndex(i);
	}
}

void Assimp_Test::OnKeyPress(GLFWwindow * window)
{
	Simulation::OnKeyPress(window);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		unsigned int bID = Graphics::GetInstance().animModels[objID].bodyID;
		Body* body = Physics::GetInstance().bodies[bID];
		glm::vec3 fwd = body->GetTransform().R[2]; 
		body->ApplyForceCOM(body->GetMass() * 10.0f * fwd);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		unsigned int bID = Graphics::GetInstance().animModels[objID].bodyID;
		Body* body = Physics::GetInstance().bodies[bID];
		body->ApplyTorque(body->GetInvMass() * 10.0f * (glm::vec3(0.0f, 1.0f, 0.0f)));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		unsigned int bID = Graphics::GetInstance().animModels[objID].bodyID;
		Body* body = Physics::GetInstance().bodies[bID];
		body->ApplyTorque(body->GetInvMass() * 10.0f * (glm::vec3(0.0f, -1.0f, 0.0f)));
	}
}

void Assimp_Test::Update(GLFWwindow* window)
{
	Simulation::Update(window);

	animation.Update();
}