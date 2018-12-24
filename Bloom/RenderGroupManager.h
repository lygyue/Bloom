/***********************************************
 * File: RenderGroupManager.h
 *
 * Author: LYG
 * Date: ËÄÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/
#pragma once
#include "Common.h"
#include <vector>

enum RenderGroup
{
	RenderGroup_BackGroud,
	RenderGroup_BeforeNormal,
	RenderGroup_Normal,
	RenderGroup_AfterNormal,
	RenderGroup_Bullet,					// bullet is a node animation ,must be in front of other collisions.
	RenderGroup_Player,
	RenderGroup_UI,
	RenderGroup_TEXT,
	RenderGroup_Final,
	RenderGroup_Max,
};

class SceneNode;
class Scene;
typedef std::vector<SceneNode*> RenderQueue;
class RenderGroupManager
{
	friend class SceneNode;
	friend class Scene;
public:
	static float GetRenderGroupDepth(RenderGroup RG = RenderGroup_Normal);

protected:
	RenderGroupManager();
	~RenderGroupManager();

	void AddSceneNode(SceneNode* SN);
	void RemoveSceneNode(SceneNode* SN);

	void RenderAllQueue() const;
private:
	RenderQueue mRenderGroups[RenderGroup_Max];
};
