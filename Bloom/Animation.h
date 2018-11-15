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
#include <vector>

class SceneNode;
class SimpleSpline;
class Vector3;
class Quaternion;
enum AnimationType
{
	Animation_Node,
	Animation_UV,
	Animation_Texture,
	Animation_Skeleton,
	Animation_Max,
};

class Animation
{
	friend class AnimationManager;
public:
	virtual void Update() {}
public:
	void SetAnimationType(AnimationType AT);
	AnimationType GetAnimationType() const;

	std::string GetName() const;
	float GetAnimationLength() const;

	void SetIsLoop(bool IsLoop);
	bool GetIsLoop() const;

	void SetPause(bool IsPause);
	bool GetIsPause() const;

	bool GetIsEnd() const;

	void SetIsAutoDestroy(bool IsAutoDestroy);
	bool GetIsAutoDestroy() const;

protected:
	Animation(std::string Name);
	virtual ~Animation();

	std::string mName;
	AnimationType mAnimationType;
	float mAnimationLength;
	float mCurrentTime;
	bool mIsEnd;
	bool mIsLoop;
	bool mIsAutoDestroy;
	bool mIsPause;
};

class NodeAnimation : public Animation
{
	friend class AnimationManager;
public:
	void AddPoint(Vector3 Pos, Quaternion Rotate, Vector3 Scale, float fTime);
	void AttachNode(SceneNode* Node);

	virtual void Update() override;
protected:
	NodeAnimation(std::string Name);
	virtual ~NodeAnimation();

	std::vector<Quaternion> mRotations;
	std::vector<Vector3> mScales;
	std::vector<float> mTimes;
	SceneNode* mAttachNode;
	SimpleSpline* mSpline;
};


class AnimationManager
{
	friend class Scene;
public:
	void Update();
	Animation* CreateAnimation(std::string Name, AnimationType AT);
	bool DestroyAnimation(Animation* Ani);
	void DestroyAnimationNextFrame(Animation* Ani);

	static AnimationManager* GetInstance();
protected:
	AnimationManager();
	~AnimationManager();

	static AnimationManager* ThisInstance;
private:
	std::map<std::string, Animation*> mAnimationMap;
	std::vector<Animation*> mDelayDestroyArray;
};