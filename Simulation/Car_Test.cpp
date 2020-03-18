
#include "Car_Test.h"
#include "../Mesh/ObjParser.h"
#include "../Components/Model.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"
#include "../Physics/CapsuleCollider.h"

void Car_Test::Init(GLFWwindow* window, int width, int height)
{
	Simulation::Init(window, width, height);

	HMesh mesh;

	unsigned int boxModel = Graphics::GetInstance().cubeModelID;
	unsigned int sphereModel = Graphics::GetInstance().sphereModelID;
	unsigned int cylinderModel = Graphics::GetInstance().cylinderModelID;

	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_df.png");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_sp.png");
	unsigned int ballTxt = Graphics::GetInstance().CreateTexture("resources/textures/earth_10k.jpg");
	unsigned int rampTxt = Graphics::GetInstance().CreateTexture("resources/textures/wood3.jpg");
	unsigned int hingeTxt = Graphics::GetInstance().CreateTexture("resources/textures/leather1.jpg");

	Material material;
	material.diffuseMap = hingeTxt;
	material.specularMap = boxSpTxt;
	material.count = 2;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	std::vector<unsigned int> bids;
	HullCollider* hull;
	SphereCollider* sphereCollider;
	R_Object obj;
	std::vector<glm::vec3> anchors;
	glm::vec3 sw(0.5f, 0.25f, 0.5f);	// wheel size
	glm::vec3 sc(3.0f, 0.1, 1.0f);		// chassis size
	float gapH = 0.1f;					// horizontal gap b/w wheel and suspension
	float gapV = 0.1f;					// vertical gap b/w wheel center and suspension
	float dw = 20.0f;					// wheel density
	float dc = 100.0f;					// chassis density
	motorSpeed = 0.0f;
	maxMotorTorque = 9999.0f;
	steerFactor = 0.025f;

	// front left wheel
	glm::vec3 p(sc.x - sw.x, sw.x, -(sc.z + gapH + sw.y));
	tx = Transform(p, glm::angleAxis(-PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bd.filterID = 1;
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	ParseObj("Resources/Models/cylinder_256.obj", mesh);
	mesh.GetColliderData(hull);
	hull->Scale(sw);
	hull->friction = 0.9f;
	hull->restitution = 0.1f;
	hull->massData->density = dw;
	//Physics::GetInstance().AddCollider(bID, hull);
	sphereCollider = new SphereCollider();
	sphereCollider->Scale(sw.x);
	sphereCollider->friction = 0.999f;
	sphereCollider->restitution = 0.1f;
	sphereCollider->massData->density = dw;
	Physics::GetInstance().AddCollider(bID, sphereCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(sw);
	obj.modelIDs.push_back(cylinderModel);
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	r_wheelIds[0] = Graphics::GetInstance().objects.size() - 1;
	obj.Clear();
	bids.push_back(bID);
	anchors.push_back(p); 

	// front right wheel
	p = glm::vec3(sc.x - sw.x, sw.x, sc.z + gapH + sw.y);
	tx = Transform(p, glm::angleAxis(-PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	mesh.GetColliderData(hull);
	hull->Scale(sw);
	hull->friction = 0.9f;
	hull->restitution = 0.1f;
	hull->massData->density = dw;
	//Physics::GetInstance().AddCollider(bID, hull);
	sphereCollider = new SphereCollider();
	sphereCollider->Scale(sw.x);
	sphereCollider->friction = 0.999f;
	sphereCollider->restitution = 0.1f;
	sphereCollider->massData->density = dw;
	Physics::GetInstance().AddCollider(bID, sphereCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(sw);
	obj.modelIDs.push_back(cylinderModel);
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	r_wheelIds[1] = Graphics::GetInstance().objects.size() - 1;
	obj.Clear();
	bids.push_back(bID);
	anchors.push_back(p);

	// rear left wheel
	p = glm::vec3(-sc.x + sw.x, sw.x, -(sc.z + gapH + sw.y));
	tx = Transform(p, glm::angleAxis(-PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	mesh.GetColliderData(hull);
	hull->Scale(sw);
	hull->friction = 0.999f;
	hull->restitution = 0.1f;
	hull->massData->density = dw;
	//Physics::GetInstance().AddCollider(bID, hull);
	sphereCollider = new SphereCollider();
	sphereCollider->Scale(sw.x);
	sphereCollider->friction = 0.999f;
	sphereCollider->restitution = 0.1f;
	sphereCollider->massData->density = dw;
	Physics::GetInstance().AddCollider(bID, sphereCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(sw);
	obj.modelIDs.push_back(cylinderModel);
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	bids.push_back(bID);
	anchors.push_back(p);

	// rear right wheel
	p = glm::vec3(-sc.x + sw.x, sw.x, sc.z + gapH + sw.y);
	tx = Transform(p, glm::angleAxis(-PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	mesh.GetColliderData(hull);
	hull->Scale(sw);
	hull->friction = 0.999f;
	hull->restitution = 0.1f;
	hull->massData->density = dw;
	//Physics::GetInstance().AddCollider(bID, hull);
	sphereCollider = new SphereCollider();
	sphereCollider->Scale(sw.x);
	sphereCollider->friction = 0.999f;
	sphereCollider->restitution = 0.1f;
	sphereCollider->massData->density = dw;
	Physics::GetInstance().AddCollider(bID, sphereCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(sw);
	obj.modelIDs.push_back(cylinderModel);
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	bids.push_back(bID);
	anchors.push_back(p);

	material.diffuseMap = boxDfTxt;
	material.specularMap = boxSpTxt;
	material.count = 2;

	// chassis
	ModelDef carModel;
	p = glm::vec3(0.0f, sw.y + gapV + sc.y, 0.0f);
	tx = Transform(p);
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	ParseObj("Resources/Models/Box.obj", mesh);
	mesh.GetColliderData(hull);
	hull->Scale(sc);
	hull->massData->density = dc;
	hull->restitution = 0.1f;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(sc);
	obj.modelIDs.push_back(boxModel);
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	bids.push_back(bID);

	std::vector<Body*>* bodies = &Physics::GetInstance().bodies;

	// connect front wheels to chassis using position joints
	wheels.push_back((*bodies)[bids[0]]);
	chassis = (*bodies)[bids[4]];

	UniversalJointDef ujd;
	ujd.Initialize(wheels[0], chassis, anchors[0], glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	UniversalJoint uj(&ujd);
	//Physics::GetInstance().uniJoints.push_back(uj);

	HingeJointDef hjd;
	hjd.Initialize(wheels[0], chassis, anchors[0], glm::vec3(0.0f, 0.0f, -1.0f));
	hjd.enableLimit = false;
	hjd.enableMotor = true;
	hjd.motorSpeed = motorSpeed;
	hjd.maxMotorTorque = maxMotorTorque;
	hjd.scale = 0.05f;

	HingeJoint hj(&hjd);
	Physics::GetInstance().hingeJoints.push_back(hj);
	int jid = Physics::GetInstance().hingeJoints.size() - 1;
	wheelJointIds.push_back(jid);

	wheels.push_back((*bodies)[bids[1]]);

	ujd.Initialize(wheels[1], chassis, anchors[1], glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	uj = UniversalJoint(&ujd);
	//Physics::GetInstance().uniJoints.push_back(uj);

	hjd.Initialize(wheels[1], chassis, anchors[1], glm::vec3(0.0f, 0.0f, -1.0f));
	hjd.enableLimit = false;
	hjd.enableMotor = true;
	hjd.motorSpeed = motorSpeed;
	hjd.maxMotorTorque = maxMotorTorque;
	hjd.scale = 0.05f;

	hj = HingeJoint(&hjd);
	Physics::GetInstance().hingeJoints.push_back(hj);
	wheelJointIds.push_back(++jid);

	// connect rear wheels to chassis using hinge joints
	wheels.push_back((*bodies)[bids[2]]);

	hjd.Initialize(wheels[2], chassis, anchors[2], glm::vec3(0.0f, 0.0f, -1.0f));
	hjd.enableLimit = false;
	hjd.enableMotor = true;
	hjd.motorSpeed = motorSpeed;
	hjd.maxMotorTorque = maxMotorTorque;
	hjd.scale = 0.05f;

	hj = HingeJoint(&hjd);
	Physics::GetInstance().hingeJoints.push_back(hj);
	wheelJointIds.push_back(++jid);

	wheels.push_back((*bodies)[bids[3]]);

	hjd.Initialize(wheels[3], chassis, anchors[3], glm::vec3(0.0f, 0.0f, -1.0f));
	hjd.enableLimit = false;
	hjd.enableMotor = true;
	hjd.motorSpeed = motorSpeed;
	hjd.maxMotorTorque = maxMotorTorque;
	hjd.scale = 0.05f;

	hj = HingeJoint(&hjd);
	Physics::GetInstance().hingeJoints.push_back(hj);
	wheelJointIds.push_back(++jid);

	torqueFactor = chassis->GetMass() + 4.0f * wheels[0]->GetMass();
}

void Car_Test::OnKeyPress(GLFWwindow* window)
{
	Simulation::OnKeyPress(window);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		if (motorSpeed < 500.0f)
		{
			motorSpeed += 0.25f;
			for (int i = 0; i < 4; ++i)
			{
				Physics::GetInstance().hingeJoints[wheelJointIds[i]].SetMotorSpeed(motorSpeed);
			}
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		if (motorSpeed > 0.0f)
		{
			motorSpeed -= 2.0f;
			for (int i = 0; i < 4; ++i)
			{
				Physics::GetInstance().hingeJoints[wheelJointIds[i]].SetMotorSpeed(motorSpeed);
			}
		}
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		glm::vec3 fc = (chassis->GetTransform().R) * glm::vec3(1.0f, 0.0f, 0.0f);
		glm::quat q = chassis->GetOrientation();
		q += 0.5f * glm::quat(0.0f, steerFactor * glm::vec3(0.0f, 1.0f, 0.0f)) * q;
		chassis->SetOrientation(q);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		glm::vec3 fc = (chassis->GetTransform().R) * glm::vec3(1.0f, 0.0f, 0.0f);
		glm::quat q = chassis->GetOrientation();
		q += 0.5f * glm::quat(0.0f, -steerFactor * glm::vec3(0.0f, 1.0f, 0.0f)) * q;
		chassis->SetOrientation(q);
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS)
	{
		if (motorSpeed <= 0.0f && motorSpeed > -10.0f)
		{
			motorSpeed -= 1.0f;
			for (int i = 0; i < 4; ++i)
			{
				Physics::GetInstance().hingeJoints[wheelJointIds[i]].SetMotorSpeed(motorSpeed);
			}
		}
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
	{
		Physics::GetInstance().hingeJoints[wheelJointIds[2]].SetMotorSpeed(0.0f);
		Physics::GetInstance().hingeJoints[wheelJointIds[3]].SetMotorSpeed(0.0f);
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_RELEASE)
	{
		Physics::GetInstance().hingeJoints[wheelJointIds[2]].SetMotorSpeed(motorSpeed);
		Physics::GetInstance().hingeJoints[wheelJointIds[3]].SetMotorSpeed(motorSpeed);
	}
}