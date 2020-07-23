
#include "SpringTest.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"
#include "../Physics/CapsuleCollider.h"
#include "../Physics/SpringConstraint.h"

void SpringTest::Init(GLFWwindow* window, int width, int height)
{
	Simulation::Init(window, width, height);

	HMesh mesh;
	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_df.png");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_sp.png");
	unsigned int ballTxt = Graphics::GetInstance().CreateTexture("resources/textures/earth_10k.jpg");
	unsigned int scifiTxt = Graphics::GetInstance().CreateTexture("resources/textures/sci_fi1.jpg");
	unsigned int wheelTxt = Graphics::GetInstance().CreateTexture("resources/textures/leather1.jpg");
	unsigned int carTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal2.jpg");

	Material material;
	material.diffuseMap = wheelTxt;
	material.specularMap = boxSpTxt;
	material.nMaps = 2;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	std::vector<unsigned int> bids;
	HullCollider* hull;
	SphereCollider* sphereCollider;
	CapsuleCollider* capsuleCollider;
	R_Mesh obj;
	
	glm::vec3 p = glm::vec3(0.0f, 9.0f, 0.0f);
	tx = Transform(p);
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	ParseObj("Resources/Models/Box.obj", mesh);
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(0.25f, 1.0f, 0.25f));
	hull->massData->density = 1.0f;
	hull->restitution = 0.1f;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.25f, 1.0f, 0.25f));
	obj.LoadModel("resources/models/box/box.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	bids.push_back(bID);
	unsigned int bid1 = bID;
	glm::vec3 p1(p.x - 0.25f, p.y + 1.0f, p.z + 0.25f);

	p = glm::vec3(0.0f, 15.0f, 0.0f);
	tx = Transform(p);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(0.25, 0.25f, 0.25));
	hull->massData->density = 1.0f;
	hull->restitution = 0.1f;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.25f));
	obj.LoadModel("resources/models/box/box.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	bids.push_back(bID);
	unsigned int bid2 = bID;
	glm::vec3 p2(p);

	std::vector<Body*>* bodies = &Physics::GetInstance().bodies;
	SpringDef sd;
	sd.l0 = 4.0f;
	sd.w = 4.0f;
	sd.c = 0.1f;
	sd.Init((*bodies)[bid1], (*bodies)[bid2], p1, p2);
	Spring s;
	s.Init(&sd);
	Physics::GetInstance().springs.push_back(s);
}

void SpringTest::OnKeyPress(GLFWwindow* window)
{
	Simulation::OnKeyPress(window);
}