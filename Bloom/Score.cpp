/***********************************************
 * File: Score.cpp
 *
 * Author: LYG
 * Date: Ê®¶þÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/

#include <xstring>
#include "Collision.h"
#include "Score.h"
#include "Scene.h"
#include "SceneNode.h"
#include "Text.h"
#include "Font.h"


static int BlockData[Block_Max] = {0, 500, 800, 100, 300, -1000, -10000000, 0};
static int BlockObjects[Grade_Max][Block_Max] = { {1, 10, 10, 10, 10, 10, 10, 1},
													{ 1, 20, 20, 20, 20, 20, 20, 1 },  
													{ 1, 30, 30, 30, 30, 30, 30, 1 } };


ScoreSystem::ScoreSystem()
{
	mCurrentScore = 0;
	mWinScore = 0;
	mTotalScore = 0;
	mBigGrade = Grade_0;
	mSmallGrade = Grade_0;
	mScoreNode = nullptr;
	mScoreText = nullptr;
	CalculateTotalScore();
}

ScoreSystem::~ScoreSystem()
{
	// not need to destroy mScoreNode and mScoreText, which been destroyed in the factory
}

int ScoreSystem::GetCurrentScore() const
{
	return mCurrentScore;
}

int ScoreSystem::GetCollisionScore(Collision* C) const
{
	return GetCollisionScore(C->GetBlockProperty());
}

int ScoreSystem::GetCollisionScore(BlockPro BP) const
{
	return BlockData[BP];
}

void ScoreSystem::CollisionTo(Collision* C)
{
	CollisionTo(C->GetBlockProperty());
}

void ScoreSystem::CollisionTo(BlockPro BP)
{
	int CollisionScore = GetCollisionScore(BP);
	mCurrentScore += CollisionScore;
	if (mCurrentScore < 0) mCurrentScore = 0;

	RefreshScore();
}

void ScoreSystem::RefreshScore()
{
	TextManager* TextMgr = Scene::GetCurrentScene()->GetTextManager();
	SceneNode* NodeFollowCamera = Scene::GetCurrentScene()->GetNodeFollowCamera();
	FontManager* FontMgr = Scene::GetCurrentScene()->GetFontManager();
	if (mScoreNode == nullptr)
	{
		mScoreNode = NodeFollowCamera->CreateChild("Score_Text_Node", Vector3(0.65f, 0.75f, RenderGroupManager::GetRenderGroupDepth(RenderGroup_TEXT)), Quaternion::IDENTITY, Vector3(1, 1, 1), RenderGroup_TEXT);
	}
	if (mScoreText)
	{
		mScoreNode->DetachMesh(mScoreText->GetAttachMesh());
		TextMgr->DestroyText(mScoreText);
		mScoreText = nullptr;
	}
	std::wstring ScoreTitle = std::wstring(L"Score:") + std::to_wstring(mCurrentScore);
	Font* F = FontMgr->GetCommonFont();
	Vector4 CommomColor = TextMgr->GetTextColorByType(CommonTextColor);
	mScoreText = TextMgr->CreateText(ScoreTitle, F, CommomColor);
	mScoreNode->AttachMesh(mScoreText->GetAttachMesh());
}

void ScoreSystem::CalculateTotalScore()
{
	mTotalScore = 0;
	for (unsigned int i = (unsigned int)Block_Apple_Red; i<= (unsigned int )Block_Circle_Yellow; i++)
	{
		mTotalScore += (BlockData[i] * BlockObjects[(unsigned int)mBigGrade][i]);
	}
}

bool ScoreSystem::IsWin() const
{
	return GetCurrentWinFactor() * float(mTotalScore) <= float(mCurrentScore);
}

bool ScoreSystem::AddGrade()
{
	mSmallGrade = ScoreGrade(unsigned int(mSmallGrade) + 1);
	if (mSmallGrade == Grade_Max)
	{
		mBigGrade = ScoreGrade(unsigned int(mBigGrade) + 1);
		mSmallGrade = Grade_0;
		if (mBigGrade == Grade_Max)
		{
			// Game Over; You win all the game.
			return false;
		}
	}
	// recalc total score
	CalculateTotalScore();
	return true;
}

ScoreGrade ScoreSystem::GetBigGrade() const
{
	return mBigGrade;
}

ScoreGrade ScoreSystem::GetSmallGrade() const
{
	return mSmallGrade;
}

float ScoreSystem::GetCurrentWinFactor() const
{
	float WinCondition[Grade_Max] = { 0.5f, 0.7f, 0.9f };
	return WinCondition[mSmallGrade];
}

int ScoreSystem::GetNumBlockObjects(BlockPro BP) const
{
	return BlockObjects[(unsigned int)mBigGrade][(unsigned int)BP];
}

float ScoreSystem::GetBulletSpeed() const
{
	// the whole screen is -1 to 1, 2 length.
	float Speed[Grade_Max] = { 2.0f / 5.0f, 2.0f / 3.0f, 2.0f / 1.0f };
	return Speed[unsigned int(mBigGrade)];
}