
#include "Collapse2.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"
#include "../Mesh/ObjParser.h"
#include "../Components/Model.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"

Collapse2& Collapse2::GetInstance()
{
	static Collapse2 instance;
	return instance;
}

void Collapse2::Init()
{
	ModelDef box, sphere, line;
	HMesh mesh;
	ParseObj("Resources/Models/Box.obj", mesh);
	mesh.GetModelData(box);
	unsigned int boxModel = Graphics::GetInstance().CreateModel(box);

	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_df.png");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_sp.png");
	unsigned int metalTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal1.jpeg");
	Material material;
	material.diffuseMap = boxDfTxt;
	material.specularMap = boxSpTxt;
	material.count = 2;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;
	R_Object obj;
	glm::vec3 sv(0.1f, 1.0f, 1.5f);
	glm::vec3 sh(1.5f, 1.0, 0.1f);
	glm::vec3 cv(-20.f, 0.5f + sv.y, 0.0f);
	glm::vec3 ch(cv.x - sv.x + sh.x, cv.y + sv.y + sh.y, cv.z);
	int NX = 25;
	int NY = 8;
	int NZ = 1;

	for (int k = 0; k < NZ; ++k)
	{
		for (int j = 0; j < NY/2; ++j)
		{
			for (int i = 0; i < NX; ++i)
			{
				float x = cv.x + ((sh.x - sv.x) * 2.0f) * (float)i;
				float y = cv.y + ((sv.y + sh.y) * 2.0f) * (float)j;
				float z = cv.z - 2.0f * sv.z * (float)k;
				glm::vec3 p(x, y, z);
				tx = Transform(p);
				bd.tx = tx;
				bd.isStatic = false;
				bID = Physics::GetInstance().AddBody(bd);
				boxCollider = new HullCollider();
				mesh.GetColliderData(boxCollider);
				boxCollider->Scale(sv);
				Physics::GetInstance().AddCollider(bID, boxCollider);
				obj.modelIDs.push_back(boxModel);
				obj.materials.push_back(material);
				obj.scale = sv;
				Graphics::GetInstance().objects.push_back(obj);
				obj.Clear();
			}
			for (int i = 0; i < NX-1; ++i)
			{
				float x = ch.x + ((sh.x - sv.x) * 2.0f) * (float)i;
				float y = ch.y + ((sv.y + sh.y) * 2.0f) * (float)j;
				float z = -2.0f * sv.z * (float)k;
				if (i % 2) z -= sv.z - sh.z;
				else z += sv.z - sh.z;
				glm::vec3 p(x, y, z);
				tx = Transform(p);
				bd.tx = tx;
				bd.isStatic = false;
				bID = Physics::GetInstance().AddBody(bd);
				boxCollider = new HullCollider();
				mesh.GetColliderData(boxCollider);
				boxCollider->Scale(sh);
				Physics::GetInstance().AddCollider(bID, boxCollider);
				obj.modelIDs.push_back(boxModel);
				obj.materials.push_back(material);
				obj.scale = sh;
				Graphics::GetInstance().objects.push_back(obj);
				obj.Clear();
			}
		}
	}

	for (int k = 0; k < NZ; ++k)
	{
		for (int j = 0; j < NY / 2; ++j)
		{
			glm::vec3 p(ch);
			p.z -= sv.z - sh.z - (2.0f * sh.z) + 2.0f * sv.z * (float)k;
			p.y += ((sv.y + sh.y) * 2.0f) * (float)j;
			tx = Transform(p);
			bd.tx = tx;
			bd.isStatic = false;
			bID = Physics::GetInstance().AddBody(bd);
			boxCollider = new HullCollider();
			mesh.GetColliderData(boxCollider);
			boxCollider->Scale(sh);
			Physics::GetInstance().AddCollider(bID, boxCollider);
			obj.modelIDs.push_back(boxModel);
			obj.materials.push_back(material);
			obj.scale = sh;
			Graphics::GetInstance().objects.push_back(obj);
			obj.Clear();
		}
		for (int j = 0; j < NY / 2; ++j)
		{
			glm::vec3 p(ch);
			p.x += ((sh.x - sv.x) * 2.0f) * (float)(NX - 2);
			p.z += sv.z - sh.z - (2.0f * sh.z) - 2.0f * sv.z * (float)k;
			p.y += ((sv.y + sh.y) * 2.0f) * (float)j;
			tx = Transform(p);
			bd.tx = tx;
			bd.isStatic = false;
			bID = Physics::GetInstance().AddBody(bd);
			boxCollider = new HullCollider();
			mesh.GetColliderData(boxCollider);
			boxCollider->Scale(sh);
			Physics::GetInstance().AddCollider(bID, boxCollider);
			obj.modelIDs.push_back(boxModel);
			obj.materials.push_back(material);
			obj.scale = sh;
			Graphics::GetInstance().objects.push_back(obj);
			obj.Clear();
		}
	}

	CreateSphere(sphere);
	unsigned int sphereModel = Graphics::GetInstance().CreateModel(sphere);
	glm::vec3 yellowGreen(0.5f, 1.0f, 0.3f);
	glm::vec3 disco(0.2f, 0.7f, 1.0f);

	tx = Transform(glm::vec3(-30.0f, 1.5f, 0.0f));
	bd.tx = tx;
	bd.isStatic = false;
	//bd.velocity = glm::vec3(10.0f, 0.0f, 0.0f);
	bID = Physics::GetInstance().AddBody(bd);
	SphereCollider* sphereCollider = new SphereCollider();
	sphereCollider->Scale(1.0f);
	sphereCollider->massData->density = 10.0f;
	Physics::GetInstance().AddCollider(bID, sphereCollider);
	obj.modelIDs.push_back(sphereModel);
	obj.materials.push_back(material);
	obj.scale = glm::vec3(1.0f);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
}