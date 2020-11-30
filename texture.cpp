#include "framework.h"
#include "WICTextureLoader.h"
#include "Texture.h"

Texture::Texture(bool selfCreateFlg)
{
	ShaderResourceView = nullptr;
	if (!selfCreateFlg)
		Create();
}


Texture::~Texture()
{
	//release();//frameworkが先に消えてるっぽい?
}

//createより後に呼ぶとSRVがRTVを描画するのではなく、画像を上書きしてしまう
//リークする
bool Texture::Load(const wchar_t * filename)
{
	HRESULT hr = S_OK;

	//テクスチャ作成//ここでリークしてる//ShaderResourceView.ReleaseAndGetAddressOf()に変更でリーク消滅2020/11/23
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	hr = DirectX::CreateWICTextureFromFile(FRAMEWORK.getDevice(), filename, resource.GetAddressOf(), ShaderResourceView.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		//return false;
	}

	return true;
}

void Texture::Set(UINT Slot ,BOOL flg)
{
	//nullセット
	if (flg == false) {
		ID3D11ShaderResourceView* srv[1] = { nullptr };
		ID3D11SamplerState*ss[1] = { nullptr };
		FRAMEWORK.getImmediateContext()->PSSetShaderResources(Slot, 1, srv);
		FRAMEWORK.getImmediateContext()->PSSetSamplers(Slot, 1, ss);
		FRAMEWORK.getImmediateContext()->DSSetShaderResources(Slot, 1, srv);
		FRAMEWORK.getImmediateContext()->DSSetSamplers(Slot, 1, ss);
		//srv[0]->Release();
		//ss[0]->Release();
		return;
	}
	//画像セット
	if (ShaderResourceView.Get()) {
		FRAMEWORK.getImmediateContext()->PSSetShaderResources(Slot, 1, ShaderResourceView.GetAddressOf());//load関数なければrtvの内容が反映される
		FRAMEWORK.getImmediateContext()->PSSetSamplers(Slot, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::LINEAR_BORDER));
		FRAMEWORK.getImmediateContext()->DSSetShaderResources(Slot, 1, ShaderResourceView.GetAddressOf());
		FRAMEWORK.getImmediateContext()->DSSetSamplers(Slot, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::LINEAR_BORDER));
	}
}

//レンダーターゲットに描画したものをシェーダーリソースとして使用できるようにする
bool Texture::Create(u_int width, u_int height, DXGI_FORMAT format)
{
	//既にSRVある=既に生成されてる
	if (ShaderResourceView.Get())
		return false;

	/*Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;*/
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;//TYPELESS:型指定なし?//UINT:符号なし整数←負を表せないし整数型になる?//SINT:符号あり整数型//FLOAT:符号あり?浮動小数点型。基本これ?
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;//RTVでありSRVとしても使えるように設定しておく！

	hr = FRAMEWORK.getDevice()->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//	レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = FRAMEWORK.getDevice()->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}


	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = FRAMEWORK.getDevice()->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}



void Texture::release()
{
	/*if (ShaderResourceView)
	{
		ShaderResourceView->Release();
		ShaderResourceView = NULL;
	}
	if (sampler)
	{
		sampler->Release();
		sampler = NULL;
	}
	if (RenderTargetView)
	{
		RenderTargetView->Release();
		RenderTargetView = NULL;
	}
	if (DepthStencilView)
	{
		DepthStencilView->Release();
		DepthStencilView = NULL;
	}*/
}


///////////////////////////////////////////////////////////////////////////////////////////////////

Texture_d::Texture_d(bool self_create)
{
	//ない場合自動で作成するように変更//9/20
	if (!ShaderResourceView&&!self_create)
		Create();
}

void Texture_d::Create(u_int width, u_int height,int tex_format,int dsv_format,int srv_format)
{
	//既にSRVある=既に生成されてる//複数回作るとリークする
	if (ShaderResourceView)
		return;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = (DXGI_FORMAT)tex_format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;//DSVでありSRVとしても使えるように設定しておく！

	hr = FRAMEWORK.getDevice()->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	if (FAILED(hr))
	{
		assert("to create texture is failed.");
	}

	//深度ステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = (DXGI_FORMAT)dsv_format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;//DSV用
	dsvd.Texture2D.MipSlice = 0;
	hr = FRAMEWORK.getDevice()->CreateDepthStencilView(Texture2D.Get(), &dsvd, &DepthStencilView);
	if (FAILED(hr))
	{
		assert("to create DSV is failed.");
	}

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = (DXGI_FORMAT)srv_format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = FRAMEWORK.getDevice()->CreateShaderResourceView(Texture2D.Get(), &srvd, &ShaderResourceView);
	if (FAILED(hr))
	{
		assert("to create SRV is failed.");
	}
}

void Texture_d::Create2(u_int width, u_int height)
{
	ID3D11Texture2D* Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R32_TYPELESS;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;//DSVでありSRVとしても使えるように設定しておく！

	hr = FRAMEWORK.getDevice()->CreateTexture2D(&texture2d_desc, NULL, &Texture2D);
	if (FAILED(hr))
	{
		assert("to create texture is failed.");
	}

	//深度ステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;//DSV用
	dsvd.Texture2D.MipSlice = 0;
	hr = FRAMEWORK.getDevice()->CreateDepthStencilView(Texture2D, &dsvd, &DepthStencilView);
	if (FAILED(hr))
	{
		assert("to create DSV is failed.");
	}

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R32_FLOAT;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = FRAMEWORK.getDevice()->CreateShaderResourceView(Texture2D, &srvd, &ShaderResourceView);
	if (FAILED(hr))
	{
		assert("to create SRV is failed.");
	}
}

void Texture_d::Set(UINT Slot, BOOL flg)
{
	if (flg == false) {
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv[1] = { nullptr };
		Microsoft::WRL::ComPtr<ID3D11SamplerState> ss[1] = { nullptr };
		FRAMEWORK.getImmediateContext()->PSSetShaderResources(Slot, 1, srv->GetAddressOf());
		FRAMEWORK.getImmediateContext()->PSSetSamplers(Slot, 1, ss->GetAddressOf());
		FRAMEWORK.getImmediateContext()->DSSetShaderResources(Slot, 1, srv->GetAddressOf());
		FRAMEWORK.getImmediateContext()->DSSetSamplers(Slot, 1, ss->GetAddressOf());
		return;
	}
	if (ShaderResourceView) {
		FRAMEWORK.getImmediateContext()->PSSetShaderResources(Slot, 1, &ShaderResourceView);//load関数なければrtvの内容が反映される
		FRAMEWORK.getImmediateContext()->PSSetSamplers(Slot, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::LINEAR_BORDER));
		FRAMEWORK.getImmediateContext()->DSSetShaderResources(Slot, 1, &ShaderResourceView);
		FRAMEWORK.getImmediateContext()->DSSetSamplers(Slot, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::LINEAR_BORDER));
	}
}