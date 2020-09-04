/***********************************************
 * File: UIElement.h
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
#include "Math/Vector2.h"

enum UIElementType
{
	ElementBackGroundSheet,
	ElementQuadListList,
	ElementUIText,
	ElementButtonBG,
	ElementButtonTitle,
	ElementButtonText,
	ElementTextBeenCover,

};

class Mesh;
class UIWindow;
class UIElement
{
	friend class UIWindowManager;
	friend class UIWindow;
public:
	UIElementType GetElementType() const;
	// this visible flag use for receiving windows message
	// if not, this element can not do any actions
	// it's not use for rendering control
	void SetVisible(bool Visible);
	bool GetVisible() const;
	Vector3 GetCenter() const;
	Vector3 GetMax() const;
	Vector3 GetMin() const;
	void Resize(int Width, int Height);
	int GetWidth() const;
	int GetHeight() const;

	void SetAttachWindow(UIWindow* W);
	UIWindow* GetAttachWindow() const;
	void SetElementMesh(Mesh* M);
	Mesh* GetElementMesh() const;

	virtual void OnLButtonDown(Vector2 ScreenPos);
	virtual void OnLButtonUp(Vector2 ScreenPos);
	virtual void OnRButtonDown(Vector2 ScreenPos);
	virtual void OnRButtonUp(Vector2 ScreenPos);
	virtual void OnMouseMove(Vector2 ScreenPos);
	virtual void OnLButtonDbclk(Vector2 ScreenPos);
	virtual void OnRButtonDbclk(Vector2 ScreenPos);

	virtual void Update() {}
protected:
	UIElement(int Width, int Height);
	virtual ~UIElement();

	void Render(Matrix4 WorldTransform);

	Mesh* mMesh;
	UIWindow* mAttachWindow;
	UIElementType mElementType;
	Vector3 mMax;
	Vector3 mMin;
	int mWidth;
	int mHeight;
	bool mVisible;
	bool mLButtonDown;
	bool mRButtonDown;
};

class UIElementBackGroundSheet : public UIElement
{
	friend class UIWindowManager;
public:
	virtual void OnLButtonDown(Vector2 ScreenPos) override;
	virtual void OnLButtonUp(Vector2 ScreenPos) override;
	virtual void OnRButtonDown(Vector2 ScreenPos) override;
	virtual void OnRButtonUp(Vector2 ScreenPos) override;
	virtual void OnMouseMove(Vector2 ScreenPos) override;
	virtual void OnLButtonDbclk(Vector2 ScreenPos) override;
	virtual void OnRButtonDbclk(Vector2 ScreenPos) override;
protected:
	UIElementBackGroundSheet(int Width, int Height);
	virtual ~UIElementBackGroundSheet();

	Vector2 mTranslate;
};
