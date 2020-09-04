/***********************************************
 * File: Renderable.cpp
 *
 * Author: LYG
 * Date: Ê®¶þÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/

#include "Renderable.h"

Renderable::Renderable()
{
	mRenderType = RenderMesh;
	mListener = nullptr;
	mAttachSceneNode = nullptr;
	mVisible = true;
	mIsPerspect = false;
}

Renderable::~Renderable()
{

}

std::string Renderable::GetName() const
{
	return mName;
}

void Renderable::SetAttachSceneNode(SceneNode* SN)
{
	mAttachSceneNode = SN;
}

SceneNode* Renderable::GetAttachSceneNode() const
{
	return mAttachSceneNode;
}

void Renderable::SetVisible(bool Visible)
{
	mVisible = Visible;
}

bool Renderable::GetVisible() const
{
	return mVisible;
}

void Renderable::SetPerspect(bool Perspect)
{
	mIsPerspect = Perspect;
}

bool Renderable::GetPerspect() const
{
	return mIsPerspect;
}

void Renderable::SetListener(RenderableListener* Listener)
{
	mListener = Listener;
}

void Renderable::RemoveListener()
{
	mListener = nullptr;
}

void Renderable::PreRender()
{
	if (mListener)
	{
		mListener->PreRender(this);
	}
}

void Renderable::PostRender()
{
	if (mListener)
	{
		mListener->PostRender(this);
	}
}