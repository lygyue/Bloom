/***********************************************
 * File: UIElement.cpp
 *
 * Author: LYG
 * Date: Ò»ÔÂ 2019
 *
 * Purpose:
 *
 * 
 **********************************************/

#include "UIElement.h"
#include "RenderGroupManager.h"
#include "Mesh.h"

UIElement::UIElement()
{
	mMesh = nullptr;
	mElementType = ElementBackGroundSheet;
}

UIElement::~UIElement()
{

}

UIElementType UIElement::GetElementType() const
{
	return mElementType;
}

void UIElement::Render(Matrix4 WorldTransform)
{
	if (mMesh)
	{
		mMesh->Render(WorldTransform);
	}
}

void UIElement::OnLButtonDown(int x, int y, unsigned int wParam)
{

}

void UIElement::OnLButtonUp(int x, int y, unsigned int wParam)
{

}

void UIElement::OnRButtonDown(int x, int y, unsigned int wParam)
{

}

void UIElement::OnRButtonUp(int x, int y, unsigned int wParam)
{

}

void UIElement::OnLButtonDbclk(int x, int y, unsigned int wParam)
{

}

void UIElement::OnRButtonDbclk(int x, int y, unsigned int wParam)
{

}

void UIElement::OnMouseMove(int x, int y, unsigned int wParam)
{

}