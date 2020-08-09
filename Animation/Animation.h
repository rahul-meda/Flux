#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h> 
#include <vector>
#include <map>
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
	Assimp::Importer importer;
	const aiScene* scene;
	float duration;
	float tps;
	float elapsed;
	float t0;
	unsigned int nBones;
	std::vector<Bone> bones;
	std::vector<glm::mat4> boneOffsets;
	std::map<std::string, unsigned int> boneMap;
	glm::mat4 invBindTx;

	void Interpolate(Transform* res, const Transform& start, const Transform& end, const float t);

	unsigned int FindKeyFrameT(float animationTime, const aiNodeAnim* nodeAnim);
	unsigned int FindKeyFrameR(float animationTime, const aiNodeAnim* nodeAnim);
	unsigned int FindKeyFrameS(float animationTime, const aiNodeAnim* nodeAnim);

	void InterpolatePosition(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);
	void InterpolateRotation(glm::quat& out, float animationTime, const aiNodeAnim* nodeAnim);
	void InterpolateScaling(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);

	const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);
	void AssimpToGlmMat4(const aiMatrix4x4& from, glm::mat4& to);

	void CalculatePose(const float elapsed, const aiNode* pNode, const glm::mat4& parentTx);

	// The transformation from bind pose to current pose
	void CalculateFinalTransform(const std::vector<Transform>& pose, const unsigned int boneId, const glm::mat4& parentTx);

public:
	std::vector<KeyFrame> frames;
	std::vector<glm::mat4> finalTransforms;
	int animIndex;

	void Init(const std::string& file, const std::vector<glm::mat4>& offsets, 
		      const std::map<std::string, unsigned int>& bMap, const glm::mat4& tx);

	int GetAnimIndex() const;
	void SetAnimIndex(const int i);

	void Update();

	std::vector<glm::mat4>& GetFinalTransforms();
};