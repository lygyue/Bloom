/*!
 * \file DataBuffer.h
 *
 * \author LYG
 * \date ËÄÔÂ 2018
 *
 * 
 */
#pragma once
#include "Common.h"

struct DepthBuffer
{
	ID3D11DepthStencilView * TexDsv;

	DepthBuffer(ID3D11Device * Device, int sizeW, int sizeH, int sampleCount = 1);

	~DepthBuffer();
};

struct DataBuffer
{
	ID3D11Buffer * D3DBuffer;
	size_t         Size;

	// CpuAccessFlag: 0, no access; 1, cpu write; 2, cpu read
	DataBuffer(ID3D11Device * Device, D3D11_BIND_FLAG use, const void* buffer, size_t size, int CpuAccessFlag = 1);

	void UpdateData(ID3D11DeviceContext * Context, const void* buffer, size_t size);

	// must be create with CpuAccessFlag = 2
	bool ReadData(ID3D11DeviceContext * Context, void* buffer, size_t size);

	~DataBuffer();
};


struct UAVBuffer
{
	ID3D11Buffer * D3DBuffer;
	size_t         Size;
	ID3D11UnorderedAccessView* UAV;

	UAVBuffer(ID3D11Device * Device, const void* buffer, size_t stride, size_t counts);
	~UAVBuffer();
};