/*!
 * \file Shader.cpp
 *
 * \author LYG
 * \date 四月 2018
 *
 * 
 */
#include "Shader.h"
#include "Scene.h"
#include "RenderSystem.h"

static char* DefaultStandardVertexShaderSrc =
"float4x4 ProjViewWorld;"
"void main(in  float4 Position  : POSITION,"
"          out float4 oPosition : SV_Position)"
"{   oPosition = mul(ProjViewWorld, Position);}";

static char* DefaultStandardSampleVertexShaderSrc =
"float4x4 ProjViewWorld;"
"void main(in  float4 Position  : POSITION,    in  float2 TexCoord : TEXCOORD0,"
"          out float4 oPosition : SV_Position, out float2 oTexCoord : TEXCOORD0)"
"{   oPosition = mul(ProjViewWorld, Position);"
"	 oTexCoord = TexCoord;}";

static char* DefaultStandardVertexColorVertexShaderSrc =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjView;"
"}"
"void main(in  float4 Position  : POSITION,    in  float4 VertexCol : COLOR0,"
"          out float4 oPosition : SV_Position, out float4 oVertexCol : COLOR0)"
"{"
"	oPosition = mul(ProjView, Position);"
//"	oPosition = Position;"
"	oVertexCol = VertexCol;}";

// Use this shader as text vertex shader. we can do a in-order fade in effects.
const char* DefaultTextSampleVertexShaderSrc =
"float4x4 ProjViewWorld;"
"void main(in  float4 Position  : POSITION,"
"		   in  float2 UV : TEXCOORD0,"
"		   in  uint Index : BLENDINDICES0,"
"          out float4 oPosition : SV_Position, out float2 oUV: TEXCOORD0, out uint oIndex : BLENDINDICES0)"
"{   oPosition = mul(ProjViewWorld, Position); "
"    oUV = UV;  oIndex = Index;}";

static char* DefaultPixelShaderSrcBlack =
"float4 main(in float4 Position : SV_Position) : SV_Target"
"{ "
"    return float4(0, 0, 0, 1); }";

static char* DefaultPixelShaderSrcWhite =
"float4 main(in float4 Position : SV_Position) : SV_Target"
"{ "
"    return float4(1, 1, 1, 1); }";

static char* DefaultPixelShaderSrcRed =
"float4 main(in float4 Position : SV_Position) : SV_Target"
"{ "
"    return float4(1, 0, 0, 1); }";

static char* DefaultPixelShaderSrcGreen =
"float4 main(in float4 Position : SV_Position) : SV_Target"
"{ "
"    return float4(0, 1, 0, 1); }";

static char* DefaultPixelShaderSrcBlue =
"float4 main(in float4 Position : SV_Position) : SV_Target"
"{ "
"    return float4(0, 0, 1, 1); }";


static char* DefaultPixelShaderSrcSimpleColor =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float4 Col;"
"}"
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	return Col;"
"}";

static char* DefaultPixelShaderSrcSimpleVertexColor =
"float4 main(in float4 Position : SV_Position, in float4 VertexColor : COLOR0) : SV_Target"
"{ "
"   return VertexColor;"
"}";

static char* DefaultPixelShaderSrcSimpleSample =
"Texture2D Texture   : register(t0); SamplerState Linear : register(s0); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{   float4 TexCol = Texture.Sample(Linear, TexCoord); "
//"    if (TexCol.a==0) clip(-1); " // If alpha = 0, don't draw
"    return TexCol;}";

static char* DefaultPixelShaderSrcSimpleFade =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float Alpha;"
"}"
"Texture2D Texture   : register(t0); SamplerState Linear : register(s0); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float4 TexCol = Texture.Sample(Linear, TexCoord); "
"	TexCol = TexCol * Alpha;"
"	return TexCol;"
"}";

static char* DefaultPixelShaderSrcSimpleFadeInOut =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float Alpha;"
"}"
"Texture2D TextureOut   : register(t0); SamplerState Linear : register(s0); "
"Texture2D TextureIn   : register(t1); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float4 TexColIn = TextureIn.Sample(Linear, TexCoord); "
"	float4 TexColOut = TextureOut.Sample(Linear, TexCoord); "
"	float4 TexCol = TexColIn * Alpha + TexColOut * (1 - Alpha);"
"	return TexCol;"
"}";

static char* DefaultPixelShaderSrcSimpleN_B_N =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float Alpha;"
"}"
"Texture2D Texture   : register(t0); SamplerState Linear : register(s0); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float4 TexCol = Texture.Sample(Linear, TexCoord); "
"	TexCol = saturate((1 - TexCol) * Alpha + TexCol);"
"	return TexCol;"
"}";

static char* DefaultPixelShaderSrcSimpleL_R_L =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float Alpha;"
"}"
"Texture2D Texture   : register(t0); SamplerState Linear : register(s0); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float4 TexCol = Texture.Sample(Linear, TexCoord); "
"	if(TexCoord.x < Alpha) TexCol.a = 0;"
"	return TexCol;"
"}";

static char* DefaultPixelShaderSrcSimpleElipseScale =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float Alpha;"
"}"
"Texture2D Texture   : register(t0); SamplerState Linear : register(s0); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float4 TexCol = Texture.Sample(Linear, TexCoord); "
"	float2 Coord = TexCoord * 2.0 - 1.0;"			// extern to -1, 1
"	Coord.x = Coord.x * 16.0 / 9.0;"				// extern to the real resolution
"	float AntiAlpha = 1.0 - Alpha;"
"	float a = AntiAlpha * 1.5 * 16.0 / 9.0;"
"	float c = a / 2.0;"
"	float2 c1 = float2(c, 0);"
"	float2 c2 = float2(c * -1.0, 0);"
"	float d = distance(c1, Coord) + distance(c2, Coord);"
"	if(d > a * 2) TexCol.a = 0;"
"	return TexCol;"
"}";

static char* DefaultPixelShaderSrcSimpleLayerAlpha =
"Texture2D TextureOut   : register(t0); SamplerState Linear : register(s0); "
"Texture2D TextureIn   : register(t1); "
"Texture2D TextureAlpha   : register(t2); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float4 TexColIn = TextureIn.Sample(Linear, TexCoord); "
"	float4 TexColOut = TextureOut.Sample(Linear, TexCoord); "
"	float Alpha = TextureAlpha.Sample(Linear, TexCoord).r;"
"	float4 TexCol = TexColIn * Alpha + TexColOut * (1 - Alpha);"
"	return TexCol;"
"}";

static char* DefaultPixelShaderSrcSimpleHelix =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float Alpha;"
"}"
"Texture2D Texture   : register(t0); SamplerState Linear : register(s0); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float2 Coord = TexCoord * 2.0 - 1.0;"
"	float Current_R = length(Coord);"
"	float AntiAlpha = 1.0 - Alpha;"
"	float Original_R = 0.0;"
"	if(AntiAlpha > 0)"
"	{"
"	Original_R = Current_R / AntiAlpha;"
"	}"
"	float Circles = 1.0;"
"	float PI = 3.141592654;"
"	float CurrentAngle = atan2(Coord.y, Coord.x);"
"	float RotateAngle = Circles * 2.0 * PI * Alpha;"
"	float OriginalAngle = CurrentAngle + RotateAngle;"
"	float x = Original_R * cos(OriginalAngle);"
"	float y = Original_R * sin(OriginalAngle);"
"	float4 TexCol;"
"	if(x > 1 || x < -1 || y > 1 || y < -1)"
"	{"
"	TexCol = float4(0, 0, 0, 0);"
"	}"
"	else"
"	{"
"	Coord = float2(x, y);"
"	Coord = (Coord + 1.0) / 2.0;"
"	TexCol = Texture.Sample(Linear, Coord);"
"	TexCol = saturate(TexCol * (1 + Alpha));"
"	}"
"	float4 OriginalTex = Texture.Sample(Linear, TexCoord);"
"	TexCol = TexCol * Alpha + OriginalTex * AntiAlpha;"
"	return TexCol;"
"}";

// 想实现那个颜料流动的，融合的渐变效果，实在是没有写出来。以后再考虑考虑
static char* DefaultPixelShaderSrcSimpleLighting =
"float4 SimpleLighting(float4 LightCol, float3 LightDir, float3 N, float3 V, float4 Ambient, float4 TexCol)"
"{"
"	float diffuseAmount = saturate(dot(LightDir, N));"
"	float3 diffuse = diffuseAmount * LightCol.rgb;"
"	float3 R = reflect(-LightDir, N);"
"	float RdotV = dot(R, V);"
"	float3 Specular = LightCol * pow(max(0.001f, RdotV), 8.0);"
"	float4 TexColOut = (Ambient + float4(diffuse, 1.0)) * TexCol + float4(Specular, 1.0);"
"	return TexColOut;"
"}"

"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float Alpha;"
"}"
"Texture2D TextureOut   : register(t0); SamplerState Linear : register(s0); "
"Texture2D TextureIn   : register(t1); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float4 LightCol = float4(0.58, 0.157, 1, 1);"
"	float3 LightDirStart = normalize(float3(-1, -1, 1));"
"	float3 LightDirEnd = normalize(float3(1, -1, 1));"
"	float4 SceneAmbient = float4(0.5, 0.5, 0.5, 1);"
"	float3 L =  LightDirStart + (LightDirEnd - LightDirStart) * Alpha;"
"	float AssumeDepth = -3.0;"
"	float2 WorldXY = TexCoord * 2.0 - 1.0;"
"	float3 ViewDir = -float3(WorldXY, AssumeDepth);"
"	float step = 1.0 / 1080.0;"
"	float2 UV;"
"	float4 TexCol1 = float4(0, 0, 0, 1);"
"	float4 TexCol2 = float4(0, 0, 0, 1);"

"	for (int i = -2; i < 3; i++)"
"	{"
"		for (int j = -2; j < 3; j++)"
"		{"
"			UV.x = TexCoord.x + j * step;"
"			UV.y = TexCoord.y + i * step;"
"			float4 TexColIn = TextureIn.Sample(Linear, UV);"
"			float4 TexColOut = TextureOut.Sample(Linear, UV); "
"			float3 N1 = normalize(TexColOut.rgb * 2.0 - 1.0);"
"			float3 N2 = normalize(TexColIn.rgb * 2.0 - 1.0);"
"			TexCol1 += SimpleLighting(LightCol, L, N1, ViewDir, SceneAmbient, TexColOut);"
"			TexCol2 += SimpleLighting(LightCol, L, N2, ViewDir, SceneAmbient, TexColIn);"
"		}"
"	}"
"	TexCol1 = TexCol1 / 25.0;"
"	TexCol2 = TexCol2 / 25.0;"
"	float4 TexCol = TexCol1 * TexCol2;"
"	float4 Col = TextureOut.Sample(Linear, TexCoord);"
"	TexCol = Col * (1.0 - Alpha) + TexCol * Alpha;"
"	return TexCol;"
"}";

static char* DefaultPixelShaderSrcSimpleInOutAndBlurBlend =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float Alpha;"
"}"
"Texture2D TextureOut   : register(t0); SamplerState Linear : register(s0); "
"Texture2D TextureIn   : register(t1); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float BlendFactor = 4.99;"
"	float step = 1.0 / 512.0;"
"	float AntiAlpha = 1.0 - Alpha;"
"	int TopBlend = BlendFactor * AntiAlpha;"
"	int BottomBlend = BlendFactor * Alpha;"
"	float BlendCount = 0.0;"
"	float4 TexColIn = float4(0, 0, 0, 1);"
"	float4 TexColOut = float4(0, 0, 0, 1);"
"	float2 Coord = TexCoord * 2.0 - 1.0;"
"	for(int i = -TopBlend; i <= TopBlend; i++)"
"	{"
"		for(int j = -TopBlend; j <= TopBlend; j++)"
"		{"
"			float2 UV_IN = float2(Coord.x + float(j) * step, Coord.y + float(i) * step);;"
"			UV_IN = (UV_IN + 1.0) / 2.0;"
"			TexColIn += TextureIn.Sample(Linear, UV_IN);"
"			BlendCount++;"
"		}"
"	}"
"	TexColIn /= BlendCount;"
"	BlendCount = 0;"
"	for(int i = -BottomBlend; i <= BottomBlend; i++)"
"	{"
"		for(int j = -BottomBlend; j <= BottomBlend; j++)"
"		{"
"			float2 UV_OUT = float2(Coord.x + float(j) * step, Coord.y + float(i) * step);"
"			UV_OUT = (UV_OUT + 1.0) / 2.0;"
"			TexColOut += TextureOut.Sample(Linear, UV_OUT);"
"			BlendCount++;"
"		}"
"	}"
"	TexColOut /= BlendCount;"
"	float4 TexCol;"
"	if(TexCoord.y <= Alpha)"
"	{"
"		TexCol = TexColIn;"
"	}"
"	else"
"	{"
"		TexCol = TexColOut;"
"	}"
"	return TexCol;"
"}";

static char* DefaultPixelShaderSrcSimplePerlineNoiseFadeInOut =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float Alpha;"
"}"
"Texture2D TextureOut   : register(t0); SamplerState Linear : register(s0); "
"Texture2D TextureNoise1   : register(t1); "
"Texture2D TextureNoise2   : register(t2); "
"Texture2D TextureNoise3   : register(t3); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float4 TexCol = TextureOut.Sample(Linear, TexCoord); "
"	float TexNoise1 = TextureNoise1.Sample(Linear, TexCoord).r;"
"	float TexNoise2 = TextureNoise2.Sample(Linear, TexCoord).r;"
"	float TexNoise3 = TextureNoise3.Sample(Linear, TexCoord).r;"
"	float a1 = min(Alpha, 0.3333) * 3 * TexNoise1;"
"	float a2 = min(max(Alpha - 0.3333, 0), 0.3333) * 3 * TexNoise2;"
"	float a3 = max((Alpha - 0.6667), 0) * 3 * TexNoise3;"
"	float a = saturate(a1 + a2 + a3);"
"	float4 AntiCol = 1 - TexCol;"
"	TexCol = saturate(TexCol + float4(a, a, a, 1) * AntiCol);"
"	return TexCol;"
"}";

static char* DefaultPixelShaderSrcSimpleUScroll =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float Alpha;"
"}"
"Texture2D TextureOut   : register(t0); SamplerState Linear : register(s0); "
"Texture2D TextureIn   : register(t1); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float u = TexCoord.x - Alpha;"
"	float4 TexCol;"
"	if(u < 0)"
"	{"
"		float2 UV_IN = float2(u + 1.0, TexCoord.y);"
"		TexCol = TextureIn.Sample(Linear, UV_IN);"
"	}"
"	else"
"	{"
"		float2 UV_OUT = float2(u, TexCoord.y);"
"		TexCol = TextureOut.Sample(Linear, UV_OUT);"
"	}"
"	return TexCol;"
"}";

static char* DefaultPixelShaderSrcSimpleFogSimulation =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float Alpha;"
"}"
"Texture2D TextureOut   : register(t0); SamplerState Linear : register(s0); "
"Texture2D TextureNoise   : register(t1); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float4 TexCol = TextureOut.Sample(Linear, TexCoord);"
"	float u = (TexCoord.x + Alpha) / 2.0;"
"	float r = TextureNoise.Sample(Linear, float2(u, TexCoord.y / 2.0)).r;"
"	float4 NoiseCol = float4(r, r, r, 1);"
"	NoiseCol = NoiseCol * TexCoord.y * TexCoord.y;"
"	TexCol = TexCol + (1 - TexCol) * NoiseCol;"
"	return TexCol;"
"}";

static char* DefaultPixelShaderSrcSimpleSampleWithBlur =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float Alpha;"
"}"
"Texture2D TextureOut   : register(t0); SamplerState Linear : register(s0); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float step = 1.0 / 128.0;"
"	float2 Coord = TexCoord * 2.0 - 1.0;"
"	float4 TexColIn = float4(0, 0, 0, 0);"
"	float BlendCount = 0;"
"	int BlendOffset = 2;"
"	for(int i = -BlendOffset; i <= BlendOffset; i++)"
"	{"
"		for(int j = -BlendOffset; j <= BlendOffset; j++)"
"		{"
"			float2 UV_IN = float2(Coord.x + float(j) * step, Coord.y + float(i) * step);"
"			UV_IN = (UV_IN + 1.0) / 2.0;"
"			TexColIn += TextureOut.Sample(Linear, UV_IN);"
"			BlendCount++;"
"		}"
"	}"
"	float4 TexCol = TexColIn / BlendCount;"
"	return TexCol;"
"}";

static char* DefaultPixelShaderSrcSimpleFontSample =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float4 TextColor;"
"}"
"Texture2D Texture   : register(t0); SamplerState Linear : register(s0); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float2 TexCol = Texture.Sample(Linear, TexCoord).rg; "
"	float4 Col = TexCol.r * TextColor;"
"	Col.a = TexCol.g;"
"	return Col;"
"}";

static char* DefaultPixelShaderSrcSimpleTextFadeIn =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float4 TextColor;"
"	float4 RegionAlpha[8];"
"}"
"Texture2D Texture   : register(t0); SamplerState Linear : register(s0); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0, in uint Index : BLENDINDICES0) : SV_Target"
"{"
"	float2 TexCol = Texture.Sample(Linear, TexCoord).rg; "
"	float4 Col = TexCol.r * TextColor;"
"	float a = ((float[4])(RegionAlpha[Index/4]))[Index%4];"
"	Col.a = TexCol.g * a;"
"	return Col;"
"}";

static char* DefaultPixelShaderSrcSimpleTextFadeOut =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float4 TextColor;"
"	float Alpha;"
"}"
"Texture2D Texture   : register(t0); SamplerState Linear : register(s0); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float2 TexCol = Texture.Sample(Linear, TexCoord).rg; "
"	float4 Col = TexCol.r * TextColor;"
"	float a = Alpha;"
"	Col.a = TexCol.g * a;"
"	return Col;"
"}";

static char* DefaultPixelShaderSrcSimpleUVAnimation =
"cbuffer SceneConstantBuffer : register(b0)"
"{"
"	float4x4 ProjViewWorld;"
"	float4 ColorStyle;"
"	float2 UVOffset;"
"}"
"Texture2D Texture   : register(t0); SamplerState Linear : register(s0); "
"float4 main(in float4 Position : SV_Position, in float2 TexCoord : TEXCOORD0) : SV_Target"
"{"
"	float2 TexCol = Texture.Sample(Linear, TexCoord + UVOffset).rg; "
"	float4 Col = TexCol.r * ColorStyle;"
"	Col.a = TexCol.r;"
"	return Col;"
"}";

std::string StandardShaderName[CutomShader] = { "Simple_Black", "Simple_White", "Simple_Red", "Simple_Green", "Simple_Blue", "Simple_Color", "Simple_Texture_Sample" ,
"Simple_Fade","Simple_Fade_In_Out", "Simple_N_B_N", "Simple_L_R_L", "Simple_Elipse_Scale", "Simple_Layer_Alpha", "Simple_Helix", "SimpleLighting", "SimpleInOutAndBlurBlend",
"Simple_PerlinNoise", "Simple_UScroll", "Simple_FogSimulation", "Simple_SampleWithBlur", "Simple_FontSample", "Simple_TextFadeIn", "Simple_TextFadeOut",
"SimpleUVAnimation", "Simple_Vertex_Color"};

Shader::Shader()
{
	mVertexShader = nullptr;
	mPixelShader = nullptr;
	mInputLayout = nullptr;
}

Shader::~Shader()
{
	SAFE_RELEASE(mVertexShader);
	SAFE_RELEASE(mPixelShader);
	SAFE_RELEASE(mInputLayout);
}

bool Shader::Initialise(ID3D11Device* Device, std::string VSD, std::string PSD, unsigned int ShaderElementFlag)
{
	D3D11_INPUT_ELEMENT_DESC ElementDesc[10];
	UINT ElePos = 0;
	UINT Offset = 0;
	if (ShaderElementFlag & Ele_Position)
	{
		ElementDesc[ElePos++] = { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, Offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Offset += 12;
	}
	if (ShaderElementFlag & Ele_TexCoord0)
	{
		ElementDesc[ElePos++] = { "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, Offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Offset += 8;
	}
	if (ShaderElementFlag & Ele_TexCoord1)
	{
		ElementDesc[ElePos++] = { "TexCoord", 1, DXGI_FORMAT_R32G32_FLOAT, 0, Offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Offset += 8;
	}
	if (ShaderElementFlag & Ele_TexCoord2)
	{
		ElementDesc[ElePos++] = { "TexCoord", 2, DXGI_FORMAT_R32G32_FLOAT, 0, Offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Offset += 8;
	}
	if (ShaderElementFlag & Ele_Normal)
	{
		ElementDesc[ElePos++] = { "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, Offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Offset += 12;
	}
	if (ShaderElementFlag & Ele_Color)
	{
		ElementDesc[ElePos++] = { "Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, Offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Offset += 12;
	}
	if (ShaderElementFlag & Ele_BlendIndices)
	{
		ElementDesc[ElePos++] = { "BLENDINDICES", 0, DXGI_FORMAT_R32_UINT, 0, Offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Offset += 4;
	}
	int numVertexDesc = ElePos;
	// Create vertex shader
	ID3DBlob * blobData;
	ID3DBlob * errorBlob = nullptr;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT result = D3DCompile(VSD.c_str(), VSD.length(), 0, 0, 0, "main", "vs_4_0", dwShaderFlags, 0, &blobData, &errorBlob);
	if (FAILED(result))
	{
		return false;
	}
	result = Device->CreateVertexShader(blobData->GetBufferPointer(), blobData->GetBufferSize(), NULL, &mVertexShader);
	if (FAILED(result))
	{
		return false;
	}
	// Create input layout
	result = Device->CreateInputLayout(ElementDesc, numVertexDesc,
		blobData->GetBufferPointer(), blobData->GetBufferSize(), &mInputLayout);
	blobData->Release();
	if (FAILED(result))
	{
		return false;
	}
	// Create pixel shader
	result = D3DCompile(PSD.c_str(), PSD.length(), 0, 0, 0, "main", "ps_4_0", dwShaderFlags, 0, &blobData, 0);
	result = Device->CreatePixelShader(blobData->GetBufferPointer(), blobData->GetBufferSize(), NULL, &mPixelShader);
	blobData->Release();
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT Shader::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

ShaderManager::ShaderManager()
{

}

ShaderManager::~ShaderManager()
{
	std::map<std::string, Shader*>::iterator it;
	for (it = mShaderArray.begin(); it != mShaderArray.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	mShaderArray.clear();
}

void ShaderManager::InitialiseStandardShaders()
{
	unsigned int ShaderElementFlag = 0;
	// simple black
	ShaderElementFlag |= Ele_Position;
	CreateCustomShader(StandardShaderName[SimpleBlack], DefaultStandardVertexShaderSrc, DefaultPixelShaderSrcBlack, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleWhite], DefaultStandardVertexShaderSrc, DefaultPixelShaderSrcWhite, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleRed], DefaultStandardVertexShaderSrc, DefaultPixelShaderSrcRed, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleGreen], DefaultStandardVertexShaderSrc, DefaultPixelShaderSrcGreen, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleBlue], DefaultStandardVertexShaderSrc, DefaultPixelShaderSrcBlue, ShaderElementFlag);

	ShaderElementFlag |= Ele_TexCoord0;
	CreateCustomShader(StandardShaderName[SimpleColor], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleColor, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleTextureSample], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleSample, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleFade], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleFade, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleFadeInOut], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleFadeInOut, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleN_B_N], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleN_B_N, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleL_R_L], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleL_R_L, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleElipseScale], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleElipseScale, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleLayerAlpha], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleLayerAlpha, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleHelix], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleHelix, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleLighting], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleLighting, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleInOutAndBlurBlend], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleInOutAndBlurBlend, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimplePerlinNoise], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimplePerlineNoiseFadeInOut, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleUScroll], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleUScroll, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleFogSimulation], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleFogSimulation, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleSampleWithBlur], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleSampleWithBlur, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleFontSample], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleFontSample, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleTextFadeOut], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleTextFadeOut, ShaderElementFlag);
	CreateCustomShader(StandardShaderName[SimpleUVAnimation], DefaultStandardSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleUVAnimation, ShaderElementFlag);
	ShaderElementFlag |= Ele_BlendIndices;
	CreateCustomShader(StandardShaderName[SimpleTextFadeIn], DefaultTextSampleVertexShaderSrc, DefaultPixelShaderSrcSimpleTextFadeIn, ShaderElementFlag);

	// custom order
	ShaderElementFlag = Ele_Position | Ele_Color;
	CreateCustomShader(StandardShaderName[SimpleColorPointForParticle], DefaultStandardVertexColorVertexShaderSrc, DefaultPixelShaderSrcSimpleVertexColor, ShaderElementFlag);
}

Shader* ShaderManager::CreateCustomShader(std::string VSD, std::string PSD, unsigned int ShaderElementFlag)
{
	return CreateCustomShader(GetAutoName(), VSD, PSD, ShaderElementFlag);
}

Shader* ShaderManager::CreateCustomShader(std::string Name, std::string VSD, std::string PSD, unsigned int ShaderElementFlag)
{
	if (mShaderArray.find(Name) != mShaderArray.end())
	{
		return mShaderArray[Name];
	}
	Shader* S = new Shader;
	bool ret = S->Initialise(Scene::GetCurrentScene()->GetRenderSystem()->GetD3d11Device(), VSD, PSD, ShaderElementFlag);

	if (ret == false)
	{
		SAFE_DELETE(S);
		return nullptr;
	}

	mShaderArray[Name] = S;
	return S;
}

Shader* ShaderManager::GetShaderByName(std::string Name)
{
	if (mShaderArray.find(Name) != mShaderArray.end())
	{
		return mShaderArray[Name];
	}
	return nullptr;
}

Shader* ShaderManager::GetShaderByType(BaseShader BS)
{
	std::string Name = StandardShaderName[BS];
	return GetShaderByName(Name);
}

void ShaderManager::DestroyShader(std::string Name)
{
	if (mShaderArray.find(Name) != mShaderArray.end())
	{
		Shader* S = mShaderArray[Name];
		SAFE_DELETE(S);
		mShaderArray.erase(Name);
	}
	return;
}