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
#include "UIWindow.h"
#include "SceneNode.h"
#include "RenderGroupManager.h"
#include "Mesh.h"
#include "Scene.h"

UIElement::UIElement(int Width, int Height)
{
	mMesh = nullptr;
	mElementType = ElementBackGroundSheet;
	mAttachWindow = nullptr;
	mVisible = true;
	mLButtonDown = false;
	mRButtonDown = false;
	mWidth = Width;
	mHeight = Height;
	//
	Vector2 V = Scene::WindowsCoordToProjectionCoord(Width, Height);
	mMax = Vector3(V.x / 2.0f, V.y / 2.0f, 0.0f);
	mMin = -mMax;
}

UIElement::~UIElement()
{

}

UIElementType UIElement::GetElementType() const
{
	return mElementType;
}

void UIElement::SetVisible(bool Visible)
{
	mVisible = Visible;
}

bool UIElement::GetVisible() const
{
	return mVisible;
}

Vector3 UIElement::GetCenter() const
{
	return (mMax + mMin) / 2.0f;
}

Vector3 UIElement::GetMax() const
{
	// we need to do a world transform
	if (mAttachWindow)
	{
		SceneNode* SN = mAttachWindow->GetAttachSceneNode();
		Vector3 WorldPos = SN->GetWorldPosition();
		// not need to consider the rotation and scale
		return mMax + WorldPos;
	}

	return mMax;
}

Vector3 UIElement::GetMin() const
{
	// we need to do a world transform
	if (mAttachWindow)
	{
		SceneNode* SN = mAttachWindow->GetAttachSceneNode();
		Vector3 WorldPos = SN->GetWorldPosition();
		// not need to consider the rotation and scale
		return mMin + WorldPos;
	}
	return mMin;
}

void UIElement::Resize(int Width, int Height)
{
	mWidth = Width;
	mHeight = Height;
	Vector2 V = Scene::WindowsCoordToProjectionCoord(Width, Height);
	mMax = Vector3(V.x / 2.0f, V.y / 2.0f, 0.0f);
	mMin = -mMax;
}

int UIElement::GetWidth() const
{
	return mWidth;
}

int UIElement::GetHeight() const
{
	return mHeight;
}

void UIElement::SetAttachWindow(UIWindow* W)
{
	mAttachWindow = W;
}

UIWindow* UIElement::GetAttachWindow() const
{
	return mAttachWindow;
}

void UIElement::SetElementMesh(Mesh* M)
{
	mMesh = M;
}

Mesh* UIElement::GetElementMesh() const
{
	return mMesh;
}

void UIElement::Render(Matrix4 WorldTransform)
{
	if (mMesh)
	{
		mMesh->RenderMesh(WorldTransform, false);
	}
}

void UIElement::OnLButtonDown(Vector2 ScreenPos)
{

}

void UIElement::OnLButtonUp(Vector2 ScreenPos)
{

}

void UIElement::OnRButtonDown(Vector2 ScreenPos)
{

}

void UIElement::OnRButtonUp(Vector2 ScreenPos)
{

}

void UIElement::OnLButtonDbclk(Vector2 ScreenPos)
{

}

void UIElement::OnRButtonDbclk(Vector2 ScreenPos)
{

}

void UIElement::OnMouseMove(Vector2 ScreenPos)
{

}

//-----------------------------------------------------------------------

UIElementBackGroundSheet::UIElementBackGroundSheet(int Width, int Height)
	: UIElement(Width, Height)
{
	mTranslate = Vector2::ZERO;
	mElementType = ElementBackGroundSheet;
}

UIElementBackGroundSheet::~UIElementBackGroundSheet()
{

}

void UIElementBackGroundSheet::OnLButtonDown(Vector2 ScreenPos)
{
	mLButtonDown = true;
	Vector2 NodePos = Vector2((float*)&(mAttachWindow->GetAttachSceneNode()->GetWorldPosition()));
	mTranslate = NodePos - ScreenPos;
}

void UIElementBackGroundSheet::OnLButtonUp(Vector2 ScreenPos)
{
	mLButtonDown = false;
	mTranslate = Vector2::ZERO;
}

void UIElementBackGroundSheet::OnLButtonDbclk(Vector2 ScreenPos)
{

}

void UIElementBackGroundSheet::OnMouseMove(Vector2 ScreenPos)
{
	if (mLButtonDown)
	{
		// move the whole window
		Vector2 Pos = mTranslate + ScreenPos;
		Vector3 NodePos = mAttachWindow->GetAttachSceneNode()->GetWorldPosition();

		mAttachWindow->GetAttachSceneNode()->Translate(Pos.x - NodePos.x, Pos.y - NodePos.y, 0);
	}
}

void UIElementBackGroundSheet::OnRButtonDbclk(Vector2 ScreenPos)
{

}

void UIElementBackGroundSheet::OnRButtonDown(Vector2 ScreenPos)
{
	mRButtonDown = true;
}

void UIElementBackGroundSheet::OnRButtonUp(Vector2 ScreenPos)
{
	mRButtonDown = false;
}