/***********************************************
 * File: Collision.cpp
 *
 * Author: LYG
 * Date: Ê®Ò»ÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/

#include "Collision.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "SceneNode.h"
#include "Timer.h"

Collision::Collision()
{
	mIsStatic = true;
	mAttachSceneNode = nullptr;
	mMax = mMin = mMaxLocalTrasform = mMinLocalTransform = Vector3::ZERO;
}

Collision::~Collision()
{

}

Collision::RelationShip Collision::Relation(Collision& Other) const
{
	if (mMax.x >= Other.mMax.x && mMax.y >= Other.mMax.y && mMin.x <= Other.mMin.x && mMin.y <= Other.mMin.y)
	{
		return Collision_Include;
	}
	else if (mMax.x <= Other.mMax.x && mMax.y <= Other.mMax.y && mMin.x >= Other.mMin.x && mMin.y >= Other.mMin.y)
	{
		return Collision_BeenIncluded;
	}
	else if (mMax.x < Other.mMin.x || mMax.y < Other.mMin.y || mMin.x > Other.mMax.x || mMin.y > Other.mMax.y)
	{
		return Collision_None;
	}
	return Collision_Intersection;
}

bool Collision::CollisionTo(Collision& Other) const
{
	if(Relation(Other) == Collision_None)
		return false;

	return true;
}

void Collision::SetMax(Vector3& Max)
{
	mMax = Max;
}

Vector3 Collision::GetMax() const
{
	return mMax;
}

void Collision::SetMin(Vector3& Min)
{
	mMin = Min;
}

Vector3 Collision::GetMin() const
{
	return mMin;
}

bool Collision::GetIsStatic() const
{
	return mIsStatic;
}

void Collision::SetAttachSceneNode(SceneNode* SN)
{
	if (mAttachSceneNode != SN)
	{
		mAttachSceneNode = SN;
		RecalculateCollision();
	}

}

SceneNode* Collision::GetAttachSceneNode() const
{
	return mAttachSceneNode;
}

void Collision::RecalculateCollision()
{
	if (mAttachSceneNode == nullptr) return;

	Vector3 Pos = mAttachSceneNode->GetWorldPosition();
	Quaternion Rotation = mAttachSceneNode->GetWorldRotation();
	Vector3 Scale = mAttachSceneNode->GetWorldScale();

	mMax = Rotation * (mMaxLocalTrasform * Scale) + Pos;
	mMin = Rotation * (mMinLocalTransform * Scale) + Pos;
}

//-----------------------------------------------------------------------

CollisionManager::CollisionManager()
{
	mCurrentFrameIndex = -1;
}

CollisionManager::~CollisionManager()
{
	for (size_t i = 0; i < mStaticCollisionArray.size(); i++)
	{
		SAFE_DELETE(mStaticCollisionArray[i]);
	}

	for (size_t i = 0; i < mDynamicCollisionArray.size(); i++)
	{
		SAFE_DELETE(mDynamicCollisionArray[i]);
	}

	mStaticCollisionArray.clear();
	mDynamicCollisionArray.clear();
}

Collision* CollisionManager::CreateCollision(Vector3 LocalMax, Vector3 LocalMin, bool IsStatic /* = true */, SceneNode* AttachSceneNode /* = nullptr */)
{
	Collision* C = new Collision();
	C->mIsStatic = IsStatic;
	C->mAttachSceneNode = AttachSceneNode;
	C->mMaxLocalTrasform = LocalMax;
	C->mMinLocalTransform = LocalMin;
	C->RecalculateCollision();
	if (IsStatic)
	{
		mStaticCollisionArray.push_back(C);
	}
	else
	{
		mDynamicCollisionArray.push_back(C);
	}
	return C;
}

bool CollisionManager::DestroyCollision(Collision* C)
{
	if (C->GetIsStatic())
	{
		for (size_t i = 0; i < mStaticCollisionArray.size(); i++)
		{
			if (mStaticCollisionArray[i] == C)
			{
				SAFE_DELETE(C);
				mStaticCollisionArray.erase(mStaticCollisionArray.begin() + i);
				return true;
			}
		}
	}
	else
	{
		for (size_t i = 0; i < mDynamicCollisionArray.size(); i++)
		{
			if (mDynamicCollisionArray[i] == C)
			{
				SAFE_DELETE(C);
				mDynamicCollisionArray.erase(mDynamicCollisionArray.begin() + i);
				return true;
			}
		}
	}
	return false;
}

void CollisionManager::Update()
{
	if (mCurrentFrameIndex != Timer::GetInstance()->GetFrameIndex())
	{
		mCurrentFrameIndex = Timer::GetInstance()->GetFrameIndex();
		for (size_t i = 0; i < mDynamicCollisionArray.size(); i++)
		{
			mDynamicCollisionArray[i]->RecalculateCollision();
		}
	}
}

std::vector<Collision*> CollisionManager::CalculateCollisionList(Collision* C)
{
	Update();
	std::vector<Collision*> V;
	for (size_t i = 0; i < mStaticCollisionArray.size(); i++)
	{
		if (mStaticCollisionArray[i]->CollisionTo(*C))
		{
			V.push_back(mStaticCollisionArray[i]);
		}
	}

	for (size_t i = 0; i < mDynamicCollisionArray.size(); i++)
	{
		if (mDynamicCollisionArray[i] != C)
		{
			if (mDynamicCollisionArray[i]->CollisionTo(*C))
			{
				V.push_back(mDynamicCollisionArray[i]);
			}
		}
	}

	return V;
}