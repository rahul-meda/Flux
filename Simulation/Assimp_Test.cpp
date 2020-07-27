
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

	glm::vec3 p(0.0f, 5.0f, 0.0f);
	tx = Transform(p, glm::angleAxis(PI * 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bd.filterID = 1;
	bID = Physics::GetInstance().AddBody(bd);
	SphereCollider* sphereCollider = new SphereCollider();
	sphereCollider->Scale(1.0f);
	sphereCollider->friction = 0.9f;
	sphereCollider->restitution = 0.1f;
	sphereCollider->massData->density = 1.0f;
	Physics::GetInstance().AddCollider(bID, sphereCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.1f));
	std::string path = "Resources/Models/sandy/erika_archer_bow_arrow.fbx";
	obj.LoadModel(path);
	Graphics::GetInstance().animModels.push_back(obj);
	obj.Clear();

	objID = Graphics::GetInstance().animModels.size() - 1;

	walkAnimation.Init(path, obj.boneOffsets, obj.boneMap, obj.invBindTx);
}

void Assimp_Test::OnKeyTap(GLFWwindow * window, int key, int scanCode, int action, int mods)
{
	Simulation::OnKeyTap(window, key, scanCode, action, mods);
}

void Assimp_Test::OnKeyPress(GLFWwindow * window)
{
	Simulation::OnKeyPress(window);
}

void Assimp_Test::Update(GLFWwindow* window)
{
	Simulation::Update(window);

	walkAnimation.Update();
}