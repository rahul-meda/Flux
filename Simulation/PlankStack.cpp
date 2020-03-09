
#include "PlankStack.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Mesh/ObjParser.h"
#include "../Components/Model.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"

PlankStack& PlankStack::GetInstance()
{
	static PlankStack instance;
	return instance;
}

void PlankStack::Init()
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

	glm::vec3 s(1.0f, 0.5f, 0.1f);

	int H = 5;

	for (int k = 0; k < 5; ++k)
	{
		for (int j = 0; j < 10; ++j)
		{
			for (int i = 0; i < H; ++i)
			{
				glm::vec3 p = glm::vec3(-20.0f + 2.0f * s.x * (float)j, 1.0f + 2.0f*(float)i, -2.0f * (float)k);
				glm::quat q = glm::angleAxis(PI * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
				tx = Transform(p, q);
				bd.tx = tx;
				bd.isStatic = false;
				bID = Physics::GetInstance().AddBody(bd);
				boxCollider = new HullCollider();
				mesh.GetColliderData(boxCollider);
				boxCollider->Scale(s);
				Physics::GetInstance().AddCollider(bID, boxCollider);
				obj.modelIDs.push_back(boxModel);
				obj.materials.push_back(material);
				obj.scale = s;
				Graphics::GetInstance().objects.push_back(obj);
				obj.Clear();
			}
			for (int i = 0; i < H; ++i)
			{
				glm::vec3 p = glm::vec3(-19.0f + 2.0f * s.x * (float)j, 2.0f + 2.0f*(float)i, -2.0f * (float)k);
				if (j % 2 == 0) p.z += 0.5f;
				else p.z -= 0.5f;
				glm::quat q = glm::identity<glm::quat>();
				tx = Transform(p, q);
				bd.tx = tx;
				bd.isStatic = false;
				bID = Physics::GetInstance().AddBody(bd);
				boxCollider = new HullCollider();
				mesh.GetColliderData(boxCollider);
				boxCollider->Scale(s);
				Physics::GetInstance().AddCollider(bID, boxCollider);
				obj.modelIDs.push_back(boxModel);
				obj.materials.push_back(material);
				obj.scale = s;
				Graphics::GetInstance().objects.push_back(obj);
				obj.Clear();
			}
		}
	}
}