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
#include "Math/Vector2.h"
#include "Math/Vector3.h"

struct TextInfo;
class Font;
class Vector4;
class Mesh;
class Text
{
	friend class TextManager;
	struct QuadStruct
	{
		Vector3 Position;
		Vector2 TexCoord;
	};
public:
	enum OriginalMode
	{
		Original_Center,
		Original_Left,
	};
	Mesh* GetAttachMesh() const;
protected:
	Text();
	~Text();

	bool BuildText(std::vector<TextInfo*>& V, bool Horizontal = true, OriginalMode OM = Original_Center);
	void BuildTextQuad(QuadStruct* QS, int StartX, int StartY, int EndX, int EndY, TextInfo* TI);
	Mesh* mAttachMesh;
};

class TextManager
{
	friend class Scene;
public:
	Text* CreateText(std::wstring Title, Font* FT, Vector4 Col, bool Horizontal = true, Text::OriginalMode OM = Text::Original_Center);
	bool DestroyText(std::wstring Title);
protected:
	TextManager();
	~TextManager();

	std::map<std::wstring, Text*> mTextMap;
};