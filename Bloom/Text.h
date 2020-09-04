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
#include "GameDef.h"

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
		unsigned int Index;
	};
public:
	enum OriginalMode
	{
		Original_Center,
		Original_Left,
	};
	Mesh* GetAttachMesh() const;
	int GetCharacterCount() const;
	Text::OriginalMode GetTextAlignMode() const;
	void SetVisible(bool Visible);
	bool GetVisible() const;
	std::wstring GetName() const;
	int GetWidth() const;
	int GetHeight() const;
protected:
	Text();
	~Text();

	bool BuildText(std::vector<TextInfo*>& V, Vector4& TextColor, bool Horizontal = true, OriginalMode OM = Original_Center);
	void BuildTextQuad(QuadStruct* QS, int StartX, int StartY, int EndX, int EndY, TextInfo* TI);
	Mesh* mAttachMesh;
	OriginalMode mAlignMode;
	int mTextCount;
	std::wstring mName;
	int mWidth;
	int mHeight;
};

enum TextColorType
{
	StartPoemTextColor,
	CommonTextColor,
	FlowingTextColor,
	TextColorMax,
};
class TextManager
{
	friend class Scene;
public:
	Text* CreateText(std::wstring Title, Font* FT, Vector4 Col, bool Horizontal = true, Text::OriginalMode OM = Text::Original_Center);
	bool DestroyText(std::wstring Title);
	bool DestroyText(Text* T);

	Vector4 GetTextColorByType(TextColorType TCT) const;
protected:
	TextManager();
	~TextManager();

	std::map<std::wstring, Text*> mTextMap;
	CLEAR_MAP(mTextMap);
};