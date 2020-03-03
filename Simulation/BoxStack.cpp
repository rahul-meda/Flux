
#include "BoxStack.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Mesh/ObjParser.h"
#include "../Components/Model.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"

BoxStack& BoxStack::GetInstance()
{
	static BoxStack instance;
	return instance;
}

void BoxStack::Init(std::vector<GameObject>& gameObjects)
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

	glm::vec3 s(1.2f, 0.56f, 0.35f);
	s *= 3.0f;

	for (int k = 0; k < 1; ++k)
	{
		for (int j = 0; j < 10; ++j)
		{
			for (int i = 0; i < 10; ++i)
			{
				float x = 2.0f * s.x * (float)i;
				float y = 0.5f + s.y + 2.0f * s.y * (float)j;
				float z = -2.0f * s.z * (float)k;
				glm::vec3 p(x, y, z);
				tx = Transform(p);
				bd.tx = tx;
				bd.isStatic = false;
				bID = Physics::GetInstance().AddBody(bd);
				boxCollider = new HullCollider();
				mesh.GetColliderData(boxCollider);
				boxCollider->Scale(glm::vec3(s));
				Physics::GetInstance().AddCollider(bID, boxCollider);
				Graphics::GetInstance().scales.push_back(glm::vec3(s));
				gameObjects.push_back(GameObject(boxModel, bID, material));
			}
		}
	}

	tx = Transform(glm::vec3(10.0f, 10.0f, 50.0f));
	bd.tx = tx;
	bd.isStatic = false;
	//bd.velocity = glm::vec3(0.0f, 0.0f, -50.0f);
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	boxCollider->massData->density = 1.0f;
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(glm::vec3(2.0f, 1.0f, 1.0f));
	Physics::GetInstance().AddCollider(bID, boxCollider);
	Graphics::GetInstance().scales.push_back(glm::vec3(2.0f, 1.0f, 1.0f));
	gameObjects.push_back(GameObject(boxModel, bID, material));
}