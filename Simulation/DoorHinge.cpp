
#include "DoorHinge.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"

DoorHinge& DoorHinge::GetInstance()
{
	static DoorHinge instance;
	return instance;
}

void DoorHinge::Init()
{
	HMesh mesh;
	ParseObj("Resources/Models/Box.obj", mesh);
	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_df.png");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_sp.png");
	unsigned int emissionTxt = Graphics::GetInstance().CreateTexture("resources/textures/matrix.jpg");
	unsigned int metalDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/wood2.jpg");
	Material material;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;
	HingeJointDef hjd;
	std::vector<Body*>* bodies = &Physics::GetInstance().bodies;
	unsigned int bid1, bid2;
	R_Mesh obj;

	float gap = 0.25f;
	glm::vec3 s1(1.0f, 0.05f, 0.05f);
	glm::vec3 s2(2.5f, 1.5f, 0.25f);
	glm::vec3 p1 = glm::vec3(0.0f, 10.0f, 0.0f);
	tx = Transform(p1);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	bid1 = bID;
	boxCollider = new HullCollider();
	boxCollider->massData->density = 1.0f;
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s1);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(s1);
	obj.LoadModel("resources/models/box/box.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	glm::vec3 p2 = p1;
	p2.y -= s1.y + gap + s2.y;
	tx = Transform(p2, glm::angleAxis(0.0f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	bid2 = bID;
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s2);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(s2);
	obj.LoadModel("resources/models/box/box.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	Body* b1 = (*bodies)[bid1];
	Body* b2 = (*bodies)[bid2];
	glm::vec3 anchor(p1);
	hjd.Initialize(b1, b2, anchor, glm::vec3(1.0f, 0.0f, 0.0f));
	hjd.enableLimit = true;
	hjd.lowerLimit = -PI * 0.5f;
	hjd.upperLimit = PI * 0.5f;
	hjd.enableMotor = true;
	hjd.motorSpeed = 0.0f;
	hjd.maxMotorTorque = 0.1f;
	hjd.scale = s1.x;

	HingeJoint hj(&hjd);
	Physics::GetInstance().hingeJoints.push_back(hj);

	p1 = glm::vec3(-20.0f, 10.0f, 0.0f);
	tx = Transform(p1, glm::angleAxis(PI * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	bid1 = bID;
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s1);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(s1);
	obj.LoadModel("resources/models/box/box.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	p2 = p1;
	p2.x += s1.y + gap + s2.y;
	tx = Transform(p2, glm::angleAxis(PI * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	bid2 = bID;
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s2);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(s2);
	obj.LoadModel("resources/models/box/box.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	b1 = (*bodies)[bid1];
	b2 = (*bodies)[bid2];
	anchor = p1;
	hjd.Initialize(b1, b2, anchor, glm::vec3(0.0f, 1.0f, 0.0f));
	hjd.enableLimit = false;
	hjd.maxMotorTorque = 1.0f;
	hjd.scale = s1.x;

	hj = HingeJoint(&hjd);
	Physics::GetInstance().hingeJoints.push_back(hj);

	p1 = p2;
	p1.x += (s2.y + gap) * 3.0f + s1.y;
	tx = Transform(p1, glm::angleAxis(PI * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	bid1 = bID;
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s1);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(s1);
	obj.LoadModel("resources/models/box/box.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	p2 = p1;
	p2.x -= s1.y + gap + s2.y;
	tx = Transform(p2, glm::angleAxis(PI * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	bid2 = bID;
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s2);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(s2);
	obj.LoadModel("resources/models/box/box.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	b1 = (*bodies)[bid1];
	b2 = (*bodies)[bid2];
	anchor = p1;
	hjd.Initialize(b1, b2, anchor, glm::vec3(0.0f, 1.0f, 0.0f));
	hjd.scale = s1.x;

	hj = HingeJoint(&hjd);
	Physics::GetInstance().hingeJoints.push_back(hj);
}