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

class SceneNode;
class Collision;
class Player
{
	friend class GameLogicManager;
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
protected:
	Player(std::string Name);
	~Player();

	SceneNode* mPlayerSceneNode;
	Collision* mPlayerCollision;
	std::string mName;
	float mSpeed;
	unsigned int mMoveDirection;
};