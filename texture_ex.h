#pragma once

#include <DirectXTex.h>
#include "utilities.h"

namespace descartes
{
	void load_texture_from_file(ID3D11Device* device, const char* filename, ID3D11ShaderResourceView** shader_resource_view, bool force_srgb = false, bool generate_mips = false, size_t levels = 1/*mipmap levels*/, bool alpha_channel_to_opaque_flag = true, bool enable_caching = true);
	void load_hdr_texture_from_file(ID3D11Device* device, const char* filename, ID3D11ShaderResourceView** shader_resource_view, bool enable_caching);
	std::string make_dummy_texture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view, DWORD value/*0xAABBGGRR*/, UINT dimensions, bool force_srgb = false, bool enable_caching = true);
	void release_all_cached_textures();
	void release_cached_texture(const char* filename);

	inline void texture2d_description(ID3D11ShaderResourceView* shader_resource_view, D3D11_TEXTURE2D_DESC& texture2d_desc)
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		shader_resource_view->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		HRESULT hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		texture2d->GetDesc(&texture2d_desc);
	}

	inline void save_to_dds(ID3D11DeviceContext* immediate_context, const wchar_t* filename, ID3D11ShaderResourceView* shader_resource_view)
	{
		HRESULT hr = S_OK;

		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		shader_resource_view->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Device> device;
		immediate_context->GetDevice(device.ReleaseAndGetAddressOf());

		DirectX::ScratchImage image;
		hr = CaptureTexture(device.Get(), immediate_context, resource.Get(), image);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = SaveToDDSFile(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::DDS_FLAGS_NONE, filename);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

