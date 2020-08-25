
#include "BallStack.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"

BallStack& BallStack::GetInstance()
{
	static BallStack instance;
	return instance;
}

void BallStack::Init()
{
	HMesh mesh;
	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_df.png");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_sp.png");
	unsigned int metalTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal1.jpeg");
	Material material;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	HullCollider* boxCollider;
	R_Mesh obj;
	glm::vec3 yellowGreen(0.5f, 1.0f, 0.3f);
	glm::vec3 disco(0.2f, 0.7f, 1.0f);
	glm::vec3 s(1.0f);

	for (int k = 0; k < 1; ++k)
	{
		for (int j = 0; j < 3; ++j)
		{
			for (int i = 0; i < 1; ++i)
			{
				float x = 2.0f * s.x * (float)i;
				float y = 0.5f + s.y + 2.0f * s.y * (float)j;
				float z = -2.0f * s.z * (float)k;
				glm::vec3 p(x, y, z);
				tx = Transform(p);
				bd.tx = tx;
				bd.isStatic = false;
				bID = Physics::GetInstance().AddBody(bd);
				if (j % 2 == 0)
				{
					boxCollider = new HullCollider();
					mesh.GetColliderData(boxCollider);
					boxCollider->Scale(glm::vec3(s));
					Physics::GetInstance().AddCollider(bID, boxCollider);
					obj.LoadModel("resources/models/box/box.obj");
					
					obj.scale = s;
					Graphics::GetInstance().objects.push_back(obj);
					obj.Clear();
				}
				else
				{
					SphereCollider* sphereCollider = new SphereCollider();
					sphereCollider->Scale(1.0f);
					Physics::GetInstance().AddCollider(bID, sphereCollider);
					obj.LoadModel("resources/models/sphere/sphere.obj");
					
					obj.scale = glm::vec3(1.0f);
					Graphics::GetInstance().objects.push_back(obj);
					obj.Clear();
				}
			}
		}
	}
}