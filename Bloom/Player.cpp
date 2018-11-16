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

Player::Player(std::string Name)
{
	mSpeed = 2 * SCREEN_SPEED;
	mName = Name;
	mMoveDirection = 0;
	mPlayerSceneNode = nullptr;
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