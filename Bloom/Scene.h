/*!
 * \file Scene.h
 *
 * \author LYG
 * \date ËÄÔÂ 2018
 *
 * 
 */
#pragma once
#include "Common.h"
#include "RenderSystem.h"
#include "RenderGroupManager.h"
#include "ResourceManager.h"
#include "EffectManager.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "SceneNode.h"
#include "GameDef.h"
#include "Log.h"

#define		NEAR_PLANE				0.01f
#define		FAR_PLANE				10000.0f

class Camera
{
public:
	Camera() 
	{
		mPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0);
		mRot = XMQuaternionIdentity();
		mFovAngle = DegreesToRadians(90);
		mAspect = 1920.0f / 1080.0f;
		mNearPlane = NEAR_PLANE;
		mFarPlane = FAR_PLANE;
		mViewMatrix = mProjectionMatrix = mViewProjectionMatrix= XMMatrixIdentity();
	}
	~Camera(){}
	void SetPosition(XMVECTOR Position)
	{
		mPos = Position;
		XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), mRot);
		mViewMatrix = (XMMatrixLookAtLH(mPos, XMVectorAdd(mPos, forward), XMVector3Rotate(XMVectorSet(0, 1, 0, 0), mRot)));
		mViewProjectionMatrix = XMMatrixMultiply(mViewMatrix, mProjectionMatrix);
	}
	void SetRotation(XMVECTOR Rotation)
	{
		mRot = Rotation;
		XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), mRot);
		mViewMatrix = (XMMatrixLookAtLH(mPos, XMVectorAdd(mPos, forward), XMVector3Rotate(XMVectorSet(0, 1, 0, 0), mRot)));
		mViewProjectionMatrix = XMMatrixMultiply(mViewMatrix, mProjectionMatrix);
	}
	void Translate(float x, float y, float z)
	{
		XMVECTOR Add = XMVectorSet(x, y, z, 0);
		mPos = XMVectorAdd(mPos, Add);
		SetPosition(mPos);
	}
	void RollPitchYaw(float Roll, float Yaw, float Pitch)
	{
		XMVECTOR Rot = XMQuaternionRotationRollPitchYaw(Roll, Yaw, Pitch);
		XMVECTOR DestRot = XMQuaternionMultiply(mRot, Rot);
		SetRotation(DestRot);
	}

	void SetProjectionParameters(float Fov, float Aspect, float Near, float Far)
	{
		mFovAngle = Fov;
		mAspect = Aspect;
		mNearPlane = Near;
		mFarPlane = Far;
		mProjectionMatrix = XMMatrixOrthographicLH(mFovAngle, mAspect, mNearPlane, mFarPlane);
		//mProjectionMatrix = XMMatrixPerspectiveFovLH(mFovAngle, mAspect, mNearPlane, mFarPlane);
		mViewProjectionMatrix = XMMatrixMultiply(mViewMatrix, mProjectionMatrix);
	}
	XMMATRIX GetViewMatrix() const
	{
		return mViewMatrix;
	}
	XMMATRIX GetProjectionMatrix() const
	{
		return mProjectionMatrix;
	}

	XMMATRIX GetProjectViewMatrix() const
	{
		return mViewProjectionMatrix;
	}

	static void* operator new(size_t size)
	{
		UNREFERENCED_PARAMETER(size);
		return _aligned_malloc(sizeof(Camera), __alignof(Camera));
	}

	static void operator delete(void* p)
	{
		_aligned_free(p);
	}
private:
	XMVECTOR mPos;
	XMVECTOR mRot;
	float mFovAngle;
	float mAspect;
	float mNearPlane;
	float mFarPlane;
	XMMATRIX mViewMatrix;
	XMMATRIX mProjectionMatrix;
	XMMATRIX mViewProjectionMatrix;
};
enum BlockPro;
class CollisionManager;
class Scene
{
	friend class GameLogicManager;
public:
	static Scene* GetCurrentScene();
	static void SetCurrentScene(Scene* S);
public:
	bool Initialise(HWND hWnd);
	void Update();
public:
	SceneNode* GetRootSceneNode() const;
	SceneNode* GetBackGroundNode() const;
	RenderSystemD3D11* GetRenderSystem() const;
	RenderGroupManager* GetRenderGroupManager() const;
	ResourceManager* GetResourceManager() const;
	MaterialManager* GetMaterialManager() const;
	MeshManager* GetMeshManager() const;
	TextureManager* GetTextureManager() const;
	EffectManager* GetEffectManager() const;
	Camera* GetCurrentCamera() const;
	LogImpl* GetLogImpl() const;
	CollisionManager* GetCollisionManager() const;

	std::string GetApplicationPath() const;

	void SetCameraAnimation(bool StartAnimation);
	bool GetCameraAnimation() const;
public:
	void RenderOneFrame();

protected:
	Scene();
	~Scene();

	void BuildApplicationPath();
	static Scene* CurrentScene;
	struct BlockInfo
	{
		int StartTexCoordX;
		int StartTexCoordY;
		int EndTexCoordX;
		int EndTexCoordY;

		bool IsStatic;
		BlockPro BP;
	};

	struct BuildStruct
	{
		int Width;
		int Height;
		int StartTexCoordX;
		int StartTexCoordY;
		int EndTexCoordX;
		int EndTexCoordY;
		int SceneNodeCount;
		RenderGroup RG;
		float NodeDepth;
		std::string MatName;
		std::string MeshName;
		std::string TexFullPath;
		std::vector<BlockInfo> BlockArray;
	};

	void BuildQuad(Vector3* Pos, Vector2* UV, int Width, int Height, int UStart, int VStart, int UEnd, int VEnd) const;
	void BuildRandomSceneNode(BuildStruct& BS);
	void InitialiseScene();
	void CreateBackGround();
	void SwitchToNextBackGround();
private:
	SceneNode* mRootSceneNode;
	SceneNode* mBackGroundNode;
	RenderSystemD3D11* mRenderSystem;
	RenderGroupManager* mRenderGroupManager;
	ResourceManager* mResourceManager;
	MaterialManager* mMaterialManager;
	MeshManager* mMeshManager;
	TextureManager* mTextureManager;
	EffectManager* mEffectManager;
	Camera* mCamera;
	LogImpl* mLog;
	std::string mApplicationPath;
	HWND mHwnd;

	GameLogicManager* mGameLogicManager;
	CollisionManager* mCollisionManager;
	bool mCameraAnimation;
};