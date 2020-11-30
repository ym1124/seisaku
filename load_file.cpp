#pragma once

//#include <windows.h>
//#include <tchar.h>
//#include <sstream>
//#include <d3d11.h>
//#include <wrl.h>
//#include <DirectXMath.h>
//#include<memory>
//#include <iostream>
//#include <string>
//#include <stdio.h>
//#include <map>

#include "WICTextureLoader.h"
#include "sprite.h"
#include "misc.h"
#include "load_file.h"

void vs_Load(ID3D11Device *pDevice, const char *cso_name, ID3D11VertexShader **vertex_shader, ID3D11InputLayout **input_layout, D3D11_INPUT_ELEMENT_DESC *input_element_desc, UINT num_elements)
{
	//同時に移らない原因
	//static char checkFile[255] = { 0 };
	char checkFile[255] = { 0 };
	if (!strcmp(checkFile, cso_name))//読み込みが終わっていれば
		return;
	

	FILE* vsfp = NULL;
	fopen_s(&vsfp, cso_name, "rb");
	fseek(vsfp, 0, SEEK_END);
	long cso_sz = ftell(vsfp);
	fseek(vsfp, 0, SEEK_SET);
	unsigned char *cso_data = new unsigned char[cso_sz];
	fread(cso_data, cso_sz, 1, vsfp);
	fclose(vsfp);
	if (vsfp == NULL)
		assert(!"ロード失敗");

	strcpy_s(checkFile, cso_name);

	//インプットレイアウト//VSに送る頂点構造体の構成情報を規定するのに必要
	HRESULT hr = pDevice->CreateInputLayout(input_element_desc, num_elements, cso_data, cso_sz, input_layout);
	_ASSERT_EXPR(SUCCEEDED(hr), "Failed to CreateInputLayout");

	//頂点シェーダー
	hr = pDevice->CreateVertexShader(cso_data, cso_sz, NULL, vertex_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), "Failed to CreateVertexShader");

	delete[] cso_data;
}

void ps_Load(ID3D11Device *pDevice, const char *cso_name, ID3D11PixelShader **pixel_shader)
{
	//写らない原因
	//static char checkFile[255] = { 0 };
	char checkFile[255] = { 0 };
	if (!strcmp(checkFile, cso_name))
		return;

	FILE* psfp = NULL;
	fopen_s(&psfp, cso_name, "rb");
	fseek(psfp, 0, SEEK_END);
	long cso_ps = ftell(psfp);
	fseek(psfp, 0, SEEK_SET);
	unsigned char *cso_data2 = new unsigned char[cso_ps];
	fread(cso_data2, cso_ps, 1, psfp);
	fclose(psfp);
	if (psfp == NULL)
		assert(!"ロード失敗");

	strcpy_s(checkFile, cso_name);

	//ピクセルシェーダー
	HRESULT hr = pDevice->CreatePixelShader(cso_data2, cso_ps, NULL, pixel_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), "Failed to CreatePixelShader");

	delete[] cso_data2;
}

void hs_Load(ID3D11Device *pDevice, const char *cso_name, ID3D11HullShader **hull_shader)
{
	char checkFile[255] = { 0 };
	if (!strcmp(checkFile, cso_name))
		return;

	FILE* psfp = NULL;
	fopen_s(&psfp, cso_name, "rb");
	fseek(psfp, 0, SEEK_END);
	long cso_ps = ftell(psfp);
	fseek(psfp, 0, SEEK_SET);
	unsigned char *cso_data2 = new unsigned char[cso_ps];
	fread(cso_data2, cso_ps, 1, psfp);
	fclose(psfp);
	if (psfp == NULL)
		assert(!"ロード失敗");

	strcpy_s(checkFile, cso_name);

	//ハルシェーダー
	HRESULT hr = pDevice->CreateHullShader(cso_data2, cso_ps, NULL, hull_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), "Failed to CreateHullShader");

	delete[] cso_data2;
}

void ds_Load(ID3D11Device *pDevice, const char *cso_name, ID3D11DomainShader **domain_shader)
{
	char checkFile[255] = { 0 };
	if (!strcmp(checkFile, cso_name))
		return;

	FILE* psfp = NULL;
	fopen_s(&psfp, cso_name, "rb");
	fseek(psfp, 0, SEEK_END);
	long cso_ps = ftell(psfp);
	fseek(psfp, 0, SEEK_SET);
	unsigned char *cso_data2 = new unsigned char[cso_ps];
	fread(cso_data2, cso_ps, 1, psfp);
	fclose(psfp);
	if (psfp == NULL)
		assert(!"ロード失敗");

	strcpy_s(checkFile, cso_name);

	//ドメインシェーダー
	HRESULT hr = pDevice->CreateDomainShader(cso_data2, cso_ps, NULL, domain_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), "Failed to DomainHullShader");

	delete[] cso_data2;
}

HRESULT load_texture_from_file(ID3D11Device *device, const wchar_t *file_name, ID3D11ShaderResourceView **shader_resource_view, D3D11_TEXTURE2D_DESC *texture2d_desc)
{
	HRESULT hr = S_OK;
	ID3D11Resource* resource;
	//ファイルからテクスチャを作成
	hr = DirectX::CreateWICTextureFromFile(device, file_name, &resource, shader_resource_view);//resourceとshader_resource_viewにテクスチャの情報を格納(?)
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ID3D11Texture2D* texture2d;
	//オブジェクトが特定のインターフェースをサポートしているかを識別
	hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	texture2d->GetDesc(texture2d_desc);

	resource->Release();//
	resource = NULL;
	texture2d->Release();//
	texture2d = NULL;
	return hr;
}

HRESULT load_dummy_texture(ID3D11Device *device, const wchar_t *file_name, ID3D11ShaderResourceView **shader_resource_view, D3D11_TEXTURE2D_DESC *texture2d_desc)
{
	std::vector<UINT> pixels;
	pixels.clear();
	pixels.resize(8 * 8);
	::memset(pixels.data(), 0xFF, pixels.size()*sizeof(UINT));

	texture2d_desc->Width = 8;
	texture2d_desc->Height = 8;
	texture2d_desc->MipLevels = 1;
	texture2d_desc->ArraySize = 1;
	texture2d_desc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2d_desc->SampleDesc.Count = 1;
	texture2d_desc->SampleDesc.Quality = 0;
	texture2d_desc->Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc->BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc->CPUAccessFlags = 0;
	texture2d_desc->MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subresource_data;
	::memset(&subresource_data, 0, sizeof(subresource_data));
	subresource_data.pSysMem = pixels.data();
	subresource_data.SysMemPitch = 8;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	HRESULT hr = device->CreateTexture2D(texture2d_desc, &subresource_data, texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
	//shader_resource_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//shader_resource_view_desc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(texture2d.Get(), /*&shader_resource_view_desc*/nullptr, shader_resource_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	return hr;
}

HRESULT make_dummy_texture(ID3D11Device *device, ID3D11ShaderResourceView **shader_resource_view)
{
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC texture2d_desc = {};
	texture2d_desc.Width = 1;
	texture2d_desc.Height = 1;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subresource_data = {};
	u_int color = 0xFFFFFFFF;
	subresource_data.pSysMem = &color;
	subresource_data.SysMemPitch = 4;
	subresource_data.SysMemSlicePitch = 4;

	ID3D11Texture2D *texture2d;
	hr = device->CreateTexture2D(&texture2d_desc, &subresource_data, &texture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
	shader_resource_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(texture2d, &shader_resource_view_desc, shader_resource_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	texture2d->Release();

	return hr;
}

void combine_resource_path(wchar_t(&combined_resource_path)[256], const wchar_t *referrer_filename, const wchar_t *referent_filename)
{
	const wchar_t delimiters[] = { L'\\', L'/' };
	// extract directory from obj_filename
	wchar_t directory[256] = {};
	for (wchar_t delimiter : delimiters)
	{
		wchar_t *p = wcsrchr(const_cast<wchar_t *>(referrer_filename), delimiter);
		if (p)
		{
			memcpy_s(directory, 256, referrer_filename, (p - referrer_filename + 1)*sizeof(wchar_t));
			break;
		}
	}
	// extract filename from resource_filename
	wchar_t filename[256];
	wcscpy_s(filename, referent_filename);
	for (wchar_t delimiter : delimiters)
	{
		wchar_t *p = wcsrchr(filename, delimiter);
		if (p)
		{
			wcscpy_s(filename, p + 1);
			break;
		}
	}
	// combine directory and filename
	wcscpy_s(combined_resource_path, directory);
	wcscat_s(combined_resource_path, filename);
};



