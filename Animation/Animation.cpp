#pragma once

#include "Animation.h"
#include <glm/gtx/transform.hpp>
#include <GLFW/glfw3.h>
#include <assimp/postprocess.h>
#include "../Graphics/Graphics.h"
#include <Windows.h>

void Animation::Init(const std::string& file, const std::vector<glm::mat4>& offsets,
					 const std::map<std::string, unsigned int>& bMap, const glm::mat4& tx)
{
	scene = importer.ReadFile(file, aiProcess_FlipUVs);
	elapsed = 0.0f;
	boneOffsets = offsets;
	boneMap = bMap;
	invBindTx = tx;
	finalTransforms = std::vector<glm::mat4>(boneOffsets.size());
	t0 = GetTickCount();
}

void Animation::Interpolate(Transform* res, const Transform& start, const Transform& end, const float t)
{
	res->position = (1.0f - t) * start.position + t * end.position;

	glm::quat q1 = glm::quat_cast(start.R);
	glm::quat q2 = glm::quat_cast(end.R);
	glm::quat q = glm::slerp(q1, q2, t);

	res->R = glm::toMat3(q);
}

unsigned int Animation::FindKeyFrameT(float animationTime, const aiNodeAnim* nodeAnim)
{
	assert(nodeAnim->mNumPositionKeys > 0);
	for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
	{
		if (animationTime < (float)nodeAnim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}
	assert(0);
	return 0;
}

unsigned int Animation::FindKeyFrameR(float animationTime, const aiNodeAnim* nodeAnim)
{
	assert(nodeAnim->mNumRotationKeys > 0);
	for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
	{
		if (animationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}
	assert(0);
	return 0;
}

unsigned int Animation::FindKeyFrameS(float animationTime, const aiNodeAnim* nodeAnim)
{
	assert(nodeAnim->mNumScalingKeys > 0);
	for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
	{
		if (animationTime < (float)nodeAnim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}
	assert(0);
	return 0;
}

void Animation::InterpolatePosition(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumPositionKeys == 1) 
	{
		out = nodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int prevFrame = FindKeyFrameT(animationTime, nodeAnim);
	unsigned int nextFrame = prevFrame + 1;
	assert(nextFrame < nodeAnim->mNumPositionKeys);
	float deltaTime = (float)(nodeAnim->mPositionKeys[nextFrame].mTime - nodeAnim->mPositionKeys[prevFrame].mTime);
	float progression = (animationTime - (float)nodeAnim->mPositionKeys[prevFrame].mTime) / deltaTime;
	assert(progression >= 0.0f && progression <= 1.0f);
	if (progression > 0.0f)
	{
		int x = 1;
	}
	const aiVector3D& start = nodeAnim->mPositionKeys[prevFrame].mValue;
	const aiVector3D& end = nodeAnim->mPositionKeys[nextFrame].mValue;
	out = start + progression * (end - start);
	out = (1.0f - progression) * start + progression * end;
}

void Animation::InterpolateRotation(glm::quat& out, float animationTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumRotationKeys == 1)
	{
		aiQuaternion q0 = nodeAnim->mRotationKeys[0].mValue;
		out = glm::quat(q0.w, q0.x, q0.y, q0.z);
		return;
	}

	unsigned int prevFrame = FindKeyFrameR(animationTime, nodeAnim);
	unsigned int nextFrame = (prevFrame + 1);
	assert(nextFrame < nodeAnim->mNumRotationKeys);
	float deltaTime = (float)(nodeAnim->mRotationKeys[nextFrame].mTime - nodeAnim->mRotationKeys[prevFrame].mTime);
	float progression = (animationTime - (float)nodeAnim->mRotationKeys[prevFrame].mTime) / deltaTime;
	assert(progression >= 0.0f && progression <= 1.0f);
	const aiQuaternion& qs = nodeAnim->mRotationKeys[prevFrame].mValue;
	const aiQuaternion& qe = nodeAnim->mRotationKeys[nextFrame].mValue;
	//aiQuaternion::Interpolate(out, start, end, progression);
	glm::quat start(qs.w, qs.x, qs.y, qs.z);
	glm::quat end(qe.w, qe.x, qe.y, qe.z);
	out = glm::slerp(start, end, progression);
	//out = out.Normalize();
}


void Animation::InterpolateScaling(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumScalingKeys == 1) 
	{
		out = nodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int prevFrame = FindKeyFrameS(animationTime, nodeAnim);
	unsigned int nextFrame = (prevFrame + 1);
	assert(nextFrame < nodeAnim->mNumScalingKeys);
	float deltaTime = (float)(nodeAnim->mScalingKeys[nextFrame].mTime - nodeAnim->mScalingKeys[prevFrame].mTime);
	float progression = (animationTime - (float)nodeAnim->mScalingKeys[prevFrame].mTime) / deltaTime;
	assert(progression >= 0.0f && progression <= 1.0f);
	const aiVector3D& start = nodeAnim->mScalingKeys[prevFrame].mValue;
	const aiVector3D& end = nodeAnim->mScalingKeys[nextFrame].mValue;
	aiVector3D delta = end - start;
	out = start + progression * delta;
}

void Animation::CalculatePose(const float animationTime, const aiNode* pNode, const glm::mat4& parentTx)
{
	std::string nodeName(pNode->mName.data);
	const aiAnimation* animation = scene->mAnimations[0];
	glm::mat4 nodeTx;
	AssimpToGlmMat4(pNode->mTransformation, nodeTx);
	const aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName);

	if (nodeAnim)
	{
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D scaling;
		InterpolateScaling(scaling, animationTime, nodeAnim);
		glm::mat4 S = glm::scale(glm::vec3(scaling.x, scaling.y, scaling.z));

		// Interpolate rotation and generate rotation transformation matrix
		//aiQuaternion rot;
		glm::quat rot;
		InterpolateRotation(rot, animationTime, nodeAnim);
		glm::mat4 R = glm::mat4(rot);

		// Interpolate translation and generate translation transformation matrix
		aiVector3D pos;
		InterpolatePosition(pos, animationTime, nodeAnim);
		glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, pos.z));

		// Combine the above transformations
		nodeTx = T * R * S;
	}

	glm::mat4 modelTx = parentTx * nodeTx;

	if (boneMap.find(nodeName) != boneMap.end())
	{
		unsigned int boneIndex = boneMap[nodeName];
		finalTransforms[boneIndex] = invBindTx * modelTx * boneOffsets[boneIndex];
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++)
	{
		CalculatePose(animationTime, pNode->mChildren[i], modelTx);
	}
}

const aiNodeAnim* Animation::FindNodeAnim(const aiAnimation* animation, const std::string& nodeName)
{
	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		const aiNodeAnim* nodeAnim = animation->mChannels[i];

		if (std::string(nodeAnim->mNodeName.data) == nodeName)
		{
			return nodeAnim;
		}
	}
	return nullptr;
}

void Animation::CalculateFinalTransform(const std::vector<Transform>& pose, const unsigned int boneId, const glm::mat4& parentTx)
{
	glm::mat4 localTx = pose[boneId].CombinedTransform();
	glm::mat4 modelTx = parentTx * localTx;

	for (unsigned int child : bones[boneId].children)
	{
		CalculateFinalTransform(pose, child, modelTx);
	}

	finalTransforms[boneId] = modelTx;
}

void Animation::Update()
{
	/*const static float dt = 1.0f / 60.0f;
	const static float duration = (float)scene->mAnimations[0]->mDuration;
	elapsed += dt;
	if (elapsed > duration)
	{
		elapsed -= duration;
	}
	
	float time_sec = (float)((double)GetTickCount() - (double)t0) / 1000.0f;

	float tps = (float)(scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f);
	float elapsedTicks = time_sec * tps;
	float animationTime = fmod(elapsedTicks, (float)scene->mAnimations[0]->mDuration);

	CalculatePose(animationTime, scene->mRootNode, glm::mat4(1.0f));*/

	int N = finalTransforms.size();
	for (int i = 0; i < N; ++i)
	{
		//Graphics::GetInstance().SetBoneTransform(i, finalTransforms[i]);
	}
}

std::vector<glm::mat4>& Animation::GetFinalTransforms()
{
	return finalTransforms;
}

void Animation::AssimpToGlmMat4(const aiMatrix4x4& from, glm::mat4& to)
{
	to[0][0] = (float)from.a1; to[0][1] = (float)from.b1; to[0][2] = (float)from.c1; to[0][3] = (float)from.d1;
	to[1][0] = (float)from.a2; to[1][1] = (float)from.b2; to[1][2] = (float)from.c2; to[1][3] = (float)from.d2;
	to[2][0] = (float)from.a3; to[2][1] = (float)from.b3; to[2][2] = (float)from.c3; to[2][3] = (float)from.d3;
	to[3][0] = (float)from.a4; to[3][1] = (float)from.b4; to[3][2] = (float)from.c4; to[3][3] = (float)from.d4;
}
