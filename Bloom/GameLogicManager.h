/***********************************************
 * File: GameLogicManager.h
 *
 * Author: LYG
 * Date: ���� 2018
 *
 * Purpose:
 *
 * 
 **********************************************/
#pragma once
#include "GameDef.h"
#include "Common.h"
#include "Player.h"

class GameLogicManager
{
public:
	GameLogicManager();
	~GameLogicManager();

	bool Initialise(HWND hWnd);

	void StartNewGame();
	void Update();

	void NotifyStartNewGame(bool Upgrade);

	Player* GetCurrentPlayer() const;
	static GameLogicManager* GetInstance();
	static void ReleaseInstance();
public:
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
	void StartNewGame(bool Upgrade);
	void InitialiseAllResource();
private:
	bool mStartNewGame;
	bool mUpdrade;
	Player* mPlayer;
	static GameLogicManager* ThisInstance;
};