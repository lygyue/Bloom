/***********************************************
 * File: Score.h
 *
 * Author: LYG
 * Date: Ê®¶þÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/

#pragma once

enum BlockPro;
class Collision;
class ScoreSystem
{
	friend class Player;
protected:
	int GetCurrentScore() const;
	int GetCollisionScore(Collision* C) const;
	int GetCollisionScore(BlockPro BP) const;
	void CollisionTo(Collision* C);
	void CollisionTo(BlockPro BP);
protected:
	ScoreSystem();
	~ScoreSystem();
private:
	int mCurrentScore;
};