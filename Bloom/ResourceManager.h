/*!
 * \file ResourceManager.h
 *
 * \author LYG
 * \date ËÄÔÂ 2018
 *
 * 
 */
#pragma once
#include <string>
#include <windows.h>
#include "Common.h"
#include "GameDef.h"
#include <vector>

class ResourceManager
{
	friend class Scene;
public:
	std::string GetRandomBackGroundImagePath() const;
	std::string GetBackGroundImageInOrder();
	std::string GetWallImagePath() const;
	std::string GetStartImagePath() const;
	std::string GetStarImagePath() const;
	std::string GetFlagImagePath() const;
	std::string GetBloomImagePath() const;
	std::string GetApplesImagePath() const;
protected:
	ResourceManager();
	~ResourceManager();

	bool Initialise();
	void InitialiseBackGroundImages();
private:
	std::string mAppPath;
	std::string mAssetsPath;
	std::vector<std::string> mBackgroundImageArray;
	int mBackGroundImageIndex;
};
