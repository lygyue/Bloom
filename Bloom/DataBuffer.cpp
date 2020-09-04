/*!
 * \file DataBuffer.cpp
 *
 * \author LYG
 * \date ËÄÔÂ 2018
 *
 * 
 */
#include "DataBuffer.h"

DepthBuffer::DepthBuffer(ID3D11Device * Device, int sizeW, int sizeH, int sampleCount /* = 1 */)
{
	DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT;
	D3D11_TEXTURE2D_DESC dsDesc;
	dsDesc.Width = sizeW;
	dsDesc.Height = sizeH;
	dsDesc.MipLevels = 1;
	dsDesc.ArraySize = 1;
	dsDesc.Format = format;
	dsDesc.SampleDesc.Count = sampleCount;
	dsDesc.SampleDesc.Quality = 0;
	dsDesc.Usage = D3D11_USAGE_DEFAULT;
	dsDesc.CPUAccessFlags = 0;
	dsDesc.MiscFlags = 0;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ID3D11Texture2D * Tex;
	Device->CreateTexture2D(&dsDesc, NULL, &Tex);
	Device->CreateDepthStencilView(Tex, NULL, &TexDsv);
	Tex->Release();
}

DepthBuffer::~DepthBuffer()
{
	SAFE_RELEASE(TexDsv);
}

DataBuffer::DataBuffer(ID3D11Device * Device, D3D11_BIND_FLAG use, const void* buffer, size_t size, int CpuAccessFlag/* = 1*/) : Size(size)
{
	D3D11_BUFFER_DESC desc;   memset(&desc, 0, sizeof(desc));
	if (CpuAccessFlag == 0)
	{
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
	}
	else if(CpuAccessFlag == 1)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (CpuAccessFlag == 2)
	{
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	}
	desc.BindFlags = use;
	desc.ByteWidth = (unsigned)size;
	D3D11_SUBRESOURCE_DATA sr;
	sr.pSysMem = buffer;
	sr.SysMemPitch = sr.SysMemSlicePitch = 0;
	HRESULT hr = Device->CreateBuffer(&desc, buffer ? &sr : NULL, &D3DBuffer);
	if (FAILED(hr))
	{
		OutputDebugStringA("DataBuffer::CreateBuffer Failed");
	}
}

void DataBuffer::UpdateData(ID3D11DeviceContext * Context, const void* buffer, size_t size)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE map;
	memset(&map, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
	hr = Context->Map(D3DBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	if (SUCCEEDED(hr))
	{
		memcpy(map.pData, buffer, size);
		Context->Unmap(D3DBuffer, 0);
	}
}

bool DataBuffer::ReadData(ID3D11DeviceContext * Context, void* buffer, size_t size)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE map;
	memset(&map, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
	hr = Context->Map(D3DBuffer, 0, D3D11_MAP_READ, 0, &map);
	if (SUCCEEDED(hr))
	{
		memcpy(buffer, map.pData, size);
		Context->Unmap(D3DBuffer, 0);
		return true;
	}

	return false;
}

DataBuffer::~DataBuffer()
{
	SAFE_RELEASE(D3DBuffer);
}


UAVBuffer::UAVBuffer(ID3D11Device * Device, const void* buffer, size_t stride, size_t counts)
{
	D3D11_BUFFER_DESC buffer_desc;
	memset(&buffer_desc, 0, sizeof(buffer_desc));

	buffer_desc.ByteWidth = UINT(stride * counts);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	buffer_desc.StructureByteStride = (UINT)stride;
	buffer_desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sr;
	sr.pSysMem = buffer? buffer:buffer;
	sr.SysMemPitch = sr.SysMemSlicePitch = 0;

	HRESULT hr;
	hr = Device->CreateBuffer(&buffer_desc, &sr, &D3DBuffer);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavbuffer_desc = {};

	uavbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
	uavbuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavbuffer_desc.Buffer.NumElements = (UINT)counts;

	hr = Device->CreateUnorderedAccessView(D3DBuffer, &uavbuffer_desc, &UAV);

}

UAVBuffer::~UAVBuffer()
{
	SAFE_RELEASE(D3DBuffer);
	SAFE_RELEASE(UAV);
}