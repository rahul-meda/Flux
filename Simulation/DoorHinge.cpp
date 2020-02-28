
#include "DoorHinge.h"
#include "../Mesh/ObjParser.h"
#include "../Components/Model.h"
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

void DoorHinge::Init(std::vector<GameObject>& gameObjects)
{
	ModelDef box, sphere, line;
	HMesh mesh;
	ParseObj("Resources/Models/Box.obj", mesh);
	mesh.GetModelData(box);
	unsigned int boxModel = Graphics::GetInstance().CreateModel(box);

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;
	HingeJointDef hjd;
	std::vector<Body*>* bodies = &Physics::GetInstance().bodies;
	unsigned int bid1, bid2;

	float gap = 1.25f;
	glm::vec3 s1(0.25f, 0.05f, 0.05f);
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
	Graphics::GetInstance().scales.push_back(s1);
	gameObjects.push_back(GameObject(boxModel, bID));

	CreateSphere(sphere);
	unsigned int sphereModel = Graphics::GetInstance().CreateModel(sphere);
	glm::vec3 yellowGreen(0.5f, 1.0f, 0.3f);
	glm::vec3 disco(0.2f, 0.7f, 1.0f);
	glm::vec3 s2(2.5f, 1.5f, 0.1f);

	glm::vec3 p2 = p1;
	p2.z += s1.z + gap + s2.y;
	tx = Transform(p2, glm::angleAxis(0.0f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	bid2 = bID;
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s2);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	Graphics::GetInstance().scales.push_back(s2);
	gameObjects.push_back(GameObject(boxModel, bID, disco));

	Body* b1 = (*bodies)[bid1];
	Body* b2 = (*bodies)[bid2];
	glm::vec3 anchor(p1);
	anchor.z += s1.z + gap * 0.5f;
	hjd.Initialize(b1, b2, anchor, glm::vec3(1.0f, 0.0f, 0.0f));
	hjd.enableLimit = true;
	hjd.lowerLimit = -PI * 0.25f;
	hjd.upperLimit = PI * 0.25f;

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
	Graphics::GetInstance().scales.push_back(s1);
	gameObjects.push_back(GameObject(boxModel, bID));

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
	Graphics::GetInstance().scales.push_back(s2);
	gameObjects.push_back(GameObject(boxModel, bID, disco));

	b1 = (*bodies)[bid1];
	b2 = (*bodies)[bid2];
	anchor = p1;
	anchor.x += s1.y + gap * 0.5f;
	//hjd.Initialize(b1, b2, anchor, glm::vec3(0.0f, 1.0f, 0.0f));

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
	Graphics::GetInstance().scales.push_back(s1);
	gameObjects.push_back(GameObject(boxModel, bID));

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
	Graphics::GetInstance().scales.push_back(s2);
	gameObjects.push_back(GameObject(boxModel, bID, disco));

	b1 = (*bodies)[bid1];
	b2 = (*bodies)[bid2];
	anchor = p1;
	anchor.x -= s1.y + gap * 0.5f;
	//hjd.Initialize(b1, b2, anchor, glm::vec3(0.0f, 1.0f, 0.0f));

	hj = HingeJoint(&hjd);
	Physics::GetInstance().hingeJoints.push_back(hj);
}