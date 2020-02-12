
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

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;

	for (int i = 0; i < 25; ++i)
	{
		glm::vec3 p = glm::vec3(0.0f, 1.5f + 2.0f*(float)i, -1.5f);
		glm::quat q = glm::angleAxis(0.75f * (float)i, glm::vec3(0.0f, 1.0f, 0.0f));
		tx = Transform(p, q);
		bd.tx = tx;
		bd.isStatic = false;
		bID = Physics::GetInstance().AddBody(bd);
		boxCollider = new HullCollider();
		mesh.GetColliderData(boxCollider);
		boxCollider->Scale(glm::vec3(1.0f));
		Physics::GetInstance().bodies.back()->AddCollider(boxCollider);
		Graphics::GetInstance().scales.push_back(glm::vec3(1.0f));
		gameObjects.push_back(GameObject(boxModel, bID));
	}
}