/***********************************************
 * File: Animation.cpp
 *
 * Author: LYG
 * Date: Ê®Ò»ÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/

#include "Animation.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Functional.h"
#include "Timer.h"
#include "SceneNode.h"
#include "Scene.h"

Animation::Animation(std::string Name)
{
	mAnimationType = Animation_Max;
	mName = Name;
	mAnimationLength = 0.0f;
	mCurrentTime = 0.0f;
	mIsEnd = false;
	mIsLoop = false;
	mIsAutoDestroy = false;
	mIsPause = false;
}

Animation::~Animation()
{

}

void Animation::SetAnimationType(AnimationType AT)
{
	mAnimationType = AT;
}

AnimationType Animation::GetAnimationType() const
{
	return mAnimationType;
}

std::string Animation::GetName() const
{
	return mName;
}

void Animation::SetAnimationLength(float AniLength)
{
	mAnimationLength = AniLength;
}

float Animation::GetAnimationLength() const
{
	return mAnimationLength;
}

void Animation::SetIsLoop(bool IsLoop)
{
	mIsLoop = IsLoop;
}

bool Animation::GetIsLoop() const
{
	return mIsLoop;
}

void Animation::SetPause(bool IsPause)
{
	mIsPause = IsPause;
}

bool Animation::GetIsPause() const
{
	return mIsPause;
}

bool Animation::IsEnd() const
{
	return mIsEnd;
}

void Animation::SetIsAutoDestroy(bool IsAutoDestroy)
{
	mIsAutoDestroy = IsAutoDestroy;
}

bool Animation::IsAutoDestroy() const
{
	return mIsAutoDestroy;
}

//-----------------------------------------------------------------------
NodeAnimation::NodeAnimation(std::string Name)
	: Animation(Name)
{
	mAnimationType = Animation_Node;
	mAttachNode = nullptr;
	mSpline = new SimpleSpline;
}

NodeAnimation::~NodeAnimation()
{
	SAFE_DELETE(mSpline);
}

void NodeAnimation::AddPoint(Vector3 Pos, Quaternion Rotate, Vector3 Scale, float fTime)
{
	mAnimationLength = fTime;

	mSpline->addPoint(Pos);
	mRotations.push_back(Rotate);
	mScales.push_back(Scale);
	mTimes.push_back(fTime);
}

void NodeAnimation::AddRoundPoint(float fTime)
{
	Vector3 Pos = mSpline->getPoint(0);
	Quaternion Rotate = mRotations[0];
	Vector3 Scale = mScales[0];
	AddPoint(Pos, Rotate, Scale, fTime);
}

void NodeAnimation::GetPoint(Vector3& Pos, Quaternion& Rotate, Vector3& Scale, float& fTime, int Index) const
{
	if (Index >= (int)mRotations.size())
	{
		return;
	}
	Pos = mSpline->getPoint(Index);
	Rotate = mRotations[Index];
	Scale = mScales[Index];
	fTime = mTimes[Index];
	return;
}

void NodeAnimation::AttachNode(SceneNode* Node)
{
	mAttachNode = Node;
}

SceneNode* NodeAnimation::GetAttachNode() const
{
	return mAttachNode;
}

void NodeAnimation::Update()
{
	if (mIsPause) return;
	if (mAttachNode == nullptr) return;
	mCurrentTime += Timer::GetInstance()->GetDeltaFloat();
	if (mCurrentTime >= mAnimationLength)
	{
		if (mIsLoop)
		{
			mCurrentTime -= mAnimationLength;
		}
		else
		{
			mIsEnd = true;
			mCurrentTime = mAnimationLength;
		}
	}
	size_t Index = 0;
	float Length = 0.0f;
	float TimeInterpolate = 0.0f;
	for (size_t i = 0; i < mTimes.size() - 1; i++)
	{
		if (mTimes[i] <= mCurrentTime && mCurrentTime <= mTimes[i + 1])
		{
			Index = i;
			Length = mTimes[i + 1] - mTimes[i];
			TimeInterpolate = mCurrentTime - mTimes[i];
			break;
		}
	}
	float t = TimeInterpolate / Length;
	Vector3 Pos = mSpline->interpolate(int(Index), t);

	Quaternion q1, q2;
	q1 = mRotations[Index];
	q2 = mRotations[Index + 1];
	Quaternion Rot = Quaternion::Slerp(t, q1, q2);
	Vector3 s1, s2;
	s1 = mScales[Index];
	s2 = mScales[Index + 1];

	Vector3 Scale = s1 + (s2 - s1) * t;

	mAttachNode->SetPosition(Pos);
	mAttachNode->SetRotation(Rot);
	mAttachNode->SetScale(Scale);
}
//-----------------------------------------------------------------------
FlowingTextAnimation::FlowingTextAnimation(std::string Name)
	: Animation(Name)
{
	mAttachNode = nullptr;
	mAcceleration = 1.0f;
	mStartPosition = Vector3::ZERO;
}

FlowingTextAnimation::~FlowingTextAnimation()
{

}

void FlowingTextAnimation::AttachNode(SceneNode* Node)
{
	mAttachNode = Node;
	mStartPosition = Node->GetWorldPosition();
}

SceneNode* FlowingTextAnimation::GetAttachNode() const
{
	return mAttachNode;
}

void FlowingTextAnimation::SetAcceleration(float Acceleration)
{
	mAcceleration = Acceleration;
}

float FlowingTextAnimation::GetAcceleration() const
{
	return mAcceleration;
}

void FlowingTextAnimation::Update()
{
	if (mIsPause) return;
	if (mAttachNode == nullptr) return;
	mCurrentTime += Timer::GetInstance()->GetDeltaFloat();
	if (mCurrentTime >= mAnimationLength)
	{
		if (mIsLoop)
		{
			mCurrentTime -= mAnimationLength;
		}
		else
		{
			mIsEnd = true;
			mCurrentTime = mAnimationLength;
		}
	}
	// Formula: s = 0.5 * a * t^2
	float YOffset = mCurrentTime * mCurrentTime * mAcceleration * 0.5f;
	Vector3 Pos = mStartPosition + Vector3(0, YOffset, 0);

	mAttachNode->SetPosition(Pos);
}

//-----------------------------------------------------------------------

AnimationManager::AnimationManager()
{

}

AnimationManager::~AnimationManager()
{

}

Animation* AnimationManager::CreateAnimation(AnimationType AT)
{
	return CreateAnimation(GetAutoName(), AT);
}

Animation* AnimationManager::CreateAnimation(std::string Name, AnimationType AT)
{
	if (mAnimationMap.find(Name) != mAnimationMap.end())
	{
		return mAnimationMap[Name];
	}
	Animation* Ani = nullptr;
	// TO DO...
	switch (AT)
	{
	case Animation_Node:
		Ani = new NodeAnimation(Name);
		break;
	case Animation_FlowingText:
		Ani = new FlowingTextAnimation(Name);
		break;
	case Animation_UV:
		break;
	case Animation_Texture:
		break;
	case Animation_Skeleton:
		break;
	case Animation_Max:
		break;
	default:
		break;
	}

	mAnimationMap[Name] = Ani;
	return Ani;
}

bool AnimationManager::DestroyAnimation(Animation* Ani)
{
	std::string Name = Ani->GetName();
	if (mAnimationMap.find(Name) == mAnimationMap.end())
	{
		return false;
	}
	
	SAFE_DELETE(mAnimationMap[Name]);
	mAnimationMap.erase(Name);
	return true;
}

void AnimationManager::Update()
{
	std::map<std::string, Animation*>::iterator it;
	for (it = mAnimationMap.begin(); it != mAnimationMap.end(); )
	{
		if (it->second->IsEnd() && it->second->IsAutoDestroy())
		{
			SAFE_DELETE(it->second);
			it = mAnimationMap.erase(it);
		}
		else
		{
			it++;
		}
	}

	for (it = mAnimationMap.begin(); it != mAnimationMap.end(); it++)
	{
		it->second->Update();
	}
}