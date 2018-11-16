/***********************************************
 * File: Collision.h
 *
 * Author: LYG
 * Date: Ê®Ò»ÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/
#pragma once
#include "Math/Vector3.h"
#include "Common.h"
#include <vector>

class SceneNode;
class Collision
{
	friend class CollisionManager;
public:
	enum RelationShip
	{
		Collision_Intersection,
		Collision_Include,
		Collision_BeenIncluded,
		Collision_None,
	};
	// need to know the relationship
	RelationShip Relation(Collision& Other) const;
	// only intersection detected
	bool CollisionTo(Collision& Other) const;

	void SetMax(Vector3& Max);
	Vector3 GetMax() const;
	void SetMin(Vector3& Min);
	Vector3 GetMin() const;

	void SetAttachSceneNode(SceneNode* SN);
	SceneNode* GetAttachSceneNode() const;

	bool GetIsStatic() const;
	// this function been called while attach scene node been changed.
	void RecalculateCollision();
protected:
	Collision();
	~Collision();


	bool mIsStatic;
	// mMax and mMin will not be changed while it is a static collision
	// mMax and mMin will be recalculated while dynamic state and collision event occur.
	Vector3 mMax;
	Vector3 mMin;

	Vector3 mMaxLocalTrasform;
	Vector3 mMinLocalTransform;
	SceneNode* mAttachSceneNode;
};

// not need to do optimization, may be do this in the future
class CollisionManager
{
	friend class Scene;
public:
	Collision* CreateCollision(Vector3 LocalMax, Vector3 LocalMin, bool IsStatic = true, SceneNode* AttachSceneNode = nullptr);
	bool DestroyCollision(Collision* C);

protected:
	CollisionManager();
	~CollisionManager();
private:
	std::vector<Collision*> mStaticCollisionArray;
	std::vector<Collision*> mDynamicCollisionArray;
};