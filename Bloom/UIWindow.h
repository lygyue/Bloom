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
class UIWindows : public Renderable
{
	friend class UIWindowManager;
public:
	virtual void Render(Matrix4& WorldTransform) override;

	virtual void OnLButtonDown(int x, int y, unsigned int wParam);
	virtual void OnLButtonUp(int x, int y, unsigned int wParam);
	virtual void OnRButtonDown(int x, int y, unsigned int wParam);
	virtual void OnRButtonUp(int x, int y, unsigned int wParam);
	virtual void OnMouseMove(int x, int y, unsigned int wParam);
	virtual void OnLButtonDbclk(int x, int y, unsigned int wParam);
	virtual void OnRButtonDbclk(int x, int y, unsigned int wParam);
protected:
	UIWindows();
	virtual ~UIWindows();

	void AddElement(UIElement* E);
	UIElement* GetElementByIndex(int Index) const;
	unsigned int GetElementCounts() const;

	std::vector<UIElement*> mElementArray;
};


class UIWindowManager
{
public:

protected:
private:
	std::vector<UIWindows*>	mWindowArray;
	std::vector<UIWindows*>	mShowWindowArray;
};