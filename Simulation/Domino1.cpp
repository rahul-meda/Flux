
#include "Domino1.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"
#include "../Physics/CapsuleCollider.h"

Domino1& Domino1::GetInstance()
{
	static Domino1 instance;
	return instance;
}

void Domino1::Init()
{
	HMesh mesh;
	ParseObj("Resources/Models/Box.obj", mesh);
	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_df.png");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_sp.png");
	unsigned int ballTxt = Graphics::GetInstance().CreateTexture("resources/textures/earth_10k.jpg");
	unsigned int rampTxt = Graphics::GetInstance().CreateTexture("resources/textures/wood3.jpg");
	unsigned int hingeTxt = Graphics::GetInstance().CreateTexture("resources/textures/leather1.jpg");
	Material material;
	material.diffuseMap = boxDfTxt;
	material.specularMap = boxSpTxt;
	material.nMaps = 2;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;
	SphereCollider* sphereCollider;
	R_Mesh obj;
	glm::vec3 s(0.35f, 0.7f, 0.05f);
	float gap = s.y * 0.5f;
	float r = 12.0f;
	int n = 512;

	for (int j = 0; j < 5; ++j)
	{
		r -= 2.0f;
		n /= 2;
		for (int i = 0; i < n; ++i)
		{
			float step = 2.0f * PI / n;
			float angle = (float)i * step;
			glm::vec3 p(r * cosf(angle), s.y, r * sinf(angle));
			glm::quat q = glm::angleAxis(-angle, glm::vec3(0.0f, 1.0f, 0.0f));
			tx = Transform(p, q);
			bd.tx = tx;
			bd.isStatic = false;
			bID = Physics::GetInstance().AddBody(bd);
			boxCollider = new HullCollider();
			boxCollider->massData->density = 1.0f;
			mesh.GetColliderData(boxCollider);
			boxCollider->Scale(s);
			Physics::GetInstance().AddCollider(bID, boxCollider);
			obj.pos = tx.position;
			obj.rot = tx.R;
			obj.posOffsets.push_back(glm::vec3(0.0f));
			obj.rotOffsets.push_back(glm::mat3(1.0f));
			obj.scales.push_back(s);
			obj.LoadModel("resources/models/box/box.obj");
			obj.materials.push_back(material);
			Graphics::GetInstance().objects.push_back(obj);
			obj.Clear();
		}
	}

	material.diffuseMap = hingeTxt;
	material.specularMap = hingeTxt;
	material.nMaps = 2;
	glm::vec3 p(12.0f + s.x + 1.0f - 0.25f, 2.0f * s.y - 0.1f, s.z + 0.1f);
	for (int j = 0; j < 5; ++j)
	{
		p.x -= 2.0f;
		p.z -= 0.25f;
		s = glm::vec3(1.0f, 0.5f, 0.025f);
		glm::quat q = glm::angleAxis(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		tx = Transform(p, q);
		bd.tx = tx;
		bd.isStatic = false;
		bID = Physics::GetInstance().AddBody(bd);
		boxCollider = new HullCollider();
		boxCollider->massData->density = 100.0f - 22.5f * (float)j;
		mesh.GetColliderData(boxCollider);
		boxCollider->Scale(s);
		Physics::GetInstance().AddCollider(bID, boxCollider);
		obj.pos = tx.position;
		obj.rot = tx.R;
		obj.posOffsets.push_back(glm::vec3(0.0f));
		obj.rotOffsets.push_back(glm::mat3(1.0f));
		obj.scales.push_back(s);
		obj.LoadModel("resources/models/box/box.obj");
		obj.materials.push_back(material);
		Graphics::GetInstance().objects.push_back(obj);
		obj.Clear();
		unsigned int bid1 = bID;

		s = glm::vec3(0.25f, 0.05f, 0.05f);
		q = glm::angleAxis(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		tx = Transform(glm::vec3(p.x, p.y + 0.75f, p.z), q);
		bd.tx = tx;
		bd.isStatic = true;
		bID = Physics::GetInstance().AddBody(bd);
		boxCollider = new HullCollider();
		boxCollider->massData->density = 1.0f;
		mesh.GetColliderData(boxCollider);
		boxCollider->Scale(s);
		Physics::GetInstance().AddCollider(bID, boxCollider);
		obj.pos = tx.position;
		obj.rot = tx.R;
		obj.posOffsets.push_back(glm::vec3(0.0f));
		obj.rotOffsets.push_back(glm::mat3(1.0f));
		obj.scales.push_back(s);
		obj.LoadModel("resources/models/box/box.obj");
		obj.materials.push_back(material);
		Graphics::GetInstance().objects.push_back(obj);
		obj.Clear();

		glm::vec3 anchor = glm::vec3(p.x, p.y + 0.75f, p.z);
		std::vector<Body*>* bodies = &Physics::GetInstance().bodies;
		Body* b1 = (*bodies)[bID];
		Body* b2 = (*bodies)[bid1];
		HingeJointDef hjd;
		hjd.Initialize(b1, b2, anchor, glm::vec3(0.0f, 1.0f, 0.0f));
		hjd.enableLimit = false;
		hjd.lowerLimit = -PI * 0.5f;
		hjd.upperLimit = PI * 0.5f;
		hjd.enableMotor = false;
		hjd.motorSpeed = 0.0f;
		hjd.maxMotorTorque = 0.005f;
		hjd.scale = 0.1f;

		HingeJoint hj(&hjd);
		Physics::GetInstance().hingeJoints.push_back(hj);
	}

	material.diffuseMap = rampTxt;
	material.specularMap = rampTxt;
	material.nMaps = 2;
	p = glm::vec3(12.0f + s.x + 1.0f - 0.25f - 1.0f, 2.0f * s.y - 0.1f + 3.0f, s.z + 0.1f + 5.0f);
	tx = Transform(p, glm::angleAxis(-PI * 0.1f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(glm::vec3(0.5f, 0.1f, 5.0f));
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(0.5f, 0.1f, 5.0f));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
}