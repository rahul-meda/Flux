
#include "Collapse3.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"
#include "../Mesh/ObjParser.h"
#include "../Components/Model.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"

Collapse3& Collapse3::GetInstance()
{
	static Collapse3 instance;
	return instance;
}

void Collapse3::Init(std::vector<GameObject>& gameObjects)
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
	glm::vec3 s(1.0f, 0.05f, 0.5f);
	int NX = 9;
	int NY = 9;
	int NZ = 1;
	float angle = (PI / 3.0f) + (PI/25.0f);
	float dy = s.y * 2.0f;
	glm::vec3 sh(s);
	sh.x += 0.1f;
	float friction = 0.9f;

	for (int k = 0; k < NZ; ++k)	
	{
		for (int j = 0; j < NY; ++j)
		{
			for (int i = 0; i < NX - j; ++i)
			{
				float x = (float)j * (s.x + 1.25f*dy) + (float)i * (2.0 * s.x + 2.0f * dy) + 0.5f* s.x+dy;
				float y = 0.5f + (float)j * (1.732f * s.x + 2.0f * dy) + 2.0f * s.y + 0.866 * s.x + s.y;
				float z = (float)k * 2.0f;
				glm::vec3 p(x, y, z);
				glm::quat q = glm::angleAxis(angle, glm::vec3(0.0f, 0.0f, 1.0f));
				tx = Transform(p, q);
				bd.tx = tx;
				bd.isStatic = false;
				bID = Physics::GetInstance().AddBody(bd);
				boxCollider = new HullCollider();
				boxCollider->friction = friction;
				mesh.GetColliderData(boxCollider);
				boxCollider->Scale(s);
				Physics::GetInstance().AddCollider(bID, boxCollider);
				Graphics::GetInstance().scales.push_back(s);
				gameObjects.push_back(GameObject(boxModel, bID, material));

				p.x += s.x-dy;
				q = glm::angleAxis(-angle, glm::vec3(0.0f, 0.0f, 1.0f));
				tx = Transform(p, q);
				bd.tx = tx;
				bd.isStatic = false;
				bID = Physics::GetInstance().AddBody(bd);
				boxCollider = new HullCollider();
				boxCollider->friction = friction;
				mesh.GetColliderData(boxCollider);
				boxCollider->Scale(s);
				Physics::GetInstance().AddCollider(bID, boxCollider);
				Graphics::GetInstance().scales.push_back(s);
				gameObjects.push_back(GameObject(boxModel, bID, material));
			}
			for (int l = 0; l < NX - j; ++l)
			{
				//float x = (float)j * (s.x + 1.5f* dy) + (float)l * (2.0 * sh.x + 0.1f * s.y) + s.x;
				float x = (float)j * (s.x + 1.25* dy) + (float)l * (2.0 * s.x + 2.0f * dy) +  s.x + 0.5f*dy;
				float y = 0.5f + sh.y + (float)j * (1.732f * s.x + 2.0f * dy);
				float z = (float)k * 2.0f;
				glm::vec3 p(x, y, z);
				tx = Transform(p);
				bd.tx = tx;
				bd.isStatic = false;
				bID = Physics::GetInstance().AddBody(bd);
				boxCollider = new HullCollider();
				boxCollider->friction = friction;
				mesh.GetColliderData(boxCollider);
				boxCollider->Scale(sh);
				Physics::GetInstance().AddCollider(bID, boxCollider);
				Graphics::GetInstance().scales.push_back(sh);
				gameObjects.push_back(GameObject(boxModel, bID, material));
			}
		}
	}

	CreateSphere(sphere);
	unsigned int sphereModel = Graphics::GetInstance().CreateModel(sphere);
	glm::vec3 yellowGreen(0.5f, 1.0f, 0.3f);
	glm::vec3 disco(0.2f, 0.7f, 1.0f);

	tx = Transform(glm::vec3(10.0f, 10.0f, 20.0f));
	bd.tx = tx;
	bd.isStatic = false;
	bd.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	bID = Physics::GetInstance().AddBody(bd);
	SphereCollider* sphereCollider = new SphereCollider();
	sphereCollider->Scale(0.5f);
	sphereCollider->massData->density = 20.0f;
	Physics::GetInstance().AddCollider(bID, sphereCollider);
	Graphics::GetInstance().scales.push_back(glm::vec3(0.5f));
	gameObjects.push_back(GameObject(sphereModel, bID, material));
}