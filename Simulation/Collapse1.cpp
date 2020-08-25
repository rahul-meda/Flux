
#include "Collapse1.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"

Collapse1& Collapse1::GetInstance()
{
	static Collapse1 instance;
	return instance;
}

void Collapse1::Init(GLFWwindow* window, int width, int height)
{
	Simulation::Init(window, width, height);

	HMesh mesh;
	ParseObj("Resources/Models/Box.obj", mesh);
	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_df.png");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_sp.png");
	unsigned int metalTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal1.jpeg");
	Material material;

	R_Mesh cube = Graphics::GetInstance().dCube;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;
	R_Mesh obj;

	glm::vec3 sv(0.1f, 1.0f, 0.1f);
	glm::vec3 sh(1.0f, 0.1f, 1.0f);
	sv *= 2.0f;
	sh *= 2.0f;
	glm::vec3 C(-20.0f, 0.5f + sv.y, 20.0f);
	for (int k = 0; k < 1; ++k)
	{
		float dz = 2.0f * sh.z * (float)k;
		for (int j = 0; j < 9; ++j)
		{
			float dy = 2.0f * (sv.y + sh.y) * float(j);
			for (int i = 0; i < 22; ++i)
			{
				float dx = 2.0f * sh.x * (float)i;
				for (int v = 0; v < 4; ++v)
				{
					float x = v % 2 ? (C.x + dx + sh.x - sv.x) :
						(C.x + dx - sh.x + sv.x);
					float y = C.y + dy;
					float z = v / 2 ? (C.z + dz + sh.z - sv.z) : (C.z + dz - sh.z + sv.z);
					glm::vec3 p(x, y, z);
					tx = Transform(p);
					bd.tx = tx;
					bd.isStatic = false;
					bID = Physics::GetInstance().AddBody(bd);
					boxCollider = new HullCollider();
					mesh.GetColliderData(boxCollider);
					boxCollider->Scale(glm::vec3(sv));
					Physics::GetInstance().AddCollider(bID, boxCollider);
					obj.LoadModel(cube);
					obj.txID = bID;
					obj.scale = sv;
					Graphics::GetInstance().objects.push_back(obj);
					obj.Clear();
				}
				float x = C.x + dx;
				float y = C.y + sv.y + sh.y + dy;
				float z = C.z + dz;
				glm::vec3 p(x, y, z);
				tx = Transform(p);
				bd.tx = tx;
				bd.isStatic = false;
				bID = Physics::GetInstance().AddBody(bd);
				boxCollider = new HullCollider();
				mesh.GetColliderData(boxCollider);
				boxCollider->Scale(glm::vec3(sh));
				Physics::GetInstance().AddCollider(bID, boxCollider);
				obj.LoadModel(cube);
				obj.txID = bID;
				obj.scale = sh;
				Graphics::GetInstance().objects.push_back(obj);
				obj.Clear();
			}
		}
	}

	tx = Transform(glm::vec3(-30.0f, 3.0f, 20.0f));
	bd.tx = tx;
	bd.isStatic = false;
	bd.velocity = glm::vec3(0.0f, 0.0f, 1.0f);
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	boxCollider->massData->density = 10.0f;
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(glm::vec3(1.0f));
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.LoadModel(cube);
	obj.txID = bID;
	obj.scale = glm::vec3(1.0f);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
}

void Collapse1::OnKeyTap(GLFWwindow * window, int key, int scanCode, int action, int mods)
{
	Simulation::OnKeyTap(window, key, scanCode, action, mods);
}

void Collapse1::OnKeyPress(GLFWwindow * window)
{
	Simulation::OnKeyPress(window);
}

void Collapse1::Update(GLFWwindow* window)
{
	Simulation::Update(window);
}