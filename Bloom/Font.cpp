/***********************************************
 * File: Font.cpp
 *
 * Author: LYG
 * Date: Ê®Ò»ÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/

#include "Font.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "Log.h"
#include "Texture.h"
#include "RenderSystem.h"
#include "GameDef.h"

#pragma comment(lib, "freetype.lib")
 //-----------------------------------------------------------------------

int Font::TextureUsed::TextureIndex = 0;
Font::TextureUsed::TextureUsed()
{
	XOffset = 0;
	YOffset = 0;
	MaxRowHeight = 0;
	TextureBuffer = nullptr;
	BufferLen = 0;
	Tex = nullptr;
	memset(FontTextureName, 0, sizeof(FontTextureName));
	sprintf_s(FontTextureName, sizeof(FontTextureName), "Font_Texture_%d", TextureIndex++);
}
Font::TextureUsed::~TextureUsed()
{
	// Not need to destroy Tex, which will be done in TextureManager.
	SAFE_DELETE_ARRAY(TextureBuffer);
}

bool Font::TextureUsed::AddText(TextInfo& TI, FT_GlyphSlot Glyph)
{
	if (Tex == nullptr)
	{
		TextureManager* TM = Scene::GetCurrentScene()->GetTextureManager();
		RenderSystemD3D11* RS = Scene::GetCurrentScene()->GetRenderSystem();
		Tex = TM->CreateTexture(FontTextureName, RS->GetD3d11Device(), FONT_IMAGE_SIZE, FONT_IMAGE_SIZE, DXGI_FORMAT_R8G8_UNORM, 1, true);
		BufferLen = FONT_IMAGE_SIZE * FONT_IMAGE_SIZE * 2;
		TextureBuffer = new unsigned char[BufferLen];
		memset(TextureBuffer, 0, BufferLen);
	}
	else
	{
		// Detect if Tex is full
		if (YOffset + Glyph->bitmap.rows >= FONT_IMAGE_SIZE)
		{
			// image is full
			// don't need this buffer now, release.
			SAFE_DELETE_ARRAY(TextureBuffer);
			return false;
		}
	}
	if (XOffset + Glyph->bitmap.width >= FONT_IMAGE_SIZE)
	{
		// next row
		XOffset = 0;
		YOffset += MaxRowHeight;
		MaxRowHeight = Glyph->bitmap.rows;
		if (YOffset + Glyph->bitmap.rows >= FONT_IMAGE_SIZE)
		{
			// image is full
			// don't need this buffer now, release.
			SAFE_DELETE_ARRAY(TextureBuffer);
			return false;
		}
	}
	// 
	return true;
}

 //-----------------------------------------------------------------------
Font::Font()
{
	mFTFace = nullptr;
	mFontType = MFMengYuan;
	mFontSize = 0;
}

Font::~Font()
{
	for each(TextureUsed* TU in mTextureUesedArray)
	{
		SAFE_DELETE(TU);
	}
	mTextureUesedArray.clear();
	std::map<unsigned long, TextInfo*>::iterator it;
	for (it = mTextMap.begin(); it != mTextMap.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	mTextMap.clear();
	FT_Done_Face(mFTFace);
	mFTFace = nullptr;
}

FontType Font::GetFontType() const
{
	return mFontType;
}

unsigned int Font::GetFontSize() const
{
	return mFontSize;
}

TextInfo* Font::GetTextInfo(unsigned long CH)
{
	TextInfo* TI = nullptr;
	if (mTextMap.find(CH) != mTextMap.end())
	{
		TI = mTextMap[CH];
	}
	else
	{

	}
	return nullptr;
}

//-----------------------------------------------------------------------

FontManager::FontManager()
{
	mFTLibrary = nullptr;
}

FontManager::~FontManager()
{
	for (size_t i = 0; i < mFontArray.size(); i++)
	{
		SAFE_DELETE(mFontArray[i]);
	}
	mFontArray.clear();
	FT_Done_FreeType(mFTLibrary);
}

Font* FontManager::GetFont(FontType FT, unsigned int FontSize)
{
	for each(Font* F in mFontArray)
	{
		if (F->GetFontType() == FT && F->GetFontSize() == FontSize)
		{
			return F;
		}
	}
	FT_Error      error;
	if (mFTLibrary == nullptr)
	{
		error = FT_Init_FreeType(&mFTLibrary);
		if (mFTLibrary == nullptr)
		{
			return nullptr;
		}
	}
	Font* F = new Font;
	ResourceManager* RM = Scene::GetCurrentScene()->GetResourceManager();
	std::string FontFileName = RM->GetFontFullPath(FT);
	error = FT_New_Face(mFTLibrary, FontFileName.c_str(), 0, &(F->mFTFace));
	if (F->mFTFace == nullptr)
	{
		LogImpl::GetInstance()->LogFormat("Can not loaded font:%s\n", FontFileName.c_str());
	}
	// assume 96 DPI
	error = FT_Set_Char_Size(F->mFTFace, FontSize * 64, FontSize * 64, 96, 96);
	F->mFontSize = FontSize;
	F->mFontType = FT;
	error = FT_Select_Charmap(F->mFTFace, FT_ENCODING_UNICODE);

	mFontArray.push_back(F);
	return F;
}