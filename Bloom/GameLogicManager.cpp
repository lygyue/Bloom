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

GameLogicManager::GameLogicManager()
{
	Scene* S = new Scene;
	Scene::SetCurrentScene(S);
	Timer* T = new Timer;
	Timer::ThisInstance = T;
}

GameLogicManager::~GameLogicManager()
{
	Scene* S = Scene::GetCurrentScene();
	SAFE_DELETE(S);
	Scene::SetCurrentScene(nullptr);
	SAFE_DELETE(Timer::ThisInstance);
	Timer::ThisInstance = nullptr;
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

}

void GameLogicManager::InitialiseAllResource()
{

}


void GameLogicManager::OnKeyDown(unsigned char Key)
{
	Camera* Cam = Scene::GetCurrentScene()->GetCurrentCamera();
	if (Key == 'A' || Key == 'a')
	{
		Cam->RollPitchYaw(0, -0.02, 0);
	}
	else if (Key == 'D' || Key == 'd')
	{
		Cam->RollPitchYaw(0, 0.02, 0);
	}
}

void GameLogicManager::OnKeyUp(unsigned char Key)
{

}

void GameLogicManager::OnLButtonDown(int x, int y, unsigned int wParam)
{

}

void GameLogicManager::OnLButtonUp(int x, int y, unsigned int wParam)
{

}

void GameLogicManager::OnRButtonDown(int x, int y, unsigned int wParam)
{

}

void GameLogicManager::OnRButtonUp(int x, int y, unsigned int wParam)
{

}

void GameLogicManager::OnLButtonDbclk(int x, int y, unsigned int wParam)
{

}

void GameLogicManager::OnRButtonDbclk(int x, int y, unsigned int wParam)
{

}

void GameLogicManager::OnMouseMove(int x, int y, unsigned int wParam)
{

}