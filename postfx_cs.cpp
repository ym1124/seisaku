#include "postfx_cs.h"
#include "shader.h"
#include "framework.h"
#include <array>

postfx_cs::postfx_cs(const wchar_t *shader_name,u_int size)
{
	wchar_t src[255];
	wcscpy_s(src, shader_name);

	compute_shader = std::make_unique<shader>();
	compute_shader->CreateCS(src, "CSMain");
	compute_shader->CreateConstantBufferOriginal(size);

	{
		//シェーダの出力先の作成
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = 1280;
		desc.Height = 720;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		auto hr = FRAMEWORK.getDevice()->CreateTexture2D(&desc, nullptr, Texture2D.GetAddressOf());
		if (FAILED(hr)) {
			throw std::runtime_error("コンピュータシェーダの出力先用のID3D11Texture2Dの作成に失敗");
		}

		//★toshiteru
		//hr = FRAMEWORK.getDevice()->CreateUnorderedAccessView(Texture2D.Get(), nullptr, mpScreenUAV.GetAddressOf());
		//if (FAILED(hr)) {
		//	throw std::runtime_error("コンピュータシェーダの出力先用のID3D11Texture2DのUnorderedAccessViewの作成に失敗");
		//}

		//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		//srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		//srvDesc.Texture2D.MostDetailedMip = 0;
		//srvDesc.Texture2D.MipLevels = 1;
		//hr = FRAMEWORK.getDevice()->CreateShaderResourceView(Texture2D.Get(), &srvDesc, cssrv.GetAddressOf());
		//if (FAILED(hr)) {
		//	throw std::runtime_error("コンピュータシェーダの出力先用のID3D11Texture2DのUnorderedAccessViewの作成に失敗");
		//}


		//★monsho
		D3D11_SHADER_RESOURCE_VIEW_DESC	srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		hr = FRAMEWORK.getDevice()->CreateShaderResourceView(Texture2D.Get(), &srvDesc, cssrv.GetAddressOf());
		if (FAILED(hr))
		{
			return;
		}

		// UAVを作成する
		D3D11_UNORDERED_ACCESS_VIEW_DESC	uavDesc;
		ZeroMemory(&uavDesc, sizeof(uavDesc));
		uavDesc.Format = desc.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		hr = FRAMEWORK.getDevice()->CreateUnorderedAccessView(Texture2D.Get(), &uavDesc, mpScreenUAV.GetAddressOf());
		if (FAILED(hr))
		{
			return;
		}
	}
}

void postfx_cs::render()
{
	//実行するシェーダをGPUに設定する
	FRAMEWORK.getImmediateContext()->CSSetShader(compute_shader.get()->cs.Get(), nullptr, 0);

	//シェーダのscreenとして扱うリソースを設定する。
	std::array<ID3D11UnorderedAccessView*, 1> ppUAVs = { { this->mpScreenUAV.Get(), } };
	std::array<UINT, 1> initCounts = { { 0u, } };
	FRAMEWORK.getImmediateContext()->CSSetUnorderedAccessViews(0, static_cast<UINT>(ppUAVs.size()), ppUAVs.data(), initCounts.data());

	//CSの実行
	//AMD製ならグループスレッドは64の倍数
	//NVIDIA製なら32の倍数が最速
	FRAMEWORK.getImmediateContext()->Dispatch(1280 / 32, 720 / 16, 1);

	//解放
	ID3D11ShaderResourceView* dummySRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	FRAMEWORK.getImmediateContext()->CSSetShaderResources(0, _countof(dummySRVs), dummySRVs);
	ID3D11UnorderedAccessView*	pUAV = NULL;
	FRAMEWORK.getImmediateContext()->CSSetUnorderedAccessViews(0, 1, &pUAV, (UINT*)&pUAV);
}

template <typename T>
void postfx_cs::updateConstantBuffer(T cb)
{
	FRAMEWORK.getImmediateContext()->UpdateSubresource(compute_shader->ConstantBufferOriginal.Get(), 0, NULL, &cb, 0, 0);
	FRAMEWORK.getImmediateContext()->CSSetConstantBuffers(2, 1, compute_shader->ConstantBufferOriginal.GetAddressOf());
}
template void postfx_cs::updateConstantBuffer<ConstantBufferForSSAO>(ConstantBufferForSSAO cb);
template void postfx_cs::updateConstantBuffer<ConstantBufferForSSR>(ConstantBufferForSSR cb);

void postfx_cs::setSRVs(ID3D11ShaderResourceView **srvs,int num_views)
{
	FRAMEWORK.getImmediateContext()->CSSetShaderResources(0, num_views, srvs);
}
