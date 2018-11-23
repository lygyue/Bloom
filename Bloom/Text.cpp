/***********************************************
 * File: Text.cpp
 *
 * Author: LYG
 * Date: Ê®Ò»ÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/

#include "Text.h"
#include "Font.h"
#include <vector>
#include "Math/Vector4.h"
#include "Math/SimpleMath.h"

Text::Text()
{
	mAttachMesh = nullptr;
}

Text::~Text()
{

}

Mesh* Text::GetAttachMesh() const
{
	return mAttachMesh;
}

bool Text::BuildText(std::vector<TextInfo *>& V, bool Horizontal /* = true */, OriginalMode OM /* = Original_Center */)
{
	if (V.size() == 0) return false;
	// not to consider one sentence in multi font textures, in this case is impossible.
	// 
	int CenterPos = V.size() >> 1;
	// build first char

	if (Horizontal)
	{
		// Horizontal align

	}
	else
	{
		// vertical align

	}

	return true;
}

 //-----------------------------------------------------------------------

TextManager::TextManager()
{

}

TextManager::~TextManager()
{
	std::map<std::string, Text*>::iterator it;
	for (it = mTextMap.begin(); it != mTextMap.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	mTextMap.clear();
}

Text* TextManager::CreateText(std::string Title, Font* FT, Vector4 Col, bool Horizontal/* = true*/, Text::OriginalMode OM/* = Text::Original_Center*/)
{
	int Len = Title.length() + 1;
	wchar_t* WT = new wchar_t[Len];

	size_t Converts = 0;
	mbstowcs_s(&Converts, WT, Len, Title.c_str(), Len);
	std::vector<TextInfo*> TextArray;
	for (int i = 0; i < Title.length(); i++)
	{
		TextInfo* TI = FT->GetTextInfo(WT[i]);
		TextArray.push_back(TI);
	}
	SAFE_DELETE(WT);
	Text* T = new Text;
	if (T->BuildText(TextArray, Horizontal, OM))
	{
		mTextMap[Title] = T;
		return T;
	}
	SAFE_DELETE(T);
	return nullptr;
}

bool TextManager::DestroyText(std::string Title)
{
	if (mTextMap.find(Title) == mTextMap.end())
	{
		return false;
	}

	Text* T = mTextMap[Title];
	SAFE_DELETE(T);
	mTextMap.erase(Title);
	return true;
}