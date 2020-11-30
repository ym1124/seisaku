//******************************************
//
//　　　　　　　 irmapクラス
//
//**********************************************

#include "ir_map.h"
#include "utils.h"

//コンストラクタ
irmap::irmap()
{
	cp = createComputeProgram(compileShader("irmap.hlsl", "main", "cs_5_0"));
	createSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
	ict = createIrcubeTexture(32, 32, DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
	createTextureUAV(ict, 0);
}

//CS生成
compute_program irmap::createComputeProgram(const Microsoft::WRL::ComPtr<ID3DBlob>& csBytecode)
{
	compute_program program;
	if (FAILED(FRAMEWORK.getDevice()->CreateComputeShader(csBytecode->GetBufferPointer(), csBytecode->GetBufferSize(), nullptr, &program.cs))) {
		throw std::runtime_error("Failed to create compute shader from compiled bytecode");
	}
	return program;
}

//CSのサンプラー生成
void irmap::createSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
{
	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = filter;
	desc.AddressU = addressMode;
	desc.AddressV = addressMode;
	desc.AddressW = addressMode;
	desc.MaxAnisotropy = (filter == D3D11_FILTER_ANISOTROPIC) ? D3D11_REQ_MAXANISOTROPY : 1;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(FRAMEWORK.getDevice()->CreateSamplerState(&desc, &compute_sampler))) {
		throw std::runtime_error("Failed to create sampler state");
	}
}

//事前計算放射照度環境マップ生成
ir_cube_tex irmap::createIrcubeTexture(UINT width, UINT height, DXGI_FORMAT format, UINT levels)
{
	ir_cube_tex texture;
	texture.width = width;
	texture.height = height;
	texture.levels = (levels > 0) ? levels : Utility::numMipmapLevels(width, height);

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = levels;
	desc.ArraySize = 6;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	if (levels == 0) {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	if (FAILED(FRAMEWORK.getDevice()->CreateTexture2D(&desc, nullptr, &texture.texture))) {
		throw std::runtime_error("Failed to create cubemap texture");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;
	if (FAILED(FRAMEWORK.getDevice()->CreateShaderResourceView(texture.texture.Get(), &srvDesc, &texture.srv))) {
		throw std::runtime_error("Failed to create cubemap texture SRV");
	}
	return texture;
}

//UAV生成
HRESULT irmap::createTextureUAV(ir_cube_tex &texture, UINT mipSlice)
{
	assert(texture.texture);

	D3D11_TEXTURE2D_DESC desc;
	texture.texture->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = desc.Format;
	if (desc.ArraySize == 1) {
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = mipSlice;
	}
	else {
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.MipSlice = mipSlice;
		uavDesc.Texture2DArray.FirstArraySlice = 0;
		uavDesc.Texture2DArray.ArraySize = desc.ArraySize;
	}

	if (FAILED(FRAMEWORK.getDevice()->CreateUnorderedAccessView(texture.texture.Get(), &uavDesc, &texture.uav))) {
		throw std::runtime_error("Failed to create texture UAV");
	}
}

Microsoft::WRL::ComPtr<ID3DBlob> irmap::compileShader(const std::string& filename, const std::string& entryPoint, const std::string& profile)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	Microsoft::WRL::ComPtr<ID3DBlob> shader;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	std::printf("Compiling HLSL shader: %s [%s]\n", filename.c_str(), entryPoint.c_str());

	if (FAILED(D3DCompileFromFile(Utility::convertToUTF16(filename).c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), profile.c_str(), flags, 0, &shader, &errorBlob))) {
		std::string errorMsg = "Shader compilation failed: " + filename;
		if (errorBlob) {
			errorMsg += std::string("\n") + static_cast<const char*>(errorBlob->GetBufferPointer());
		}
		throw std::runtime_error(errorMsg);
	}
	return shader;
}

void irmap::runComputeShader(cube_mesh* c_mesh)
{
	ID3D11UnorderedAccessView* const nullUAV[] = { nullptr };

	FRAMEWORK.getImmediateContext()->CSSetShaderResources(0, 1, c_mesh->getShaderResourceViewAddress());
	FRAMEWORK.getImmediateContext()->CSSetSamplers(0, 1, compute_sampler.GetAddressOf());
	FRAMEWORK.getImmediateContext()->CSSetUnorderedAccessViews(0, 1, ict.uav.GetAddressOf(), nullptr);
	FRAMEWORK.getImmediateContext()->CSSetShader(cp.cs.Get(), nullptr, 0);
	FRAMEWORK.getImmediateContext()->Dispatch(ict.width / 32, ict.height / 32, 6);
	FRAMEWORK.getImmediateContext()->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
}