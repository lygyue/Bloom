/***********************************************
 * File: Particle.cpp
 *
 * Author: LYG
 * Date: °ËÔÂ 2020
 *
 * Purpose:
 *
 * 
 **********************************************/

#include "Particle.h"
#include "SimpleMath.h"
#include "SceneNode.h"
#include "Scene.h"
#include "RenderSystem.h"
#include "Shader.h"

Particle::Particle(std::string Name)
{
	mName								= Name;
	mGpuOptimization					= false;
	mLifeTime							= 0.0f;
	mLifeTimeTolerance					= 0.0f;
	mStartCol							= Vector3::ZERO;
	mStartColTolerance					= 0.0f;
	mEndCol								= Vector3::ZERO;
	mEndColTolerance					= 0.0f;

	mProducePerSecond					= 0;

	mDirection							= Vector3::ZERO;
	mDirectionTolerance					= Vector3::ZERO;

	mStartOffsetTolerance				= Vector3::ZERO;
	mVelocity							= 0.0f;
	mVelocityTolerance					= 0.0f;
	mAcceleration						= 0.0f;
	mAccelerationVelocity				= 0.0f;

	mUpdateDelta						= 0.033333f;
	mMaxElementCount					= 0;
	mVertexCount						= 0;
	mMaterial							= nullptr;
	mVertexBuffer						= nullptr;
	mVertexElementSize					= 0;
	mPrimitiveType						= D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	mIndexBuffer						= nullptr;
	mIndexCount							= 0;
	mIndexFormat						= DXGI_FORMAT_R16_UINT;
}

Particle::~Particle()
{
	SAFE_DELETE(mVertexBuffer);
	SAFE_DELETE(mIndexBuffer);
}


void Particle::ForceGpuOptimization(bool UseGpu /* = true */)
{
	mGpuOptimization = UseGpu;
}

void Particle::SetMaterial(Material* Mat)
{
	mMaterial = Mat;
}
Material* Particle::GetMaterial() const
{
	return mMaterial;
}

void Particle::SetLifeTimeRange(float TotalTime, float Tolerance)
{
	mLifeTime = TotalTime;
	mLifeTimeTolerance = Tolerance;
}
void Particle::SetColorRange(Vector3 StartCol, float StartTolerance, Vector3 EndCol, float EndTolerance)
{
	mStartCol = StartCol;
	mStartColTolerance = StartTolerance;
	mEndCol = EndCol;
	mEndColTolerance = EndTolerance;
}

void Particle::SetMaxCount(int MaxCount)
{
	mMaxElementCount = MaxCount;
}
int Particle::GetMaxCount() const
{
	return mMaxElementCount;
}

void Particle::SetProduceCountPerSecond(int Count)
{
	mProducePerSecond = Count;
}
int Particle::GetProduceCountPerSecond() const
{
	return mProducePerSecond;
}

void Particle::SetDirection(Vector3 Direction, float Tolerance)
{
	mDirection = Direction;
	mDirectionTolerance = Vector3(RangeRandom(-Tolerance, Tolerance), RangeRandom(-Tolerance, Tolerance), RangeRandom(-Tolerance, Tolerance));
}
void Particle::SetStartOffset(Vector3 OffsetRange)
{
	mStartOffsetTolerance = OffsetRange;
}
void Particle::SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY PriType)
{
	mPrimitiveType = PriType;
}

void Particle::SetVelocity(float Velocity, float Tolerance)
{
	mVelocity = Velocity;
	mVelocityTolerance = Tolerance;
}
void Particle::SetAcceleration(float Acceleration, float Tolerance)
{
	mAcceleration = Acceleration;
	mAccelerationVelocity = Tolerance;
}

void Particle::SetUpdateDelta(float Delta)
{
	mUpdateDelta = Delta;
}

Vector3 Particle::CalcNewDirection(Vector3 Direction) const
{
	return Direction;
}

float Particle::CalcRealTimeVelocity(float Velociry, float Acceleration, float Delta) const
{
	float VT = Velociry + Acceleration * Delta;
	return VT;
}

void Particle::Render(Matrix4& WorldTransform)
{
	// Render the particle

}

bool Particle::Initialise()
{

	return true;
}

void Particle::Update(float Delta)
{

}


ParticleSmoke::ParticleSmoke(std::string Name): Particle(Name)
{
	mVertexData						= nullptr;
	mVertexElementSize				= sizeof(VertexDataPoint);
	mConstBufferForBuildParticle	= nullptr;
	mSmokeDataUAV					= nullptr;
	mSmokeVertexDataUAV				= nullptr;
	mBuildSmokeCS					= nullptr;
	mCalcSmokeCS					= nullptr;
	mSegmentLifeTime				= nullptr;
	mSegmentCount					= 0;
	mSegmentSize					= 0;
	mSegmentBuildIndex				= 0;
	mPlayTimes						= 0.0f;
}


ParticleSmoke::~ParticleSmoke()
{
	SAFE_DELETE_ARRAY(mSegmentLifeTime);
	SAFE_RELEASE(mBuildSmokeCS);
	SAFE_RELEASE(mCalcSmokeCS);
	SAFE_DELETE(mSmokeVertexDataUAV);
	SAFE_DELETE(mSmokeDataUAV);
	SAFE_DELETE(mConstBufferForBuildParticle);
	SAFE_DELETE_ARRAY(mVertexData);
	std::list<ParticleSmokeData*>::iterator it;
	for (it = mParticleData.begin(); it != mParticleData.end(); it++)
	{
		delete (*it);
	}
	mParticleData.clear();
}

void ParticleSmoke::BuildParticleParameters(ParticleParameter* PP)
{
	PP->LifeTime = mLifeTime;
	PP->LifeTimeTolerance = mLifeTimeTolerance;

	PP->StartCol = mStartCol;
	PP->StartColTolerance = mStartColTolerance;
	PP->EndCol = mEndCol;
	PP->EndColTolerance = mEndColTolerance;

	PP->Direction = mDirection;
	PP->DirectionTolerance = mDirectionTolerance;

	PP->WorldPosition = GetAttachSceneNode()->GetPosition();
	PP->StartOffsetTolerance = mStartOffsetTolerance;

	PP->Velocity = mVelocity;
	PP->VelocityTolerance = mVelocityTolerance;
	PP->Acceleration = mAcceleration;
	PP->AccelerationVelocity = mAccelerationVelocity;
	PP->SegmentIndex = mSegmentBuildIndex;
	PP->PlayTimes = mPlayTimes;
	PP->UpdateDelta = mUpdateDelta;

	return;
}

int ParticleSmoke::UpdateAndGetFreeSegment(float MaxLifeTime)
{
	int Index = -1;
	for (size_t i = 0; i < mSegmentCount; i++)
	{
		if (mSegmentLifeTime[i] < 0)
		{
			if (Index == -1)
			{
				Index = i;
				mSegmentLifeTime[i] = 0;
			}
		}
		else
		{
			mSegmentLifeTime[i] += mUpdateDelta;
			if (mSegmentLifeTime[i] >= MaxLifeTime)
			{
				mSegmentLifeTime[i] = -1;
			}
		}
	}
	return Index;
}

bool ParticleSmoke::Initialise()
{
	if (mMaxElementCount == 0)
	{
		return false;
	}

	ID3D11Device* Device = Scene::GetCurrentScene()->GetRenderSystem()->GetD3d11Device();
	if (mGpuOptimization)
	{
		float UpdateCountPerSecond = 1.0f / mUpdateDelta;
		int SegSize = int((float)mProducePerSecond / UpdateCountPerSecond);

		int Mod = SegSize % 4;
		if (Mod > 0)
		{
			mSegmentSize = SegSize + 4 - Mod;
		}
		else
		{
			mSegmentSize = SegSize;
		}
		mSegmentCount = (mMaxElementCount / mSegmentSize) + 1;
		// reset max element
		mMaxElementCount = mSegmentCount * mSegmentSize;

		mSegmentLifeTime = new float[mSegmentCount];
		for (int i = 0; i < mSegmentCount; i++)
		{
			mSegmentLifeTime[i] = -1.0f;
		}

		char ConstBuffer[ConstBufferLen] = {0};
		ParticleParameter PP;
		BuildParticleParameters(&PP);
		memcpy(ConstBuffer, &PP, sizeof(PP));
		mConstBufferForBuildParticle = new DataBuffer(Device, D3D11_BIND_CONSTANT_BUFFER, ConstBuffer, ConstBufferLen);

		int UAVElementSize = sizeof(ParticleSmokeData);
		ParticleSmokeData* InitSmoke = new ParticleSmokeData[mMaxElementCount];
		memset(InitSmoke, 0, UAVElementSize * mMaxElementCount);
		mSmokeDataUAV = new UAVBuffer(Device, InitSmoke, UAVElementSize, mMaxElementCount);

		VertexDataPoint* VDP = new VertexDataPoint[mMaxElementCount];
		memset(VDP, 0, sizeof(VertexDataPoint) * mMaxElementCount);
		mSmokeVertexDataUAV = new UAVBuffer(Device, VDP, sizeof(VertexDataPoint), mMaxElementCount);
		mVertexElementSize = sizeof(VertexDataPoint);
		mVertexBuffer = new DataBuffer(Device, D3D11_BIND_VERTEX_BUFFER, mVertexData, mMaxElementCount * mVertexElementSize, 0);

		SAFE_DELETE_ARRAY(InitSmoke);
		SAFE_DELETE_ARRAY(VDP);

		// Compile Shaders
		ID3DBlob* pBlob = nullptr;
		HRESULT hr = Shader::CompileShaderFromFile(L"ParticleSmoke.hlsl", "BuildSmokeData", "cs_5_0", &pBlob);
		if (FAILED(hr))
		{
			OutputDebugStringA("Compile Compute Shader Failed: BuildSmokeData()");
		}
		hr = Device->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &mBuildSmokeCS);
		if (FAILED(hr))
		{	
			OutputDebugStringA("Create Compute Shader Failed: mBuildSmokeCS");
		}
		pBlob->Release();

		hr = Shader::CompileShaderFromFile(L"ParticleSmoke.hlsl", "BuildSmokeVertexData", "cs_5_0", &pBlob);
		if (FAILED(hr))
		{
			OutputDebugStringA("Compile Compute Shader Failed: BuildSmokeVertexData()");
		}
		hr = Device->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &mCalcSmokeCS);
		if (FAILED(hr))
		{
			OutputDebugStringA("Create Compute Shader Failed: mCalcSmokeCS");
		}
		pBlob->Release();
	}
	else
	{
		mVertexData = new VertexDataPoint[mMaxElementCount];
		memset(mVertexData, 0, sizeof(VertexDataPoint) * mMaxElementCount);

		mVertexElementSize = sizeof(VertexDataPoint);

		mVertexBuffer = new DataBuffer(Device, D3D11_BIND_VERTEX_BUFFER, mVertexData, mMaxElementCount * mVertexElementSize);
	}
	return true;
}

void ParticleSmoke::Update(float Delta)
{
	static float UpdateDelta = 0.0f;
	UpdateDelta += Delta;
	mPlayTimes += Delta;
	if (UpdateDelta >= mUpdateDelta)
	{
		UpdateDelta -= mUpdateDelta;
		Delta = mUpdateDelta;
	}
	else
	{
		// do nothing
		return;
	}
	if (mGpuOptimization)
	{
		// Calculate which segment index to build new particle
		static float MaxLifeTime = mLifeTime + mLifeTimeTolerance;
		mSegmentBuildIndex = UpdateAndGetFreeSegment(MaxLifeTime);
		// Update const buffer
		char ConstBuffer[ConstBufferLen] = { 0 };
		ParticleParameter PP;
		BuildParticleParameters(&PP);
		memcpy(ConstBuffer, &PP, sizeof(PP));
		RenderSystemD3D11* RS = Scene::GetCurrentScene()->GetRenderSystem();
		ID3D11DeviceContext* Context = RS->GetD3d11Context();
		mConstBufferForBuildParticle->UpdateData(Context, ConstBuffer, ConstBufferLen);

		// Compute shader to build the new segment
		Context->CSSetConstantBuffers(0, 1, &(mConstBufferForBuildParticle->D3DBuffer));

		Context->CSSetUnorderedAccessViews(0, 1, &mSmokeDataUAV->UAV, nullptr);
		Context->CSSetUnorderedAccessViews(1, 1, &mSmokeVertexDataUAV->UAV, nullptr);

		Context->CSSetShader(mBuildSmokeCS, nullptr, 0);
		Context->Dispatch(1, 1, 1);
		// Compute shader to update all smoke particles
		Context->CSSetShader(mCalcSmokeCS, nullptr, 0);

		Context->Dispatch(mSegmentCount, 1, 1);
		// copy the new data to the vertex buffer
		Context->CopyResource(mVertexBuffer->D3DBuffer, mSmokeVertexDataUAV->D3DBuffer);
	}
	else
	{
		// Update all elements
		// Calculate exist elements
		mVertexCount = 0;
		std::list<ParticleSmokeData*>::iterator it = mParticleData.begin();
		for (it; it != mParticleData.end(); )
		{
			ParticleSmokeData* PD = *it;
			PD->CurrentLifeTime += Delta;
			if (PD->CurrentLifeTime >= PD->TotalLifeTime)
			{
				delete(*it);
				mParticleData.erase(it++);
			}
			else
			{
				// update all data
				PD->Direction = CalcNewDirection(PD->Direction);
				float Velocity = CalcRealTimeVelocity(PD->CurrentVelocity, PD->Acceleration, Delta);
				PD->CurrentPos += PD->Direction * Velocity * Delta;
				Vector3 Col = PD->ColStart + (PD->ColEnd - PD->ColStart) * (PD->CurrentLifeTime / PD->TotalLifeTime);
				VertexDataPoint VDP;
				VDP.Col = Col;
				VDP.WorldPos = PD->CurrentPos;
				memcpy(mVertexData + mVertexCount, &VDP, sizeof(VertexDataPoint));
				// ++
				++it;
				++mVertexCount;
			}
		}

		// Add new elemenets
		int AddCount = int(float(mProducePerSecond) * Delta);
		if (AddCount + mVertexCount >= mMaxElementCount)
		{
			AddCount = mMaxElementCount - mVertexCount;
		}
		for (int i = 0; i < AddCount; i++)
		{
			ParticleSmokeData* PD = new ParticleSmokeData;
			PD->TotalLifeTime = mLifeTime + RangeRandom(-mLifeTimeTolerance, mLifeTimeTolerance);
			PD->CurrentLifeTime = 0;
			PD->ColStart = mStartCol + Vector3(RangeRandom(-mStartColTolerance, mStartColTolerance), RangeRandom(-mStartColTolerance, mStartColTolerance), RangeRandom(-mStartColTolerance, mStartColTolerance));
			
			PD->ColStart.x = Clamp(PD->ColStart.x, 0.0f, 1.0f);
			PD->ColStart.y = Clamp(PD->ColStart.y, 0.0f, 1.0f);
			PD->ColStart.z = Clamp(PD->ColStart.z, 0.0f, 1.0f);

			PD->ColEnd = mEndCol + Vector3(RangeRandom(-mEndColTolerance, mEndColTolerance), RangeRandom(-mEndColTolerance, mEndColTolerance), RangeRandom(-mEndColTolerance, mEndColTolerance));

			PD->ColEnd.x = Clamp(PD->ColEnd.x, (0.0f), (1.0f));
			PD->ColEnd.y = Clamp(PD->ColEnd.y, (0.0f), (1.0f));
			PD->ColEnd.z = Clamp(PD->ColEnd.z, (0.0f), (1.0f));

			PD->Direction = mDirection + Vector3(RangeRandom(-mDirectionTolerance.x, mDirectionTolerance.x), RangeRandom(-mDirectionTolerance.y, mDirectionTolerance.y), RangeRandom(-mDirectionTolerance.z, mDirectionTolerance.z));

			PD->CurrentPos = GetAttachSceneNode()->GetPosition() + Vector3(RangeRandom(-mStartOffsetTolerance.x, mStartOffsetTolerance.x), RangeRandom(-mStartOffsetTolerance.y, mStartOffsetTolerance.y), RangeRandom(-mStartOffsetTolerance.z, mStartOffsetTolerance.z));

			PD->CurrentVelocity = mVelocity + RangeRandom(-mVelocityTolerance, mVelocityTolerance);
			PD->Acceleration = mAcceleration + RangeRandom(-mAccelerationVelocity, mAccelerationVelocity);
			mParticleData.push_back(PD);
			VertexDataPoint VDP;
			VDP.Col = PD->ColStart;
			VDP.WorldPos = PD->CurrentPos;
			memcpy(mVertexData + mVertexCount, &VDP, sizeof(VertexDataPoint));
			++mVertexCount;
		}

		// update vertex data
		if (mVertexCount > 0)
		{
			RenderSystemD3D11* RS = Scene::GetCurrentScene()->GetRenderSystem();
			ID3D11DeviceContext* Context = RS->GetD3d11Context();
			mVertexBuffer->UpdateData(Context, mVertexData, mVertexCount * mVertexElementSize);
		}
	}
}

void ParticleSmoke::Render(Matrix4& WorldTransform)
{
	if (mVertexCount <= 0 && !mGpuOptimization) return;
	char UniformBuffer[MAX_CONST_BUFFER];
	memset(UniformBuffer, 0, sizeof(UniformBuffer));

	XMMATRIX XMProjectionView = Scene::GetCurrentScene()->GetCurrentCamera()->GetProjectViewMatrixPerspect();

	memcpy(UniformBuffer + 0, &XMProjectionView, 64);

	RenderSystemD3D11* RS = Scene::GetCurrentScene()->GetRenderSystem();
	ID3D11DeviceContext* Context = RS->GetD3d11Context();
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE map;
	hr = Context->Map(RS->GetUniformBuffer()->D3DBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, UniformBuffer, MAX_CONST_BUFFER);
	Context->Unmap(RS->GetUniformBuffer()->D3DBuffer, 0);

	Shader* S = mMaterial->GetShader();

	Context->IASetInputLayout(S->GetD3d11InputLayout());
	UINT offset = 0;
	Context->IASetVertexBuffers(0, 1, &mVertexBuffer->D3DBuffer, &mVertexElementSize, &offset);
	Context->IASetPrimitiveTopology(mPrimitiveType);
	Context->VSSetShader(S->GetD3d11VertexShader(), NULL, 0);
	Context->PSSetShader(S->GetD3d11PixelShader(), NULL, 0);

	ID3D11SamplerState* SampleState = mMaterial->GetSampleState();
	Context->PSSetSamplers(0, 1, &(SampleState));
	Context->RSSetState(mMaterial->GetRasterState());

	Context->OMSetDepthStencilState(mMaterial->GetDepthStencilState(), 0);
	Context->OMSetBlendState(mMaterial->GetBlendState(), NULL, 0xffffffff);
	if (mGpuOptimization)
	{
		Context->Draw(mMaxElementCount, 0);
	}
	else
	{
		Context->Draw(mVertexCount, 0);
	}
}





ParticleManager::ParticleManager()
{

}

ParticleManager::~ParticleManager()
{
	Clear();
}

ParticleSmoke* ParticleManager::CreateSmoke(std::string Name/* = ""*/)
{
	if (Name == "")
	{
		Name = GetAutoName();
	}
	if (mParticleArray.find(Name) != mParticleArray.end())
	{
		return static_cast<ParticleSmoke*>(mParticleArray[Name]);
	}
	ParticleSmoke* P = new ParticleSmoke(Name);

	mParticleArray[Name] = P;

	return P;
}

void ParticleManager::Update(float Delta)
{
	std::map<std::string, Particle*>::iterator it;
	for (it = mParticleArray.begin(); it != mParticleArray.end(); it++)
	{
		it->second->Update(Delta);
	}
}

void ParticleManager::DestroyParticle(std::string Name)
{
	if (mParticleArray.find(Name) == mParticleArray.end())
	{
		return;
	}

	Particle* P = mParticleArray[Name];

	SAFE_DELETE(P);
	mParticleArray.erase(Name);
}

void ParticleManager::DestroyParticle(Particle* P)
{
	std::string Name = P->GetName();
	DestroyParticle(Name);
}