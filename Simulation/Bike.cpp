
#include "Bike.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"
#include "../Physics/CapsuleCollider.h"

Bike& Bike::GetInstance()
{
	static Bike instance;
	return instance;
}

void Bike::Init()
{
	HMesh mesh;
	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_df.png");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_sp.png");
	unsigned int ballTxt = Graphics::GetInstance().CreateTexture("resources/textures/earth_10k.jpg");
	unsigned int rampTxt = Graphics::GetInstance().CreateTexture("resources/textures/wood3.jpg");
	unsigned int hingeTxt = Graphics::GetInstance().CreateTexture("resources/textures/leather1.jpg");

	Material material;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	std::vector<unsigned int> bids;
	HullCollider* hull;
	SphereCollider* sphereCollider;
	R_Mesh obj;
	glm::vec3 s(0.5f, 0.16f, 0.5f);

	glm::vec3 p(-0.55f, 0.6f, 0.0f);
	tx = Transform(p, glm::angleAxis(-PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	//bd.velocity = glm::vec3(1.0f, 0.0f, 0.0f);
	bd.angularVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	ParseObj("Resources/Models/cylinder_collider.obj", mesh);
	mesh.GetColliderData(hull);
	hull->Scale(s);
	hull->friction = 1.0f;
	hull->massData->density = 1.0f;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(s);
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	bids.push_back(bID);
	glm::vec3 anchor(p);

	p = glm::vec3(0.0f, 1.3f, 0.0f);
	tx = Transform(p);
	bd.tx = tx;
	bd.isStatic = false;
	bd.angularVelocity = glm::vec3(0.0f);
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	ParseObj("Resources/Models/Box.obj", mesh);
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(1.1f, 0.1f, 0.1f));
	hull->massData->density = 1.0f;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(1.1f, 0.1f, 0.1f));
	obj.LoadModel("resources/models/box/box.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	bids.push_back(bID);

	std::vector<Body*>* bodies = &Physics::GetInstance().bodies;
	Body* b1 = (*bodies)[bids[0]];
	Body* b2 = (*bodies)[bids[1]];
	HingeJointDef hjd;
	hjd.Initialize(b1, b2, glm::vec3(-0.55f, 0.6f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	hjd.enableLimit = false;
	hjd.enableMotor = false;
	hjd.motorSpeed = 1.0f;
	hjd.maxMotorTorque = 1000.0f;
	hjd.scale = 0.05f;

	HingeJoint hj(&hjd);
	//Physics::GetInstance().hingeJoints.push_back(hj);

	PositionJointDef pjd;
	pjd.Initialize(b1, b2, glm::vec3(-0.55f, 0.6f, 0.0f));

	PositionJoint pj(&pjd);
	Physics::GetInstance().posJoints.push_back(pj);

	p = glm::vec3(1.55f, 2.6f, 0.0f);
	tx = Transform(p, glm::angleAxis(-PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bd.angularVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	ParseObj("Resources/Models/cylinder_collider.obj", mesh);
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(2.5f, 0.25f, 2.5f));
	hull->friction = 1.0f;
	hull->massData->density = 1.0f;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(2.5f, 0.25f, 2.5f));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	bids.push_back(bID);

	b1 = (*bodies)[bids[2]];
	b2 = (*bodies)[bids[1]];
	hjd.Initialize(b1, b2, glm::vec3(0.55f, 0.6f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	hjd.enableLimit = false;
	hjd.enableMotor = false;
	hjd.motorSpeed = 0.1f;
	hjd.maxMotorTorque = 1.0f;
	hjd.scale = 0.05f;

	hj = HingeJoint(&hjd);
	//Physics::GetInstance().hingeJoints.push_back(hj);
}