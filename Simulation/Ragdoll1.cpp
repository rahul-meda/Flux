
#include "Ragdoll1.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"

Ragdoll1& Ragdoll1::GetInstance()
{
	static Ragdoll1 instance;
	return instance;
}

void Ragdoll1::Init()
{
	HMesh mesh;
	ParseObj("Resources/Models/Box.obj", mesh);
	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_df.png");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_sp.png");
	unsigned int metalTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal1.jpeg");
	Material material;
	material.diffuseMap = boxDfTxt;
	material.specularMap = boxSpTxt;
	material.nMaps = 2;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;
	HingeJointDef hjd;
	std::vector<Body*>* bodies = &Physics::GetInstance().bodies;
	unsigned int bid1, bid2;
	R_Mesh obj;

	glm::vec3 p1 = glm::vec3(0.0f, 30.0f, 0.0f);
	tx = Transform(p1);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	bid1 = bID;
	boxCollider = new HullCollider();
	boxCollider->massData->density = 1.0f;
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(glm::vec3(2.0f, 0.05f, 0.05f));
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.LoadModel("resources/models/box/box.obj");
	
	obj.scale = glm::vec3(2.0f, 0.05f, 0.05f);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	glm::vec3 yellowGreen(0.5f, 1.0f, 0.3f);
	glm::vec3 disco(0.2f, 0.7f, 1.0f);
	glm::vec3 s(0.25f, 2.0f, 0.25f);

	glm::vec3 p2 = glm::vec3(0.0f, 20.0f - s.y, 0.0f);
	tx = Transform(p2);
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	bid2 = bID;
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.LoadModel("resources/models/box/box.obj");
	
	obj.scale = s;
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	Body* b1 = (*bodies)[bid1];
	Body* b2 = (*bodies)[bid2];
	glm::vec3 anchor(p2);
	anchor.y += s.y + 1.5f;
	hjd.Initialize(b1, b2, anchor, glm::vec3(1.0f, 0.0f, 0.0f));
	hjd.enableLimit = false;
	hjd.lowerLimit = 0.0f;
	hjd.upperLimit = PI;
	hjd.scale = 0.25f;

	HingeJoint hj(&hjd);
	Physics::GetInstance().hingeJoints.push_back(hj);

	p1 = p2;
	p2.y -= s.y + s.x + 0.5f;
	p2.x += s.y - s.x;
	tx = Transform(p2, glm::angleAxis(PI * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	bid1 = bid2;
	bid2 = bID;
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.LoadModel("resources/models/box/box.obj");
	
	obj.scale = s;
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	b1 = (*bodies)[bid1];
	b2 = (*bodies)[bid2];
	anchor.x = p1.x;
	anchor.y = (p1.y + p2.y) * 0.5f;
	hjd.Initialize(b1, b2, anchor, glm::vec3(0.0f, 1.0f, 0.0f));
	hjd.enableLimit = false;
	hjd.lowerLimit = 0.0f;
	hjd.upperLimit = PI * 0.5f;
	hjd.scale = 0.25f;
	hjd.enableMotor = true;
	hjd.maxMotorTorque = 0.3f;

	hj = HingeJoint(&hjd);
	Physics::GetInstance().hingeJoints.push_back(hj);
}