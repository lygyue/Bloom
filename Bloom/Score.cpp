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

#include "Collision.h"
#include "Score.h"

static int BlockData[Block_Max] = {0, 500, 800, 100, 300, -1000, -10000000, 0};

ScoreSystem::ScoreSystem()
{
	mCurrentScore = 0;
}

ScoreSystem::~ScoreSystem()
{

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
	mCurrentScore += GetCollisionScore(BP);
}