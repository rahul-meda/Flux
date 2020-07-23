
#include "BoxStack.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"
#include "../Physics/CapsuleCollider.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"

BoxStack& BoxStack::GetInstance()
{
	static BoxStack instance;
	return instance;
}

void BoxStack::Init()
{
	HMesh mesh;
	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_df.png");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_sp.png");
	unsigned int metalTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal2.jpg");
	unsigned int linkTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal1.jpeg");
	Material material;
	material.diffuseMap = boxDfTxt;
	material.specularMap = boxDfTxt;
	material.nMaps = 2;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;
	R_Mesh obj;

	glm::vec3 s(1.2f, 0.56f, 0.35f);
	//s *= 1.5f;

	for (int k = 0; k < 5; ++k)
	{
		for (int j = 0; j < 11; ++j)
		{
			for (int i = 0; i < 10; ++i)
			{
				float x = 2.0f * s.x * (float)i;
				float y = 0.5f + s.y + 2.0f * s.y * (float)j;
				float z = -(2.0f * s.z + 2.0f) * (float)k;
				glm::quat q = glm::identity<glm::quat>();
				if (j % 2 == 1)
					x -= s.x;
				else if (i == 9)
					q = glm::angleAxis(0.5f * PI, glm::vec3(0.0f, 1.0f, 0.0f));
				glm::vec3 p(x, y, z);

				tx = Transform(p, q);
				bd.tx = tx;
				bd.isStatic = false;
				bID = Physics::GetInstance().AddBody(bd);
				boxCollider = new HullCollider();
				mesh.GetColliderData(boxCollider);
				boxCollider->Scale(glm::vec3(s));
				boxCollider->massData->density = 2.0f;
				Physics::GetInstance().AddCollider(bID, boxCollider);
				obj.pos = tx.position;
				obj.rot = tx.R;
				obj.posOffsets.push_back(glm::vec3(0.0f));
				obj.rotOffsets.push_back(glm::mat3(1.0f));
				obj.scales.push_back(s);
				obj.LoadModel("resources/models/box/box.obj");
				
				Graphics::GetInstance().objects.push_back(obj);
				obj.Clear();
			}
		}
	}

	tx = Transform(glm::vec3(10.0f, 35.0f, 10.0f));
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(glm::vec3(1.0f, 0.1f, 1.0f));
	boxCollider->massData->density = 2.0f;
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(1.0f, 0.1f, 1.0f));
	obj.LoadModel("resources/models/box/box.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	unsigned int bID1 = bID;
	glm::vec3 p;
	float gap = 0.1f;
	int NL = 10;
	material.diffuseMap = linkTxt;
	material.specularMap = linkTxt;
	material.nMaps = 2;

	for (int i = 0; i < NL; ++i)
	{
		p = glm::vec3(10.0f, 34.0f, 10.75f + (float)i * (2.0f + gap));
		tx = Transform(p, glm::angleAxis(-PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
		bd.tx = tx;
		bd.isStatic = false;
		unsigned int bID2 = Physics::GetInstance().AddBody(bd);
		CapsuleCollider* c = new CapsuleCollider();
		c->Scale(0.5f);
		c->H = 0.5f;
		c->upB = glm::vec3(0.0f, 1.0f, 0.0f);
		c->massData->density = 100.0f;
		Physics::GetInstance().AddCollider(bID2, c);
		obj.pos = tx.position;
		obj.rot = tx.R;
		obj.posOffsets.push_back(glm::vec3(0.0f));
		obj.rotOffsets.push_back(glm::mat3(1.0f));
		obj.scales.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
		obj.LoadModel("resources/models/cylinder/cylinder.obj");
		
		obj.posOffsets.push_back(glm::vec3(0.0f, 0.5f, 0.0f));
		obj.rotOffsets.push_back(glm::mat3(1.0f));
		obj.scales.push_back(glm::vec3(0.5f));
		obj.LoadModel("resources/models/sphere/sphere.obj");
		
		obj.posOffsets.push_back(glm::vec3(0.0f, -0.5f, 0.0f));
		obj.rotOffsets.push_back(glm::mat3(1.0f));
		obj.scales.push_back(glm::vec3(0.5f));
		obj.LoadModel("resources/models/sphere/sphere.obj");
		
		Graphics::GetInstance().objects.push_back(obj);
		obj.Clear();

		PositionJointDef pjd;
		std::vector<Body*>* bodies = &Physics::GetInstance().bodies;

		pjd.Initialize((*bodies)[bID1], (*bodies)[bID2], glm::vec3(p.x, p.y, 10.0f -gap + (float)i * (2.0f + gap)));
		bID1 = bID2;

		PositionJoint pj(&pjd);
		Physics::GetInstance().posJoints.push_back(pj);
	}

	material.diffuseMap = metalTxt;
	material.specularMap = metalTxt;
	material.nMaps = 2;
	p.z += 3.75f + gap + 4.0f;
	tx = Transform(p);
	bd.tx = tx;
	bd.isStatic = false;
	unsigned int bID2 = Physics::GetInstance().AddBody(bd);
	SphereCollider* sphereCollider = new SphereCollider();
	sphereCollider->Scale(3.0f);
	sphereCollider->massData->density = 10.0f;
	Physics::GetInstance().AddCollider(bID2, sphereCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(3.0f));
	obj.LoadModel("resources/models/sphere/sphere.obj");
	
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	PositionJointDef pjd;
	std::vector<Body*>* bodies = &Physics::GetInstance().bodies;

	pjd.Initialize((*bodies)[bID1], (*bodies)[bID2], glm::vec3(p.x, p.y, 10.0f -gap + (float)NL * (2.0f + gap)));
	bID1 = bID2;

	PositionJoint pj(&pjd);
	Physics::GetInstance().posJoints.push_back(pj);
}