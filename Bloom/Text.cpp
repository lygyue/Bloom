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

 //-----------------------------------------------------------------------

TextManager::TextManager()
{

}

TextManager::~TextManager()
{

}

Text* TextManager::CreateText(std::string Title, Font*, Vector4 Col)
{
	return nullptr;
}