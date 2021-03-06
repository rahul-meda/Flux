
#include "Chain1.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"

Chain1& Chain1::GetInstance()
{
	static Chain1 instance;
	return instance;
}

void Chain1::Init()
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

	glm::vec3 p1 = glm::vec3(25.0f, 20.0f, 0.0f);
	tx = Transform(p1);
	bd.tx = tx;
	bd.isStatic = true;
	bID = Physics::GetInstance().AddBody(bd);
	boxCollider = new HullCollider();
	boxCollider->massData->density = 1.0f;
	mesh.GetColliderData(boxCollider);
	boxCollider->Scale(glm::vec3(25.0f, 0.05f, 0.05f));
	Physics::GetInstance().AddCollider(bID, boxCollider);
	obj.LoadModel("resources/models/box/box.obj");
	
	obj.scale = glm::vec3(25.0f, 0.05f, 0.05f);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	glm::vec3 yellowGreen(0.5f, 1.0f, 0.3f);
	glm::vec3 disco(0.2f, 0.7f, 1.0f);

	int NL = 30;
	int NC = 25;
	for (int j = 0; j < NC; ++j)
	{
		for (int i = 0; i < NL; ++i)
		{
			glm::vec3 p2 = glm::vec3((float)j, 20.0f, 0.75f * (i + 1.0f));
			tx = Transform(p2);
			bd.tx = tx;
			bd.isStatic = false;
			bID = Physics::GetInstance().AddBody(bd);
			SphereCollider* sphereCollider = new SphereCollider();
			sphereCollider->Scale(0.25f);
			Physics::GetInstance().AddCollider(bID, sphereCollider);
			obj.LoadModel("resources/models/sphere/sphere.obj");
			
			obj.scale = glm::vec3(0.25f);
			Graphics::GetInstance().objects.push_back(obj);
			obj.Clear();

			PositionJointDef pjd;
			std::vector<Body*>* bodies = &Physics::GetInstance().bodies;
			if(i == 0)
				pjd.Initialize((*bodies)[0], (*bodies)[i+NL*j + 1], glm::vec3(p2.x, p2.y, 0.75f*(float)i));
			else
				pjd.Initialize((*bodies)[i+NL*j], (*bodies)[i+NL*j + 1], glm::vec3(p2.x, p2.y, 0.75f*(float)i));

			PositionJoint pj(&pjd);
			Physics::GetInstance().posJoints.push_back(pj);
		}
	}
}