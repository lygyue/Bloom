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
#include <vector>

struct TextInfo;
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

	bool BuildText(std::vector<TextInfo*>& V, bool Horizontal = true, OriginalMode OM = Original_Center);
	Mesh* mAttachMesh;
};

class TextManager
{
	friend class Scene;
public:
	Text* CreateText(std::string Title, Font* FT, Vector4 Col, bool Horizontal = true, Text::OriginalMode OM = Text::Original_Center);
	bool DestroyText(std::string Title);
protected:
	TextManager();
	~TextManager();

	std::map<std::string, Text*> mTextMap;
};