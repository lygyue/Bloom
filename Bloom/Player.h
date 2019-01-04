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
#include "Math/Vector3.h"
#include "EffectManager.h"
#include <vector>

class SceneNode;
class Collision;
class ScoreSystem;
class Text;
class Player : public ITimerListener ,public Effect::EffectListener
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

	void SetHangUpState(bool Flag);
	bool GetHangUpState() const;

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
	// override from Effect::EffectListener
	virtual void OnDestroy(Effect* E) override;

	virtual void OnTimer(unsigned int EventID, void* UserData) override;
protected:
	ScoreSystem* GetScoreSystem() const;
protected:
	Player(std::string Name);
	~Player();

	void Initialise();
	void CreateFlowingText(Vector3 Pos, int PlayerScore);
	void RefreshSpeed(int PlayerScore);
	void Reset();

	SceneNode* mPlayerSceneNode;
	Collision* mPlayerCollision;
	ScoreSystem* mPlayerScore;
	std::vector<Text*> mFlowingTextArray;
	std::vector<SceneNode*> mFlowingNodeArray;
	std::string mName;
	float mSpeed;
	float mDefaultSpeed;
	float mMaxSpeed;
	unsigned int mMoveDirection;
	bool mHangingUp;
};