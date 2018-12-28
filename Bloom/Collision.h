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
#include "GameDef.h"
#include <vector>

class SceneNode;
enum BlockPro
{
	Block_None,
	Block_Apple_Red,
	Block_Apple_Yellow,
	Block_Circle_Carmine,
	Block_Circle_Yellow,
	Block_Bullet_DecreaseLife,
	Block_Die,
	Block_Final_Flag,
	Block_Max,
};
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

	void SetVisible(bool Visible);
	bool GetVisible() const;
	bool GetIsStatic() const;
	// this function been called while attach scene node been changed.
	void RecalculateCollision();

	BlockPro GetBlockProperty() const;
protected:
	Collision();
	~Collision();


	bool mIsStatic;
	bool mIsVisible;
	// mMax and mMin will not be changed while it is a static collision
	// mMax and mMin will be recalculated while dynamic state and collision event occur.
	Vector3 mMax;
	Vector3 mMin;

	Vector3 mMaxLocalTrasform;
	Vector3 mMinLocalTransform;
	SceneNode* mAttachSceneNode;
	BlockPro mBlockProperty;
};

// not need to do optimization, may be do this in the future
class CollisionManager
{
	friend class Scene;
public:
	Collision* CreateCollision(Vector3 LocalMax, Vector3 LocalMin, bool IsStatic = true, SceneNode* AttachSceneNode = nullptr, BlockPro BP = Block_Die);
	bool DestroyCollision(Collision* C);

	std::vector<Collision*> CalculateCollisionList(Collision* C);
protected:
	CollisionManager();
	~CollisionManager();

	// not need to update every frame.
	void Update();
private:
	std::vector<Collision*> mStaticCollisionArray;
	std::vector<Collision*> mDynamicCollisionArray;

	unsigned long mCurrentFrameIndex;
	CLEAR_VECTOR2(mStaticCollisionArray, mDynamicCollisionArray);
};