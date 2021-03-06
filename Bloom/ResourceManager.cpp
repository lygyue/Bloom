/*!
 * \file ResourceManager.cpp
 *
 * \author LYG
 * \date ���� 2018
 *
 * 
 */

#include "ResourceManager.h"
#include "Scene.h"
#include "Math/SimpleMath.h"
#include "Font.h"
#include <windows.h>

#define Game_BACKGROUND_PATH		"Resource\\BackGround\\"
#define Game_ASSETS_PATH			"Resource\\Assets\\"
#define Game_FONT_PATH				"Resource\\Fonts\\"
#define RESOURCE_WALL				"Wall.png"
#define RESOURCE_BLOOM				"Bloom.png"
#define RESOURCE_APPLES				"Apples.png"
#define RESOURCE_FLAG				"Flag.png"
#define RESOURCE_START				"start.png"
#define RESOURCE_STAR				"star1.png"

ResourceManager::ResourceManager()
{
	mBackGroundImageIndex = 0;
}

ResourceManager::~ResourceManager()
{

}

bool ResourceManager::Initialise()
{
	mAppPath = Scene::GetCurrentScene()->GetApplicationPath();
	mAssetsPath = mAppPath + Game_ASSETS_PATH;
	InitialiseBackGroundImages();
	return true;
}

void ResourceManager::InitialiseBackGroundImages()
{
	BOOL bFind = TRUE;
	WIN32_FIND_DATAA fileData;
	char szOldCurDir[MAX_PATH];
	GetCurrentDirectoryA(sizeof(szOldCurDir), szOldCurDir);
	char szDirPath[MAX_PATH];
	std::string AppPath = Scene::GetCurrentScene()->GetApplicationPath();
	memset(szDirPath, 0, sizeof(szDirPath));
	strcpy_s(szDirPath, MAX_PATH, AppPath.c_str());
	strcat_s(szDirPath, MAX_PATH, Game_BACKGROUND_PATH);
	SetCurrentDirectoryA(szDirPath);
	HANDLE hFind = FindFirstFileA("*", &fileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return;
	}
	while (bFind) 
	{
		// don't consider as a folder
		std::string Name = szDirPath;
		int nLen = (int)strlen(fileData.cFileName);
		if (nLen > 4)
		{
			char szExtern[5];
			memset(szExtern, 0, 5);
			strcpy_s(szExtern, 5, fileData.cFileName + nLen - 3);
			if (_stricmp(szExtern, "jpg") == 0)
			{
				Name += fileData.cFileName;
				mBackgroundImageArray.push_back(Name);
			}
		}
		bFind = FindNextFileA(hFind, &fileData);
	}
	SetCurrentDirectoryA(szOldCurDir);
	FindClose(hFind);
}

std::string ResourceManager::GetRandomBackGroundImagePath() const
{
	static int LastIndex = 0;
	int Index = 0;
	while (true)
	{
		Index = (int)RangeRandom(0.0f, float(mBackgroundImageArray.size() - 0.001));
		if (LastIndex != Index)
		{
			LastIndex = Index;
			break;
		}
	}
	return mBackgroundImageArray[Index];
}

std::string ResourceManager::GetBackGroundImageInOrder()
{
	return mBackgroundImageArray[mBackGroundImageIndex++];
}

std::string ResourceManager::GetApplesImagePath() const
{
	return mAssetsPath + RESOURCE_APPLES;
}

std::string ResourceManager::GetStarImagePath() const
{
	return mAssetsPath + RESOURCE_STAR;
}

std::string ResourceManager::GetStartImagePath() const
{
	return mAssetsPath + RESOURCE_START;
}

std::string ResourceManager::GetWallImagePath() const
{
	return mAssetsPath + RESOURCE_WALL;
}

std::string ResourceManager::GetFlagImagePath() const
{
	return mAssetsPath + RESOURCE_FLAG;
}

std::string ResourceManager::GetBloomImagePath() const
{
	return mAssetsPath + RESOURCE_BLOOM;
}

std::string ResourceManager::GetFontFullPath(FontType FT) const
{
	static std::string FontFileNames[FontMax] = {"MFDingHei.ttf", "MFFanSong.ttf", "MFJingYue.ttf", "MFJinHua.ttf", "MFLingYi.ttf", "MFMengYuan.ttf", "MFNianZhen.ttf", "MFSiYan.ttf"
													, "MFSuBai.ttf", "MFWenShang.ttf", "MFXingYan.ttf", "MFXiYue.ttf", "MFYiFeng.ttf", "MFYuYi.ttf", "MFZhuoHei.ttf"
													, "xindexingcao57.ttf"};

	std::string FileName = mAppPath + Game_FONT_PATH + FontFileNames[FT];

	return FileName;
}