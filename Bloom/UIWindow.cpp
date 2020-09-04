/***********************************************
 * File: UIWindow.cpp
 *
 * Author: LYG
 * Date: 一月 2019
 *
 * Purpose:
 *
 * 
 **********************************************/

#include "UIElement.h"
#include "UIWindow.h"
#include "Texture.h"
#include "Scene.h"
#include "SceneNode.h"
#include "Math/PerlinNoise.h"
#include "Math/Vector4.h"
#include "RenderSystem.h"
#include "Material.h"
#include "Mesh.h"
#include "Animation.h"
#include "RenderGroupManager.h"

UIWindow::UIWindow()
{

}

UIWindow::~UIWindow()
{

}

void UIWindow::SetVisible(bool Visible)
{
	__super::SetVisible(Visible);

	// notify the manager which been actived or not
	UIWindowManager* UIM = Scene::GetCurrentScene()->GetUIWindowManager();
	UIM->NotifyActive(this, Visible);
}

void UIWindow::Render(Matrix4& WorldTransform)
{
	if (GetVisible() == false) return;

	for (size_t i = 0; i < mElementArray.size(); i++)
	{
		mElementArray[i]->Render(WorldTransform);
	}
}

void UIWindow::OnLButtonDown(Vector2 ScreenPos)
{
	// first, active this window to the top
	SetVisible(true);
	UIElement* E = IntersectElement(ScreenPos);
	if (E)
	{
		E->OnLButtonDown(ScreenPos);
	}
}

void UIWindow::OnLButtonUp(Vector2 ScreenPos)
{
	UIElement* E = IntersectElement(ScreenPos);
	if (E)
	{
		E->OnLButtonUp(ScreenPos);
	}
}
void UIWindow::OnRButtonDown(Vector2 ScreenPos)
{
	UIElement* E = IntersectElement(ScreenPos);
	if (E)
	{
		E->OnRButtonDown(ScreenPos);
	}
}
void UIWindow::OnRButtonUp(Vector2 ScreenPos)
{
	UIElement* E = IntersectElement(ScreenPos);
	if (E)
	{
		E->OnRButtonUp(ScreenPos);
	}
}
void UIWindow::OnMouseMove(Vector2 ScreenPos)
{
	UIElement* E = IntersectElement(ScreenPos);
	if (E)
	{
		E->OnMouseMove(ScreenPos);
	}
}
void UIWindow::OnLButtonDbclk(Vector2 ScreenPos)
{
	UIElement* E = IntersectElement(ScreenPos);
	if (E)
	{
		E->OnLButtonDbclk(ScreenPos);
	}
}
void UIWindow::OnRButtonDbclk(Vector2 ScreenPos)
{
	UIElement* E = IntersectElement(ScreenPos);
	if (E)
	{
		E->OnRButtonDbclk(ScreenPos);
	}
}

UIElement* UIWindow::IntersectElement(Vector2 ScreenPos) const
{
	for (int i = (int)mElementArray.size() - 1; i >= 0; i--)
	{
		Vector2 RectMax = Vector2((float*)&(mElementArray[i]->GetMax()));
		Vector2 RectMin = Vector2((float*)&(mElementArray[i]->GetMin()));
		if (ScreenPos > RectMax && ScreenPos < RectMax)
		{
			return mElementArray[i];
		}
	}
	return nullptr;
}

void UIWindow::Update()
{

}

bool UIWindow::AddElement(UIElement* E)
{
	if (mElementArray.empty())
	{
		if (E->GetElementType() != ElementBackGroundSheet)
		{
			return false;
		}
	}
	mElementArray.push_back(E);
	E->SetAttachWindow(this);
	return true;
}

bool UIWindow::RemoveElement(int Index)
{
	// background sheet can not be remove
	if (Index >= (int)mElementArray.size() || Index <= 0)
	{
		return false;
	}
	mElementArray.erase(mElementArray.begin() + Index);
	return true;
}

unsigned int UIWindow::GetElementCounts() const
{
	return (unsigned int)mElementArray.size();
}

UIElement* UIWindow::GetElementByIndex(int Index) const
{
	return mElementArray[Index];
}

Vector2 UIWindow::GetMax() const
{
	Vector3 V = mElementArray[0]->GetMax();
	return Vector2(V.x, V.y);
}

Vector2 UIWindow::GetMin() const
{
	Vector3 V = mElementArray[0]->GetMin();
	return Vector2(V.x, V.y);
}

//-----------------------------------------------------------------------
UIWindowManager::UIWindowManager()
{
	mBackGroundSheetTexture = nullptr;
	mSystemWindow = nullptr;
	mConfigWindow = nullptr;
}

UIWindowManager::~UIWindowManager()
{

}

void UIWindowManager::NotifyActive(UIWindow* W, bool Visible/* = true*/)
{
	for (size_t i = 0; i < mShowWindowArray.size(); i++)
	{
		if (W == mShowWindowArray[i])
		{
			if (Visible)
			{
				// the last one, do nothing
				if (i == mShowWindowArray.size() - 1)
				{
					return;
				}
				// move to the last
				mShowWindowArray.erase(mShowWindowArray.begin() + i);
				mShowWindowArray.push_back(W);
				return;
			}
			else
			{
				mShowWindowArray.erase(mShowWindowArray.begin() + i);
				return;
			}
		}
	}
	// not finded
	if(Visible)
		mShowWindowArray.push_back(W);
}

D3d11Texture* UIWindowManager::GetBackGroundSheetTexture()
{
	if (mBackGroundSheetTexture == nullptr)
	{
		TextureManager* TexMgr = Scene::GetCurrentScene()->GetTextureManager();
		RenderSystemD3D11* RS = Scene::GetCurrentScene()->GetRenderSystem();
		unsigned char* TextureData = new unsigned char[BackGroundSheetTextureSize * BackGroundSheetTextureSize];
		mBackGroundSheetTexture = TexMgr->CreateTexture("BackGroundSheetTexturePerlinNoise", RS->GetD3d11Device(), BackGroundSheetTextureSize, BackGroundSheetTextureSize, DXGI_FORMAT_R8_UNORM, 1, true);
		// fill the texture data with perlin noise
		BuildPerlinTexture(TextureData, BackGroundSheetTextureSize, BackGroundSheetTextureSize, rand());
		mBackGroundSheetTexture->BlitToTexture(TextureData, BackGroundSheetTextureSize * BackGroundSheetTextureSize);
		SAFE_DELETE_ARRAY(TextureData);
	}
	return mBackGroundSheetTexture;
}

void UIWindowManager::Update()
{
	for (size_t i = 0; i < mShowWindowArray.size(); i++)
	{
		mShowWindowArray[i]->Update();
	}
}

void UIWindowManager::Clear()
{
	mSystemWindow = nullptr;
	mConfigWindow = nullptr;
	mShowWindowArray.clear();
	for (size_t i = 0; i < mWindowArray.size(); i++)
	{
		SAFE_DELETE(mWindowArray[i]);
	}
	mWindowArray.clear();
	for (size_t i = 0; i < mUIElementArray.size(); i++)
	{
		SAFE_DELETE(mUIElementArray[i]);
	}
	mUIElementArray.clear();
	TextureManager* TexMgr = Scene::GetCurrentScene()->GetTextureManager();
	TexMgr->DestroyTexture(mBackGroundSheetTexture);
	mBackGroundSheetTexture = nullptr;
}


UIWindow* UIWindowManager::IntersectWindow(int x, int y) const
{
	Vector2 v = Scene::WindowsCoordToProjectionCoord(x, y);
	if (mShowWindowArray.size() == 0) return nullptr;

	for (int i = (int)mShowWindowArray.size() - 1; i >= 0; i--)
	{
		Vector2 RectMax = mShowWindowArray[i]->GetMax();
		Vector2 RectMin = mShowWindowArray[i]->GetMin();
		if ( v > RectMax && v <  RectMax)
		{
			return mShowWindowArray[i];
		}
	}

	return nullptr;
}

void UIWindowManager::OnLButtonDown(int x, int y, unsigned int wParam)
{
	UIWindow* W = IntersectWindow(x, y);
	if (W)
	{
		Vector2 ScreenPos = Scene::WindowsCoordToProjectionCoord(x, y);
		W->OnLButtonDown(ScreenPos);
	}
}
void UIWindowManager::OnLButtonUp(int x, int y, unsigned int wParam)
{
	UIWindow* W = IntersectWindow(x, y);
	if (W)
	{
		Vector2 ScreenPos = Scene::WindowsCoordToProjectionCoord(x, y);
		W->OnLButtonUp(ScreenPos);
	}
}
void UIWindowManager::OnRButtonDown(int x, int y, unsigned int wParam)
{
	UIWindow* W = IntersectWindow(x, y);
	if (W)
	{
		Vector2 ScreenPos = Scene::WindowsCoordToProjectionCoord(x, y);
		W->OnRButtonDown(ScreenPos);
	}
}
void UIWindowManager::OnRButtonUp(int x, int y, unsigned int wParam)
{
	UIWindow* W = IntersectWindow(x, y);
	if (W)
	{
		Vector2 ScreenPos = Scene::WindowsCoordToProjectionCoord(x, y);
		W->OnRButtonUp(ScreenPos);
	}
}
void UIWindowManager::OnMouseMove(int x, int y, unsigned int wParam)
{
	UIWindow* W = IntersectWindow(x, y);
	if (W)
	{
		Vector2 ScreenPos = Scene::WindowsCoordToProjectionCoord(x, y);
		W->OnMouseMove(ScreenPos);
	}
}
void UIWindowManager::OnLButtonDbclk(int x, int y, unsigned int wParam)
{
	UIWindow* W = IntersectWindow(x, y);
	if (W)
	{
		Vector2 ScreenPos = Scene::WindowsCoordToProjectionCoord(x, y);
		W->OnLButtonDbclk(ScreenPos);
	}
}
void UIWindowManager::OnRButtonDbclk(int x, int y, unsigned int wParam)
{
	UIWindow* W = IntersectWindow(x, y);
	if (W)
	{
		Vector2 ScreenPos = Scene::WindowsCoordToProjectionCoord(x, y);
		W->OnRButtonDbclk(ScreenPos);
	}
}
UIWindow* UIWindowManager::CreateUIWindow(std::string Name, bool Visible)
{
	for (size_t i = 0; i < mWindowArray.size(); i++)
	{
		if (mWindowArray[i]->GetName() == Name)
		{
			mWindowArray[i]->SetVisible(Visible);
			return mWindowArray[i];
		}
	}
	UIWindow* W = new UIWindow;
	W->mName = Name;
	W->SetVisible(Visible);
	mWindowArray.push_back(W);

	return W;
}

void UIWindowManager::DestroyUIWindows(UIWindow* W)
{
	DestroyUIWindows(W->GetName());
}

void UIWindowManager::DestroyUIWindows(std::string Name)
{
	for (int i = (int)mWindowArray.size() - 1; i >= 0; i--)
	{
		if (mWindowArray[i]->GetName() == Name)
		{
			SAFE_DELETE(mWindowArray[i]);
			mWindowArray.erase(mWindowArray.begin() + i);
			break;
		}
	}
	for (int i = (int)mShowWindowArray.size() - 1; i >= 0; i--)
	{
		if (mShowWindowArray[i]->GetName() == Name)
		{
			mShowWindowArray.erase(mShowWindowArray.begin() + i);
			break;
		}
	}
}

UIElement* UIWindowManager::CreateUIElementBackGroundSheet(int Width, int Height)
{
	UIElement* E = new UIElementBackGroundSheet(Width, Height);

	mUIElementArray.push_back(E);
	return E;
}

void UIWindowManager::DestroyUIElement(UIElement* E)
{
	for (int i = (int)mUIElementArray.size() - 1; i >= 0; i--)
	{
		if (mUIElementArray[i] == E)
		{
			SAFE_DELETE(E);
			mUIElementArray.erase(mUIElementArray.begin() + i);
			break;
		}
	}
}

UIWindow* UIWindowManager::GetSystemWindow()
{
	if (mSystemWindow == nullptr)
	{
		mSystemWindow = CreateUIWindow("UIWindowManager_SystemWindow", false);
		// build system windows
		int BackGroundWidth = 300;
		int BackGroundHeight = 500;
		// there are three buttons：
		// 界面配置
		// 操作配置
		// 退出游戏
		int ButtonWidth = 260;
		int ButtonHeight = 140;
		// not need to save the pointer of the material, animation, mesh...which will be destroy at the end.
		UIElement* E = CreateUIElementBackGroundSheet(BackGroundWidth, BackGroundHeight);
		// create mesh and material
		MaterialManager* MM = Scene::GetCurrentScene()->GetMaterialManager();
		MeshManager* MeshM = Scene::GetCurrentScene()->GetMeshManager();
		Material* Mat = MM->CreateMaterial("System_BackGroundSheet", SimpleUVAnimation);
		Mat->SetDepthFunc(D3D11_COMPARISON_ALWAYS);
		Mat->ReBuild();
		AnimationManager* AM = Scene::GetCurrentScene()->GetAnimationManager();
		UIBackGroundAnimation* Ani = (UIBackGroundAnimation*)AM->CreateAnimation("SystemBackGroundSheetAnimation", Animation_UI_UV);
		// calculate random range
		float UMax = float(BackGroundSheetTextureSize - BackGroundWidth) / float(BackGroundSheetTextureSize) - 0.1f;
		float VMax = float(BackGroundSheetTextureSize - BackGroundHeight) / float(BackGroundSheetTextureSize) - 0.1f;
		float z = RenderGroupManager::GetRenderGroupDepth(RenderGroup_UI);
		float SegTime = 2.0f;
		Vector2 FirstUV = Vector2(RangeRandom(0.0f, UMax), RangeRandom(0.0f, VMax));
		Ani->AddPoint(Vector3(0, 0, z), SegTime * 0.0f);
		Vector2 Temp = Vector2(RangeRandom(0.1f, UMax), RangeRandom(0.1f, VMax));
		Temp -= FirstUV;
		Ani->AddPoint(Vector3(Temp.x, Temp.y, z), SegTime * 1.0f);
		Temp = Vector2(RangeRandom(0.1f, UMax), RangeRandom(0.1f, VMax));
		Temp -= FirstUV;
		Ani->AddPoint(Vector3(Temp.x, Temp.y, z), SegTime * 2.0f);
		Temp = Vector2(RangeRandom(0.1f, UMax), RangeRandom(0.1f, VMax));
		Temp -= FirstUV;
		Ani->AddPoint(Vector3(Temp.x, Temp.y, z), SegTime * 3.0f);
		Temp = Vector2(RangeRandom(0.1f, UMax), RangeRandom(0.1f, VMax));
		Temp -= FirstUV;
		Ani->AddPoint(Vector3(Temp.x, Temp.y, z), SegTime * 4.0f);
		Ani->AddRoundPoint(SegTime * 5.0f);
		Ani->AttachMaterial(Mat);
		Ani->SetStyleColor(*(GetUIStyleColor()));
		Ani->SetIsLoop(true);
		Mat->SetTexture(GetBackGroundSheetTexture());
		Vector2 UVOffset = Vector2(float(BackGroundWidth) / float(BackGroundSheetTextureSize), float(BackGroundHeight) / float(BackGroundSheetTextureSize));
		Vector2 PosOffset = Scene::WindowsCoordToWindowsPercent(BackGroundWidth, BackGroundHeight);
		Vector2 UV[4] = { FirstUV, Vector2(FirstUV.x + UVOffset.x, FirstUV.y), FirstUV + UVOffset , Vector2(FirstUV.x, FirstUV.y + UVOffset.y)};
		Vector3 Pos[4];
 		Pos[0] = Vector3(-PosOffset.x, PosOffset.y, 0);
 		Pos[1] = Vector3(PosOffset.x, PosOffset.y, 0);
 		Pos[2] = Vector3(PosOffset.x, -PosOffset.y, 0);
 		Pos[3] = Vector3(-PosOffset.x, -PosOffset.y, 0);
		Mesh* M = MeshM->CreateQuad(Pos, UV);
		M->SetMaterial(Mat);
		E->SetAttachWindow(mSystemWindow);
		E->SetElementMesh(M);
		mSystemWindow->AddElement(E);
		SceneNode* SN = Scene::GetCurrentScene()->GetRootSceneNode()->CreateChild(Vector3(0, 0, 0), Quaternion::IDENTITY, Vector3(1, 1, 1), RenderGroup_UI);

		SN->AttachRenderable(mSystemWindow);
	}
	return mSystemWindow;
}

UIWindow* UIWindowManager::GetConfigWindows()
{
	if (mConfigWindow == nullptr)
	{
		mConfigWindow = CreateUIWindow("UIWindowManager_ConfigWindow", false);
		// build config window

	}
	return mConfigWindow;
}

Vector4* UIWindowManager::GetUIStyleColor() const
{
	// Pink
	static Vector4 Col = Vector4(249.0f / 255.0f, 204.0f / 255.0f, 226.0f / 255.0f, 1.0f);
	return &Col;
}