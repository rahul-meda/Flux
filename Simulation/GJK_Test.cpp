
#include "GJK_Test.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"

GJK_Test& GJK_Test::GetInstance()
{
	static GJK_Test instance;
	return instance;
}

void GJK_Test::Init()
{
	HMesh mesh;
	ParseObj("Resources/Models/Box.obj", mesh);
	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_df.png");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_sp.png");
	unsigned int metalTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal1.jpeg");
	Material material;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;
	R_Mesh obj;

	glm::vec3 p1 = glm::vec3(0.0f, 10.0f, 0.0f);
	glm::quat q = glm::angleAxis(0.75f, glm::vec3(0.0f, 0.0f, 1.0f));
	tx = Transform(p1, q);
	bd.tx = tx;
	bd.isStatic = false;
	bd.angularVelocity = glm::vec3(1.0f);
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	boxCollider->massData->density = 1.0f;
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(glm::vec3(1.0f, 3.0f, 1.0f));
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.LoadModel("resources/models/box/box.obj");
	
	obj.scale = glm::vec3(1.0f, 3.0f, 1.0f);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	glm::vec3 yellowGreen(0.5f, 1.0f, 0.3f);
	glm::vec3 disco(0.2f, 0.7f, 1.0f);

	glm::vec3 p2 = glm::vec3(10.0f, 20.0f, 0.0f);
	tx = Transform(p2);
	bd.tx = tx;
	bd.isStatic = false;
	bd.angularVelocity = glm::vec3(0.0f);
	bID = Physics::GetInstance().AddBody(bd);
	SphereCollider* sphereCollider = new SphereCollider();
	sphereCollider->Scale(1.0f);
	Physics::GetInstance().AddCollider(bID, sphereCollider);
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	obj.scale = glm::vec3(1.0f);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	PositionJointDef pjd;
	std::vector<Body*>* bodies = &Physics::GetInstance().bodies;
	pjd.Initialize((*bodies)[0], (*bodies)[1], glm::vec3((p1 + p2) * 0.5f));

	PositionJoint pj(&pjd);
	Physics::GetInstance().posJoints.push_back(pj);
}