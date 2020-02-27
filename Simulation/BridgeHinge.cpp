
#include "BridgeHinge.h"
#include "../Mesh/ObjParser.h"
#include "../Components/Model.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"

BridgeHinge& BridgeHinge::GetInstance()
{
	static BridgeHinge instance;
	return instance;
}

void BridgeHinge::Init(std::vector<GameObject>& gameObjects)
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

	glm::vec3 s(0.1f, 1.5f, 0.1f);
	glm::vec3 p1 = glm::vec3(0.0f, 10.0f, 0.0f);
	tx = Transform(p1);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	boxCollider->massData->density = 1.0f;
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(s);
	Physics::GetInstance().AddCollider(bID, boxCollider);
	Graphics::GetInstance().scales.push_back(s);
	gameObjects.push_back(GameObject(boxModel, bID));

	CreateSphere(sphere);
	unsigned int sphereModel = Graphics::GetInstance().CreateModel(sphere);
	glm::vec3 yellowGreen(0.5f, 1.0f, 0.3f);
	glm::vec3 disco(0.2f, 0.7f, 1.0f);

	float gap = 0.1f;

	int NL = 10;
	for (int i = 0; i < NL; ++i)
	{
		glm::vec3 p2(-(2.0f * s.x + gap) * (i + 1.0f), 10.0f, 0.0f);
		tx = Transform(p2);
		bd.tx = tx;
		bd.isStatic = false;
		bID = Physics::GetInstance().AddBody(bd);
		boxCollider = new HullCollider();
		mesh.GetColliderData(boxCollider);
		boxCollider->Scale(s);
		Physics::GetInstance().AddCollider(bID, boxCollider);
		Graphics::GetInstance().scales.push_back(s);
		gameObjects.push_back(GameObject(boxModel, bID, disco));

		HingeJointDef hjd;
		std::vector<Body*>* bodies = &Physics::GetInstance().bodies;
		glm::vec3 anchor = (p1 + p2) * 0.5f;
		p1 = p2;
		hjd.Initialize((*bodies)[i], (*bodies)[i + 1], anchor, glm::vec3(0.0f, 1.0f, 0.0f));

		HingeJoint hj(&hjd);
		Physics::GetInstance().hingeJoints.push_back(hj);

		PositionJointDef pjd;
		anchor = (p1 + p2) * 0.5f;
		p1 = p2;
		pjd.Initialize((*bodies)[i], (*bodies)[i + 1], anchor);

		PositionJoint pj(&pjd);
		//Physics::GetInstance().posJoints.push_back(pj);

		anchor.y -= s.y;
		pjd.Initialize((*bodies)[i], (*bodies)[i + 1], anchor);

	    pj = PositionJoint(&pjd);
		//Physics::GetInstance().posJoints.push_back(pj);
	}
}