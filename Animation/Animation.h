#pragma once

#include <vector>
#include "../Components/Transform.h"

struct Bone
{
	unsigned int id;
	unsigned int parent;
	std::vector<unsigned int> children;

	Bone(const unsigned int id, const unsigned int parent, const std::vector<unsigned int>& children)
		: id(id), parent(parent), children(children)
	{}
};

struct KeyFrame
{
	float timeStamp;
	std::vector<Transform> pose;	
};

class Animation
{
private:
	float duration;
	float elapsed;
	unsigned int nBones;
	std::vector<Bone> bones;

	void Interpolate(Transform* res, const Transform& start, const Transform& end, const float t);

	void CalculatePose(const float elapsed);

	// The transformation from bind pose to current pose
	void CalculateFinalTransform(const std::vector<Transform>& pose, const unsigned int boneId, const glm::mat4& parentTx);

public:
	std::vector<KeyFrame> frames;
	std::vector<glm::mat4> finalTransforms;

	void Init(const std::vector<Bone>& bones, const std::vector<KeyFrame>& frames);

	// boneId  - generate poses for this bone
	// l       - length of the bone
	// angle   - list of angles at specific keyframes
	// nFrames - no.of frames generated for a pair of angles
	void GeneratePoses(unsigned int boneId, float l, std::vector<float> angle, int nFrames);

	void Update();

	std::vector<glm::mat4>& GetFinalTransforms();
};