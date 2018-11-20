/***********************************************
 * File: RenderGroupManager.cpp
 *
 * Author: LYG
 * Date: ���� 2018
 *
 * Purpose:
 *
 * 
 **********************************************/

#include "RenderGroupManager.h"
#include "SceneNode.h"
#include "Math/Matrix4.h"

RenderGroupManager::RenderGroupManager()
{

}

RenderGroupManager::~RenderGroupManager()
{

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
			int MeshCount = RQ[j]->GetAttachMeshCount();
			for (int k = 0; k < MeshCount; k++)
			{
				Mesh* mesh = RQ[j]->GetAttachMeshByIndex(k);
				mesh->RenderMesh(M);
			}
		}
	}
}