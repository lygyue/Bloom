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

Animation::Animation()
{

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
//-----------------------------------------------------------------------
NodeAnimation::NodeAnimation()
{

}

NodeAnimation::~NodeAnimation()
{

}

void NodeAnimation::Update()
{

}

//-----------------------------------------------------------------------

AnimationManager::AnimationManager()
{

}

AnimationManager::~AnimationManager()
{

}