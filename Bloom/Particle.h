/***********************************************
 * File: Particle.h
 *
 * Author: LYG
 * Date: °ËÔÂ 2020
 *
 * Purpose:
 *
 * 
 **********************************************/
#pragma once

#include <xstring>
#include <map>
#include <list>
#include "Common.h"
#include "Renderable.h"
#include "Material.h"
#include "Math/Vector4.h"
#include "Math/Vector3.h"
#include "DataBuffer.h"

// for const buffer, gpu build particle.
struct ParticleParameter
{
	Vector4 StartCol;
	Vector4 EndCol;

	Vector4 Direction;
	Vector4 DirectionTolerance;

	Vector4 WorldPosition;
	Vector4 StartOffsetTolerance;

	float LifeTime;
	float LifeTimeTolerance;
	float StartColTolerance;
	float EndColTolerance;
	float Velocity;
	float VelocityTolerance;
	float Acceleration;
	float AccelerationVelocity;
	// Time to build random vector
	float PlayTimes;
	float UpdateDelta;
	// describe which segment been fresh
	int SegmentIndex;
};

struct ParticleSmokeData
{
	float TotalLifeTime;
	float CurrentLifeTime;
	float CurrentVelocity;
	float Acceleration;
	Vector3 CurrentPos;
	Vector3 Direction;
	Vector3 ColStart;
	Vector3 ColEnd;
	ParticleSmokeData()
	{
		memset(this, 0, sizeof(ParticleSmokeData));
	}
};

struct VertexDataPoint
{
	Vector3 WorldPos;
	Vector3 Col;
};

class ParticleManager;
class Particle : public Renderable
{
	friend class ParticleManager;
public:
	void SetMaterial(Material* Mat);
	Material* GetMaterial() const;

	void SetLifeTimeRange(float TotalTime, float Tolerance);
	void SetColorRange(Vector3 StartCol, float StartTolerance, Vector3 EndCol, float EndTolerance);

	void SetMaxCount(int MaxCount);
	int GetMaxCount() const;

	void SetProduceCountPerSecond(int Count);
	int GetProduceCountPerSecond() const;

	void SetDirection(Vector3 Direction, float Tolerance);
	void SetStartOffset(Vector3 OffsetRange);
	void SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY PriType);

	void SetVelocity(float Velocity, float Tolerance);
	void SetAcceleration(float Acceleration, float Tolerance);

	void SetUpdateDelta(float Delta);
	// we will detected myself. but you can force gpu optimization.
	void ForceGpuOptimization(bool UseGpu = true);
	virtual bool Initialise();
protected:
	Particle(std::string Name);
	virtual ~Particle();

	virtual void Update(float Delta);
	virtual void Render(Matrix4& WorldTransform) override;

	// calculate new direction, may be consider wind factor
	Vector3 CalcNewDirection(Vector3 Direction) const;
	float CalcRealTimeVelocity(float Velociry, float Acceleration, float Delta) const;
protected:
	bool mGpuOptimization;

	float mLifeTime;
	float mLifeTimeTolerance;
	
	Vector3 mStartCol;
	float mStartColTolerance;
	Vector3 mEndCol;
	float mEndColTolerance;

	int mProducePerSecond;

	Vector3 mDirection;
	Vector3 mDirectionTolerance;

	Vector3 mStartOffsetTolerance;
	float mVelocity;
	float mVelocityTolerance;
	float mAcceleration;
	float mAccelerationVelocity;

	int mMaxElementCount;

	float mUpdateDelta;

	int mVertexCount;
	Material* mMaterial;
	DataBuffer* mVertexBuffer;
	UINT mVertexElementSize;
	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveType;

	DataBuffer* mIndexBuffer;
	UINT mIndexCount;
	DXGI_FORMAT mIndexFormat;
};

class ParticleSmoke : public Particle
{
	friend class ParticleManager;
public:
	virtual bool Initialise() override;
protected:
	ParticleSmoke(std::string Name);
	virtual ~ParticleSmoke();

	virtual void Update(float Delta) override;
	virtual void Render(Matrix4& WorldTransform) override;

	void BuildParticleParameters(ParticleParameter* PP);
	int UpdateAndGetFreeSegment(float MaxLifeTime);

	std::list<ParticleSmokeData*> mParticleData;
	VertexDataPoint* mVertexData;

	DataBuffer* mConstBufferForBuildParticle;
	static const int ConstBufferLen = 1024;

	UAVBuffer* mSmokeDataUAV;
	UAVBuffer* mSmokeVertexDataUAV;
	ID3D11ComputeShader* mBuildSmokeCS;
	ID3D11ComputeShader* mCalcSmokeCS;
	// Record the segment lifetime, which update every "UpdateTime", not every frame.
	float* mSegmentLifeTime;

	int mSegmentCount;
	int mSegmentSize;
	int mSegmentBuildIndex;
	float mPlayTimes;
};

class ParticleManager
{
	friend class Scene;
public:
	ParticleSmoke* CreateSmoke(std:: string Name = "");


	void DestroyParticle(std::string Name);
	void DestroyParticle(Particle* P);
protected:
	ParticleManager();
	~ParticleManager();

	void Update(float Delta);

	std::map<std::string, Particle*> mParticleArray;
	GET_AUTO_NAME("Particle");
	CLEAR_MAP(mParticleArray);
};