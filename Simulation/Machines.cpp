
#include "Machines.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"
#include "../Physics/CapsuleCollider.h"

Machines& Machines::GetInstance()
{
	static Machines instance;
	return instance;
}

void Machines::Init()
{
	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_df.png");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_sp.png");
	unsigned int metalTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal2.jpg");

	HMesh mesh;
	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* hull;
	R_Mesh obj;

	float H = 50.0f;
	glm::vec3 p = glm::vec3(0.0f, H, 0.0f);
	glm::quat q = glm::identity<glm::quat>();
	glm::vec3 s(0.25f, 40.0f, 0.25f);

	Graphics::GetInstance().AddPointLight(glm::vec3(0.0f, H, 3.0f));

	tx = Transform(p, q);
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	ParseObj("Resources/Models/box.obj", mesh);
	mesh.GetColliderData(hull);
	hull->massData->density = 15.0f;
	hull->Scale(s);
	Physics::GetInstance().AddCollider(bID, hull);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(s);
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	int N = 4;
	for (int i = 1; i < N; ++i)
	{
		float angle = ((float)i / (float)(N - 1)) * 0.5f * PI;
		glm::vec3 axis(0.0f, 0.0f, 1.0f);
		glm::quat q1 = glm::angleAxis(angle, axis);
		tx = Transform(p, q1);
		hull = new HullCollider();
		hull->txB.position = glm::vec3(0.0f);
		hull->txB.R = tx.R;
		mesh.GetColliderData(hull);
		hull->massData->density = 15.0f;
		hull->Scale(s);
		Physics::GetInstance().AddCollider(bID, hull);
		obj.posOffsets.push_back(glm::vec3(0.0f));
		obj.rotOffsets.push_back(tx.R);
		obj.scales.push_back(s);
		obj.LoadModel("resources/models/box/box.obj");
		
	}
	for (int i = 1; i < N - 1; ++i)
	{
		float angle = ((float)i / (float)(N - 1)) * 0.5f * PI;
		glm::vec3 axis(0.0f, 0.0f, -1.0f);
		glm::quat q1 = glm::angleAxis(angle, axis);
		tx = Transform(p, q1);
		hull = new HullCollider();
		hull->txB.position = glm::vec3(0.0f);
		hull->txB.R = tx.R;
		mesh.GetColliderData(hull);
		hull->massData->density = 15.0f;
		hull->Scale(s);
		Physics::GetInstance().AddCollider(bID, hull);
		obj.posOffsets.push_back(glm::vec3(0.0f));
		obj.rotOffsets.push_back(tx.R);
		obj.scales.push_back(s);
		obj.LoadModel("resources/models/box/box.obj");
		
	}

	tx = Transform(p, glm::angleAxis(PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	hull = new HullCollider();
	hull->txB.position = glm::vec3(0.0f);
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->massData->density = 15.0f;
	hull->Scale(glm::vec3(1.0f, 2.0f, 1.0f));
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(1.0f, 2.0f, 1.0f));
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	unsigned int bID1 = bID;

	tx = Transform(glm::vec3(0.0f, 0.5f * H, 3.0f), glm::identity<glm::quat>());
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(0.5f, 0.5f * H, 0.5f));
	Physics::GetInstance().AddCollider(bID, hull);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.5f, 0.5f * H, 0.5f));
	obj.LoadModel("resources/models/box/box.obj");
	
	hull = new HullCollider();
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(0.5f, 0.5f * H, 0.5f));
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(glm::vec3(0.0f, 0.0f, -6.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.5f, 0.5f * H, 0.5f));
	obj.LoadModel("resources/models/box/box.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	tx = Transform(glm::vec3(0.0f, H, 3.0f), glm::identity<glm::quat>());
	bd.tx = tx;
	bd.isStatic = true;
	unsigned int bID2 = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(0.1f, 0.1f, 0.5f));
	Physics::GetInstance().AddCollider(bID2, hull);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.1f, 0.1f, 0.5f));
	obj.LoadModel("resources/models/box/box.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	std::vector<Body*>* bodies = &Physics::GetInstance().bodies;
	Body* b1 = (*bodies)[bID2];
	Body* b2 = (*bodies)[bID1];
	HingeJointDef hjd;
	glm::vec3 anchor(p);
	hjd.Initialize(b1, b2, anchor, glm::vec3(0.0f, 0.0f, 1.0f));
	hjd.enableLimit = false;
	hjd.enableMotor = true;
	hjd.motorSpeed = 0.7f;
	hjd.maxMotorTorque = 5000.0f;
	hjd.scale = 0.5f;

	HingeJoint hj(&hjd);
	Physics::GetInstance().hingeJoints.push_back(hj);

	float step = (2.0f * PI) / (4 * (N - 1));

	for (int i = 0; i < 4 * (N - 1); ++i)
	{
		float cos = cosf((float)i * step);
		float sin = sinf((float)i * step);
		p.x = 43.0f * cos;
		p.y = H + 43.0f * sin;
		p.z = 0.0f;
		tx = Transform(p, glm::identity<glm::quat>());
		bd.tx = tx;
		bd.isStatic = false;
		bID2 = Physics::GetInstance().AddBody(bd);
		hull = new HullCollider();
		mesh.GetColliderData(hull);
		hull->Scale(glm::vec3(1.0f));
		Physics::GetInstance().AddCollider(bID2, hull);
		obj.pos = tx.position;
		obj.rot = tx.R;
		obj.posOffsets.push_back(glm::vec3(0.0f));
		obj.rotOffsets.push_back(glm::mat3(1.0f));
		obj.scales.push_back(glm::vec3(1.0f));
		obj.LoadModel("resources/models/box/box.obj");
		
		Graphics::GetInstance().objects.push_back(obj);
		obj.Clear();

		b1 = (*bodies)[bID1];
		b2 = (*bodies)[bID2];
		anchor.x = 40.0f * cos;
		anchor.y = H + 40.0f * sin;
		anchor.z = 0.0f;
		hjd.Initialize(b1, b2, anchor, glm::vec3(0.0f, 0.0f, 1.0f));
		hjd.enableLimit = false;
		hjd.enableMotor = false;
		hjd.scale = 0.5f;

		hj = HingeJoint(&hjd);
		Physics::GetInstance().hingeJoints.push_back(hj);
	}
}