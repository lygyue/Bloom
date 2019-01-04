/*!
 * \file Material.h
 *
 * \author LYG
 * \date ËÄÔÂ 2018
 *
 * 
 */
#pragma once
#include "Texture.h"
#include "Shader.h"
#include "GameDef.h"

#define		MAX_CONST_BUFFER		128

class MaterialManager;
class Material
{
	friend class MaterialManager;
	friend class Mesh;
	static const int MaxTexture = 16;
public:
	void SetTexture(D3d11Texture* Tex, int TextureIndex = 0);
	D3d11Texture* GetTexture(int TextureIndex) const;
	void SetMainTextureIndex(int TextureIndex);
	int GetMainTextureIndex() const;
	D3d11Texture* GetMainTexture() const;
	int GetTextureCount() const;
	void ResetTexture();
	bool ReBuild();
	void Clear();
	void SetShader(Shader* S);
	Shader* GetShader() const;
	std::string GetName() const;
	char* GetConstBufferPointer();
	void SetConstBufferLen(int Len);
	int GetConstBufferLen() const;
public:
	void SetTextureAddressMode(D3D11_TEXTURE_ADDRESS_MODE DTAM = D3D11_TEXTURE_ADDRESS_WRAP);
	void SetTextureFilter(D3D11_FILTER DF = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
	void SetMultiSampeEnable(bool Enable = true);
	void SetFillMode(D3D11_FILL_MODE DFM = D3D11_FILL_SOLID);
	void SetDepthEnable(bool Enable = true);
	void SetDepthFunc(D3D11_COMPARISON_FUNC DCF = D3D11_COMPARISON_LESS);
	void SetBlendEnable(bool Enable = true);
	void SetBlendOP(D3D11_BLEND_OP DBO = D3D11_BLEND_OP_ADD);
protected:
	Material(std::string Name);
	~Material();

	char								mConstBuffer[MAX_CONST_BUFFER];
	int									mConstBufferLen;
	std::string							mName;
	int									mMainTextureIndex;
	int									mTextureCount;
	D3d11Texture					*	mTex[MaxTexture];
	Shader							*	mShader;
	ID3D11SamplerState				*	mSamplerState;
	ID3D11RasterizerState			*	mRasterizer;
	ID3D11DepthStencilState			*	mDepthState;
	ID3D11BlendState				*	mBlendState;

	D3D11_TEXTURE_ADDRESS_MODE			mTextureAddressMode;
	D3D11_FILTER						mTextureFilter;
	D3D11_COMPARISON_FUNC				mTextureCompareFunc;
	unsigned int						mTextureMaxAnisotropy;
	float								mTextureMaxLOD;
	bool								mMultisampleEnable;
	D3D11_FILL_MODE						mFillMode;
	bool								mDepthEnable;
	D3D11_COMPARISON_FUNC				mDepthFunc;
	bool								mBlendEnable;
	D3D11_BLEND_OP						mBlendOP;
};

class MaterialManager
{
	friend class Scene;
public:
	ShaderManager* GetShaderManager() const;
	// if BS == CutomShader£¬you must set your custom shader by calling SetShader() function.
	Material* CreateMaterial(std::string Name, BaseShader BS);
	Material* CreateMaterial(BaseShader BS);
	Material* GetMaterialByName(std::string Name);
	void DestroyMaterial(std::string Name);
	void DestroyMaterial(Material* Mat);
protected:
	MaterialManager();
	~MaterialManager();

	void Initialise();
private:
	std::map<std::string, Material*> mMaterialArray;
	ShaderManager* mShaderManager;

	GET_AUTO_NAME("Material");
	CLEAR_MAP(mMaterialArray);
};
