
#include "Ragdoll1.h"
#include "../Mesh/ObjParser.h"
#include "../Components/Model.h"
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

void Ragdoll1::Init(std::vector<GameObject>& gameObjects)
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

	glm::vec3 p1 = glm::vec3(0.0f, 20.1f, 0.0f);
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
	Graphics::GetInstance().scales.push_back(glm::vec3(2.0f, 0.05f, 0.05f));
	gameObjects.push_back(GameObject(boxModel, bID));

	CreateSphere(sphere);
	unsigned int sphereModel = Graphics::GetInstance().CreateModel(sphere);
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
	Graphics::GetInstance().scales.push_back(s);
	gameObjects.push_back(GameObject(boxModel, bID, disco));

	Body* b1 = (*bodies)[bid1];
	Body* b2 = (*bodies)[bid2];
	glm::vec3 anchor(p2);
	anchor.y += s.y + 0.05f;
	hjd.Initialize(b1, b2, anchor, glm::vec3(1.0f, 0.0f, 0.0f));

	HingeJoint hj(&hjd);
	Physics::GetInstance().hingeJoints.push_back(hj);

	p1 = p2;
	p2.y -= s.y + s.x + 0.05f;
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
	Graphics::GetInstance().scales.push_back(s);
	gameObjects.push_back(GameObject(boxModel, bID, disco));

	b1 = (*bodies)[bid1];
	b2 = (*bodies)[bid2];
	anchor.x = p1.x;
	anchor.y = (p1.y + p2.y) * 0.5f;
	hjd.Initialize(b1, b2, anchor, glm::vec3(0.0f, 1.0f, 0.0f));

	hj = HingeJoint(&hjd);
	Physics::GetInstance().hingeJoints.push_back(hj);
}