
#include "HingeJointTest.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"

HingeJointTest& HingeJointTest::GetInstance()
{
	static HingeJointTest instance;
	return instance;
}

void HingeJointTest::Init()
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

	glm::vec3 p1 = glm::vec3(25.0f, 20.0f, 0.0f);
	tx = Transform(p1);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	boxCollider->massData->density = 1.0f;
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(glm::vec3(25.0f, 0.05f, 0.05f));
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.LoadModel("resources/models/box/box.obj");
	
	obj.scale = glm::vec3(25.0f, 0.05f, 0.05f);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	glm::vec3 yellowGreen(0.5f, 1.0f, 0.3f);
	glm::vec3 disco(0.2f, 0.7f, 1.0f);
	glm::vec3 s(0.5f, 3.0f, 0.5f);

	int N = 1;
	for (int i = 0; i < N; ++i)
	{
		glm::vec3 p2 = glm::vec3(2.5f * (float)i, 20.0f - s.y - 1.0f, 0.0f);
		tx = Transform(p2);
		bd.tx = tx;
		bd.isStatic = false;
		bID = Physics::GetInstance().AddBody(bd);
		boxCollider = new HullCollider();
		mesh.GetColliderData(boxCollider);
		boxCollider->Scale(s);
		Physics::GetInstance().AddCollider(bID, boxCollider);
		obj.LoadModel("resources/models/box/box.obj");
		
		obj.scale = s;
		Graphics::GetInstance().objects.push_back(obj);
		obj.Clear();

		HingeJointDef hjd;
		std::vector<Body*>* bodies = &Physics::GetInstance().bodies;
		hjd.Initialize((*bodies)[0], (*bodies)[i + 1], glm::vec3(p2.x, p1.y, p1.z), glm::vec3(1.0f, 0.0f, 0.0f));

		HingeJoint hj(&hjd);
		Physics::GetInstance().hingeJoints.push_back(hj);
	}
}