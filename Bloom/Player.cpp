/***********************************************
 * File: Player.cpp
 *
 * Author: LYG
 * Date: Ê®Ò»ÔÂ 2018
 *
 * Purpose:
 *
 * 
 **********************************************/

#include "Player.h"
#include "GameDef.h"
#include "Math/Vector3.h"
#include "Timer.h"
#include "SceneNode.h"
#include "Scene.h"
#include "Collision.h"
#include "Score.h"
#include "Font.h"
#include "Text.h"
#include "Animation.h"
#include "GameLogicManager.h"

Player::Player(std::string Name)
{
	mSpeed = 4 * SCREEN_SPEED;
	mDefaultSpeed = mSpeed;
	mMaxSpeed = mSpeed * 4.0f;
	mName = Name;
	mMoveDirection = 0;
	mPlayerSceneNode = nullptr;
	mPlayerCollision = nullptr;
	mPlayerScore = nullptr;
}

Player::~Player()
{
	SAFE_DELETE(mPlayerScore);
}

std::string Player::GetName() const
{
	return mName;
}

void Player::SetPlayerSpeed(float Speed)
{
	mSpeed = Speed;
}

float Player::GetPlayerSpeed() const
{
	return mSpeed;
}

void Player::SetPlayerNode(SceneNode* SN)
{
	mPlayerSceneNode = SN;
}

SceneNode* Player::GetPlayerNode() const
{
	return mPlayerSceneNode;
}

void Player::SetPlayerCollision(Collision* C)
{
	mPlayerCollision = C;
}

Collision* Player::GetPlayerCollision() const
{
	return mPlayerCollision;
}

ScoreSystem* Player::GetScoreSystem() const
{
	return mPlayerScore;
}

void Player::Initialise()
{
	mPlayerScore = new ScoreSystem;
}

void Player::CreateFlowingText(Vector3 Pos, int PlayerScore)
{
	// Force the depth to the text depth
	Pos.z = RenderGroupManager::GetRenderGroupDepth(RenderGroup_TEXT);
	FontManager* FM = Scene::GetCurrentScene()->GetFontManager();
	Font* F = FM->GetCommonFont();
	TextManager* TM = Scene::GetCurrentScene()->GetTextManager();
	std::wstring str;
	if (PlayerScore > 0)
	{
		str = L"+";
	}
	str += std::to_wstring(PlayerScore);
	Text* T = TM->CreateText(str, F, TM->GetTextColorByType(FlowingTextColor));
	SceneNode* RootNode = Scene::GetCurrentScene()->GetRootSceneNode();
	EffectManager* EM = Scene::GetCurrentScene()->GetEffectManager();
	SceneNode* S = RootNode->CreateChild(Pos, Quaternion::IDENTITY, Vector3(1, 1, 1), RenderGroup_TEXT);
	S->AttachMesh(T->GetAttachMesh());
	EffectTextFadeOut* E = (EffectTextFadeOut*)EM->CreateEffect(Effect_Text_Fade_Out, FLOWING_TEXT_TIME, false, true, 0.0f, T->GetAttachMesh(), S, nullptr, 0, this);
	E->SetTextColor(TM->GetTextColorByType(FlowingTextColor));
	mFlowingTextArray.push_back(T);
	mFlowingNodeArray.push_back(S);
	// Build flowing text animation
	AnimationManager* AM = Scene::GetCurrentScene()->GetAnimationManager();
	FlowingTextAnimation* Ani = static_cast<FlowingTextAnimation*>(AM->CreateAnimation(Animation_FlowingText));
	Ani->SetIsAutoDestroy(true);
	Ani->SetIsLoop(false);
	Ani->AttachNode(S);
	Ani->SetAnimationLength(FLOWING_TEXT_TIME);
	Ani->SetAcceleration(RangeRandom(0.01f, 0.2f));
}

void Player::RefreshSpeed(int PlayerScore)
{
	if (PlayerScore > 0)
	{
		float Factor = float(PlayerScore) * 0.001f + 1.0f;
		mSpeed *= Factor;
		if (mSpeed > mMaxSpeed)
		{
			mSpeed = mMaxSpeed;
		}
	}
	else
	{
		// Been bullet shot, and turn to the default speed
		mSpeed = mDefaultSpeed;
	}
}

void Player::Update()
{
	if (mPlayerSceneNode == nullptr) return;

	float Delta = Timer::GetInstance()->GetDeltaFloat();
	if (mMoveDirection > 0)
	{
		// calculate the move direction
		Vector3 V[4] = {Vector3(-1, 0, 0), Vector3(0, 1, 0), Vector3(1, 0, 0), Vector3(0, -1, 0)};
		Vector3 Direction = Vector3::ZERO;
		for (int i = 0; i < 4; i++)
		{
			if ((1 << i) & mMoveDirection)
			{
				Direction += V[i];
			}
		}
		Direction.normalise();
		Direction *= mSpeed;
		Direction *= Delta;
		mPlayerSceneNode->Translate(Direction);
	}

	if (Timer::GetInstance()->GetDelta() > 0)
	{
		// do a collision detect.
		std::vector<Collision*> Result;
		CollisionManager* CM = Scene::GetCurrentScene()->GetCollisionManager();
		Result = CM->CalculateCollisionList(mPlayerCollision);
		for each(Collision* C in Result)
		{
			BlockPro BP = C->GetBlockProperty();
			switch (BP)
			{
			case Block_Apple_Red:
			case Block_Apple_Yellow:
			case Block_Circle_Carmine:
			case Block_Circle_Yellow:
			{
				// they do the same action
				mPlayerScore->CollisionTo(BP);
				int Score = mPlayerScore->GetCollisionScore(C);
				CreateFlowingText(mPlayerSceneNode->GetWorldPosition(), Score);
				RefreshSpeed(Score);
				SceneNode* SN = C->GetAttachSceneNode();
				SN->GetParentSceneNode()->RemoveAndDestroyChild(SN);
				CM->DestroyCollision(C);
				break;
			}
			case Block_Bullet_DecreaseLife:
			{
				mPlayerScore->CollisionTo(BP);
				C->SetVisible(false);
				// Create Flowing Text
				int Score = mPlayerScore->GetCollisionScore(C);
				CreateFlowingText(mPlayerSceneNode->GetWorldPosition(), Score);
				RefreshSpeed(Score);
				// set a timer to restart the collision
				Timer::GetInstance()->AddOnceTimer(this, 0, BULLET_COLLISION_TIME, C);
				break;
			}
			case Block_Die:
			{
				GameLogicManager::GetInstance()->StartNewGame(false);
				mPlayerScore->Reset();
				break;
			}
			case Block_Final_Flag:
			{
				if (mPlayerScore->IsWin())
				{
					if (!mPlayerScore->AddGrade())
					{
						// go into the ending.
					}
					GameLogicManager::GetInstance()->StartNewGame(true);
				}
				else
				{
					GameLogicManager::GetInstance()->StartNewGame(false);
				}
				mPlayerScore->Reset();
				break;
			}
			default:
				break;
			}
		}
	}
}

void Player::OnKeyDown(unsigned char Key)
{
	switch (Key)
	{
	case VK_LEFT:
	{
		mMoveDirection |= MD_Left;
		break;
	}
	case VK_UP:
	{
		mMoveDirection |= MD_Up;
		break;
	}
	case VK_RIGHT:
	{
		mMoveDirection |= MD_Right;
		break;
	}
	case VK_DOWN:
	{
		mMoveDirection |= MD_Down;
		break;
	}
	default:
		break;
	}
}

void Player::OnKeyUp(unsigned char Key)
{
	switch (Key)
	{
	case VK_LEFT:
	{
		mMoveDirection &= (~MD_Left);
		break;
	}
	case VK_UP:
	{
		mMoveDirection &= (~MD_Up);
		break;
	}
	case VK_RIGHT:
	{
		mMoveDirection &= (~MD_Right);
		break;
	}
	case VK_DOWN:
	{
		mMoveDirection &= (~MD_Down);
		break;
	}
	default:
		break;
	}
}

void Player::OnLButtonDown(int x, int y, unsigned int wParam)
{

}

void Player::OnLButtonUp(int x, int y, unsigned int wParam)
{

}

void Player::OnRButtonDown(int x, int y, unsigned int wParam)
{

}

void Player::OnRButtonUp(int x, int y, unsigned int wParam)
{

}

void Player::OnLButtonDbclk(int x, int y, unsigned int wParam)
{

}

void Player::OnRButtonDbclk(int x, int y, unsigned int wParam)
{

}

void Player::OnMouseMove(int x, int y, unsigned int wParam)
{

}

void Player::OnDestroy(Effect* E)
{
	// Stop rendering the text mesh. and destroy sometimes later. 
	// Because there is a flowing text animation running, you must wait for the animation been destroyed.
	mFlowingTextArray[0]->GetAttachMesh()->SetVisible(false);
	Timer::GetInstance()->AddOnceTimer(this, 1, 100);
}

void Player::OnTimer(unsigned int EventID, void* UserData)
{
	if (EventID == 0)
	{
		Collision* C = static_cast<Collision*>(UserData);
		C->SetVisible(true);
	}
	if (EventID == 1)
	{
		TextManager* TM = Scene::GetCurrentScene()->GetTextManager();
		TM->DestroyText(mFlowingTextArray[0]);
		mFlowingTextArray.erase(mFlowingTextArray.begin());
		SceneNode* S = mFlowingNodeArray[0];
		SceneNode* Parent = S->GetParentSceneNode();
		Parent->RemoveAndDestroyChild(S);
		mFlowingNodeArray.erase(mFlowingNodeArray.begin());
	}
}