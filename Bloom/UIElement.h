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

enum UIElementType
{
	ElementbackGround,
	ElementText,

};

class Mesh;
class UIElement
{
public:
	UIElementType GetElementType() const;
protected:
	UIElement();
	virtual ~UIElement();

	void Render(Matrix4& WorldTransform);

	Mesh* mMesh;
	UIElementType mElementType;
};