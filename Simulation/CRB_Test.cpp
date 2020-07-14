
#include "CRB_Test.h"
#include "../Mesh/ObjParser.h"
#include "../Components/Model.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"
#include "../Physics/CapsuleCollider.h"

CRB_Test& CRB_Test::GetInstance()
{
	static CRB_Test instance;
	return instance;
}

void CRB_Test::Init(GLFWwindow* window, int width, int height)
{
	Simulation::Init(window, width, height);

	ModelDef box, sphere, line;
	HMesh mesh;
	ParseObj("Resources/Models/Box.obj", mesh);
	unsigned int boxModel = Graphics::GetInstance().cubeModelID;

	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_df.png");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_sp.png");
	unsigned int ballTxt = Graphics::GetInstance().CreateTexture("resources/textures/sci_fi1.jpg");
	unsigned int carTxt = Graphics::GetInstance().CreateTexture("resources/textures/batman1.jpg");
	Material material;
	material.diffuseMap = ballTxt;
	material.specularMap = ballTxt;
	material.count = 2;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;
	R_Object obj;

	glm::vec3 p1 = glm::vec3(0.0f, 10.0f, 0.0f);
	glm::quat q = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p1, q);
	bd.tx = tx;
	bd.isStatic = false;
	bd.angularVelocity = glm::vec3(0.0f);
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	boxCollider->massData->density = 1.0f;
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(glm::vec3(0.5f, 3.0f, 0.5f));
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.5f, 3.0f, 0.5f));
	obj.modelIDs.push_back(boxModel);
	obj.materials.push_back(material);

	q = glm::angleAxis(0.75f, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p1, q);
	boxCollider = new HullCollider();
	boxCollider->txB.position = glm::vec3(0.0f);
	boxCollider->txB.R = tx.R;
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(glm::vec3(0.5f, 3.0f, 0.5f));
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.5f, 3.0f, 0.5f));
	obj.modelIDs.push_back(boxModel);
	obj.materials.push_back(material);

	q = glm::angleAxis(-0.75f, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p1, q);
	boxCollider = new HullCollider();
	boxCollider->txB.position = glm::vec3(0.0f);
	boxCollider->txB.R = tx.R;
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(glm::vec3(0.5f, 3.0f, 0.5f));
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.5f, 3.0f, 0.5f));
	obj.modelIDs.push_back(boxModel);
	obj.materials.push_back(material);

	unsigned int sphereModel = Graphics::GetInstance().sphereModelID;
	glm::vec3 yellowGreen(0.5f, 1.0f, 0.3f);
	glm::vec3 disco(0.2f, 0.7f, 1.0f);

	material.diffuseMap = ballTxt;
	material.specularMap = ballTxt;
	material.count = 2;
	glm::vec3 p2 = glm::vec3(0.0f, 4.0f, 0.0f);
	tx = Transform(p2);
	SphereCollider* sphereCollider = new SphereCollider();
	sphereCollider->com = p2;
	sphereCollider->Scale(1.0f);
	Physics::GetInstance().AddCollider(bID, sphereCollider);
	obj.posOffsets.push_back(p2);
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(1.0f));
	obj.modelIDs.push_back(sphereModel);
	obj.materials.push_back(material);

	p2 = glm::vec3(0.0f, -4.0f, 0.0f);
	tx = Transform(p2);
	sphereCollider = new SphereCollider();
	sphereCollider->com = p2;
	sphereCollider->Scale(1.0f);
	Physics::GetInstance().AddCollider(bID, sphereCollider);
	obj.posOffsets.push_back(p2);
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(1.0f));
	obj.modelIDs.push_back(sphereModel);
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	unsigned int cylinderModel = Graphics::GetInstance().cylinderModelID;
	p1 = glm::vec3(10.0f, 5.0f, 10.0f);
	q = glm::angleAxis(0.0f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p1, q);
	bd.tx = tx;
	bd.isStatic = false;
	bd.angularVelocity = glm::vec3(0.0f);
	bID = Physics::GetInstance().AddBody(bd);
	HullCollider* cylinderCollider = new HullCollider();
	ParseObj("Resources/Models/cylinder_collider.obj", mesh);
	mesh.GetColliderData(cylinderCollider);
	cylinderCollider->Scale(glm::vec3(1.0f, 2.0f, 1.0f));
	Physics::GetInstance().AddCollider(bID, cylinderCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(1.0f, 2.0f, 1.0f));
	obj.modelIDs.push_back(cylinderModel);
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	p1 = glm::vec3(12.0f, 10.0f, 10.0f);
	q = glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p1, q);
	bd.tx = tx;
	bd.isStatic = true;
	bd.angularVelocity = glm::vec3(0.0f);
	bID = Physics::GetInstance().AddBody(bd);
	CapsuleCollider* capsuleCollider = new CapsuleCollider();
	capsuleCollider->Scale(1.0f);
	capsuleCollider->H = 2.0f;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(1.0f, 2.0f, 1.0f));
	obj.modelIDs.push_back(cylinderModel);
	obj.materials.push_back(material);
	obj.posOffsets.push_back(glm::vec3(0.0f, 2.0f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(1.0f));
	obj.modelIDs.push_back(sphereModel);
	obj.materials.push_back(material);
	obj.posOffsets.push_back(glm::vec3(0.0f, -2.0f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(1.0f));
	obj.modelIDs.push_back(sphereModel);
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	p1 = glm::vec3(12.0f, 15.0f, 10.0f);
	q = glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p1, q);
	bd.tx = tx;
	bd.isStatic = false;
	bd.angularVelocity = glm::vec3(0.0f);
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	capsuleCollider->Scale(1.0f);
	capsuleCollider->H = 1.0f;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	obj.modelIDs.push_back(cylinderModel);
	obj.materials.push_back(material);
	obj.posOffsets.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(1.0f));
	obj.modelIDs.push_back(sphereModel);
	obj.materials.push_back(material);
	obj.posOffsets.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(1.0f));
	obj.modelIDs.push_back(sphereModel);
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
}

void CRB_Test::OnKeyTap(GLFWwindow * window, int key, int scanCode, int action, int mods)
{
	Simulation::OnKeyTap(window, key, scanCode, action, mods);
}

void CRB_Test::OnKeyPress(GLFWwindow * window)
{
	Simulation::OnKeyPress(window);
}
