/*!
 * \file Scene.cpp
 *
 * \author LYG
 * \date 四月 2018
 *
 * 
 */
#include "Scene.h"
#include "Math/Vector2.h"
#include "Timer.h"
#include "Animation.h"
#include "GameLogicManager.h"
#include "Collision.h"
#include "Font.h"
#include "Text.h"
#include "UIWindow.h"
#include "ResourceManager.h"
#include "RenderSystem.h"
#include "SceneNode.h"
#include "Score.h"
#include "Material.h"
#include "Mesh.h"
#include "Particle.h"

Scene* Scene::CurrentScene = nullptr;
Scene::Scene()
{
	BuildApplicationPath();
	mHwnd = nullptr;
	mBackGroundNode = nullptr;
	mRootSceneNode = new SceneNode("Root_Scene_Node");
	mRenderSystem = new RenderSystemD3D11;
	mRenderGroupManager = new RenderGroupManager;
	mResourceManager = new ResourceManager;
	mMaterialManager = new MaterialManager;
	mTextureManager = new TextureManager;
	mEffectManager = new EffectManager;
	mMeshManager = new MeshManager;
	mCamera = new Camera;
	mCollisionManager = new CollisionManager;
	mAnimationManager = new AnimationManager;
	mFontManager = new FontManager;
	mTextManager = new TextManager;
	mUIWindowsManager = new UIWindowManager;
	mParticleManager = new ParticleManager;
	mGameLogicManager = nullptr;
	mCurrentFrameRate = 0;
	mRenderFrameRate = false;
	mWindowWidth = 0;
	mWindowHeight = 0;
	mNodeFollowCamera = nullptr;
	mFrameRateText = nullptr;
	mFrameRateTextTitle = nullptr;
	mFrameRateTextNode = nullptr;
	mFrameRateTextTitleNode = nullptr;
	mCurrentPoemIndex = 0;
	mCurrentGameTime = 0.0f;
}

Scene::~Scene()
{
	SAFE_DELETE(mRootSceneNode);
	SAFE_DELETE(mRenderSystem);
	SAFE_DELETE(mRenderGroupManager);
	SAFE_DELETE(mResourceManager);
	SAFE_DELETE(mEffectManager);
	SAFE_DELETE(mCamera);
	SAFE_DELETE(mCollisionManager);
	SAFE_DELETE(mAnimationManager);
	SAFE_DELETE(mFontManager);
	SAFE_DELETE(mTextManager);
	SAFE_DELETE(mUIWindowsManager);
	SAFE_DELETE(mParticleManager);

	// must be at the release end order because this is the base element of the scene
	SAFE_DELETE(mMeshManager);
	SAFE_DELETE(mMaterialManager);
	SAFE_DELETE(mTextureManager);
	// don't need to delete
	mBackGroundNode = nullptr;
}

void Scene::BuildApplicationPath()
{
	// Get Current Directory
	char Temp[MAX_PATH];
	memset(Temp, 0, MAX_PATH);

	GetModuleFileNameA(NULL, Temp, MAX_PATH);
	int nLen = (int)strlen(Temp);
	while (nLen)
	{
		if (Temp[nLen] == '\\' || Temp[nLen] == '/')
		{
			break;
		}
		Temp[nLen--] = '\0';
	}
	mApplicationPath = Temp;
}

bool Scene::Initialise(HWND hWnd)
{
	mHwnd = hWnd;
	RECT rc;
	GetClientRect(hWnd, &rc);
	mWindowWidth = rc.right - rc.left;
	mWindowHeight = rc.bottom - rc.top;
	mCamera->SetProjectionParameters(DegreesToRadians(90), float(mWindowWidth) / float(mWindowHeight), NEAR_PLANE, FAR_PLANE);
	if (mRenderSystem->Initialise(mWindowWidth, mWindowHeight, mHwnd) == false)
	{
		return false;
	}
	if (mResourceManager->Initialise() == false)
	{
		return false;
	}
	mMaterialManager->Initialise();
	CreateSceneContent();
	return true;
}

void Scene::CreateSceneContent()
{
	mBackGroundNode = mRootSceneNode->CreateChild("BackGround_Node", Vector3(0, 0, RenderGroupManager::GetRenderGroupDepth(RenderGroup_BackGroud)), Quaternion::IDENTITY, Vector3(1, 1, 1), RenderGroup_BackGroud);
	mNodeFollowCamera = mRootSceneNode->CreateChild("Follow_Camera_Node", Vector3(0, 0, 0), Quaternion::IDENTITY, Vector3(1, 1, 1), RenderGroup_TEXT);
	CreateBackGround();
	CreateFrameRateText();
	InitialiseScene();
	return;
}

void Scene::ClearScene()
{
	mCurrentGameTime = 0.0f;
	mFrameRateText = nullptr;
	mFrameRateTextTitle = nullptr;
	mNodeFollowCamera = nullptr;
	mFrameRateTextNode = nullptr;
	mFrameRateTextTitleNode = nullptr;
	mCameraAnimation = false;
	mRootSceneNode->DestroyAllChild();
	mRenderGroupManager->Clear();
	mEffectManager->Clear();
	mCamera->SetPosition(XMVectorSet(0, 0, 0, 0));
	mTextManager->Clear();
	mFontManager->Clear();
	mUIWindowsManager->Clear();
	mCollisionManager->Clear();
	mAnimationManager->Clear();
	// must be at the release end order because this is the base element of the scene
	mMeshManager->Clear();
	mMaterialManager->Clear();
	mTextureManager->Clear();
}

void Scene::SetCameraAnimation(bool StartAnimation)
{
	mCameraAnimation = StartAnimation;
}

bool Scene::GetCameraAnimation() const
{
	return mCameraAnimation;
}

void Scene::Update()
{
	mEffectManager->Update();
	mAnimationManager->Update();
	mUIWindowsManager->Update();
	mCurrentGameTime += Timer::GetInstance()->GetDeltaFloat();
	float BeginTime = 35.0f;
	if (mCurrentGameTime > BeginTime) mCameraAnimation = true;
	if (mCurrentGameTime > BeginTime + GAME_TIME - SCREEN_PASS_TIME / 2) mCameraAnimation = false;
	if (mCameraAnimation)
	{
		// update camera
		float Delta = Timer::GetInstance()->GetDeltaFloat();
		float xOffset = (Delta / SCREEN_PASS_TIME) * 2.0f;
		mCamera->Translate(xOffset, 0, 0);
		mNodeFollowCamera->Translate(xOffset, 0, 0);
	}
	mParticleManager->Update(Timer::GetInstance()->GetDeltaFloat());
	RenderOneFrame();
	return;
}

Scene* Scene::GetCurrentScene()
{
	return CurrentScene;
}

void Scene::SetCurrentScene(Scene* S)
{
	CurrentScene = S;
}

RenderSystemD3D11* Scene::GetRenderSystem() const
{
	return mRenderSystem;
}

RenderGroupManager* Scene::GetRenderGroupManager() const
{
	return mRenderGroupManager;
}

ResourceManager* Scene::GetResourceManager() const
{
	return mResourceManager;
}

MaterialManager* Scene::GetMaterialManager() const
{
	return mMaterialManager;
}

MeshManager* Scene::GetMeshManager() const
{
	return mMeshManager;
}

TextureManager* Scene::GetTextureManager() const
{
	return mTextureManager;
}

EffectManager* Scene::GetEffectManager() const
{
	return mEffectManager;
}

Camera* Scene::GetCurrentCamera() const
{
	return mCamera;
}

CollisionManager* Scene::GetCollisionManager() const
{
	return mCollisionManager;
}

AnimationManager* Scene::GetAnimationManager() const
{
	return mAnimationManager;
}

FontManager* Scene::GetFontManager() const
{
	return mFontManager;
}

TextManager* Scene::GetTextManager() const
{
	return mTextManager;
}

UIWindowManager* Scene::GetUIWindowManager() const
{
	return mUIWindowsManager;
}

ParticleManager* Scene::GetParticleManager() const
{
	return mParticleManager;
}

std::string Scene::GetApplicationPath() const
{
	return mApplicationPath;
}

SceneNode* Scene::GetRootSceneNode() const
{
	return mRootSceneNode;
}

SceneNode* Scene::GetNodeFollowCamera() const
{
	return mNodeFollowCamera;
}

SceneNode* Scene::GetBackGroundNode() const
{
	return mBackGroundNode;
}

void Scene::RenderOneFrame()
{
	mRenderSystem->SetAndClearRenderTarget();
	mRenderSystem->SetViewport(0, 0, float(mWindowWidth), float(mWindowHeight));
	mRenderGroupManager->RenderAllQueue();
	mRenderSystem->Present();
}

void Scene::BuildQuad(Vector3* Pos, Vector2* UV, int Width, int Height, int UStart, int VStart, int UEnd, int VEnd) const
{
	float W = float(UEnd - UStart) / float(mWindowWidth);
	float H = float(VEnd - VStart) / float(mWindowHeight);
	Pos[0] = Vector3(-W / 2.0f, H / 2.0f, 0);
	Pos[1] = Vector3(W / 2.0f, H / 2.0f, 0);
	Pos[2] = Vector3(W / 2.0f, -H / 2.0f, 0);
	Pos[3] = Vector3(-W / 2.0f, -H / 2.0f, 0);
	float US = float(UStart) / float(Width);
	float VS = float(VStart) / float(Height);
	float UE = float(UEnd) / float(Width);
	float VE = float(VEnd) / float(Height);
	UV[0] = Vector2(US, VS);
	UV[1] = Vector2(UE, VS);
	UV[2] = Vector2(UE, VE);
	UV[3] = Vector2(US, VE);
	return;
}

void Scene::BuildRandomSceneNode(BuildStruct& BS)
{
	Vector3 Pos[4];
	Vector2 UV[4];
	float BackGroundWidth = (GAME_TIME / SCREEN_PASS_TIME) * 2.0f;
	BuildQuad(Pos, UV, BS.Width, BS.Height, BS.StartTexCoordX, BS.StartTexCoordY, BS.EndTexCoordX, BS.EndTexCoordY);
	char Name[128];
	Material* Mat = mMaterialManager->CreateMaterial(BS.MatName, SimpleSampleWithBlur);
	D3d11Texture* Tex = mTextureManager->LoadTextureFromFile(BS.TexFullPath, mRenderSystem->GetD3d11Device(), BS.TexFullPath.c_str(), false);
	Mat->SetTexture(Tex);
	Mesh* M = mMeshManager->CreateQuad(BS.MeshName, Pos, UV);
	M->SetMaterial(Mat);
	for (int i = 0; i < BS.SceneNodeCount; i++)
	{
		memset(Name, 0, sizeof(Name));
		sprintf_s(Name, 128, "%s_%d", BS.MeshName.c_str(), i);
		SceneNode* SN = mRootSceneNode->CreateChild(Name, Vector3(RangeRandom(0, BackGroundWidth), RangeRandom(-0.8f, 0.8f), BS.NodeDepth), Quaternion::IDENTITY, Vector3(1, 1, 1), BS.RG);
		SN->AttachMesh(M);
		for (size_t j = 0; j < BS.BlockArray.size(); j++)
		{
			Vector3 Max = Vector3::ZERO, Min = Vector3::ZERO;
			float PercentMinX = float(BS.BlockArray[j].StartTexCoordX - BS.StartTexCoordX) / float(BS.EndTexCoordX - BS.StartTexCoordX);
			float PercentMaxX = float(BS.BlockArray[j].EndTexCoordX - BS.StartTexCoordX) / float(BS.EndTexCoordX - BS.StartTexCoordX);
			float PercentMinY = float(BS.BlockArray[j].StartTexCoordY - BS.StartTexCoordY) / float(BS.EndTexCoordY - BS.StartTexCoordY);
			float PercentMaxY = float(BS.BlockArray[j].EndTexCoordY - BS.StartTexCoordY) / float(BS.EndTexCoordY - BS.StartTexCoordY);

			Min.x = Lerp(PercentMinX, Pos[3].x, Pos[1].x);
			Min.y = Lerp(PercentMinY, Pos[3].y, Pos[1].y);
			Max.x = Lerp(PercentMaxX, Pos[3].x, Pos[1].x);
			Max.y = Lerp(PercentMaxY, Pos[3].y, Pos[1].y);
			mCollisionManager->CreateCollision(Max, Min, BS.BlockArray[j].IsStatic, SN, BS.BlockArray[j].BP);
		}
	}
}

void Scene::InitialiseScene()
{
	ScoreSystem* SS = GameLogicManager::GetInstance()->GetCurrentPlayer()->GetScoreSystem();
	SS->RefreshScore();
	// create start
	Vector3 Pos[4];
	Vector2 UV[4];
	int Width = 260;
	int Height = 300;
	BuildQuad(Pos, UV, Width, Height, 0, 0, Width, Height);
	SceneNode* StartNode = mRootSceneNode->CreateChild("Start_Node", Vector3(-0.7f, 0, RenderGroupManager::GetRenderGroupDepth()), Quaternion::IDENTITY, Vector3(1, 1, 1));
	Mesh* M = mMeshManager->CreateQuad("Start_Mesh", Pos, UV);
	Material* Mat = mMaterialManager->CreateMaterial("Start_Material_0", SimpleSampleWithBlur);
	std::string StartImageFileName = mResourceManager->GetStartImagePath();

	D3d11Texture* Tex = mTextureManager->LoadTextureFromFile(StartImageFileName, mRenderSystem->GetD3d11Device(), StartImageFileName.c_str(), false);

	Mat->SetTexture(Tex);
	M->SetMaterial(Mat);
	StartNode->AttachMesh(M);

	// Flag
	Width = 150;
	Height = 410;
	float BackGroundWidth = (GAME_TIME / SCREEN_PASS_TIME) * 2.0f;
	BuildQuad(Pos, UV, Width, Height, 0, 0, Width, Height);
	SceneNode* FlagNode = mRootSceneNode->CreateChild("Flag_Node", Vector3(BackGroundWidth - 1, 0, RenderGroupManager::GetRenderGroupDepth()), Quaternion::IDENTITY, Vector3(1, 1, 1));
	M = mMeshManager->CreateQuad("Flag_Mesh", Pos, UV);
	Mat = mMaterialManager->CreateMaterial("Flag_Material_0", SimpleSampleWithBlur);
	std::string FlagImageFileName = mResourceManager->GetFlagImagePath();

	Tex = mTextureManager->LoadTextureFromFile(FlagImageFileName, mRenderSystem->GetD3d11Device(), FlagImageFileName.c_str(), false);

	Mat->SetTexture(Tex);
	M->SetMaterial(Mat);
	FlagNode->AttachMesh(M);
	mCollisionManager->CreateCollision(Pos[1], Pos[3], true, FlagNode, Block_Final_Flag);

	// Bloom
	Width = 50;
	Height = 70;
	BuildQuad(Pos, UV, Width, Height, 0, 0, Width, Height);
	SceneNode* BloomNode = mRootSceneNode->CreateChild("Bloom_Node", Vector3(-0.7f, -0.03f, RenderGroupManager::GetRenderGroupDepth(RenderGroup_Player)), Quaternion::IDENTITY, Vector3(1, 1, 1), RenderGroup_Player);
	M = mMeshManager->CreateQuad("Bloom_Mesh", Pos, UV);
	Mat = mMaterialManager->CreateMaterial("Bloom_Material_0", SimpleSampleWithBlur);
	std::string BloomImageFileName = mResourceManager->GetBloomImagePath();

	Tex = mTextureManager->LoadTextureFromFile(BloomImageFileName, mRenderSystem->GetD3d11Device(), BloomImageFileName.c_str(), false);

	Mat->SetTexture(Tex);
	M->SetMaterial(Mat);
	BloomNode->AttachMesh(M);
	Collision* C = mCollisionManager->CreateCollision(Pos[1], Pos[3], false, BloomNode, Block_None);
	mGameLogicManager->GetCurrentPlayer()->SetPlayerNode(BloomNode);
	mGameLogicManager->GetCurrentPlayer()->SetPlayerCollision(C);

	// Create bloom start animation
	NodeAnimation* Ani = (NodeAnimation*)mAnimationManager->CreateAnimation("Bloom_Node_Animation", Animation_Node);
	Ani->SetIsAutoDestroy(true);
	Ani->AttachNode(BloomNode);
	Ani->AddPoint(Vector3(-0.7f, -0.03f, RenderGroupManager::GetRenderGroupDepth(RenderGroup_Player)), Quaternion::IDENTITY, Vector3(1, 1, 1), 0);
	Ani->AddPoint(Vector3(-0.5f, 0.7f, RenderGroupManager::GetRenderGroupDepth(RenderGroup_Player)), Quaternion::IDENTITY, Vector3(1, 1, 1), 1.7f);
	Ani->AddPoint(Vector3(-0.3f, -0.7f, RenderGroupManager::GetRenderGroupDepth(RenderGroup_Player)), Quaternion::IDENTITY, Vector3(1, 1, 1), 3.5f);
	Ani->AddPoint(Vector3(0, -0.03f, RenderGroupManager::GetRenderGroupDepth(RenderGroup_Player)), Quaternion::IDENTITY, Vector3(1, 1, 1), 5.0f);
	// Wall
	BlockInfo BI;
	BuildStruct BS;
	BS.RG = RenderGroup_Normal;
	BS.Width = 40;
	BS.Height = 856;
	BS.SceneNodeCount = SS->GetNumBlockObjects(Block_Die);
	BS.StartTexCoordX = BS.StartTexCoordY = 0;
	BS.EndTexCoordX = 40;
	BS.EndTexCoordY = 856;
	BI.BP = Block_Die;
	BI.IsStatic = true;
	BI.StartTexCoordX = BI.StartTexCoordY = 0;
	BI.EndTexCoordX = 20;
	BI.EndTexCoordY = 30;
	BS.BlockArray.push_back(BI);
	BI.StartTexCoordY = 130;
	BI.EndTexCoordY = 197;
	BS.BlockArray.push_back(BI);
	BI.StartTexCoordY = 318;
	BI.EndTexCoordY = 466;
	BS.BlockArray.push_back(BI);
	BI.StartTexCoordY = 526;
	BI.EndTexCoordY = 852;
	BS.BlockArray.push_back(BI);
	BS.NodeDepth = RenderGroupManager::GetRenderGroupDepth();
	BS.MatName = "Wall_Material_0";
	BS.MeshName = "Wall_Mesh_0";
	BS.TexFullPath = mResourceManager->GetWallImagePath();
	BuildRandomSceneNode(BS);
	BS.BlockArray.clear();
	// red apples
	BS.Width = 730;
	BS.Height = 260;
	BS.SceneNodeCount = SS->GetNumBlockObjects(Block_Apple_Red);;
	BS.StartTexCoordX = 136;
	BS.StartTexCoordY = 84;
	BS.EndTexCoordX = 136 + 66;
	BS.EndTexCoordY = 84 + 66;
	BI.BP = Block_Apple_Red;
	BI.StartTexCoordX = BS.StartTexCoordX;
	BI.StartTexCoordY = BS.StartTexCoordY;
	BI.EndTexCoordX = BS.EndTexCoordX;
	BI.EndTexCoordY = BS.EndTexCoordY;
	BS.BlockArray.push_back(BI);
	BS.NodeDepth = RenderGroupManager::GetRenderGroupDepth(RenderGroup_AfterNormal);
	BS.MatName = "Red_Apple_Material_0";
	BS.MeshName = "Red_Apple_Mesh_0";
	BS.TexFullPath = mResourceManager->GetApplesImagePath();
	BuildRandomSceneNode(BS);
	BS.BlockArray.clear();
	// yellow apples
	BS.Width = 730;
	BS.Height = 260;
	BS.SceneNodeCount = SS->GetNumBlockObjects(Block_Apple_Yellow);;
	BS.StartTexCoordX = 200;
	BS.StartTexCoordY = 84;
	BS.EndTexCoordX = 200 + 66;
	BS.EndTexCoordY = 84 + 66;
	BI.BP = Block_Apple_Yellow;
	BI.StartTexCoordX = BS.StartTexCoordX;
	BI.StartTexCoordY = BS.StartTexCoordY;
	BI.EndTexCoordX = BS.EndTexCoordX;
	BI.EndTexCoordY = BS.EndTexCoordY;
	BS.BlockArray.push_back(BI);
	BS.NodeDepth = RenderGroupManager::GetRenderGroupDepth(RenderGroup_AfterNormal);
	BS.MatName = "Yellow_Apple_Material_0";
	BS.MeshName = "Yellow_Apple_Mesh_0";
	BS.TexFullPath = mResourceManager->GetApplesImagePath();
	BuildRandomSceneNode(BS);
	BS.BlockArray.clear();
	// Carmine Circles
	BS.Width = 730;
	BS.Height = 260;
	BS.SceneNodeCount = SS->GetNumBlockObjects(Block_Circle_Carmine);;
	BS.StartTexCoordX = 370;
	BS.StartTexCoordY = 28;
	BS.EndTexCoordX = 370 + 116;
	BS.EndTexCoordY = 28 + 170;
	BI.BP = Block_Circle_Carmine;
	BI.StartTexCoordX = BS.StartTexCoordX;
	BI.StartTexCoordY = BS.StartTexCoordY;
	BI.EndTexCoordX = BS.EndTexCoordX;
	BI.EndTexCoordY = BS.EndTexCoordY;
	BS.BlockArray.push_back(BI);
	BS.NodeDepth = RenderGroupManager::GetRenderGroupDepth(RenderGroup_AfterNormal);
	BS.MatName = "Carmine_Circle_Material_0";
	BS.MeshName = "Carmine_Circle_Mesh_0";
	BS.TexFullPath = mResourceManager->GetApplesImagePath();
	BuildRandomSceneNode(BS);
	BS.BlockArray.clear();
	// Yellow Circles
	BS.Width = 730;
	BS.Height = 260;
	BS.SceneNodeCount = SS->GetNumBlockObjects(Block_Circle_Yellow);;
	BS.StartTexCoordX = 522;
	BS.StartTexCoordY = 28;
	BS.EndTexCoordX = 522 + 116;
	BS.EndTexCoordY = 28 + 170;
	BI.BP = Block_Circle_Yellow;
	BI.StartTexCoordX = BS.StartTexCoordX;
	BI.StartTexCoordY = BS.StartTexCoordY;
	BI.EndTexCoordX = BS.EndTexCoordX;
	BI.EndTexCoordY = BS.EndTexCoordY;
	BS.BlockArray.push_back(BI);
	BS.NodeDepth = RenderGroupManager::GetRenderGroupDepth(RenderGroup_AfterNormal);
	BS.MatName = "Yellow_Circle_Material_0";
	BS.MeshName = "Yellow_Circle_Mesh_0";
	BS.TexFullPath = mResourceManager->GetApplesImagePath();
	BuildRandomSceneNode(BS);
	BS.BlockArray.clear();
	// Stars
	BS.RG = RenderGroup_BeforeNormal;
	BS.Width = 230;
	BS.Height = 194;
	BS.SceneNodeCount = 10;
	BS.StartTexCoordX = 0;
	BS.StartTexCoordY = 0;
	BS.EndTexCoordX = 230;
	BS.EndTexCoordY = 194;
	BS.NodeDepth = RenderGroupManager::GetRenderGroupDepth(RenderGroup_BeforeNormal);
	BS.MatName = "Star_Material_0";
	BS.MeshName = "Star_Mesh_0";
	BS.TexFullPath = mResourceManager->GetStarImagePath();
	BuildRandomSceneNode(BS);

	// Bullets
	CreateBullets();
	// Particles
	CreateParticle();
	float PoemX = 0.3f;
	// assume the poem in the center of the screen, and the poem is 4 column.
	float StepX = PoemX * 2.0f / 4.0f;
	float PoemY = 0;
	mCurrentPoemIndex = 0;
	mStartPoem[0] = { L"黄河远上白云间", "Start_Poem_Node1", "TextFadeIn_1" , "TextFadeOut_1", nullptr,  Vector3(PoemX - 0 * StepX, PoemY, RenderGroupManager::GetRenderGroupDepth(RenderGroup_TEXT))};
	mStartPoem[1] = { L"一片孤城万仞山", "Start_Poem_Node2", "TextFadeIn_2" , "TextFadeOut_2", nullptr,  Vector3(PoemX - 1 * StepX, PoemY, RenderGroupManager::GetRenderGroupDepth(RenderGroup_TEXT)) };
	mStartPoem[2] = { L"羌笛何须怨杨柳", "Start_Poem_Node3", "TextFadeIn_3" , "TextFadeOut_3", nullptr,  Vector3(PoemX - 2 * StepX, PoemY, RenderGroupManager::GetRenderGroupDepth(RenderGroup_TEXT)) };
	mStartPoem[3] = { L"春风不度玉门关", "Start_Poem_Node4", "TextFadeIn_4" , "TextFadeOut_4", nullptr,  Vector3(PoemX - 3 * StepX, PoemY, RenderGroupManager::GetRenderGroupDepth(RenderGroup_TEXT)) };
	CreateStartPoemEffect(mCurrentPoemIndex++, true);
}

void Scene::CreateStartPoemEffect(int Index, bool IsFadeIn)
{
	Vector4 StartPoemColor = mTextManager->GetTextColorByType(StartPoemTextColor);
	if (IsFadeIn)
	{
		// create start message
		std::wstring StartMessage = mStartPoem[Index].PoemText;
		Font* F = mFontManager->GetStartPoemFont();
		Text* T = mTextManager->CreateText(StartMessage, F, StartPoemColor, false);

		SceneNode* TextNode1 = mRootSceneNode->CreateChild(mStartPoem[Index].NodeName, mStartPoem[Index].NodePosition, Quaternion::IDENTITY, Vector3(1, 1, 1), RenderGroup_TEXT);
		TextNode1->AttachMesh(T->GetAttachMesh());
		EffectTextFadeIn* E = (EffectTextFadeIn*)mEffectManager->CreateEffect(mStartPoem[Index].FadeInEffectName, Effect_Text_Fade_In_In_Order, 8.0f, false, true, 0.0f, T->GetAttachMesh(), TextNode1, nullptr, 0, this);
		E->SetTextColor(StartPoemColor);
		E->SetTextCount(T->GetCharacterCount());
		mStartPoem[Index].T = T;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			SceneNode* S = mRootSceneNode->GetChildByName(mStartPoem[i].NodeName);
			EffectTextFadeOut* E = (EffectTextFadeOut*)mEffectManager->CreateEffect(mStartPoem[i].FadeOutEffectName, Effect_Text_Fade_Out, 2.0f, false, true, 0.0f, mStartPoem[i].T->GetAttachMesh(), S, nullptr, ElementSceneNode, this);
			E->SetTextColor(StartPoemColor);
		}
	}
}

void Scene::CreateBullets()
{
	float BackGroundWidth = (GAME_TIME / SCREEN_PASS_TIME) * 2.0f;
	ScoreSystem* SS = GameLogicManager::GetInstance()->GetCurrentPlayer()->GetScoreSystem();
	int BulletCount = SS->GetNumBlockObjects(Block_Bullet_DecreaseLife);
	for (int i = 0; i < BulletCount; i++)
	{
		Material* Mat = mMaterialManager->CreateMaterial(SimpleColor);
		// Color
		Vector4 Col = Vector4(RangeRandom(0.0f, 1.0f), RangeRandom(0.0f, 1.0f), RangeRandom(0.0f, 1.0f), 1.0f);
		char* TempPtr = Mat->GetConstBufferPointer();
		memcpy(TempPtr, &Col, sizeof(Vector4));
		Mat->SetConstBufferLen(sizeof(Vector4));
		float Radius = RangeRandom(0.01f, 0.02f);
		Mesh* M = mMeshManager->CreateSphere(32, 32, Radius);
		M->SetMaterial(Mat);
		float CurrentDepth = RenderGroupManager::GetRenderGroupDepth(RenderGroup_Bullet);
		SceneNode* SN = mRootSceneNode->CreateChild(Vector3(RangeRandom(0, BackGroundWidth), RangeRandom(-0.8f, 0.8f), CurrentDepth), Quaternion::IDENTITY, Vector3(1, 1, 1), RenderGroup_Bullet);
		SN->AttachMesh(M);
		// Create Animation
		NodeAnimation* Ani = static_cast<NodeAnimation*>(mAnimationManager->CreateAnimation(Animation_Node));
		Ani->SetIsLoop(true);
		Ani->SetIsAutoDestroy(false);
		Ani->AttachNode(SN);
		// Random Node Count
		int NodeCount = (int)RangeRandom(2.0f, 7.0f);
		ScoreSystem* SS = GameLogicManager::GetInstance()->GetCurrentPlayer()->GetScoreSystem();
		float fSegTime = BULLET_ANI_SEG_TIME * (1.0f - BULLET_ANI_SEG_TIME_FADE * float(SS->GetBigGrade()));
		for (int i = 0; i < NodeCount; i++)
		{
			Ani->AddPoint(Vector3(RangeRandom(0, BackGroundWidth), RangeRandom(-1.2f, 1.2f), CurrentDepth), Quaternion::IDENTITY, Vector3(1, 1, 1), fSegTime * float(i));
		}
		Ani->AddRoundPoint(fSegTime * float(NodeCount));

		Vector3 Max = Vector3::ZERO, Min = Vector3::ZERO;

		Min.x = -Radius;
		Min.y = -Radius;
		Max.x = Radius;
		Max.y = Radius;
		mCollisionManager->CreateCollision(Max, Min, false, SN, Block_Bullet_DecreaseLife);
	}
}

void Scene::CreateParticle()
{
	// Smoke
	ParticleSmoke* PS = mParticleManager->CreateSmoke();
	PS->SetLifeTimeRange(5.0f, 0.5f);
	PS->SetVelocity(0.2f, 0.02f);
	PS->SetAcceleration(0.1f, 0.01f);
	PS->SetDirection(Vector3(0.0f, 1.0f, 0.0f), 0.4f);
	PS->SetMaxCount(30000);
	PS->SetProduceCountPerSecond(5000);
	PS->SetColorRange(Vector3(0.0411f, 0.0254f, 0.0353f), 0.1f, Vector3(0.9549f, 0.9235f, 0.927f), 0.1f);
	PS->SetStartOffset(Vector3(0.5f, 0.1f, 0.5f));
//	PS->ForceGpuOptimization();

	SceneNode* PS_Node = mRootSceneNode->CreateChild("Particle_Node_Smoke", Vector3(0.0f, -0.8f, 1.0f), Quaternion::IDENTITY, Vector3(1, 1, 1), RenderGroup_ParticleSmoke);
	PS_Node->AttachRenderable(PS);
	PS->Initialise();

	Material* Mat = mMaterialManager->CreateMaterial(SimpleColorPointForParticle);
	Mat->SetDepthFunc(D3D11_COMPARISON_ALWAYS);
	Mat->SetCullMode(D3D11_CULL_NONE);
	Mat->ReBuild();

	PS->SetMaterial(Mat);
}

void Scene::CreateBackGround()
{
	Vector3 Pos[4];
	// -1, 1
	float BackGroundWidth = (GAME_TIME / SCREEN_PASS_TIME) * 2.0f;

	Pos[0] = Vector3(-1.0f, BackGroundWidth / 2.0f, 0);
	Pos[1] = Vector3(BackGroundWidth, BackGroundWidth / 2.0f, 0);
	Pos[2] = Vector3(BackGroundWidth, -BackGroundWidth / 2.0f, 0);
	Pos[3] = Vector3(-1.0f, -BackGroundWidth / 2.0f, 0);
	Mesh* M = mMeshManager->CreateQuad("Back_Ground_Image_0", Pos);

	Material* Mat = mMaterialManager->CreateMaterial("Back_Ground_Material_0", SimpleTextureSample);

	std::string BackGroundImageFileName = mResourceManager->GetBackGroundImageInOrder();

	D3d11Texture* Tex = mTextureManager->LoadTextureFromFile(BackGroundImageFileName, mRenderSystem->GetD3d11Device(), BackGroundImageFileName.c_str(), false);

	Mat->SetTexture(Tex);
	M->SetMaterial(Mat);

	mBackGroundNode->AttachMesh(M);
}

void Scene::CreateFrameRateText()
{
	// create start message
	std::wstring StartMessage = L"帧率：";
	Font* F = mFontManager->GetCommonFont();
	Vector4 CommomColor = mTextManager->GetTextColorByType(CommonTextColor);
	mFrameRateTextTitle = mTextManager->CreateText(StartMessage, F, CommomColor);

	mFrameRateTextTitleNode = mNodeFollowCamera->CreateChild("FrameRate_Title_Node", Vector3(-0.74f, 0.75f, RenderGroupManager::GetRenderGroupDepth(RenderGroup_TEXT)), Quaternion::IDENTITY, Vector3(1, 1, 1), RenderGroup_TEXT);
	mFrameRateTextTitleNode->AttachMesh(mFrameRateTextTitle->GetAttachMesh());

	mFrameRateTextNode = mNodeFollowCamera->CreateChild("FrameRate_Node", Vector3(-0.68f, 0.75f, RenderGroupManager::GetRenderGroupDepth(RenderGroup_TEXT)), Quaternion::IDENTITY, Vector3(1, 1, 1), RenderGroup_TEXT);
}

void Scene::SwitchToNextBackGround()
{
	Material* Mat = mMaterialManager->GetMaterialByName("Back_Ground_Material_0");

	std::string BackGroundImageFileName = mResourceManager->GetBackGroundImageInOrder();
	D3d11Texture* Tex = mTextureManager->LoadTextureFromFile(BackGroundImageFileName, mRenderSystem->GetD3d11Device(), BackGroundImageFileName.c_str(), false);
	
	D3d11Texture* OldTexture = Mat->GetTexture(0);
	mTextureManager->DestroyTexture(OldTexture);

	Mat->SetTexture(Tex);
}

void Scene::SetRenderFrameRate(bool RenderFrameRate /* = true */)
{
	mRenderFrameRate = RenderFrameRate;
	if (mFrameRateText)
		mFrameRateText->SetVisible(mRenderFrameRate);
}

bool Scene::GetRenderFrameRate() const
{
	return mRenderFrameRate;
}

void Scene::SetCurrentFrameRate(int FrameRate)
{
	mCurrentFrameRate = FrameRate;
	Font* F = mFontManager->GetFont(MFMengYuan, 16, FSOutline, 2);
	if (mFrameRateText)
	{
		mFrameRateTextNode->DetachMesh(mFrameRateText->GetAttachMesh());
		// destroy text
		mTextManager->DestroyText(mFrameRateText);
	}
	std::wstring FrameRateText = std::to_wstring(mCurrentFrameRate);
	mFrameRateText = mTextManager->CreateText(FrameRateText, F, Vector4(1, 0, 0, 1));
	if (mFrameRateText == nullptr)
	{
		OutputDebugStringA("CreateText Failed at SetCurrentFrameRate");
	}
	mFrameRateTextNode->AttachMesh(mFrameRateText->GetAttachMesh());
}

int Scene::GetCurrentFrameRate() const
{
	return mCurrentFrameRate;
}

int Scene::GetWindowWidth() const
{
	return mWindowWidth;
}

int Scene::GetWindowHeight() const
{
	return mWindowHeight;
}

Vector2 Scene::WindowsCoordToProjectionCoord(int x, int y)
{
	float _x = float(x) / float(CurrentScene->GetWindowWidth() - 1);
	float _y = float(CurrentScene->GetWindowHeight() - 1 - y) / float(CurrentScene->GetWindowHeight() - 1);
	// covert (0, 1) to (-1, 1)
	Vector2 v = Vector2(_x * 2.0f - 1.0f, _y * 2.0f - 1.0f);

	return v;
}

Vector2 Scene::WindowsCoordToWindowsPercent(int x, int y)
{
	float _x = float(x) / float(CurrentScene->GetWindowWidth());
	float _y = float(y) / float(CurrentScene->GetWindowHeight());

	Vector2 v = Vector2(_x, _y);

	return v;
}

void Scene::OnInitialise(Effect* E)
{
	// do nothing
}

void Scene::OnEnd(Effect* E)
{
	// do nothing
}

void Scene::OnDestroy(Effect* E)
{
	if (E->GetType() == Effect_Text_Fade_In_In_Order)
	{
		if (mCurrentPoemIndex < 4)
		{
			// in fade in process
			CreateStartPoemEffect(mCurrentPoemIndex++, true);
		}
		else
		{
			mCurrentPoemIndex = 0;
			Timer::GetInstance()->AddOnceTimer((ITimerListener*)this, 0, 0);
		}
	}
	else
	{
		mCurrentPoemIndex++;
		// all fade out destroyed
		if (mCurrentPoemIndex == 4)
		{
			Timer::GetInstance()->AddOnceTimer((ITimerListener*)this, 1, 0);
		}
	}
}

void Scene::OnTimer(unsigned int EventID, void* UserData)
{
	if (EventID == 0)
	{
		// destroy fade in, start to fade out
		CreateStartPoemEffect(0, false);
	}
	else if (EventID == 1)
	{
		// all fade out been destroyed
		// start to destroy all the text elements
		mTextManager->DestroyText(mStartPoem[0].T);
		mTextManager->DestroyText(mStartPoem[1].T);
		mTextManager->DestroyText(mStartPoem[2].T);
		mTextManager->DestroyText(mStartPoem[3].T);
	}
}