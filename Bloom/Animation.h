/***********************************************
 * File: Animation.h
 *
 * Author: LYG
 * Date: Ê®Ò»ÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/
#pragma once
#include "Common.h"
#include <map>

enum AnimationType
{
	Animation_Node,
	Animation_UV,
	Animation_Texture,
	Animation_Skeleton,
};

class Animation
{
	friend class AnimationManager;
public:
	virtual void Update() {}
public:
	void SetAnimationType(AnimationType AT);
	AnimationType GetAnimationType() const;
protected:
	Animation();
	virtual ~Animation();
private:
	AnimationType mAnimationType;
};

class NodeAnimation : public Animation
{
	friend class AnimationManager;
public:
	virtual void Update() override;
protected:
	NodeAnimation();
	virtual ~NodeAnimation();

};


class AnimationManager
{
	friend class Scene;
public:
	Animation* CreateAnimation(std::string Name, AnimationType AT);
protected:
	AnimationManager();
	~AnimationManager();
private:
	std::map<std::string, Animation*> mAnimationMap;
};