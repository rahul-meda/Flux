
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

void PlankStack::Init(std::vector<GameObject>& gameObjects)
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
				Physics::GetInstance().bodies.back()->AddCollider(boxCollider);
				Graphics::GetInstance().scales.push_back(s);
				gameObjects.push_back(GameObject(boxModel, bID));
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
				Physics::GetInstance().bodies.back()->AddCollider(boxCollider);
				Graphics::GetInstance().scales.push_back(s);
				gameObjects.push_back(GameObject(boxModel, bID));
			}
		}
	}

	for (int i = 0; i < H; ++i)
	{
		glm::vec3 p = glm::vec3(-19.0f, 2.0f + 2.0f*(float)i, -2.5f);
		glm::quat q = glm::identity<glm::quat>();
		tx = Transform(p, q);
		bd.tx = tx;
		bd.isStatic = false;
		bID = Physics::GetInstance().AddBody(bd);
		boxCollider = new HullCollider();
		mesh.GetColliderData(boxCollider);
		boxCollider->Scale(s);
		Physics::GetInstance().bodies.back()->AddCollider(boxCollider);
		Graphics::GetInstance().scales.push_back(s);
		gameObjects.push_back(GameObject(boxModel, bID));
	}

	for (int i = 0; i < H; ++i)
	{
		glm::vec3 p = glm::vec3(-19.0f, 2.0f + 2.0f*(float)i, -0.5f);
		glm::quat q = glm::identity<glm::quat>();
		tx = Transform(p, q);
		bd.tx = tx;
		bd.isStatic = false;
		bID = Physics::GetInstance().AddBody(bd);
		boxCollider = new HullCollider();
		mesh.GetColliderData(boxCollider);
		boxCollider->Scale(s);
		Physics::GetInstance().bodies.back()->AddCollider(boxCollider);
		Graphics::GetInstance().scales.push_back(s);
		gameObjects.push_back(GameObject(boxModel, bID));
	}

	for (int i = 0; i < H; ++i)
	{
		glm::vec3 p = glm::vec3(19.0f, 2.0f + 2.0f*(float)i, 0.5f);
		glm::quat q = glm::identity<glm::quat>();
		tx = Transform(p, q);
		bd.tx = tx;
		bd.isStatic = false;
		bID = Physics::GetInstance().AddBody(bd);
		boxCollider = new HullCollider();
		mesh.GetColliderData(boxCollider);
		boxCollider->Scale(s);
		Physics::GetInstance().bodies.back()->AddCollider(boxCollider);
		Graphics::GetInstance().scales.push_back(s);
		gameObjects.push_back(GameObject(boxModel, bID));
	}

	for (int i = 0; i < H; ++i)
	{
		glm::vec3 p = glm::vec3(19.0f, 2.0f + 2.0f*(float)i, -1.5f);
		glm::quat q = glm::identity<glm::quat>();
		tx = Transform(p, q);
		bd.tx = tx;
		bd.isStatic = false;
		bID = Physics::GetInstance().AddBody(bd);
		boxCollider = new HullCollider();
		mesh.GetColliderData(boxCollider);
		boxCollider->Scale(s);
		Physics::GetInstance().bodies.back()->AddCollider(boxCollider);
		Graphics::GetInstance().scales.push_back(s);
		gameObjects.push_back(GameObject(boxModel, bID));
	}

	for (int i = 0; i < H; ++i)
	{
		glm::vec3 p = glm::vec3(20.0f, 1.0f + 2.0f*(float)i, 0.0f);
		glm::quat q = glm::angleAxis(PI * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
		tx = Transform(p, q);
		bd.tx = tx;
		bd.isStatic = false;
		bID = Physics::GetInstance().AddBody(bd);
		boxCollider = new HullCollider();
		mesh.GetColliderData(boxCollider);
		boxCollider->Scale(s);
		Physics::GetInstance().bodies.back()->AddCollider(boxCollider);
		Graphics::GetInstance().scales.push_back(s);
		gameObjects.push_back(GameObject(boxModel, bID));
	}

	for (int i = 0; i < H; ++i)
	{
		glm::vec3 p = glm::vec3(20.0f, 1.0f + 2.0f*(float)i, -2.0f);
		glm::quat q = glm::angleAxis(PI * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
		tx = Transform(p, q);
		bd.tx = tx;
		bd.isStatic = false;
		bID = Physics::GetInstance().AddBody(bd);
		boxCollider = new HullCollider();
		mesh.GetColliderData(boxCollider);
		boxCollider->Scale(s);
		Physics::GetInstance().bodies.back()->AddCollider(boxCollider);
		Graphics::GetInstance().scales.push_back(s);
		gameObjects.push_back(GameObject(boxModel, bID));
	}

	/*tx = Transform(glm::vec3(-20.75, 12.5f, -2.0f), glm::identity<glm::quat>());
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(glm::vec3(0.5f, 12.5f, 2.0f));
	Physics::GetInstance().bodies.back()->AddCollider(boxCollider);
	Graphics::GetInstance().scales.push_back(glm::vec3(0.5f, 12.5f, 2.0f));
	gameObjects.push_back(GameObject(boxModel, bID, sandy));

	tx = Transform(glm::vec3(20.75, 12.5f, -2.0f), glm::identity<glm::quat>());
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(glm::vec3(0.5f, 12.5f, 2.0f));
	Physics::GetInstance().bodies.back()->AddCollider(boxCollider);
	Graphics::GetInstance().scales.push_back(glm::vec3(0.5f, 12.5f, 2.0f));
	gameObjects.push_back(GameObject(boxModel, bID, sandy));*/
}