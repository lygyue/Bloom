/*!
 * \file Scene.cpp
 *
 * \author LYG
 * \date ËÄÔÂ 2018
 *
 * 
 */
#include "Scene.h"
#include "Math/Vector2.h"
#include "Timer.h"

Scene* Scene::CurrentScene = nullptr;
Scene::Scene()
{
	mHwnd = nullptr;
	// Get Current Directory
	char Temp[MAX_PATH];
	memset(Temp, 0, MAX_PATH);

	GetModuleFileNameA(NULL, Temp, MAX_PATH);
	int nLen = strlen(Temp);
	while (nLen)
	{
		if (Temp[nLen] == '\\' || Temp[nLen] == '/')
		{
			break;
		}
		Temp[nLen--] = '\0';
	}
	mApplicationPath = Temp;
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
	mLog = new LogImpl(Temp);
	mCamera->SetProjectionParameters(DegreesToRadians(90), float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), NEAR_PLANE, FAR_PLANE);
}

Scene::~Scene()
{
	SAFE_DELETE(mRootSceneNode);
	SAFE_DELETE(mRenderSystem);
	SAFE_DELETE(mRenderGroupManager);
	SAFE_DELETE(mResourceManager);
	SAFE_DELETE(mMaterialManager);
	SAFE_DELETE(mTextureManager);
	SAFE_DELETE(mEffectManager);
	SAFE_DELETE(mMeshManager);
	SAFE_DELETE(mCamera);
	SAFE_DELETE(mLog);
	// don't need to delete
	mBackGroundNode = nullptr;
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
	mBackGroundNode = mRootSceneNode->CreateChild("BackGround_Node", Vector3(0, 0, (FAR_PLANE - 10.0f)), Quaternion::IDENTITY, Vector3(1, 1, 1), RenderGroup_BackGroud);
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
	if (mCameraAnimation)
	{
		// update camera
		float Delta = float(Timer::GetInstance()->GetDelta()) * 0.001;

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

LogImpl* Scene::GetLogImpl() const
{
	return mLog;
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
	Pos[0] = Vector3(-W / 2.0, H / 2.0, 0);
	Pos[1] = Vector3(W / 2.0, H / 2.0, 0);
	Pos[2] = Vector3(W / 2.0, -H / 2.0, 0);
	Pos[3] = Vector3(-W / 2.0, -H / 2.0, 0);
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
		SceneNode* SN = mRootSceneNode->CreateChild(Name, Vector3(RangeRandom(0, BackGroundWidth), RangeRandom(-0.8, 0.8), BS.NodeDepth), Quaternion::IDENTITY, Vector3(1, 1, 1));
		SN->AttachMesh(M);
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
	SceneNode* StartNode = mRootSceneNode->CreateChild("Start_Node", Vector3(-0.7, 0, FAR_PLANE - 100), Quaternion::IDENTITY, Vector3(1, 1, 1));
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
	SceneNode* FlagNode = mRootSceneNode->CreateChild("Flag_Node", Vector3(BackGroundWidth - 1, 0, FAR_PLANE - 200), Quaternion::IDENTITY, Vector3(1, 1, 1));
	M = mMeshManager->CreateQuad("Flag_Mesh", Pos, UV);
	Mat = mMaterialManager->CreateMaterial("Flag_Material_0", SimpleSampleWithBlur);
	std::string FlagImageFileName = mResourceManager->GetFlagImagePath();

	Tex = mTextureManager->LoadTextureFromFile(FlagImageFileName, mRenderSystem->GetD3d11Device(), FlagImageFileName.c_str(), false);

	Mat->SetTexture(Tex);
	M->SetMaterial(Mat);
	FlagNode->AttachMesh(M);

	// Wall
	BuildStruct BS;
	BS.Width = 40;
	BS.Height = 856;
	BS.SceneNodeCount = 10;
	BS.StartTexCoordX = BS.StartTexCoordY = 0;
	BS.EndTexCoordX = 40;
	BS.EndTexCoordY = 856;
	BS.NodeDepth = FAR_PLANE - 100;
	BS.MatName = "Wall_Material_0";
	BS.MeshName = "Wall_Mesh_0";
	BS.TexFullPath = mResourceManager->GetWallImagePath();
	BuildRandomSceneNode(BS);
	// red apples
	BS.Width = 730;
	BS.Height = 260;
	BS.SceneNodeCount = 10;
	BS.StartTexCoordX = 136;
	BS.StartTexCoordY = 84;
	BS.EndTexCoordX = 136 + 66;
	BS.EndTexCoordY = 84 + 66;
	BS.NodeDepth = FAR_PLANE - 150;
	BS.MatName = "Red_Apple_Material_0";
	BS.MeshName = "Red_Apple_Mesh_0";
	BS.TexFullPath = mResourceManager->GetApplesImagePath();
	BuildRandomSceneNode(BS);
	// yellow apples
	BS.Width = 730;
	BS.Height = 260;
	BS.SceneNodeCount = 10;
	BS.StartTexCoordX = 200;
	BS.StartTexCoordY = 84;
	BS.EndTexCoordX = 200 + 66;
	BS.EndTexCoordY = 84 + 66;
	BS.NodeDepth = FAR_PLANE - 160;
	BS.MatName = "Yellow_Apple_Material_0";
	BS.MeshName = "Yellow_Apple_Mesh_0";
	BS.TexFullPath = mResourceManager->GetApplesImagePath();
	BuildRandomSceneNode(BS);
	// Carmine Circles
	BS.Width = 730;
	BS.Height = 260;
	BS.SceneNodeCount = 10;
	BS.StartTexCoordX = 370;
	BS.StartTexCoordY = 28;
	BS.EndTexCoordX = 370 + 116;
	BS.EndTexCoordY = 28 + 170;
	BS.NodeDepth = FAR_PLANE - 170;
	BS.MatName = "Carmine_Circle_Material_0";
	BS.MeshName = "Carmine_Circle_Mesh_0";
	BS.TexFullPath = mResourceManager->GetApplesImagePath();
	BuildRandomSceneNode(BS);
	// Yellow Circles
	BS.Width = 730;
	BS.Height = 260;
	BS.SceneNodeCount = 10;
	BS.StartTexCoordX = 522;
	BS.StartTexCoordY = 28;
	BS.EndTexCoordX = 522 + 116;
	BS.EndTexCoordY = 28 + 170;
	BS.NodeDepth = FAR_PLANE - 180;
	BS.MatName = "Yellow_Circle_Material_0";
	BS.MeshName = "Yellow_Circle_Mesh_0";
	BS.TexFullPath = mResourceManager->GetApplesImagePath();
	BuildRandomSceneNode(BS);
	// Stars
	BS.Width = 230;
	BS.Height = 194;
	BS.SceneNodeCount = 10;
	BS.StartTexCoordX = 0;
	BS.StartTexCoordY = 0;
	BS.EndTexCoordX = 230;
	BS.EndTexCoordY = 194;
	BS.NodeDepth = FAR_PLANE - 190;
	BS.MatName = "Star_Material_0";
	BS.MeshName = "Star_Mesh_0";
	BS.TexFullPath = mResourceManager->GetStarImagePath();
	BuildRandomSceneNode(BS);
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