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
}

Renderable::~Renderable()
{

}

std::string Renderable::GetName() const
{
	return mName;
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