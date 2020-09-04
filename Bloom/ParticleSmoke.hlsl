

#define			SEGMENT_SIZE			168



struct RWVertexData
{
	float3 WorldPos;
	float3 Col;
};

struct RWSmokeData
{
	float TotalLifeTime;
	float CurrentLifeTime;
	float CurrentVelocity;
	float Acceleration;
	float3 CurrentPos;
	float3 Direction;
	float3 ColStart;
	float3 ColEnd;
};

cbuffer CB : register(b0)
{
	float4 StartCol;
	float4 EndCol;
	float4 Direction;
	float4 DirectionTolerance;
	float4 WorldPosition;
	float4 StartOffsetTolerance;

	float LifeTime;
	float LifeTimeTolerance;

	float StartColTolerance;

	float EndColTolerance;

	float Velocity;
	float VelocityTolerance;
	float Acceleration;
	float AccelerationVelocity;
	// describe which segment been fresh
	float PlayTimes;
	float UpdateDelta;
	int SegmentIndex;
};

RWStructuredBuffer<RWSmokeData> SmokeData : register(u0);
RWStructuredBuffer<RWVertexData> VertexBuffer : register(u1);

float rand(float2 n)
{
	return frac(sin(dot(n.xy, float2(12.9898, 78.233)))* 43758.5453) * 2.0 - 1.0;
}

float rand_0_1(float2 n)
{
	return frac(sin(dot(n.xy, float2(12.9898, 78.233)))* 43758.5453);
}

// Dispath:(1, 0, 0)
[numthreads(SEGMENT_SIZE, 1, 1)]
void BuildSmokeData(uint3 Gid : SV_GroupID,
	uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID,
	uint GI : SV_GroupIndex)
{
	uint ThreadOffset = GTid.x;
	// calculate offset to build new particle
	uint TotalOffset = SegmentIndex * SEGMENT_SIZE + ThreadOffset;

	RWSmokeData Temp = SmokeData[TotalOffset];

	float xSeed = TotalOffset + SegmentIndex;
	float2 n = float2(xSeed + LifeTime, PlayTimes);

	float r = rand(n);

	Temp.TotalLifeTime = LifeTime + r * LifeTimeTolerance;
	Temp.CurrentLifeTime = 0.0f;
	
	n = float2(xSeed + Velocity, PlayTimes);
	Temp.CurrentVelocity = Velocity + rand(n) * VelocityTolerance;
	n = float2(xSeed + Acceleration, PlayTimes);
	Temp.Acceleration =Acceleration + rand(n) * AccelerationVelocity;

	n = n + StartOffsetTolerance.xy;
	Temp.CurrentPos = WorldPosition.xyz + rand(n) * StartOffsetTolerance;

	n = n + DirectionTolerance.xy;
	Temp.Direction = Direction.xyz + rand(n) * DirectionTolerance;

	n.x = n.x + StartColTolerance;
	r = rand(n);
	Temp.ColStart = clamp(StartCol.xyz + float3(r * StartColTolerance, r * StartColTolerance, r * StartColTolerance), float3(0, 0, 0), float3(1, 1, 1));

	n.x = n.x + EndColTolerance;
	r = rand(n);
	Temp.ColEnd = clamp(EndCol.xyz + float3(r * EndColTolerance, r * EndColTolerance, r * EndColTolerance), float3(0, 0, 0), float3(1, 1, 1));

	SmokeData[TotalOffset] = Temp;
}


float CalcRealTimeVelocity(float Velociry, float Acceleration, float Delta)
{
	float VT = Velociry + Acceleration * Delta;
	return VT;
}


[numthreads(SEGMENT_SIZE, 1, 1)]
void BuildSmokeVertexData(uint3 Gid : SV_GroupID,
	uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID,
	uint GI : SV_GroupIndex)
{
	// calculate real offset
	uint Offset = Gid.x * SEGMENT_SIZE + GTid.x;

	RWSmokeData Temp = SmokeData[Offset];

	Temp.CurrentLifeTime = Temp.CurrentLifeTime + UpdateDelta;

	if (Temp.CurrentLifeTime < Temp.TotalLifeTime)
	{
		float Velocity = CalcRealTimeVelocity(Temp.CurrentVelocity, Temp.Acceleration, UpdateDelta);
		Temp.CurrentPos += (Temp.Direction * Velocity * UpdateDelta);
		float3 Col = Temp.ColStart + (Temp.ColEnd - Temp.ColStart) * (Temp.CurrentLifeTime / Temp.TotalLifeTime);

		RWVertexData VDP;
		VDP.Col = Col;
		VDP.WorldPos = Temp.CurrentPos;
		VertexBuffer[Offset] = VDP;
		
		SmokeData[Offset] = Temp;
	}
	else
	{
		// error data, this particle is dead.
		VertexBuffer[Offset].WorldPos = float3(-10000, -10000, -10000);
	}
}