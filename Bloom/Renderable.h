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

	virtual void Render(Matrix4& WorldTransform){}
protected:
	RenderType mRenderType;
	RenderableListener* mListener;

	std::string mName;
};