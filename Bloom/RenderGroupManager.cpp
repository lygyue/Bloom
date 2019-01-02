/***********************************************
 * File: RenderGroupManager.cpp
 *
 * Author: LYG
 * Date: ËÄÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/

#include "RenderGroupManager.h"
#include "SceneNode.h"
#include "Math/Matrix4.h"
#include "GameDef.h"
#include "Mesh.h"

RenderGroupManager::RenderGroupManager()
{

}

RenderGroupManager::~RenderGroupManager()
{

}

float RenderGroupManager::GetRenderGroupDepth(RenderGroup RG/* = RenderGroup_Normal*/)
{
	return FAR_PLANE - float(RG + 1) * 100.0f;
}

void RenderGroupManager::AddSceneNode(SceneNode* SN)
{
	RenderGroup RG = SN->GetRenderGroup();
	RenderQueue& RQ = mRenderGroups[RG];
	RQ.push_back(SN);
}

void RenderGroupManager::RemoveSceneNode(SceneNode* SN)
{
	RenderGroup RG = SN->GetRenderGroup();
	RenderQueue& RQ = mRenderGroups[RG];
	for (int i = 0; i < RQ.size(); i++)
	{
		if (RQ[i] == SN)
		{
			RQ.erase(RQ.begin() + i);
			break;
		}
	}
}

void RenderGroupManager::RenderAllQueue() const
{
	for (size_t i = 0; i < RenderGroup_Max; i++)
	{
		RenderQueue RQ = mRenderGroups[i];
		for (size_t j = 0; j < RQ.size(); j++)
		{
			Matrix4 M = RQ[j]->GetWorldTransform();
			//XMMATRIX M = RQ[j]->GetWorldTransformD3DMath();
			int RenderableCount = RQ[j]->GetAttachRenderableCount();
			for (int k = 0; k < RenderableCount; k++)
			{
				Renderable* R = RQ[j]->GetAttachRenderableByIndex(k);
				R->Render(M);
			}
		}
	}
}

void RenderGroupManager::Clear()
{
	for (size_t i = 0; i < RenderGroup_Max; i++)
	{
		RenderQueue RQ = mRenderGroups[i];
		RQ.clear();
	}
}