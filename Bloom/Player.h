/***********************************************
 * File: Player.h
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
#include "Timer.h"

class SceneNode;
class Collision;
class ScoreSystem;
class Player : public ITimerListener
{
	friend class GameLogicManager;
	friend class Scene;
	enum MoveDirection
	{
		MD_Left = 1 << 0,
		MD_Up = 1 << 1,
		MD_Right = 1 << 2,
		MD_Down = 1 << 3,
	};
public:
	std::string GetName() const;
	
	void SetPlayerSpeed(float Speed);
	float GetPlayerSpeed() const;

	void SetPlayerNode(SceneNode* SN);
	SceneNode* GetPlayerNode() const;

	void SetPlayerCollision(Collision* C);
	Collision* GetPlayerCollision() const;

	void Update();

	void OnKeyDown(unsigned char Key);
	void OnKeyUp(unsigned char Key);
	void OnLButtonDown(int x, int y, unsigned int wParam);
	void OnLButtonUp(int x, int y, unsigned int wParam);
	void OnRButtonDown(int x, int y, unsigned int wParam);
	void OnRButtonUp(int x, int y, unsigned int wParam);
	void OnMouseMove(int x, int y, unsigned int wParam);
	void OnLButtonDbclk(int x, int y, unsigned int wParam);
	void OnRButtonDbclk(int x, int y, unsigned int wParam);

public:
	virtual void OnTimer(unsigned int EventID, void* UserData) override;
protected:
	ScoreSystem* GetScoreSystem() const;
protected:
	Player(std::string Name);
	~Player();

	void Initialise();

	SceneNode* mPlayerSceneNode;
	Collision* mPlayerCollision;
	ScoreSystem* mPlayerScore;
	std::string mName;
	float mSpeed;
	unsigned int mMoveDirection;
};