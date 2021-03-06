/***********************************************
 * File: GameLogicManager.cpp
 *
 * Author: LYG
 * Date: ���� 2018
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
#include "UIWindow.h"
#include "Log.h"

GameLogicManager* GameLogicManager::ThisInstance = nullptr;

GameLogicManager::GameLogicManager()
{
	mStartNewGame = false;
	mUpdrade = false;
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
	InitialiseAllResource();
	Scene* S = Scene::GetCurrentScene();
	bool ret = S->Initialise(hWnd);
	if (!ret)
	{
		return false;
	}

	return true;
}

GameLogicManager* GameLogicManager::GetInstance()
{
	if (ThisInstance == nullptr)
	{
		ThisInstance = new GameLogicManager;
	}
	return ThisInstance;
}

void GameLogicManager::ReleaseInstance()
{
	SAFE_DELETE(ThisInstance);
}

void GameLogicManager::StartNewGame()
{
	int ret = MessageBoxA(NULL, "Are you sure to start a new game?", "Game", MB_YESNO);
	if (ret == IDYES)
	{

	}
}

void GameLogicManager::NotifyStartNewGame(bool Upgrade)
{
	mStartNewGame = true;
	mUpdrade = Upgrade;
}

void GameLogicManager::StartNewGame(bool Upgrade)
{
	mPlayer->Reset();
	Timer::GetInstance()->Clear();
	Scene::GetCurrentScene()->ClearScene();
	Scene::GetCurrentScene()->CreateSceneContent();
	if (Upgrade)
	{

	}
	else
	{

	}
	mStartNewGame = false;
	mUpdrade = false;
}

void GameLogicManager::Update()
{
	if (mStartNewGame)
	{
		StartNewGame(mUpdrade);
	}
	Timer::ThisInstance->Update();
	// calculate frame rate
	static int FrameRate = 0;
	static float DeltaTime = 0.0f;
	DeltaTime += Timer::ThisInstance->GetDeltaFloat();
	FrameRate++;
	if (DeltaTime >= 1.0f)
	{
		DeltaTime = fmod(DeltaTime, 1.0f);
		Scene::GetCurrentScene()->SetRenderFrameRate(true);
		Scene::GetCurrentScene()->SetCurrentFrameRate(FrameRate);
		FrameRate = 0;
	}
	Scene::GetCurrentScene()->Update();
	mPlayer->Update();
}

void GameLogicManager::InitialiseAllResource()
{
	mPlayer->Initialise();
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
	else if (Key == VK_ESCAPE)
	{
		UIWindow* W = Scene::GetCurrentScene()->GetUIWindowManager()->GetSystemWindow();
		bool Visible = W->GetVisible();
		W->SetVisible(!Visible);
	}
}

void GameLogicManager::OnKeyUp(unsigned char Key)
{
	mPlayer->OnKeyUp(Key);
}

void GameLogicManager::OnLButtonDown(int x, int y, unsigned int wParam)
{
	mPlayer->OnLButtonDown(x, y, wParam);
	Scene::GetCurrentScene()->GetUIWindowManager()->OnLButtonDown(x, y, wParam);
}

void GameLogicManager::OnLButtonUp(int x, int y, unsigned int wParam)
{
	mPlayer->OnLButtonUp(x, y, wParam);
	Scene::GetCurrentScene()->GetUIWindowManager()->OnLButtonUp(x, y, wParam);
}

void GameLogicManager::OnRButtonDown(int x, int y, unsigned int wParam)
{
	mPlayer->OnRButtonDown(x, y, wParam);
	Scene::GetCurrentScene()->GetUIWindowManager()->OnRButtonDown(x, y, wParam);
}

void GameLogicManager::OnRButtonUp(int x, int y, unsigned int wParam)
{
	mPlayer->OnRButtonUp(x, y, wParam);
	Scene::GetCurrentScene()->GetUIWindowManager()->OnRButtonUp(x, y, wParam);
}

void GameLogicManager::OnLButtonDbclk(int x, int y, unsigned int wParam)
{
	mPlayer->OnLButtonDbclk(x, y, wParam);
	Scene::GetCurrentScene()->GetUIWindowManager()->OnLButtonDbclk(x, y, wParam);
}

void GameLogicManager::OnRButtonDbclk(int x, int y, unsigned int wParam)
{
	mPlayer->OnRButtonDbclk(x, y, wParam);
	Scene::GetCurrentScene()->GetUIWindowManager()->OnRButtonDbclk(x, y, wParam);
}

void GameLogicManager::OnMouseMove(int x, int y, unsigned int wParam)
{
	mPlayer->OnMouseMove(x, y, wParam);
	Scene::GetCurrentScene()->GetUIWindowManager()->OnMouseMove(x, y, wParam);
}