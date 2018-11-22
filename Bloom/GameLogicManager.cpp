/***********************************************
 * File: GameLogicManager.cpp
 *
 * Author: LYG
 * Date: ËÄÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/
#include "GameLogicManager.h"
#include "Math/Vector3.h"
#include "Scene.h"
#include "Timer.h"
#include "EffectManager.h"
#include "Log.h"

GameLogicManager::GameLogicManager()
{
	Scene* S = new Scene;
	Scene::SetCurrentScene(S);
	S->mGameLogicManager = this;
	Timer* T = new Timer;
	Timer::ThisInstance = T;
	LogImpl::ThisInstance = new LogImpl(S->GetApplicationPath().c_str());
	mPlayer = new Player("MainPlayer");
}

GameLogicManager::~GameLogicManager()
{
	Scene* S = Scene::GetCurrentScene();
	SAFE_DELETE(S);
	Scene::SetCurrentScene(nullptr);
	SAFE_DELETE(Timer::ThisInstance);
	SAFE_DELETE(LogImpl::ThisInstance);
}

bool GameLogicManager::Initialise(HWND hWnd)
{
	Scene* S = Scene::GetCurrentScene();
	bool ret = S->Initialise(hWnd);
	if (!ret)
	{
		return false;
	}
	InitialiseAllResource();
	return true;
}

void GameLogicManager::StartNewGame()
{
	int ret = MessageBoxA(NULL, "Are you sure to start a new game?", "Game", MB_YESNO);
	if (ret == IDYES)
	{

	}
}

void GameLogicManager::Update()
{
	Timer::ThisInstance->Update();
	Scene::GetCurrentScene()->Update();
	mPlayer->Update();
}

void GameLogicManager::InitialiseAllResource()
{

}

Player* GameLogicManager::GetCurrentPlayer() const
{
	return mPlayer;
}

void GameLogicManager::OnKeyDown(unsigned char Key)
{
	mPlayer->OnKeyDown(Key);
	static BOOL PauseGame = FALSE;
	if (Key == VK_SPACE)
	{
		PauseGame = !PauseGame;
		Timer::GetInstance()->SetPauseTimer(PauseGame);
	}
}

void GameLogicManager::OnKeyUp(unsigned char Key)
{
	mPlayer->OnKeyUp(Key);
}

void GameLogicManager::OnLButtonDown(int x, int y, unsigned int wParam)
{
	mPlayer->OnLButtonDown(x, y, wParam);
}

void GameLogicManager::OnLButtonUp(int x, int y, unsigned int wParam)
{
	mPlayer->OnLButtonUp(x, y, wParam);
}

void GameLogicManager::OnRButtonDown(int x, int y, unsigned int wParam)
{
	mPlayer->OnRButtonDown(x, y, wParam);
}

void GameLogicManager::OnRButtonUp(int x, int y, unsigned int wParam)
{
	mPlayer->OnRButtonUp(x, y, wParam);
}

void GameLogicManager::OnLButtonDbclk(int x, int y, unsigned int wParam)
{
	mPlayer->OnLButtonDbclk(x, y, wParam);
}

void GameLogicManager::OnRButtonDbclk(int x, int y, unsigned int wParam)
{
	mPlayer->OnRButtonDbclk(x, y, wParam);
}

void GameLogicManager::OnMouseMove(int x, int y, unsigned int wParam)
{
	mPlayer->OnMouseMove(x, y, wParam);
}