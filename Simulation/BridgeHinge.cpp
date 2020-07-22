
#include "BridgeHinge.h"
#include "../Mesh/ObjParser.h"
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

void BridgeHinge::Init()
{
	HMesh mesh;
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
	R_Mesh obj;

	glm::vec3 s(0.5f, 1.5f, 0.5f);
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
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);
	obj.scale = s;
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	glm::vec3 yellowGreen(0.5f, 1.0f, 0.3f);
	glm::vec3 disco(0.2f, 0.7f, 1.0f);

	float gap = 0.25f;

	int NL = 20;
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
		obj.LoadModel("resources/models/box/box.obj");
		obj.materials.push_back(material);
		obj.scale = s;
		Graphics::GetInstance().objects.push_back(obj);
		obj.Clear();

		HingeJointDef hjd;
		std::vector<Body*>* bodies = &Physics::GetInstance().bodies;
		glm::vec3 anchor = (p1 + p2) * 0.5f;
		p1 = p2;
		hjd.Initialize((*bodies)[i], (*bodies)[i + 1], anchor, glm::vec3(0.0f, 1.0f, 0.0f));
		hjd.scale = 0.25f;

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