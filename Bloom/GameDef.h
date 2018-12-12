/*!
 * \file GameDef.h
 *
 * \author LYG
 * \date ËÄÔÂ 2018
 *
 * 
 */
#pragma once

#define WINDOW_WIDTH		1600
#define WINDOW_HEIGHT		1024

// 120 seconds
#define GAME_TIME			120.0f
#define SCREEN_PASS_TIME	40.0f
#define SCREEN_SPEED		(2.0f / SCREEN_PASS_TIME)

#define	NEAR_PLANE			0.01f
#define	FAR_PLANE			10000.0f

#define FONT_IMAGE_SIZE		1024

#define MSAA_LEVEL			4

#define VERTICAL_SYNCHRONIZATION	1

enum ElementType
{
	ElementShader			= 1 << 0,
	ElementTexture			= 1 << 1,
	ElementMaterial			= 1 << 2,
	ElementMesh				= 1 << 3,
	ElementSceneNode		= 1 << 4,
	ElementFont				= 1 << 5,
	ElementText				= 1 << 6,
	ElementEffect			= 1 << 7,
	ElementAnimation		= 1 << 8,
};