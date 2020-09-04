/***********************************************
 * File: UIWindow.h
 *
 * Author: LYG
 * Date: Ò»ÔÂ 2019
 *
 * Purpose:
 *
 * 
 **********************************************/
#pragma once

#include <vector>
#include "Renderable.h"

class UIElement;
class D3d11Texture;
class UIWindow : public Renderable
{
	friend class UIWindowManager;
public:

	// First one must be back ground sheet
	bool AddElement(UIElement* E);
	bool RemoveElement(int Index);
	UIElement* GetElementByIndex(int Index) const;
	unsigned int GetElementCounts() const;

	virtual void SetVisible(bool Visible) override;
	virtual void Render(Matrix4& WorldTransform) override;

	virtual void OnLButtonDown(Vector2 ScreenPos);
	virtual void OnLButtonUp(Vector2 ScreenPos);
	virtual void OnRButtonDown(Vector2 ScreenPos);
	virtual void OnRButtonUp(Vector2 ScreenPos);
	virtual void OnMouseMove(Vector2 ScreenPos);
	virtual void OnLButtonDbclk(Vector2 ScreenPos);
	virtual void OnRButtonDbclk(Vector2 ScreenPos);
protected:
	UIWindow();
	virtual ~UIWindow();

	UIElement* IntersectElement(Vector2 ScreenPos) const;
	// May be have some animations in the window, if not, please return directly.
	virtual void Update();

	Vector2 GetMax() const;
	Vector2 GetMin() const;

	std::vector<UIElement*> mElementArray;
};


class UIWindowManager
{
	friend class Scene;
	friend class UIWindow;
public:
	D3d11Texture* GetBackGroundSheetTexture();

	void OnLButtonDown(int x, int y, unsigned int wParam);
	void OnLButtonUp(int x, int y, unsigned int wParam);
	void OnRButtonDown(int x, int y, unsigned int wParam);
	void OnRButtonUp(int x, int y, unsigned int wParam);
	void OnMouseMove(int x, int y, unsigned int wParam);
	void OnLButtonDbclk(int x, int y, unsigned int wParam);
	void OnRButtonDbclk(int x, int y, unsigned int wParam);

	UIWindow* CreateUIWindow(std::string Name, bool Visible);
	void DestroyUIWindows(std::string Name);
	void DestroyUIWindows(UIWindow* W);

	UIElement* CreateUIElementBackGroundSheet(int Width, int Height);
	void DestroyUIElement(UIElement* E);

	// some const windows in the game
	UIWindow* GetSystemWindow();
	UIWindow* GetConfigWindows();

	Vector4* GetUIStyleColor() const;
protected:
	UIWindowManager();
	~UIWindowManager();

	void NotifyActive(UIWindow* W, bool Visible = true);
	// Update use for animation updating, may be it's texture animation.
	void Update();
	void Clear();
	UIWindow* IntersectWindow(int x, int y) const;

	const int BackGroundSheetTextureSize = 1024;
private:
	D3d11Texture* mBackGroundSheetTexture;
	UIWindow* mSystemWindow;
	UIWindow* mConfigWindow;
	std::vector<UIWindow*>	mWindowArray;
	std::vector<UIWindow*>	mShowWindowArray;
	std::vector<UIElement*> mUIElementArray;
};