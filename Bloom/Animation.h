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
#include "GameDef.h"
#include <map>
#include <vector>
#include "Math/Vector3.h"

class SceneNode;
class SimpleSpline;
class Quaternion;
enum AnimationType
{
	Animation_Node,
	Animation_FlowingText,
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
	void SetAnimationLength(float AniLength);
	float GetAnimationLength() const;

	void SetIsLoop(bool IsLoop);
	bool GetIsLoop() const;

	void SetPause(bool IsPause);
	bool GetIsPause() const;

	bool IsEnd() const;

	void SetIsAutoDestroy(bool IsAutoDestroy);
	bool IsAutoDestroy() const;

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
	void GetPoint(Vector3& Pos, Quaternion& Rotate, Vector3& Scale, float& fTime, int Index) const;
	void AttachNode(SceneNode* Node);
	SceneNode* GetAttachNode() const;
	void AddRoundPoint(float fTime);

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

class FlowingTextAnimation : public Animation
{
	friend class AnimationManager;
public:
	void AttachNode(SceneNode* Node);
	SceneNode* GetAttachNode() const;
	void SetAcceleration(float Acceleration);
	float GetAcceleration() const;
	virtual void Update() override;

protected:
	FlowingTextAnimation(std::string Name);
	virtual ~FlowingTextAnimation();

	SceneNode* mAttachNode;
	float mAcceleration;
	Vector3 mStartPosition;
};

class AnimationManager
{
	friend class Scene;
public:
	void Update();
	Animation* CreateAnimation(std::string Name, AnimationType AT);
	Animation* CreateAnimation(AnimationType AT);
	bool DestroyAnimation(Animation* Ani);

protected:
	AnimationManager();
	~AnimationManager();

private:
	std::map<std::string, Animation*> mAnimationMap;
	GET_AUTO_NAME("Animation")
};