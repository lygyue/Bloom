/*!
 * \file Material.cpp
 *
 * \author LYG
 * \date ËÄÔÂ 2018
 *
 * 
 */

#include "Material.h"
#include "Scene.h"
#include "RenderSystem.h"

Material::Material(std::string Name)
{
	memset(mConstBuffer, 0, sizeof(mConstBuffer));
	memset(mTex, 0, sizeof(mTex));

	mSamplerState				= nullptr;
	mRasterizer					= nullptr;
	mDepthState					= nullptr;
	mBlendState					= nullptr;
	mConstBufferLen				= 0;
	mMainTextureIndex			= 0;
	mTextureCount				= 0;
	mShader						= nullptr;
	mName						= Name;
	mTextureAddressMode			= D3D11_TEXTURE_ADDRESS_WRAP;
	mTextureFilter				= D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	mTextureCompareFunc			= D3D11_COMPARISON_NEVER;
	mTextureMaxAnisotropy		= 0;
	mTextureMaxLOD				= D3D11_FLOAT32_MAX;
	mMultisampleEnable			= true;
	mFillMode					= D3D11_FILL_SOLID;
	mDepthEnable				= true;
	mDepthFunc					= D3D11_COMPARISON_LESS;
	mBlendEnable				= true;
	mBlendOP					= D3D11_BLEND_OP_ADD;
	ReBuild();
}

Material::~Material()
{
	Clear();
}

char* Material::GetConstBufferPointer()
{
	return mConstBuffer;
}

void Material::SetConstBufferLen(int Len)
{
	mConstBufferLen = Len;
}

int Material::GetConstBufferLen() const
{
	return mConstBufferLen;
}

std::string Material::GetName() const
{
	return mName;
}

Shader* Material::GetShader() const
{
	return mShader;
}

void Material::SetShader(Shader* S)
{
	mShader = S;
}

void Material::SetTextureAddressMode(D3D11_TEXTURE_ADDRESS_MODE DTAM /* = D3D11_TEXTURE_ADDRESS_WRAP */)
{
	mTextureAddressMode = DTAM;
}

void Material::SetTextureFilter(D3D11_FILTER DF /* = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR */)
{
	mTextureFilter = DF;
}

void Material::SetMultiSampeEnable(bool Enable /* = true */)
{
	mMultisampleEnable = Enable;
}

void Material::SetFillMode(D3D11_FILL_MODE DFM /* = D3D11_FILL_SOLID */)
{
	mFillMode = DFM;
}

void Material::SetDepthEnable(bool Enable /* = true */)
{
	mDepthEnable = Enable;
}

void Material::SetDepthFunc(D3D11_COMPARISON_FUNC DCF /* = D3D11_COMPARISON_LESS */)
{
	mDepthFunc = DCF;
}

void Material::SetBlendEnable(bool Enable /* = true */)
{
	mBlendEnable = Enable;
}

void Material::SetBlendOP(D3D11_BLEND_OP DBO /* = D3D11_BLEND_OP_ADD */)
{
	mBlendOP = DBO;
}

void Material::Clear()
{
	SAFE_RELEASE(mSamplerState);
	SAFE_RELEASE(mRasterizer);
	SAFE_RELEASE(mDepthState);
	SAFE_RELEASE(mBlendState);
}

bool Material::ReBuild()
{
	Clear();
	HRESULT result = S_OK;
	ID3D11Device* Device = Scene::GetCurrentScene()->GetRenderSystem()->GetD3d11Device();
	// Create sampler state
	D3D11_SAMPLER_DESC ss; memset(&ss, 0, sizeof(ss));
	ss.AddressU = ss.AddressV = ss.AddressW = mTextureAddressMode;
	ss.Filter = mTextureFilter;
	ss.ComparisonFunc = mTextureCompareFunc;
	ss.MaxAnisotropy = mTextureMaxAnisotropy;
	ss.MaxLOD = mTextureMaxLOD;
	result = Device->CreateSamplerState(&ss, &mSamplerState);
	if (FAILED(result))
	{
		return false;
	}

	// Create rasterizer
	D3D11_RASTERIZER_DESC rs; memset(&rs, 0, sizeof(rs));
	if(mMultisampleEnable)
		rs.AntialiasedLineEnable = false;
	else
		rs.AntialiasedLineEnable = true;
	rs.DepthClipEnable = true;
	rs.MultisampleEnable = mMultisampleEnable;				// if this true, rs.AntialiasedLineEnable must be false.
	rs.CullMode = D3D11_CULL_BACK;
	rs.FillMode = mFillMode;
	result = Device->CreateRasterizerState(&rs, &mRasterizer);
	if (FAILED(result))
	{
		return false;
	}
	// Create depth state
	D3D11_DEPTH_STENCIL_DESC dss;
	memset(&dss, 0, sizeof(dss));
	dss.DepthEnable = mDepthEnable;
	dss.DepthFunc = mDepthFunc;
	dss.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	result = Device->CreateDepthStencilState(&dss, &mDepthState);
	if (FAILED(result))
	{
		return false;
	}
	//Create blend state - trans or otherwise
	D3D11_BLEND_DESC bm;
	memset(&bm, 0, sizeof(bm));
	bm.RenderTarget[0].BlendEnable = mBlendEnable;
	bm.RenderTarget[0].BlendOp = bm.RenderTarget[0].BlendOpAlpha = mBlendOP;
	bm.RenderTarget[0].SrcBlend = bm.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	bm.RenderTarget[0].DestBlend = bm.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	bm.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	result = Device->CreateBlendState(&bm, &mBlendState);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void Material::ResetTexture()
{
	for (int i = 0; i < mTextureCount; i++)
	{
		mTex[i] = nullptr;
	}
	mTextureCount = 0;
}

int Material::GetTextureCount() const
{
	return mTextureCount;
}

D3d11Texture* Material::GetTexture(int TextureIndex) const
{
	return mTex[TextureIndex];
}

void Material::SetTexture(D3d11Texture* Tex, int TextureIndex /* = 0 */)
{
	if (TextureIndex >= MaxTexture)
	{
		return;
	}
	if (mTextureCount < TextureIndex + 1)
	{
		mTextureCount = TextureIndex + 1;
	}
	mTex[TextureIndex] = Tex;
}

void Material::SetMainTextureIndex(int TextureIndex)
{
	mMainTextureIndex = TextureIndex;
}

int Material::GetMainTextureIndex() const
{
	return mMainTextureIndex;
}

D3d11Texture* Material::GetMainTexture() const
{
	return mTex[mMainTextureIndex];
}
//-----------------------------------------------------------------------
MaterialManager::MaterialManager()
{
	mShaderManager = new ShaderManager;
}

MaterialManager::~MaterialManager()
{
	std::map<std::string, Material*>::iterator it;
	for (it = mMaterialArray.begin(); it != mMaterialArray.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	mMaterialArray.clear();
}

void MaterialManager::Initialise()
{
	mShaderManager->InitialiseStandardShaders();
}

ShaderManager* MaterialManager::GetShaderManager() const
{
	return mShaderManager;
}

Material* MaterialManager::CreateMaterial(std::string Name, BaseShader BS)
{
	if (mMaterialArray.find(Name) != mMaterialArray.end())
	{
		return mMaterialArray[Name];
	}
	Material* Mat = new Material(Name);
	if (BS != CutomShader)
	{
		Mat->SetShader(mShaderManager->GetShaderByType(BS));
	}
	mMaterialArray[Name] = Mat;
	return Mat;
}

Material* MaterialManager::CreateMaterial(BaseShader BS)
{
	return CreateMaterial(GetAutoName(), BS);
}

Material* MaterialManager::GetMaterialByName(std::string Name)
{
	if (mMaterialArray.find(Name) == mMaterialArray.end())
	{
		return nullptr;
	}
	return mMaterialArray[Name];
}

void MaterialManager::DestroyMaterial(std::string Name)
{
	if (mMaterialArray.find(Name) == mMaterialArray.end())
	{
		return;
	}

	Material* M = mMaterialArray[Name];
	SAFE_DELETE(M);
	mMaterialArray.erase(Name);
	return;
}

void MaterialManager::DestroyMaterial(Material* Mat)
{
	return DestroyMaterial(Mat->GetName());
}