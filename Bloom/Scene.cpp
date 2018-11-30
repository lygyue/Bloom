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
	mGameLogicManager = nullptr;
	mCamera->SetProjectionParameters(DegreesToRadians(90), float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), NEAR_PLANE, FAR_PLANE);
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
	if (mRenderSystem->Initialise(WINDOW_WIDTH, WINDOW_HEIGHT, mHwnd) == false)
	{
		return false;
	}
	if (mResourceManager->Initialise() == false)
	{
		return false;
	}
	mMaterialManager->Initialise();
	mBackGroundNode = mRootSceneNode->CreateChild("BackGround_Node", Vector3(0, 0, RenderGroupManager::GetRenderGroupDepth(RenderGroup_BackGroud)), Quaternion::IDENTITY, Vector3(1, 1, 1), RenderGroup_BackGroud);
	CreateBackGround();
	InitialiseScene();
	return true;
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
	static float CurrentTime = 0.0f;
	CurrentTime += Timer::GetInstance()->GetDeltaFloat();
	float BeginTime = 10.0f;
	if (CurrentTime > BeginTime) mCameraAnimation = true;
	if (CurrentTime > BeginTime + GAME_TIME - SCREEN_PASS_TIME / 2) mCameraAnimation = false;
	if (mCameraAnimation)
	{
		// update camera
		float Delta = Timer::GetInstance()->GetDeltaFloat();

		float xOffset = (Delta / SCREEN_PASS_TIME) * 2.0f;
		mCamera->Translate(xOffset, 0, 0);
	}
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

std::string Scene::GetApplicationPath() const
{
	return mApplicationPath;
}

SceneNode* Scene::GetRootSceneNode() const
{
	return mRootSceneNode;
}

SceneNode* Scene::GetBackGroundNode() const
{
	return mBackGroundNode;
}

void Scene::RenderOneFrame()
{
	mRenderSystem->SetAndClearRenderTarget();
	mRenderSystem->SetViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	mRenderGroupManager->RenderAllQueue();
	mRenderSystem->Present();
}

void Scene::BuildQuad(Vector3* Pos, Vector2* UV, int Width, int Height, int UStart, int VStart, int UEnd, int VEnd) const
{
	float W = float(UEnd - UStart) / float(WINDOW_WIDTH);
	float H = float(VEnd - VStart) / float(WINDOW_HEIGHT);
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
	BS.SceneNodeCount = 10;
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
	BS.SceneNodeCount = 10;
	BS.StartTexCoordX = 136;
	BS.StartTexCoordY = 84;
	BS.EndTexCoordX = 136 + 66;
	BS.EndTexCoordY = 84 + 66;
	BI.BP = Block_Apple;
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
	BS.SceneNodeCount = 10;
	BS.StartTexCoordX = 200;
	BS.StartTexCoordY = 84;
	BS.EndTexCoordX = 200 + 66;
	BS.EndTexCoordY = 84 + 66;
	BI.BP = Block_Apple;
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
	BS.SceneNodeCount = 10;
	BS.StartTexCoordX = 370;
	BS.StartTexCoordY = 28;
	BS.EndTexCoordX = 370 + 116;
	BS.EndTexCoordY = 28 + 170;
	BI.BP = Block_Apple;
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
	BS.SceneNodeCount = 10;
	BS.StartTexCoordX = 522;
	BS.StartTexCoordY = 28;
	BS.EndTexCoordX = 522 + 116;
	BS.EndTexCoordY = 28 + 170;
	BI.BP = Block_Apple;
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

	// create start message
	std::wstring StartMessage = L"黄河远上白云间";
	Font* F = mFontManager->GetFont(MFMengYuan, 64, FSOutline, 2);
	Text* T = mTextManager->CreateText(StartMessage, F, Vector4(0.8f, 0.5f, 0.3f, 1), true);

	SceneNode* TextNode1 = mRootSceneNode->CreateChild("Start_Text_Node1", Vector3(0, 0, RenderGroupManager::GetRenderGroupDepth(RenderGroup_TEXT)), Quaternion::IDENTITY, Vector3(1, 1, 1), RenderGroup_TEXT);
	TextNode1->AttachMesh(T->GetAttachMesh());
	EffectTextFadeIn* E = (EffectTextFadeIn*)mEffectManager->CreateEffect("TextFadeIn_1", Effect_Text_Fade_In_In_Order, 8.0f, false, true, 0.0f, T->GetAttachMesh());
	E->SetTextColor(Vector4(0.8f, 0.5f, 0.3f, 1));
	E->SetTextCount(T->GetTextCount());
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

void Scene::SwitchToNextBackGround()
{
	Material* Mat = mMaterialManager->GetMaterialByName("Back_Ground_Material_0");

	std::string BackGroundImageFileName = mResourceManager->GetBackGroundImageInOrder();

	D3d11Texture* Tex = mTextureManager->LoadTextureFromFile(BackGroundImageFileName, mRenderSystem->GetD3d11Device(), BackGroundImageFileName.c_str(), false);

	D3d11Texture* OldTexture = Mat->GetTexture(0);
	mTextureManager->DestroyTexture(OldTexture);

	Mat->SetTexture(Tex);
}