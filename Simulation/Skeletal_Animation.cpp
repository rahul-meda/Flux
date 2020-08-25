
#include "Skeletal_Animation.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"
#include "../Physics/CapsuleCollider.h"
#include <glm/gtx/matrix_decompose.hpp>

Skeletal_Animation& Skeletal_Animation::GetInstance()
{
	static Skeletal_Animation instance;
	return instance;
}

void Skeletal_Animation::Init(GLFWwindow* window, int width, int height)
{
	Simulation::Init(window, width, height);

	pause = false;
	pos = glm::vec3(0.0f, 3.0f, 0.0f);
	rot = glm::angleAxis(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	R = glm::mat3(1.0f);
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

	unsigned int objId = Graphics::GetInstance().objects.size() - 1;

	// pelivs / root
	glm::vec3 p = glm::vec3(0.0f, 3.0f, 0.0f);
	glm::quat q = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	CapsuleCollider* capsuleCollider = new CapsuleCollider();
	float r = 0.25f;
	float h = 0.25f;
	capsuleCollider->Scale(r);
	capsuleCollider->H = h;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(r, h, r));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	// upper right leg
	p = glm::vec3(0.5f, 2.0f, 0.0f);
	q = glm::angleAxis(0.0f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	capsuleCollider->Scale(0.25f);
	capsuleCollider->H = 0.5f;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.25f, 0.5f, 0.25f));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, 0.5f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.25f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -0.5f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.25f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	// lower right leg
	p = glm::vec3(0.5f, 0.75f, 0.0f);
	q = glm::angleAxis(0.0f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	capsuleCollider->Scale(0.5f);
	capsuleCollider->H = 1.0f;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.25f, 0.5f, 0.25f));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, 0.5f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.25f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -0.5f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.25f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	// upper left leg
	p = glm::vec3(-0.5f, 2.0f, 0.0f);
	q = glm::angleAxis(0.0f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	capsuleCollider->Scale(0.25f);
	capsuleCollider->H = 0.5f;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.25f, 0.5f, 0.25f));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, 0.5f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.25f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -0.5f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.25f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	// lower left leg
	p = glm::vec3(-0.5f, 0.75f, 0.0f);
	q = glm::angleAxis(0.0f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	capsuleCollider->Scale(0.25f);
	capsuleCollider->H = 0.5f;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.25f, 0.5f, 0.25f));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, 0.5f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.25f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -0.5f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.25f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	// spine (should be flexible?)
	p = glm::vec3(0.0f, 4.25f, 0.0f);
	q = glm::angleAxis(0.0f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	r = 0.05f;
	h = 0.05f;
	capsuleCollider->Scale(r);
	capsuleCollider->H = h;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(r, h, r));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	// upper right arm
	p = glm::vec3(0.9f, 5.0f, 0.0f);
	q = glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	capsuleCollider->Scale(0.2f);
	capsuleCollider->H = 0.4f;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.2f, 0.4f, 0.2f));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, 0.4f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.2f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -0.4f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.2f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	// lower right arm
	p = glm::vec3(1.85f, 5.0f, 0.0f);
	q = glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	capsuleCollider->Scale(0.2f);
	capsuleCollider->H = 0.4f;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.2f, 0.4f, 0.2f));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, 0.4f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.2f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -0.4f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.2f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	// upper left arm
	p = glm::vec3(-0.9f, 5.0f, 0.0f);
	q = glm::angleAxis(-0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	capsuleCollider->Scale(0.2f);
	capsuleCollider->H = 0.4f;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.2f, 0.4f, 0.2f));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, 0.4f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.2f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -0.4f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.2f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	// lower right arm
	p = glm::vec3(-1.85f, 5.0f, 0.0f);
	q = glm::angleAxis(-0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	capsuleCollider->Scale(0.2f);
	capsuleCollider->H = 0.4f;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.2f, 0.4f, 0.2f));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, 0.4f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.2f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -0.4f, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.2f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	// neck
	p = glm::vec3(0.0f, 5.45f, 0.0f);
	q = glm::angleAxis(0.0f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	r = 0.1f;
	h = 0.2f;
	capsuleCollider->Scale(r);
	capsuleCollider->H = h;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(r, h, r));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	// head
	p = glm::vec3(0.0f, 5.7f, 0.0f);
	q = glm::angleAxis(0.0f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	SphereCollider* sphereCollider = new SphereCollider();
	sphereCollider->com = glm::vec3(0.0f);
	sphereCollider->Scale(0.25f);
	Physics::GetInstance().AddCollider(bID, sphereCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.25f, 0.25f, 0.3f));
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	p = glm::vec3(0.0f, 2.5f, 0.0f);
	q = glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	r = 0.25f;
	h = 0.5f;
	capsuleCollider->Scale(r);
	capsuleCollider->H = h;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(r, h, r));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	p = glm::vec3(0.0f, 3.5f, 0.0f);
	q = glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	r = 0.25f;
	h = 0.25f;
	capsuleCollider->Scale(r);
	capsuleCollider->H = h;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(r, h, r));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	p = glm::vec3(0.0f, 4.0f, 0.0f);
	q = glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	r = 0.25f;
	h = 0.25f;
	capsuleCollider->Scale(r);
	capsuleCollider->H = h;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(r, h, r));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	p = glm::vec3(0.0f, 4.5f, 0.0f);
	q = glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	r = 0.25f;
	h = 0.25f;
	capsuleCollider->Scale(r);
	capsuleCollider->H = h;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(r, h, r));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	p = glm::vec3(0.0f, 5.0f, 0.0f);
	q = glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	r = 0.25f;
	h = 0.5f;
	capsuleCollider->Scale(r);
	capsuleCollider->H = h;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(r, h, r));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	p = glm::vec3(0.5f, 0.3f, 0.0f);
	q = glm::angleAxis(0.5f * PI, glm::vec3(1.0f, 0.0f, 0.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	r = 0.2f;
	h = 0.2f;
	capsuleCollider->Scale(r);
	capsuleCollider->H = h;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(r, h, r));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	p = glm::vec3(-0.5f, 0.3f, 0.0f);
	q = glm::angleAxis(0.5f * PI, glm::vec3(1.0f, 0.0f, 0.0f));
	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	capsuleCollider = new CapsuleCollider();
	r = 0.2f;
	h = 0.2f;
	capsuleCollider->Scale(r);
	capsuleCollider->H = h;
	capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
	Physics::GetInstance().AddCollider(bID, capsuleCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(r, h, r));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.posOffsets.push_back(glm::vec3(0.0f, -h, 0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(r));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	objIds.push_back(++objId);

	std::vector<Bone> bones;
	bones.push_back(Bone(0,  0,	 {1, 3, 5, 12, 13, 14, 15, 16}));	// root
	bones.push_back(Bone(1,  0,	 {2}));								// u_rightLeg
	bones.push_back(Bone(2,  1,	 {17}));							// l_rightLeg
	bones.push_back(Bone(3,  0,  {4}));							    // u_leftLeg
	bones.push_back(Bone(4,  3,  {18}));								// l_leftLeg
	bones.push_back(Bone(5,  0,  {6, 8, 10}));						// spine
	bones.push_back(Bone(6,  5,  {7}));								// u_rightArm
	bones.push_back(Bone(7,  6,  {}));								// l_rightArm
	bones.push_back(Bone(8,  5,  {9}));								// u_leftArm
	bones.push_back(Bone(9,  8,  {}));								// l_leftArm
	bones.push_back(Bone(10, 5,  {11}));							// neck
	bones.push_back(Bone(11, 10, {}));								// head
	bones.push_back(Bone(12, 0, {}));
	bones.push_back(Bone(13, 0, {}));
	bones.push_back(Bone(14, 0, {}));
	bones.push_back(Bone(15, 0, {}));
	bones.push_back(Bone(16, 0, {}));
	bones.push_back(Bone(17, 2, {}));
	bones.push_back(Bone(18, 4, {}));

	std::vector<Transform> tf(bones.size());
	tf[0] = Transform(glm::vec3(0.0f, 0.0f, 0.0f));
	tf[1] = Transform(glm::vec3(0.5f, -1.0f, 0.0f));
	tf[2] = Transform(glm::vec3(0, -1.25f, 0));
	tf[3] = Transform(glm::vec3(-0.5f, -1.0f, 0));
	tf[4] = Transform(glm::vec3(0, -1.25f, 0));
	tf[5] = Transform(glm::vec3(0, 1.25f, 0));
	tf[6] = Transform(glm::vec3(0.65f, 0.15f, 0));
	tf[7] = Transform(glm::vec3(0.0f, -0.95f, 0));
	tf[8] = Transform(glm::vec3(-0.65f, 0.15f, 0));
	tf[9] = Transform(glm::vec3(0.0f, -0.95f, 0));
	tf[10] = Transform(glm::vec3(0, 1.25f, 0));
	tf[11] = Transform(glm::vec3(0, 0.25f, 0));
	tf[12] = Transform(glm::vec3(0, -0.5f, 0), glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f)));
	tf[13] = Transform(glm::vec3(0, 0.5f, 0), glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f)));
	tf[14] = Transform(glm::vec3(0, 1.0f, 0), glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f)));
	tf[15] = Transform(glm::vec3(0, 1.5f, 0), glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f)));
	tf[16] = Transform(glm::vec3(0, 2.0f, 0), glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 0.0f, 1.0f)));
	tf[17] = Transform(glm::vec3(0.0f, -0.7f, -0.25f), glm::angleAxis(0.5f * PI, glm::vec3(1.0f, 0.0f, 0.0f)));
	tf[18] = Transform(glm::vec3(0.0f, -0.7f, -0.25f), glm::angleAxis(0.5f * PI, glm::vec3(1.0f, 0.0f, 0.0f)));

	std::vector<KeyFrame> frames(9);

	frames[0].timeStamp = 0.0f;
	frames[1].timeStamp = 0.15f;
	frames[2].timeStamp = 0.25f;
	frames[3].timeStamp = 0.4f;
	frames[4].timeStamp = 0.5f;
	frames[5].timeStamp = 0.65f;
	frames[6].timeStamp = 0.767f;
	frames[7].timeStamp = 0.883f;
	frames[8].timeStamp = 1.0f;

	for (int i = 0; i < frames.size(); ++i)
	{
		for (int j = 0; j < tf.size(); ++j)
		{
			frames[i].pose.push_back(tf[j]);
		}
	}

	float l = 0.5f;
	float angle = 0.166f * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[0].pose[1] = Transform(p, q);
	angle = 0.25f * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[0].pose[3] = Transform(p, q);
	angle = 0.055f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[0].pose[4] = Transform(p, q);
	angle = 0.055 * PI;
	p = glm::vec3(0.65f, 0.15f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[0].pose[6] = Transform(p, q);
	angle = 0.11 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[0].pose[7] = Transform(p, q);
	angle = 0.055 * PI;
	p = glm::vec3(-0.65f, 0.15f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[0].pose[8] = Transform(p, q);
	angle = 0.11 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[0].pose[9] = Transform(p, q);

	angle = 0.25f * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[1].pose[1] = Transform(p, q);
	angle = 0.25f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[1].pose[2] = Transform(p, q);
	angle = 0.25f * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[1].pose[3] = Transform(p, q);
	angle = 0.166f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[1].pose[4] = Transform(p, q);
	angle = 0.11 * PI;
	p = glm::vec3(0.65f, 0.15f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[1].pose[6] = Transform(p, q);
	angle = 0.055 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[1].pose[7] = Transform(p, q);
	angle = 0.11 * PI;
	p = glm::vec3(-0.65f, 0.15f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[1].pose[8] = Transform(p, q);
	angle = 0.166 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[1].pose[9] = Transform(p, q);

	angle = 0.083f * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[2].pose[1] = Transform(p, q);
	angle = 0.083f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[2].pose[2] = Transform(p, q);
	angle = 0.083f * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[2].pose[3] = Transform(p, q);
	angle = 0.333f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[2].pose[4] = Transform(p, q);
	angle = 0.055 * PI;
	p = glm::vec3(0.65f, 0.15f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[2].pose[6] = Transform(p, q);
	angle = 0.055 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[2].pose[7] = Transform(p, q);
	angle = 0.055 * PI;
	p = glm::vec3(-0.65f, 0.15f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[2].pose[8] = Transform(p, q);
	angle = 0.22 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[2].pose[9] = Transform(p, q);

	angle = 0.166f * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[3].pose[1] = Transform(p, q);
	angle = 0.166f * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[3].pose[3] = Transform(p, q);
	angle = 0.166f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[3].pose[4] = Transform(p, q);
	angle = 0.166 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[3].pose[9] = Transform(p, q);

	angle = 0.25 * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[4].pose[1] = Transform(p, q);
	angle = 0.055f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[4].pose[2] = Transform(p, q);
	angle = 0.166f * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[4].pose[3] = Transform(p, q);
	angle = 0.055 * PI;
	p = glm::vec3(0.65f, 0.15f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[4].pose[6] = Transform(p, q);
	angle = 0.11 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[4].pose[7] = Transform(p, q);
	angle = 0.055 * PI;
	p = glm::vec3(-0.65f, 0.15f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[4].pose[8] = Transform(p, q);
	angle = 0.11 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[4].pose[9] = Transform(p, q);

	frames[5].pose[0] = Transform(glm::vec3(0.0f, -0.2f, 0.0f));
	angle = 0.25f * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[5].pose[1] = Transform(p, q);
	angle = 0.166f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[5].pose[2] = Transform(p, q);
	angle = 0.25f * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[5].pose[3] = Transform(p, q);
	angle = 0.25f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[5].pose[4] = Transform(p, q);
	angle = 0.11 * PI;
	p = glm::vec3(0.65f, 0.15f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[5].pose[6] = Transform(p, q);
	angle = 0.166 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[5].pose[7] = Transform(p, q);
	angle = 0.11 * PI;
	p = glm::vec3(-0.65f, 0.15f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[5].pose[8] = Transform(p, q);
	angle = 0.055 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[5].pose[9] = Transform(p, q);

	angle = 0.083 * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[6].pose[1] = Transform(p, q);
	angle = 0.333f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[6].pose[2] = Transform(p, q);
	angle = 0.083f * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[6].pose[3] = Transform(p, q);
	angle = 0.083f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[6].pose[4] = Transform(p, q);
	angle = 0.055 * PI;
	p = glm::vec3(0.65f, 0.15f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[6].pose[6] = Transform(p, q);
	angle = 0.22 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[6].pose[7] = Transform(p, q);
	angle = 0.055 * PI;
	p = glm::vec3(-0.65f, 0.15f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[6].pose[8] = Transform(p, q);
	angle = 0.055 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[6].pose[9] = Transform(p, q);

	frames[7].pose[0] = Transform(glm::vec3(0.0f, 0.2f, 0.0f));
	angle = 0.166 * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[7].pose[1] = Transform(p, q);
	angle = 0.166f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[7].pose[2] = Transform(p, q);
	angle = 0.166f * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[7].pose[3] = Transform(p, q);
	angle = 0.166 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[7].pose[7] = Transform(p, q);

	angle = 0.166f * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[8].pose[1] = Transform(p, q);
	angle = 0.25f * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[8].pose[3] = Transform(p, q);
	angle = 0.083f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[8].pose[4] = Transform(p, q);
	angle = 0.055 * PI;
	p = glm::vec3(0.65f, 0.15f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[8].pose[6] = Transform(p, q);
	angle = 0.11 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[8].pose[7] = Transform(p, q);
	angle = 0.055 * PI;
	p = glm::vec3(-0.65f, 0.15f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[8].pose[8] = Transform(p, q);
	angle = 0.11 * PI;
	p = glm::vec3(0.0f, -0.95f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[8].pose[9] = Transform(p, q);

	//walkAnimation.Init(bones, frames);

	/*frames.clear();
	frames = std::vector<KeyFrame>(9);

	frames[0].timeStamp = 0.0f;
	frames[1].timeStamp = 0.15f;
	frames[2].timeStamp = 0.25f;
	frames[3].timeStamp = 0.4f;
	frames[4].timeStamp = 0.5f;
	frames[5].timeStamp = 0.65f;
	frames[6].timeStamp = 0.767f;
	frames[7].timeStamp = 0.883f;
	frames[8].timeStamp = 1.0f;

	for (int i = 0; i < frames.size(); ++i)
	{
		for (int j = 0; j < tf.size(); ++j)
		{
			frames[i].pose.push_back(tf[j]);
		}
	}

	angle = 0.25 * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[0].pose[1] = Transform(p, q);
	angle = 0.25f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[0].pose[2] = Transform(p, q);
	angle = 0.25f * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[0].pose[3] = Transform(p, q);

	angle = 0.5f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[1].pose[2] = Transform(p, q);

	angle = 0.25 * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[2].pose[1] = Transform(p, q);
	angle = 0.5f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[2].pose[2] = Transform(p, q);
	angle = 0.25f * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[2].pose[3] = Transform(p, q);

	angle = 0.25 * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[3].pose[1] = Transform(p, q);
	angle = 0.25f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[3].pose[2] = Transform(p, q);
	angle = 0.25f * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[3].pose[3] = Transform(p, q);

	angle = 0.166 * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[4].pose[1] = Transform(p, q);
	angle = 0.166f * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[4].pose[3] = Transform(p, q);
	angle = 0.33f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[4].pose[4] = Transform(p, q);

	angle = 0.5f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[5].pose[4] = Transform(p, q);

	angle = 0.25f * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[6].pose[1] = Transform(p, q);
	angle = 0.25 * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[6].pose[3] = Transform(p, q);
	angle = 0.5f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[6].pose[4] = Transform(p, q);

	angle = 0.25f * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[7].pose[1] = Transform(p, q);
	angle = 0.25 * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[7].pose[3] = Transform(p, q);
	angle = 0.25f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[7].pose[4] = Transform(p, q);

	angle = 0.166f * PI;
	p = glm::vec3(0.5f, -1.0f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[8].pose[1] = Transform(p, q);
	angle = 0.33f * PI;
	p = glm::vec3(0.0f, -1.25f + (l - l * cosf(angle)), l * sinf(angle));
	q = glm::angleAxis(-angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[8].pose[2] = Transform(p, q);
	angle = 0.166 * PI;
	p = glm::vec3(-0.5f, -1.0f + (l - l * cosf(angle)), -l * sinf(angle));
	q = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));
	frames[8].pose[3] = Transform(p, q);

	runAnimation.Init(bones, frames);*/
}

void Skeletal_Animation::OnKeyTap(GLFWwindow * window, int key, int scanCode, int action, int mods)
{
	Simulation::OnKeyTap(window, key, scanCode, action, mods);

	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		pause = !pause;
	}
}

void Skeletal_Animation::OnKeyPress(GLFWwindow * window)
{
	Simulation::OnKeyPress(window);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		glm::vec3 fwd = -R[2];
		const static float t = 0.05;
		pos += t * fwd;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		const static float w = 0.5f;
		rot += deltaT * 0.5f * glm::quat(0.0f, glm::vec3(0.0f, -w, 0.0f)) * rot;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		const static float w = 0.5f;
		rot += deltaT * 0.5f * glm::quat(0.0f, glm::vec3(0.0f, w, 0.0f)) * rot;
	}
}

void Skeletal_Animation::Update(GLFWwindow* window)
{
	Simulation::Update(window);

	R = glm::toMat3(rot);
	//Physics::GetInstance().bodies[objIds[0]]->SetTransform(Transform(pos, rot));
	//Physics::GetInstance().positions[objIds[0]] = Position(pos, rot);

	for (int i = 0; i < walkAnimation.frames.size(); ++i)
	{
		walkAnimation.frames[i].pose[0] = Transform(pos, rot);
	}

	if (!pause)
	{
		walkAnimation.Update();
		//runAnimation.Update();
	}
	
	int i = objIds[0];
	/*for (glm::mat4& tx : walkAnimation.finalTransforms)
	{
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(tx, scale, rotation, translation, skew, perspective);

		Physics::GetInstance().bodies[i]->SetTransform(Transform(translation, rotation));
		Physics::GetInstance().positions[i] = Position(translation, rotation);

		Graphics::GetInstance().objects[i].pos = translation;
		Graphics::GetInstance().objects[i].rot = glm::toMat3(rotation);
		++i;
	}*/
}