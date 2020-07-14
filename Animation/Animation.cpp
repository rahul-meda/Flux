#pragma once

#include "Animation.h"

void Animation::Init(const std::vector<Bone>& bones, const std::vector<KeyFrame>& keyFrames)
{
	frames = keyFrames;
	duration = 1.2f;
	elapsed = 0.0f;
	nBones = bones.size();
	this->bones = bones;
	finalTransforms = std::vector<glm::mat4>(nBones);
}

void Animation::Interpolate(Transform* res, const Transform& start, const Transform& end, const float t)
{
	res->position = (1.0f - t) * start.position + t * end.position;

	glm::quat q1 = glm::quat_cast(start.R);
	glm::quat q2 = glm::quat_cast(end.R);
	glm::quat q = glm::slerp(q1, q2, t);

	res->R = glm::toMat3(q);
}

void Animation::CalculatePose(const float elapsed)
{
	KeyFrame prevFrame = frames[0];
	KeyFrame nextFrame = frames[0];
	int nFrames = frames.size();

	for (int i = 0; i < nFrames; ++i)
	{
		nextFrame = frames[i];

		if (elapsed < nextFrame.timeStamp)
		{
			break;
		}

		prevFrame = frames[i];
	}

	float frameDuration = nextFrame.timeStamp - prevFrame.timeStamp;
	float progression = elapsed - prevFrame.timeStamp;
	progression /= frameDuration;

	std::vector<Transform> pose(nBones);
	for (int i = 0; i < nBones; ++i)
	{
		Transform prev = prevFrame.pose[i];
		Transform next = nextFrame.pose[i];

		Interpolate(&pose[i], prev, next, progression);
	}

	CalculateFinalTransform(pose, 0, glm::mat4(1.0f));
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
	const static float dt = 1.0f / 60.0f;

	elapsed += dt;

	if (elapsed >= duration)
	{
		elapsed -= duration;
	}

	CalculatePose(elapsed);
}

std::vector<glm::mat4>& Animation::GetFinalTransforms()
{
	return finalTransforms;
}
