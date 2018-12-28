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

enum ScoreGrade
{
	Grade_0,
	Grade_1,
	Grade_2,
	Grade_Max,
};

enum BlockPro;
class Collision;
class SceneNode;
class Text;
class ScoreSystem
{
	friend class Player;
	friend class Scene;
protected:
	ScoreSystem();
	~ScoreSystem();

	int GetCurrentScore() const;
	int GetCollisionScore(Collision* C) const;
	int GetCollisionScore(BlockPro BP) const;
	void CollisionTo(Collision* C);
	void CollisionTo(BlockPro BP);

	int GetNumBlockObjects(BlockPro BP) const;
	float GetBulletSpeed() const;
protected:
	void Reset();
	void RefreshScore();
	void CalculateTotalScore();
	bool IsWin() const;
	// u win all, return false, otherwise return true.
	bool AddGrade();
	ScoreGrade GetBigGrade() const;
	ScoreGrade GetSmallGrade() const;
	float GetCurrentWinFactor() const;
private:
	int mCurrentScore;
	int mWinScore;
	int mTotalScore;
	ScoreGrade mBigGrade;
	ScoreGrade mSmallGrade;
	SceneNode* mScoreNode;
	Text* mScoreText;
};