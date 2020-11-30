#pragma once

//#include <DirectXMath.h>
#include "utils.h"
#include "environment_cubemap.h"
#include "load_file.h"
#include "framework.h"


environment_cubemap::environment_cubemap(ID3D11Device* device, UINT dimension ,DXGI_FORMAT format, bool generate_mips) :
	//generated_mips(generate_mips), miplevels(generate_mips ? static_cast<size_t>(std::floor(std::log2(std::max(dimension, dimension)))) + 1 : 1)
	generated_mips(generate_mips), miplevels(generate_mips ? static_cast<size_t>(std::floor(std::log2(dimension))) + 1 : 1)
{
	//RTVとSRVを作成
	HRESULT hr = S_OK;
	{
		D3D11_TEXTURE2D_DESC texture2d_desc;
		texture2d_desc.Width = dimension;
		texture2d_desc.Height = dimension;
		texture2d_desc.MipLevels = generate_mips ? 0 : 1;
		texture2d_desc.ArraySize = number_of_faces;
		texture2d_desc.Format = format;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = generate_mips ? D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS : D3D11_RESOURCE_MISC_TEXTURECUBE;

		//Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = device->CreateTexture2D(&texture2d_desc, 0, texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc = {};
		render_target_view_desc.Format = texture2d_desc.Format;
		render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		render_target_view_desc.Texture2DArray.MipSlice = 0;

		render_target_view_desc.Texture2DArray.ArraySize = 1;
		for (UINT index_of_face = 0; index_of_face < number_of_faces; ++index_of_face)
		{
			render_target_view_desc.Texture2DArray.FirstArraySlice = index_of_face;
			hr = device->CreateRenderTargetView(texture2d.Get(), &render_target_view_desc, &render_target_views[index_of_face]);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		//後でロードするなら作らなくていい?//2020/11/24
		//D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
		//shader_resource_view_desc.Format = texture2d_desc.Format;
		//shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		//shader_resource_view_desc.TextureCube.MipLevels = generate_mips ? -1 : 1;
		//shader_resource_view_desc.TextureCube.MostDetailedMip = 0;
		//hr = device->CreateShaderResourceView(texture2d.Get(), &shader_resource_view_desc, shader_resource_view.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		D3D11_TEXTURE2D_DESC texture2d_desc = {};
		texture2d_desc.Width = dimension;
		texture2d_desc.Height = dimension;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = number_of_faces;
		texture2d_desc.Format = DXGI_FORMAT_R32_TYPELESS;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		//Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = device->CreateTexture2D(&texture2d_desc, 0, texture2d.ReleaseAndGetAddressOf());//ここを&もしくはReleaseAndGetAddressOfに変更することでリーク消せる
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
		depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Flags = 0;
		depth_stencil_view_desc.Texture2DArray.MipSlice = 0;
		depth_stencil_view_desc.Texture2DArray.ArraySize = 1;
		for (UINT index_of_face = 0; index_of_face < number_of_faces; ++index_of_face)
		{
			depth_stencil_view_desc.Texture2DArray.FirstArraySlice = index_of_face;
			hr = device->CreateDepthStencilView(texture2d.Get(), &depth_stencil_view_desc, &depth_stencil_views[index_of_face]);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}

	viewport.Width = static_cast<float>(dimension);
	viewport.Height = static_cast<float>(dimension);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	for (int index_of_face = 0; index_of_face < number_of_faces; ++index_of_face)
	{
		cameras[index_of_face] = std::make_unique<main_camera>();//main_cameraじゃだめかも
	}
}

//RTV,DSVクリア
void environment_cubemap::clear(ID3D11DeviceContext* immediate_context, size_t face)
{
	float color[4] = { 0, 0, 0, 1 };
	immediate_context->ClearRenderTargetView(render_target_views[face], color);
	immediate_context->ClearDepthStencilView(depth_stencil_views[face], D3D11_CLEAR_DEPTH, 1, 0);
}

void environment_cubemap::activate(ID3D11DeviceContext* immediate_context, size_t face)
{
	number_of_cached_viewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	immediate_context->RSGetViewports(&number_of_cached_viewports, cached_viewports);
	immediate_context->RSSetViewports(1, &viewport);

	//現在セットされているRTVとDSVを格納
	immediate_context->OMGetRenderTargets(1, cached_render_target_view.GetAddressOf(), cached_depth_stencil_view.GetAddressOf());
	//cubemapのRTVとDSVをセット
	immediate_context->OMSetRenderTargets(1, &render_target_views[face], depth_stencil_views[face]);
}

//textureをddsとして保存
void environment_cubemap::save_to_dds(ID3D11DeviceContext* immediate_context, const wchar_t* filename)
{
	DirectX::ScratchImage image;
	ID3D11Resource* resource;
	shader_resource_view.Get()->GetResource(&resource);//SRVをresourceに書き込む
	ID3D11Device* device;
	immediate_context->GetDevice(&device);

	HRESULT hr = S_OK;
	hr = CaptureTexture(device, immediate_context, resource, image);//textureとしてimageに保存
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = SaveToDDSFile(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::DDS_FLAGS_NONE, filename);//DDSとして書き出す
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	device->Release();
	resource->Release();
	image.Release();
}

void environment_cubemap::load_dds(ID3D11DeviceContext* immediate_context, const wchar_t* filename)
{
	HRESULT hr = S_OK;
	DirectX::ScratchImage image;
	//DDSファイルの読み込み
	hr = LoadFromDDSFile(filename, DirectX::DDS_FLAGS_NONE, &metadata, image);//metadataのarraysizeが6じゃないと環境マップになってないぽい
	//metadataが(任意画像サイズ,任意画像サイズ,深度値(1),配列の大きさ(6),ミップレベル(1))これなら映る
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateShaderResourceViewEx(FRAMEWORK.getDevice(), image.GetImages(), image.GetImageCount(), metadata,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, DirectX::TEX_MISC_TEXTURECUBE, false, getShaderResourceViewAddress());//SRV作成
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

//cubeマップ用のテクスチャを作る//deactiveもしないとだめ
//	*******************こいつの中でscene描画して完結できるように改造する！！！！****************************************

//void environment_cubemap::capture(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT4 eye)
//{
//	calculate_matrix(eye);//カメラの撮影用行列を用意
//	for (int index_of_face = 0; index_of_face < number_of_faces; ++index_of_face)
//	{
//		cameras[index_of_face]->Active();//
//		clear(immediate_context, index_of_face);
//		activate(immediate_context, index_of_face);
//	}
//}

void environment_cubemap::captureBegine(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT4 eye , int loop_num)
{
	//TODO:先にdds読み込んでるとうまく書きかえれない

	calculate_matrix(eye);//カメラの撮影用行列を用意
	cameras[loop_num]->active();
	//CBセット
	//FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(4, 1, cameras[loop_num]->getConstantBufferAdress());
	clear(immediate_context, loop_num);
	activate(immediate_context, loop_num);
}

void environment_cubemap::captureEnd(ID3D11DeviceContext* immediate_context)
{
	//保存してたやつにセットしなおす
	immediate_context->RSSetViewports(number_of_cached_viewports, cached_viewports);
	immediate_context->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(), cached_depth_stencil_view.Get());
}

void environment_cubemap::calculate_matrix(DirectX::XMFLOAT4 eye/*camera location in world space*/, bool perspective_projection, float fovy_or_view_width, float near_z, float far_z)
{
	//カメラの向きバッファ
	DirectX::XMFLOAT4 focuses[number_of_faces] = {
		{ eye.x + 1, eye.y, eye.z, 1 }, // +X
		{ eye.x - 1, eye.y, eye.z, 1 }, // -X
		{ eye.x, eye.y + 1, eye.z, 1 }, // +Y
		{ eye.x, eye.y - 1, eye.z, 1 }, // -Y
		{ eye.x, eye.y, eye.z + 1, 1 }, // +Z
		{ eye.x, eye.y, eye.z - 1, 1 }, // -Z
	};
	DirectX::XMFLOAT4 uppers[number_of_faces] = {
		{ 0, 1, 0, 0 }, // +X
		{ 0, 1, 0, 0 }, // -X
		{ 0, 0, -1, 0 }, // +Y
		{ 0, 0, +1, 0 }, // -Y
		{ 0, 1, 0, 0 }, // +Z
		{ 0, 1, 0, 0 }, // -Z
	};
	for (int index_of_face = 0; index_of_face < number_of_faces; ++index_of_face)
	{
		//cameras[index_of_face]->perspective_projection = perspective_projection;//perspectiveにするフラグ
		cameras[index_of_face]->setEye(DirectX::XMFLOAT3(eye.x, eye.y, eye.z));
		cameras[index_of_face]->setFocus(DirectX::XMFLOAT3(focuses[index_of_face].x, focuses[index_of_face].y, focuses[index_of_face].z));
		cameras[index_of_face]->setUp(DirectX::XMFLOAT3(uppers[index_of_face].x, uppers[index_of_face].y, uppers[index_of_face].z));
		cameras[index_of_face]->setFov(fovy_or_view_width);
		cameras[index_of_face]->setAspect(1.0f);
		cameras[index_of_face]->setNearZ(near_z);
		cameras[index_of_face]->setFarZ(far_z);
	}
}

void environment_cubemap::update()
{
}

void environment_cubemap::render()
{
}

void environment_cubemap::release()
{
	for (UINT i = 0; i < 6u; i++)
	{
		if (render_target_views[i])
		{
			render_target_views[i]->Release();
			render_target_views[i] = nullptr;
		}
		if (depth_stencil_views[i])
		{
			depth_stencil_views[i]->Release();
			depth_stencil_views[i] = nullptr;
		}
	}

	//if(shader_resource_view)
	//	shader_resource_view->Release();

	//if (cached_render_target_view)
	//	cached_render_target_view->Release();
	//if (cached_depth_stencil_view)
	//	cached_depth_stencil_view->Release();
}