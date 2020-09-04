/***********************************************
 * File: Renderable.h
 *
 * Author: LYG
 * Date: Ê®¶þÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/
#pragma once
#include "Math/Matrix4.h"

enum RenderType
{
	RenderMesh,
	RenderUI,
	RenderParticle,

};
class SceneNode;
class Renderable
{
public:
	struct RenderableListener
	{
		virtual void PreRender(Renderable* R){}
		virtual void PostRender(Renderable* R){}
	};
public:
	Renderable();
	virtual ~Renderable();

	std::string GetName() const;
	void SetListener(RenderableListener* Listener);
	void RemoveListener();
	void PreRender();
	void PostRender();

	virtual void SetAttachSceneNode(SceneNode* SN);
	virtual SceneNode* GetAttachSceneNode() const;
	virtual void SetVisible(bool Visible);
	virtual bool GetVisible() const;
	virtual void Render(Matrix4& WorldTransform){}

	virtual void SetPerspect(bool Perspect);
	virtual bool GetPerspect() const;
protected:
	RenderType mRenderType;
	RenderableListener* mListener;
	SceneNode* mAttachSceneNode;

	std::string mName;
	bool mVisible;
	bool mIsPerspect;
};