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
#include "GameDef.h"
#include "Scene.h"

Text::Text()
{
	mAttachMesh = nullptr;
	mAlignMode = Original_Center;
	mTextCount = 0;
}

Text::~Text()
{
	Scene::GetCurrentScene()->GetMaterialManager()->DestroyMaterial(mAttachMesh->GetMaterial());
	MeshManager* MM = Scene::GetCurrentScene()->GetMeshManager();
	MM->DestroyMesh(mAttachMesh);
}

Mesh* Text::GetAttachMesh() const
{
	return mAttachMesh;
}

int Text::GetTextCount() const
{
	return mTextCount;
}

Text::OriginalMode Text::GetTextAlignMode() const
{
	return mAlignMode;
}

void Text::BuildTextQuad(QuadStruct* QS, int StartX, int StartY, int EndX, int EndY, TextInfo* TI)
{
	Vector3 Pos1 = Vector3(float(StartX << 1) / float(WINDOW_WIDTH), float(StartY << 1) / float(WINDOW_HEIGHT), 0);
	Vector3 Pos2 = Vector3(float(EndX << 1) / float(WINDOW_WIDTH), float(EndY << 1) / float(WINDOW_HEIGHT), 0);
	QS[0].Position = Pos1;
	QS[1].Position = Vector3(Pos2.x, Pos1.y, 0);
	QS[2].Position = Pos2;
	QS[3].Position = Vector3(Pos1.x, Pos2.y, 0);
	QS[0].TexCoord = TI->UVLeftTop;
	QS[1].TexCoord = Vector2(TI->UVRightBottom.x, TI->UVLeftTop.y);
	QS[2].TexCoord = TI->UVRightBottom;
	QS[3].TexCoord = Vector2(TI->UVLeftTop.x, TI->UVRightBottom.y);
	return;
}

bool Text::BuildText(std::vector<TextInfo *>& V, Vector4& TextColor, bool Horizontal /* = true */, OriginalMode OM /* = Original_Center */)
{
	if (V.size() == 0) return false;
	mTextCount = (int)V.size();
	mAlignMode = OM;
	// not to consider one sentence in multi font textures, in this case is useless.
	// this is a simple game, not a network game, we don't have a chat system, and we have less message to show.
	int CenterPos = int(V.size() >> 1);
	// build first char
	// see also https://www.freetype.org/freetype2/docs/tutorial/step2.html
	// use the first character's coordinate system as the whole sentence coordinate.
	int StartX = 0, StartY = 0, EndX = 0, EndY = 0, HorizontalAdvance = 0, VerticalAdvance = 0;
	// every character is a quad, with 4 element
	QuadStruct* QS = new QuadStruct[V.size() * 4];
	short* IndexBuffer = new short[V.size() * 6];
	short QuadIndexBuffer[6] = { 0, 1, 3, 1, 2, 3 };
	for (size_t i = 0; i < V.size(); i++)
	{
		if (Horizontal) 
		{
			// Horizontal align
			StartX = HorizontalAdvance;
			StartX += V[i]->horiBearingX;
			StartY = V[i]->horiBearingY;
			EndX = StartX + V[i]->width;
			EndY = -(V[i]->height - V[i]->horiBearingY);
			HorizontalAdvance += V[i]->horiAdvance;
			QuadStruct VS[4];
			BuildTextQuad(VS, StartX, StartY, EndX, EndY, V[i]);
			VS[0].Index = VS[1].Index = VS[2].Index = VS[3].Index = (unsigned int)i;
			memcpy(&QS[i * 4], VS, sizeof(QuadStruct) * 4);
			for (int j = 0; j < 6; j++)
			{
				IndexBuffer[i * 6 + j] = QuadIndexBuffer[j] + short(i * 4);
			}
		}
		else
		{
			// Vertical align
			StartY = VerticalAdvance;
			StartX = V[i]->vertBearingX;
			StartY -= V[i]->vertBearingY;
			EndX = V[i]->width + StartX;	// StartX is less than 0
			EndY = StartY - V[i]->height;
			VerticalAdvance -= V[i]->vertAdvance;
			QuadStruct VS[4];
			BuildTextQuad(VS, StartX, StartY, EndX, EndY, V[i]);
			VS[0].Index = VS[1].Index = VS[2].Index = VS[3].Index = (unsigned int)i;
			memcpy(&QS[i * 4], VS, sizeof(QuadStruct) * 4);
			for (int j = 0; j < 6; j++)
			{
				IndexBuffer[i * 6 + j] = QuadIndexBuffer[j] + short(i * 4);
			}
		}
	}
	Vector3 Translate = Vector3::ZERO;
	switch (OM)
	{
	case Text::Original_Center:
		// do a translate of all the vertexes
		if (Horizontal)
		{
			HorizontalAdvance /= 2;
			Translate = Vector3(float(-HorizontalAdvance * 2) / float(WINDOW_WIDTH), 0, 0);
		}
		else
		{
			VerticalAdvance /= 2;
			Translate = Vector3(0, float(-VerticalAdvance * 2) / float(WINDOW_HEIGHT), 0);
		}
		break;
	case Text::Original_Left:
		// do nothing, it's default calculate
		break;
	default:
		break;
	}
	for (size_t i = 0; i < V.size() * 4; i++)
	{
		QS[i].Position += Translate;
	}
	// build mesh and material
	MeshManager* MM = Scene::GetCurrentScene()->GetMeshManager();
	mAttachMesh = MM->CreateMesh(QS, sizeof(QuadStruct), int(V.size() * 4), IndexBuffer, int(V.size() * 6));
	Material* Mat = Scene::GetCurrentScene()->GetMaterialManager()->CreateMaterial(SimpleFontSample);
	char* TempPtr = Mat->GetConstBufferPointer();
	memcpy(TempPtr, &TextColor, sizeof(Vector4));
	Mat->SetConstBufferLen(sizeof(Vector4));
	TextInfo* TI = V[0];
	// they all use the same texture in this situation
	Mat->SetTexture(TI->Tex);
	mAttachMesh->SetMaterial(Mat);
	SAFE_DELETE_ARRAY(QS);
	SAFE_DELETE_ARRAY(IndexBuffer);
	return true;
}

 //-----------------------------------------------------------------------

TextManager::TextManager()
{

}

TextManager::~TextManager()
{
	std::map<std::wstring, Text*>::iterator it;
	for (it = mTextMap.begin(); it != mTextMap.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	mTextMap.clear();
}

Text* TextManager::CreateText(std::wstring Title, Font* FT, Vector4 Col, bool Horizontal/* = true*/, Text::OriginalMode OM/* = Text::Original_Center*/)
{
	std::vector<TextInfo*> TextArray;
	for (int i = 0; i < Title.length(); i++)
	{
		TextInfo* TI = FT->GetTextInfo(Title[i]);
		TextArray.push_back(TI);
	}

	Text* T = new Text;
	if (T->BuildText(TextArray, Col, Horizontal, OM))
	{
		mTextMap[Title] = T;
		return T;
	}
	SAFE_DELETE(T);
	return nullptr;
}

bool TextManager::DestroyText(std::wstring Title)
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