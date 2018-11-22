/***********************************************
 * File: Text.h
 *
 * Author: LYG
 * Date: Ê®Ò»ÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/
#pragma once
#include "Common.h"
#include <map>

class Font;
class Vector4;
class Mesh;
class Text
{
	friend class TextManager;
public:
	enum OriginalMode
	{
		Original_Center,
		Original_LeftBottom,
		Original_LeftTop,
	};
	Mesh* GetAttachMesh() const;
protected:
	Text();
	~Text();

	Mesh* mAttachMesh;
};

class TextManager
{
	friend class Scene;
public:
	Text* CreateText(std::string Title, Font*, Vector4 Col);
protected:
	TextManager();
	~TextManager();


	std::map<std::string, Text*> mTextMap;
};