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

bool Animation::GetIsEnd() const
{
	return mIsEnd;
}

void Animation::SetIsAutoDestroy(bool IsAutoDestroy)
{
	mIsAutoDestroy = IsAutoDestroy;
}

bool Animation::GetIsAutoDestroy() const
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

void NodeAnimation::AttachNode(SceneNode* Node)
{
	mAttachNode = Node;
}

void NodeAnimation::Update()
{
	if (mIsPause) return;
	if (mAttachNode == nullptr) return;
	mCurrentTime += Timer::GetInstance()->GetDeltaFloat();
	if (mCurrentTime > mAnimationLength)
	{
		if (mIsLoop)
		{
			mCurrentTime -= mAnimationLength;
		}
		else
		{
			mIsEnd = true;
			if (mIsAutoDestroy)
			{
				AnimationManager::GetInstance()->DestroyAnimationNextFrame(this);
			}
			return;
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
	Vector3 Pos = mSpline->interpolate(Index, t);

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

AnimationManager* AnimationManager::ThisInstance = nullptr;

AnimationManager::AnimationManager()
{

}

AnimationManager::~AnimationManager()
{

}

AnimationManager* AnimationManager::GetInstance()
{
	return ThisInstance;
}

Animation* AnimationManager::CreateAnimation(std::string Name, AnimationType AT)
{
	if (mAnimationMap.find(Name) != mAnimationMap.end())
	{
		return mAnimationMap[Name];
	}
	Animation* Ani = nullptr;
	switch (AT)
	{
	case Animation_Node:
		Ani = new NodeAnimation(Name);
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
}

void AnimationManager::DestroyAnimationNextFrame(Animation* Ani)
{
	mDelayDestroyArray.push_back(Ani);
}

void AnimationManager::Update()
{
	for (size_t i = 0; i < mDelayDestroyArray.size(); i++)
	{
		DestroyAnimation(mDelayDestroyArray[i]);
	}
	mDelayDestroyArray.clear();

	std::map<std::string, Animation*>::iterator it;
	for (it = mAnimationMap.begin(); it != mAnimationMap.end(); it++)
	{
		it->second->Update();
	}
}