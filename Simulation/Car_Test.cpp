
#include "Car_Test.h"
#include "../Mesh/ObjParser.h"
#include "../Graphics/Graphics.h"
#include "../Mesh/Geometry.h"
#include "../Components/Body.h"
#include "../Physics/Physics.h"
#include "../Physics/HullCollider.h"
#include "../Physics/SphereCollider.h"
#include "../Physics/CapsuleCollider.h"
#include "../Physics/Contact.h"

void Car_Test::Init(GLFWwindow* window, int width, int height)
{
	Simulation::Init(window, width, height);

	fwd = up = right = glm::vec3(0.0f);
	grip = 2.0f;
	turning = false;
	drag = 10.0f;
	steerFactor = 0.25f;

	HMesh mesh;

	unsigned int boxDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_df.png");
	unsigned int boxSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/container2_sp.png");
	unsigned int ballTxt = Graphics::GetInstance().CreateTexture("resources/textures/earth_10k.jpg");
	unsigned int scifiTxt = Graphics::GetInstance().CreateTexture("resources/textures/sci_fi1.jpg");
	unsigned int wheelDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/leather1.jpg");
	unsigned int wheelEmTxt = Graphics::GetInstance().CreateTexture("resources/textures/dragon1_em.jpg");
	unsigned int bodyDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/carbon_fiber3_df.jpg");
	unsigned int bodySpTxt = Graphics::GetInstance().CreateTexture("resources/textures/carbon_fiber3_sp.jpg");
	unsigned int spoilerDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal21.jpg");
	unsigned int spoilerSpTxt = Graphics::GetInstance().CreateTexture("resources/textures/metal21.jpg");
	unsigned int spoilerEmTxt = Graphics::GetInstance().CreateTexture("resources/textures/border3_em.jpg");
	unsigned int doorEmTxt = Graphics::GetInstance().CreateTexture("resources/textures/flames5_em.jpg");
	unsigned int engineDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/carbon_fiber4_df.jpg");
	unsigned int engineEmTxt = Graphics::GetInstance().CreateTexture("resources/textures/lightning2_em.jpg");
	unsigned int hoodEmTxt = Graphics::GetInstance().CreateTexture("resources/textures/hood5_em.jpg");
	unsigned int windowEmTxt = Graphics::GetInstance().CreateTexture("resources/textures/border8_em.jpg");
	unsigned int bonetEmTxt = Graphics::GetInstance().CreateTexture("resources/textures/flames4_em.jpg");
	unsigned int wolfDfTxt = Graphics::GetInstance().CreateTexture("resources/textures/witcher3_medallion.jpg");
	unsigned int batEmTxt = Graphics::GetInstance().CreateTexture("resources/textures/batman1.jpg");

	Material material;
	material.diffuseMap = wheelDfTxt;
	material.specularMap = boxSpTxt;
	material.emissionMap = wheelEmTxt;
	material.nMaps = 3;

	Transform tx;
	BodyDef bd;
	unsigned int bID = 0;
	std::vector<unsigned int> bids;
	HullCollider* hull;
	SphereCollider* sphereCollider;
	CapsuleCollider* capsuleCollider;
	R_Mesh obj;
	std::vector<glm::vec3> anchors;
	glm::vec3 sw(0.9f, 0.3f, 0.9f);		// wheel size
	glm::vec3 sc(5.0f, 0.3f, 2.0f);		// chassis size
	float gapH = 0.1f;					// gap b/w chassis and wheel
	float gapV = 0.5f;					// vertical gap b/w chassis and ground
	float dw = 200.0f;					// wheel density
	float dc = 20.0f;					// chassis density
	float er = 0.0f;					// coeff of restitution
	motorSpeed = 0.0f;
	maxMotorTorque = 9999.0f;

	// front left wheel
	glm::vec3 p(sc.x - gapH - sw.x , sw.x, -sc.z - gapH - sw.y);
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
	hull->restitution = er;
	hull->massData->density = dw;
	sphereCollider = new SphereCollider();
	sphereCollider->Scale(sw.x);
	sphereCollider->friction = 0.9f;
	sphereCollider->restitution = 0.1f;
	sphereCollider->massData->density = dw;
	Physics::GetInstance().AddCollider(bID, sphereCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(sw);
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	bids.push_back(bID);
	anchors.push_back(p); 

	// front right wheel
	p = glm::vec3(sc.x - gapH - sw.x, sw.x, sc.z + gapH + sw.y);
	tx = Transform(p, glm::angleAxis(-PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	mesh.GetColliderData(hull);
	hull->Scale(sw);
	hull->friction = 0.9f;
	hull->restitution = er;
	hull->massData->density = dw;
	//Physics::GetInstance().AddCollider(bID, hull);
	sphereCollider = new SphereCollider();
	sphereCollider->Scale(sw.x);
	sphereCollider->friction = 0.9f;
	sphereCollider->restitution = 0.1f;
	sphereCollider->massData->density = dw;
	Physics::GetInstance().AddCollider(bID, sphereCollider);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(sw);
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	bids.push_back(bID);
	anchors.push_back(p);

	// rear left wheel
	p = glm::vec3(-sc.x + gapH + sw.x, sw.x, -sc.z - gapH - sw.y);
	tx = Transform(p, glm::angleAxis(-PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	mesh.GetColliderData(hull);
	hull->Scale(sw);
	hull->friction = 0.999f;
	hull->restitution = er;
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
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	bids.push_back(bID);
	anchors.push_back(p);

	// rear right wheel
	p = glm::vec3(-sc.x + gapH + sw.x, sw.x, sc.z + gapH + sw.y);
	tx = Transform(p, glm::angleAxis(-PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)));
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	mesh.GetColliderData(hull);
	hull->Scale(sw);
	hull->friction = 0.999f;
	hull->restitution = er;
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
	obj.LoadModel("resources/models/cylinder/cylinder.obj");
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	bids.push_back(bID);
	anchors.push_back(p);

	material.diffuseMap  = bodyDfTxt;
	material.specularMap = bodySpTxt;
	material.emissionMap = windowEmTxt;
	material.nMaps = 3;

	// chassis
	p = glm::vec3(0.0f, gapV + sc.y, 0.0f);
	tx = Transform(p);
	bd.tx = tx;
	bd.isStatic = false;
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	ParseObj("Resources/Models/Box.obj", mesh);
	mesh.GetColliderData(hull);
	hull->Scale(sc);
	hull->massData->density = dc;
	hull->restitution = er;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(sc);
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	material.diffuseMap  = bodyDfTxt;
	material.specularMap = bodySpTxt;
	material.emissionMap = engineEmTxt;
	material.nMaps = 3;

	p = glm::vec3(0.0f, 1.0f, 0.0f);
	tx = Transform(p);
	hull = new HullCollider();
	hull->txB.position = p;
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(0.5f));
	hull->massData->density = dc;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(tx.position);
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.5f));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	p = glm::vec3(0.0f, 0.9f, -sc.z - gapH);
	tx = Transform(p);
	hull = new HullCollider();
	hull->txB.position = p;
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(3.0f, 0.6f, 0.1f));
	hull->massData->density = dc;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(tx.position);
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(3.0f, 0.6f, 0.1f));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	p = glm::vec3(0.0f, 0.9f, sc.z + gapH);
	tx = Transform(p);
	hull = new HullCollider();
	hull->txB.position = p;
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(3.0f, 0.6f, 0.1f));
	hull->massData->density = dc;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(tx.position);
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(3.0f, 0.6f, 0.1f));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	material.diffuseMap = bodyDfTxt;
	material.specularMap = bodySpTxt;
	material.emissionMap = bonetEmTxt;
	material.nMaps = 3;

	p = glm::vec3(sc.x + 0.1f, 0.9f, 0.0f);
	tx = Transform(p);
	hull = new HullCollider();
	hull->txB.position = p;
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(0.3f, 0.6f, sc.z));
	hull->massData->density = dc;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(tx.position);
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.3f, 0.6f, sc.z));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	material.diffuseMap = bodyDfTxt;
	material.specularMap = bodySpTxt;
	material.emissionMap = bonetEmTxt;
	material.nMaps = 3;

	p = glm::vec3(-sc.x - 0.2f, 0.9f, 0.0f);
	tx = Transform(p);
	hull = new HullCollider();
	hull->txB.position = p;
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(0.5f, 0.6f, sc.z));
	hull->massData->density = dc;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(tx.position);
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.5f, 0.6f, sc.z));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	p = glm::vec3(-sc.x - 0.4f, 1.6f, 1.75f);
	tx = Transform(p, glm::angleAxis(0.25f * PI, glm::vec3(0.0f, 0.0f, 1.0f)));
	hull = new HullCollider();
	hull->txB.position = p;
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(0.05f, 0.5f, 0.05));
	hull->massData->density = dc;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(tx.position);
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.05f, 0.5f, 0.05));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	p = glm::vec3(-sc.x - 0.4f, 1.6f, -1.75f);
	tx = Transform(p, glm::angleAxis(0.25f * PI, glm::vec3(0.0f, 0.0f, 1.0f)));
	hull = new HullCollider();
	hull->txB.position = p;
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(0.05f, 0.5f, 0.05));
	hull->massData->density = dc;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(tx.position);
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.05f, 0.5f, 0.05));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	material.diffuseMap  = spoilerDfTxt;
	material.specularMap = spoilerSpTxt;
	material.emissionMap = spoilerEmTxt;
	material.nMaps = 3;

	p = glm::vec3(-sc.x - 1.2f, 2.0f, 0.0f);
	tx = Transform(p);
	hull = new HullCollider();
	hull->txB.position = p;
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(0.5f, 0.05f, 2.0f));
	hull->massData->density = dc;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(tx.position);
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.5f, 0.05f, 2.0f));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	material.diffuseMap  = bodyDfTxt;
	material.specularMap = bodySpTxt;
	material.emissionMap = hoodEmTxt;
	material.nMaps = 3;

	p = glm::vec3(3.0f, 1.4f, 0.0f);
	tx = Transform(p);
	hull = new HullCollider();
	hull->txB.position = p;
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(2.0f, 0.097f, 1.97f));
	hull->massData->density = dc;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(tx.position);
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(2.0f, 0.097f, 1.97f));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	material.diffuseMap = bodyDfTxt;
	material.specularMap = bodySpTxt;
	material.emissionMap = spoilerEmTxt;
	material.nMaps = 3;

	p = glm::vec3(0.5f, 1.9f, 0.0f);
	tx = Transform(p, glm::angleAxis(-0.25f * PI, glm::vec3(0.0f, 0.0f, 1.0f)));
	hull = new HullCollider();
	hull->txB.position = p;
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(0.707f, 0.1f, 2.0f));
	hull->massData->density = dc;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(tx.position);
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(0.707f, 0.1f, 2.0f));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	material.diffuseMap = bodyDfTxt;
	material.specularMap = bodySpTxt;
	material.emissionMap = bonetEmTxt;
	material.nMaps = 3;

	p = glm::vec3(-0.9f, 2.4f, 0.0f);
	tx = Transform(p);
	hull = new HullCollider();
	hull->txB.position = p;
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(1.0f, 0.1f, 2.0f));
	hull->massData->density = dc;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(tx.position);
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(1.0f, 0.1f, 2.0f));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	material.diffuseMap = bodyDfTxt;
	material.specularMap = bodySpTxt;
	material.emissionMap = spoilerEmTxt;
	material.nMaps = 3;

	p = glm::vec3(-3.5f, 1.854f, 0.0f);
	tx = Transform(p, glm::angleAxis(0.1f * PI, glm::vec3(0.0f, 0.0f, 1.0f)));
	hull = new HullCollider();
	hull->txB.position = p;
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(1.732f, 0.1f, 1.97f));
	hull->massData->density = dc;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(tx.position);
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(1.732f, 0.1f, 1.97f));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	material.diffuseMap = bodyDfTxt;
	material.specularMap = bodySpTxt;
	material.emissionMap = engineEmTxt;
	material.nMaps = 3;

	p = glm::vec3(-sc.x + gapH + sw.x, sw.x, -sc.z - gapH + 0.1f);
	tx = Transform(p, glm::angleAxis(0.15f, glm::vec3(0.0f, 1.0f, 0.0f)));
	hull = new HullCollider();
	hull->txB.position = p;
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(1.0f, 0.6f, 0.1f));
	hull->massData->density = dc;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(tx.position);
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(1.0f, 0.6f, 0.1f));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	p = glm::vec3(-sc.x + gapH + sw.x, sw.x, sc.z + gapH - 0.1f);
	tx = Transform(p, glm::angleAxis(-0.15f, glm::vec3(0.0f, 1.0f, 0.0f)));
	hull = new HullCollider();
	hull->txB.position = p;
	hull->txB.R = tx.R;
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(1.0f, 0.57f, 0.1f));
	hull->massData->density = dc;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.posOffsets.push_back(tx.position);
	obj.rotOffsets.push_back(tx.R);
	obj.scales.push_back(glm::vec3(1.0f, 0.57f, 0.1f));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);

	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();
	bids.push_back(bID);

	std::vector<Body*>* bodies = &Physics::GetInstance().bodies;

	// connect front wheels to chassis using position joints
	wheels.push_back((*bodies)[bids[0]]);
	chassis = (*bodies)[bids[4]];

	glm::vec3 axisW = glm::vec3(0.0f, 0.0f, 1.0f);
	float hz = 40.0f;
	float zeta = 0.7f;

	WheelJointDef wjd;
	wjd.Initialize(chassis, wheels[0], anchors[0], axisW);
	wjd.frequencyHz = hz;
	wjd.dampingRatio = zeta;
	wjd.enableMotor = true;
	wjd.motorSpeed = motorSpeed;
	wjd.maxMotorTorque = maxMotorTorque;
	WheelJoint wj(&wjd);
	//Physics::GetInstance().wheelJoints.push_back(wj);

	glm::vec3 off1(0.0f, 0.9f, 0.0f);
	glm::vec3 off2(0.0f, 0.0f, 0.0f);

	SpringDef sd;
	sd.l0 = 0.8f;
	sd.w = 1.0f;
	sd.c = 0.9f;
	sd.Init(chassis, wheels[0], anchors[0] + off1, anchors[0] + off2);
	Spring spring;
	spring.Init(&sd);
	Physics::GetInstance().springs.push_back(spring);

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

	wjd.Initialize(chassis, wheels[1], anchors[1], axisW);
	wjd.frequencyHz = hz;
	wjd.dampingRatio = zeta;
	wjd.enableMotor = true;
	wjd.motorSpeed = motorSpeed;
	wjd.maxMotorTorque = maxMotorTorque;
	wj = WheelJoint(&wjd);
	//Physics::GetInstance().wheelJoints.push_back(wj);

	sd.Init(chassis, wheels[1], anchors[1] + off1, anchors[1] + off2);
	spring.Init(&sd);
	Physics::GetInstance().springs.push_back(spring);

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

	wjd.Initialize(chassis, wheels[2], anchors[2], axisW);
	wjd.frequencyHz = hz;
	wjd.dampingRatio = zeta;
	wjd.enableMotor = true;
	wjd.motorSpeed = motorSpeed;
	wjd.maxMotorTorque = maxMotorTorque;
	wj = WheelJoint(&wjd);
	//Physics::GetInstance().wheelJoints.push_back(wj);

	sd.Init(chassis, wheels[2], anchors[2] + off1, anchors[2] + off2);
	spring.Init(&sd);
	Physics::GetInstance().springs.push_back(spring);

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

	wjd.Initialize(chassis, wheels[3], anchors[3], axisW);
	wjd.frequencyHz = hz;
	wjd.dampingRatio = zeta;
	wjd.enableMotor = true;
	wjd.motorSpeed = motorSpeed;
	wjd.maxMotorTorque = maxMotorTorque;
	wj = WheelJoint(&wjd);
	//Physics::GetInstance().wheelJoints.push_back(wj);

	sd.Init(chassis, wheels[3], anchors[3] + off1, anchors[3] + off2);
	spring.Init(&sd);
	Physics::GetInstance().springs.push_back(spring);

	hjd.Initialize(wheels[3], chassis, anchors[3], glm::vec3(0.0f, 0.0f, -1.0f));
	hjd.enableLimit = false;
	hjd.enableMotor = true;
	hjd.motorSpeed = motorSpeed;
	hjd.maxMotorTorque = maxMotorTorque;
	hjd.scale = 0.05f;

	hj = HingeJoint(&hjd);
	Physics::GetInstance().hingeJoints.push_back(hj);
	wheelJointIds.push_back(++jid);

	mass = chassis->GetMass() + 4.0f * wheels[0]->GetMass();

	p = glm::vec3(10.0f, 0.0f, 0.0f);
	tx = Transform(p, glm::angleAxis(0.5f, glm::vec3(0.0f, 0.0f, 1.0f)));
	bd.tx = tx;
	bd.isStatic = true;
	bd.filterID = 0;
	bID = Physics::GetInstance().AddBody(bd);
	hull = new HullCollider();
	mesh.GetColliderData(hull);
	hull->Scale(glm::vec3(10.0f, 0.25f, 10.0f));
	hull->massData->density = dc;
	hull->restitution = 0.1f;
	Physics::GetInstance().AddCollider(bID, hull);
	obj.pos = tx.position;
	obj.rot = tx.R;
	obj.posOffsets.push_back(glm::vec3(0.0f));
	obj.rotOffsets.push_back(glm::mat3(1.0f));
	obj.scales.push_back(glm::vec3(10.0f, 0.25f, 10.0f));
	obj.LoadModel("resources/models/box/box.obj");
	obj.materials.push_back(material);
	Graphics::GetInstance().objects.push_back(obj);
	obj.Clear();

	camera.body = chassis;

	glm::vec3 s(0.1f, 2.5f, 10.0f);
	int N = 20;
	float a = 0.5f * PI / N;
	float h = s.x / tanf(0.5f * a);
	float r = 25.0f;
	for (int i = 0; i < 4 * N; ++i)
	{
		p = glm::vec3(r * cosf(a * (float)i), r * sinf(a * (float)i) + r - 0.4f,  50.0f);
		tx = Transform(p, glm::angleAxis(a * (float)i, glm::vec3(0.0f, 0.0f, 1.0f)));
		bd.tx = tx;
		bd.isStatic = true;
		bd.filterID = 0;
		bID = Physics::GetInstance().AddBody(bd);
		hull = new HullCollider();
		mesh.GetColliderData(hull);
		hull->Scale(s);
		hull->massData->density = 1.0f;
		hull->restitution = 0.1f;
		Physics::GetInstance().AddCollider(bID, hull);
		obj.pos = tx.position;
		obj.rot = tx.R;
		obj.posOffsets.push_back(glm::vec3(0.0f));
		obj.rotOffsets.push_back(glm::mat3(1.0f));
		obj.scales.push_back(s);
		obj.LoadModel("resources/models/box/box.obj");
		obj.materials.push_back(material);
		Graphics::GetInstance().objects.push_back(obj);
		obj.Clear();
	}

	N = 8;
	for (int i = 0; i < N; ++i)
	{
		p = glm::vec3(-400.0f + 100.0f * i, 1.0f, -50.0f);
		tx = Transform(p);
		bd.tx = tx;
		bd.isStatic = true;
		bd.filterID = 0;
		bID = Physics::GetInstance().AddBody(bd);
		hull = new HullCollider();
		mesh.GetColliderData(hull);
		hull->Scale(glm::vec3(0.1f, 1.0f, 0.1f));
		hull->massData->density = 1.0f;
		hull->restitution = 0.1f;
		Physics::GetInstance().AddCollider(bID, hull);
		obj.pos = tx.position;
		obj.rot = tx.R;
		obj.posOffsets.push_back(glm::vec3(0.0f));
		obj.rotOffsets.push_back(glm::mat3(1.0f));
		obj.scales.push_back(glm::vec3(0.1f, 1.0f, 0.1f));
		obj.LoadModel("resources/models/box/box.obj");
		obj.materials.push_back(material);
		Graphics::GetInstance().objects.push_back(obj);
		obj.Clear();
	}

	N = 8;
	for (int i = 0; i < N; ++i)
	{
		p = glm::vec3(2.0f * (float)i, -0.1f, -100.0f);
		glm::quat q = glm::angleAxis(0.5f * PI, glm::vec3(1.0f, 0.0f, 0.0f));
		tx = Transform(p, q);
		bd.tx = tx;
		bd.isStatic = true;
		bID = Physics::GetInstance().AddBody(bd);
		CapsuleCollider* capsuleCollider = new CapsuleCollider();
		capsuleCollider->Scale(0.5f);
		capsuleCollider->H = 4.0f;
		capsuleCollider->upB = glm::vec3(0.0f, 1.0f, 0.0f);
		capsuleCollider->restitution = 0.0f;
		Physics::GetInstance().AddCollider(bID, capsuleCollider);
		obj.pos = tx.position;
		obj.rot = tx.R;
		obj.posOffsets.push_back(glm::vec3(0.0f));
		obj.rotOffsets.push_back(glm::mat3(1.0f));
		obj.scales.push_back(glm::vec3(0.5f, 4.0f, 0.5f));
		obj.LoadModel("resources/models/cylinder/cylinder.obj");
		obj.materials.push_back(material);
		obj.posOffsets.push_back(glm::vec3(0.0f, 4.0f, 0.0f));
		obj.rotOffsets.push_back(tx.R);
		obj.scales.push_back(glm::vec3(0.5f));
		obj.LoadModel("resources/models/sphere/sphere.obj"); 
		obj.materials.push_back(material);
		obj.posOffsets.push_back(glm::vec3(0.0f, -4.0f, 0.0f));
		obj.rotOffsets.push_back(tx.R);
		obj.scales.push_back(glm::vec3(0.5f));
		obj.LoadModel("resources/models/sphere/sphere.obj");
		obj.materials.push_back(material);
		Graphics::GetInstance().objects.push_back(obj);
		obj.Clear();
	}
}

void Car_Test::OnKeyTap(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	Simulation::OnKeyTap(window, key, scanCode, action, mods);

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		camera.follow = true;
		camera.body = chassis;
	}
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		drag += 10.0f;
		if (drag > 30.0f)
			drag = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		grip += 1.0f;
		if (grip > 5.0f)
			grip = 5.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		grip -= 1.0f;
		if (grip < 1.0f)
			grip = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		steerFactor += 0.05f;
		if (steerFactor > 0.3f)
			steerFactor = 0.3f;
	}
	else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		steerFactor -= 0.05f;
		if (steerFactor < 0.1f)
			steerFactor = 0.1f;
	}
	if(key == GLFW_KEY_RIGHT_CONTROL && action == GLFW_RELEASE)
	{
		Physics::GetInstance().hingeJoints[wheelJointIds[2]].SetMotorSpeed(motorSpeed);
		Physics::GetInstance().hingeJoints[wheelJointIds[3]].SetMotorSpeed(motorSpeed);
	}
	else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE)
	{
		Physics::GetInstance().hingeJoints[wheelJointIds[0]].SetMotorSpeed(motorSpeed);
		Physics::GetInstance().hingeJoints[wheelJointIds[1]].SetMotorSpeed(motorSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		motorSpeed = 0.0f;
	}
}

void Car_Test::OnKeyPress(GLFWwindow* window)
{
	Simulation::OnKeyPress(window);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		if (motorSpeed < 100.0f)
		{
			motorSpeed += 0.5f;
			for (int i = 0; i < 4; ++i)
			{
				Physics::GetInstance().hingeJoints[wheelJointIds[i]].SetMotorSpeed(motorSpeed);
				//Physics::GetInstance().wheelJoints[wheelJointIds[i]].SetMotorSpeed(motorSpeed);
			}
		}
		chassis->ApplyForceCOM(50.0f * mass * glm::vec3(fwd));
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		if(motorSpeed > 2.0f)
		{
			motorSpeed -= 2.0f;
			for (int i = 0; i < 4; ++i)
			{
				Physics::GetInstance().hingeJoints[wheelJointIds[i]].SetMotorSpeed(motorSpeed);
				//Physics::GetInstance().wheelJoints[wheelJointIds[i]].SetMotorSpeed(motorSpeed);
			}
			
			chassis->ApplyForceCOM(50.0f * mass * glm::vec3(-fwd));
			chassis->ApplyForceCOM(-10.0f * mass * up);
		}
		else
		{
			motorSpeed = 0.0f;
			for (int i = 0; i < 4; ++i)
			{
				Physics::GetInstance().hingeJoints[wheelJointIds[i]].SetMotorSpeed(motorSpeed);
				//Physics::GetInstance().wheelJoints[wheelJointIds[i]].SetMotorSpeed(motorSpeed);
			}
		}
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		glm::quat q = chassis->GetOrientation();
		q += 0.5f * glm::quat(0.0f, 0.1f * up) * q;
		q = wheels[0]->GetOrientation();
		q += 0.5f * glm::quat(0.0f, 0.1f * up) * q;
		wheels[0]->SetOrientation(q);
		q = wheels[1]->GetOrientation();
		q += 0.5f * glm::quat(0.0f, 0.1f * up) * q;
		wheels[1]->SetOrientation(q);

		chassis->ApplyTorque(100.0f * steerFactor * mass * up);

		chassis->ApplyTorque(-10.0f * mass * fwd);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		glm::quat q = chassis->GetOrientation();
		q += 0.5f * glm::quat(0.0f, -0.1f * up) * q;
		q = wheels[0]->GetOrientation();
		q += 0.5f * glm::quat(0.0f, -0.1f * up) * q;
		wheels[0]->SetOrientation(q);
		q = wheels[1]->GetOrientation();
		q += 0.5f * glm::quat(0.0f, -0.1f * up) * q;
		wheels[1]->SetOrientation(q);

		chassis->ApplyTorque(-100.0f * steerFactor * mass * up);

		chassis->ApplyTorque(10.0f * mass * fwd);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
	{
		Physics::GetInstance().hingeJoints[wheelJointIds[2]].SetMotorSpeed(0.0f);
		Physics::GetInstance().hingeJoints[wheelJointIds[3]].SetMotorSpeed(0.0f);
		//Physics::GetInstance().wheelJoints[wheelJointIds[2]].SetMotorSpeed(0.0f);
		//Physics::GetInstance().wheelJoints[wheelJointIds[3]].SetMotorSpeed(0.0f);

		chassis->ApplyForceCOM(-10.0f * mass * up);
		chassis->ApplyForceCOM(-50.0f * mass * fwd);
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Physics::GetInstance().hingeJoints[wheelJointIds[0]].SetMotorSpeed(0.0f);
		Physics::GetInstance().hingeJoints[wheelJointIds[1]].SetMotorSpeed(0.0f);
		//Physics::GetInstance().wheelJoints[wheelJointIds[0]].SetMotorSpeed(0.0f);
		//Physics::GetInstance().wheelJoints[wheelJointIds[1]].SetMotorSpeed(0.0f);

		chassis->ApplyForceCOM(-10.0f * mass * up);
		chassis->ApplyForceCOM(-50.0f * mass * fwd);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS)
	{
		if (motorSpeed <= 0.0f && motorSpeed > -20.0f)
		{
			motorSpeed -= 1.0f;
			for (int i = 0; i < 4; ++i)
			{
				Physics::GetInstance().hingeJoints[wheelJointIds[i]].SetMotorSpeed(motorSpeed);
				//Physics::GetInstance().wheelJoints[wheelJointIds[i]].SetMotorSpeed(motorSpeed);
			}
			chassis->ApplyForceCOM(-20.0f * mass * glm::vec3(fwd));
		}
	}
	 if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
	{
		chassis->SetAngularVelocity(glm::vec3(0.0f));
		chassis->ApplyTorque(-100.0f * mass * fwd);
	}
	 if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS)
	{
		chassis->ApplyTorque(100.0f * mass * fwd);
	}
	 if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
	{
		chassis->ApplyTorque(200.0f * mass * right);
	}
	 if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
	{
		chassis->ApplyTorque(-200.0f * mass * right);
	}
}

bool Car_Test::IsGripping()
{
	auto contacts = Physics::GetInstance().contactManager.contacts;
	for (auto c : contacts)
	{
		if (c->GetColliderA()->body->filterID == 1 || c->GetColliderB()->body->filterID == 1)
			return true;
	}
	return false;
}

void Car_Test::Update(GLFWwindow* window)
{
	Simulation::Update(window);

	txC = camera.body->GetTransform();
	glm::mat3 R = txC.R;
	fwd = R * glm::vec3(1.0f, 0.0f, 0.0f);
	up = R * glm::vec3(0.0f, 1.0f, 0.0f);
	right = R * glm::vec3(0.0f, 0.0f, 1.0f);

	if (!Physics::GetInstance().pause && IsGripping())
	{
		chassis->ApplyForceCOM(-drag * mass * up);

		glm::vec3 c = chassis->GetCentroid();
		c -= Physics::GetInstance().minSep;
		chassis->SetCentroid(c);

		glm::vec3 vc = chassis->GetVelocity();
		float slide = glm::dot(vc, right);
		chassis->ApplyForceCOM(-slide * grip * mass * right);
	}

	if (camera.follow)
	{
		camera.fwd = fwd;
		camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
		camera.position = 0.0f * prevPos + (1.0f) * (R * glm::vec3(-15.0f, 5.0f, 0.0f) + txC.position);
		camera.target = camera.position + camera.fwd;
	}

	prevPos = camera.position;
}