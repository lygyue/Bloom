/***********************************************
 * File: Player.cpp
 *
 * Author: LYG
 * Date: ʮһ�� 2018
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

Player::Player(std::string Name)
{
	mSpeed = 2 * SCREEN_SPEED;
	mName = Name;
	mMoveDirection = 0;
	mPlayerSceneNode = nullptr;
	mPlayerCollision = nullptr;
}

Player::~Player()
{

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
			if (C->GetBlockProperty() == Block_Apple)
			{
				SceneNode* SN = C->GetAttachSceneNode();
				SN->GetParentSceneNode()->RemoveAndDestroyChild(SN);
				CM->DestroyCollision(C);
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