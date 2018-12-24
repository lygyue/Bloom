/***********************************************
 * File: Font.h
 *
 * Author: LYG
 * Date: Ê®Ò»ÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/
#pragma once
#include <vector>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include "Math/Vector2.h"

enum FontType
{
	MFDingHei,
	MFFanSong,
	MFJingYue,
	MFJinHua,
	MFLingYi,
	MFMengYuan,
	MFNianZhen,
	MFSiYan,
	MFSuBai,
	MFWenShang,
	MFXingYan,
	MFXiYue,
	MFYiFeng,
	MFYuYi,
	MFZhuoHei,
	// gen you ye fonts
	YGYXingCao,
	FontMax,
};
enum FontStyle
{
	FSNormal = 1 << 0,
	FSBold = 1 << 1,
	FSItalic = 1 << 2,
	FSUnderLine = 1 << 3,
	FSOutline = 1 << 4,
	FSRotate = 1 << 5,					// Can not combine with FSItalic, clock wise
};

class D3d11Texture;
struct TextInfo
{
	Vector2 UVLeftTop;
	Vector2 UVRightBottom;
	int width;
	int height;
	int horiBearingX;
	int horiBearingY;
	int horiAdvance;
	int vertBearingX;
	int vertBearingY;
	int vertAdvance;
	D3d11Texture* Tex;
};

class Font
{
	friend class FontManager;
	struct TextureUsed
	{
	public:
		TextureUsed();
		~TextureUsed();

		// if return false, the texture is full, need a new trxture.
		bool AddText(TextInfo& TI, FT_Bitmap& BitMap, bool IsL8 = true);
		D3d11Texture* Tex;
		int XOffset;
		int YOffset;
		int MaxRowHeight;
		static int TextureIndex;
		char FontTextureName[128];
		unsigned char* TextureBuffer;
		int BufferLen;
	};
public:
	FontType GetFontType() const;
	unsigned int GetFontSize() const;
	unsigned int GetFontStyle() const;
	int GetOutlineWidth() const;

	TextInfo* GetTextInfo(unsigned long CH);
protected:
	Font();
	~Font();

	FT_Face mFTFace;
	FT_Library mFTLibrary;
	FT_Stroker mFTStroker;
	FontType mFontType;
	unsigned int mFontStyle;
	int mOutlineWidth;
	int mBoldWidth;
	float mRotateDegree;
	unsigned int mFontSize;
	std::map<unsigned long, TextInfo*> mTextMap;
	std::vector<TextureUsed*> mTextureUesedArray;
};

class FontManager
{
	friend class Scene;
public:
	Font* GetFont(FontType FT, unsigned int FontSize, unsigned int FontStyle = FSNormal, int OutLineWidth = 0, int BoldWidth = 0, float RotateDegree = 0);

	Font* GetStartPoemFont();
	Font* GetCommonFont();
protected:
	FontManager();
	~FontManager();
	FT_Library mFTLibrary;
	std::vector<Font*> mFontArray;
};