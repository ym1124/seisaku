
#include "frame_buffer.h"
#include "framework.h"
//#include "view.h"
#include "blend.h"
#include "sprite.h"
#include "texture.h"

//Resource being set to OM RenderTarget slot 0 is still bound on input
//同じリソースに同時にread/writeしようとすると出る//https://ja.stackoverflow.com/questions/4737/d3d11%E3%81%AE%E3%83%87%E3%83%90%E3%83%83%E3%82%B0%E3%83%AC%E3%82%A4%E3%83%A4%E3%83%BC%E3%81%A7%E5%87%BA%E3%82%8B%E8%AD%A6%E5%91%8A%E3%81%8C%E5%8F%96%E3%82%8C%E3%81%AA%E3%81%84

//Texture2Dふたつ作る
//RTVを前の状態で記憶しておくクラス
frame_buffer::frame_buffer()
{
	//if (blend == nullptr)
	//	blend =new blender(FRAMEWORK.getDevice());

	D3D11_TEXTURE2D_DESC RTV_SRV_Tex2d_Desc;
	D3D11_TEXTURE2D_DESC DSV_SRV_Tex2d_Desc;
	//RTVとSRV兼任用テクスチャ
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
		HRESULT hr = S_OK;
		//	テクスチャ作成
		ZeroMemory(&RTV_SRV_Tex2d_Desc, sizeof(RTV_SRV_Tex2d_Desc));
		RTV_SRV_Tex2d_Desc.Width = FRAMEWORK.SCREEN_WIDTH;
		RTV_SRV_Tex2d_Desc.Height = FRAMEWORK.SCREEN_HEIGHT;
		RTV_SRV_Tex2d_Desc.MipLevels = 1;
		RTV_SRV_Tex2d_Desc.ArraySize = 1;
		RTV_SRV_Tex2d_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		RTV_SRV_Tex2d_Desc.SampleDesc.Count = 1;
		RTV_SRV_Tex2d_Desc.Usage = D3D11_USAGE_DEFAULT;
		RTV_SRV_Tex2d_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		RTV_SRV_Tex2d_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;//RTVでありSRVとしても使えるように設定しておく！

		hr = FRAMEWORK.getDevice()->CreateTexture2D(&RTV_SRV_Tex2d_Desc, NULL, Texture2D.GetAddressOf());
		if (FAILED(hr))
		{
			assert("to create texture is failed.");
		}

		//	レンダーターゲットビュー作成
		D3D11_RENDER_TARGET_VIEW_DESC rtvd;
		ZeroMemory(&rtvd, sizeof(rtvd));
		rtvd.Format = RTV_SRV_Tex2d_Desc.Format;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvd.Texture2D.MipSlice = 0;
		hr = FRAMEWORK.getDevice()->CreateRenderTargetView(Texture2D.Get(), &rtvd, cached_render_target_view.GetAddressOf());
		if (FAILED(hr))
		{
			assert("to create RTV is failed.");
		}


		//	シェーダーリソースビュー作成
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = 1;
		hr = FRAMEWORK.getDevice()->CreateShaderResourceView(Texture2D.Get(), &srvd, render_target_shader_resource_view.GetAddressOf());
		if (FAILED(hr))
		{
			assert("to create SRV is failed.");
		}
	}

	//DSVとSRV兼任用テクスチャ
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
		HRESULT hr = S_OK;
		//	テクスチャ作成
		ZeroMemory(&DSV_SRV_Tex2d_Desc, sizeof(DSV_SRV_Tex2d_Desc));
		DSV_SRV_Tex2d_Desc.Width = FRAMEWORK.SCREEN_WIDTH;
		DSV_SRV_Tex2d_Desc.Height = FRAMEWORK.SCREEN_HEIGHT;
		DSV_SRV_Tex2d_Desc.MipLevels = 1;
		DSV_SRV_Tex2d_Desc.ArraySize = 1;
		DSV_SRV_Tex2d_Desc.Format = DXGI_FORMAT_R24G8_TYPELESS;//深度ステンシル用
		DSV_SRV_Tex2d_Desc.SampleDesc.Count = 1;
		DSV_SRV_Tex2d_Desc.SampleDesc.Quality = 0;
		DSV_SRV_Tex2d_Desc.Usage = D3D11_USAGE_DEFAULT;
		DSV_SRV_Tex2d_Desc.CPUAccessFlags = 0;
		DSV_SRV_Tex2d_Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;//SRV入れると作成失敗

		hr = FRAMEWORK.getDevice()->CreateTexture2D(&DSV_SRV_Tex2d_Desc, NULL, Texture2D.GetAddressOf());
		if (FAILED(hr))
		{
			assert("to create texture is failed.");
		}

		//深度ステンシルビュー作成
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(dsvd));
		dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;//DSV用
		dsvd.Texture2D.MipSlice = 0;
		hr = FRAMEWORK.getDevice()->CreateDepthStencilView(Texture2D.Get(), &dsvd, cached_depth_stencil_view.GetAddressOf());
		if (FAILED(hr))
		{
			assert("to create RTV is failed.");
		}

		//	シェーダーリソースビュー作成
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = 1;
		hr = FRAMEWORK.getDevice()->CreateShaderResourceView(Texture2D.Get(), &srvd, depth_stencil_shader_resource_view.GetAddressOf());
		if (FAILED(hr))
		{
			assert("to create SRV is failed.");
		}

		//深度ステンシルステート作成
		D3D11_DEPTH_STENCIL_DESC dsd;
		dsd.DepthEnable = TRUE;//深度テストするか
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;//
		dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;//更新条件
		dsd.StencilEnable = FALSE;//ステンシルテストするかしないか
		dsd.StencilReadMask = 0xFF;
		dsd.StencilWriteMask = 0xFF;
		dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		FRAMEWORK.getDevice()->CreateDepthStencilState(&dsd, DepthStencilState.GetAddressOf());
		if (FAILED(hr))
		{
			assert("Failed to CreateDepthStencilState");
		}
	}

	//深度ビューテスト
	//テクスチャ作成
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
	D3D11_TEXTURE2D_DESC txDesc;
	ZeroMemory(&txDesc, sizeof(txDesc));
	txDesc.Width = FRAMEWORK.SCREEN_WIDTH;
	txDesc.Height = FRAMEWORK.SCREEN_HEIGHT;
	txDesc.MipLevels = 1;
	txDesc.ArraySize = 1;
	txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	txDesc.SampleDesc.Count = 1;
	txDesc.SampleDesc.Quality = 0;
	txDesc.Usage = D3D11_USAGE_DEFAULT;
	txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	txDesc.CPUAccessFlags = 0;
	txDesc.MiscFlags = 0;
	FRAMEWORK.getDevice()->CreateTexture2D(&txDesc, NULL, depth_stencil_buffer.GetAddressOf());

	//深度ステンシルビュー
	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.Format = txDesc.Format;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;
	FRAMEWORK.getDevice()->CreateDepthStencilView(depth_stencil_buffer.Get(), &dsDesc, DepthStencilView2.GetAddressOf());
}

void frame_buffer::swapRenderTargetViewBegin(Texture *texture, DirectX::XMFLOAT4 light, int blend_type, DirectX::XMFLOAT2 vp_size, DirectX::XMFLOAT4 clear_color, bool dsvFlg)
{
	//今セットされているレンダーターゲットを保存
	FRAMEWORK.getImmediateContext()->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());
	//書き込み先の設定
	render_target_view = texture->GetRenderTarget();//textureのレンダーターゲットを設定する
	//ID3D11DepthStencilView* dsv = texture->GetDepthStencil();//textureで同じ大きさで作る//textureでdsvも作る
	depth_stencil_view = cached_depth_stencil_view;//FRAMEWORKのRTVの深度情報いるならこっち
	/////////////////////////////////2020/6/29


	//warning潰しのnullset
	ID3D11ShaderResourceView* dummySRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	FRAMEWORK.getImmediateContext()->PSSetShaderResources(0, _countof(dummySRVs), dummySRVs);
	FRAMEWORK.getImmediateContext()->DSSetShaderResources(0, _countof(dummySRVs), dummySRVs);

	if (dsvFlg)
	{
		FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());//dsv更新許可
	}
	if (!dsvFlg)
	{
		depth_stencil_view = DepthStencilView2;
		FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());//深度情報は使用しない(仮の深度ビューを与えておく)
	}

	// 画面とZステンシルバッファのクリア
	float ClearColor[4] = { clear_color.x,clear_color.y,clear_color.z,clear_color.w }; //red,green,blue,alpha
	FRAMEWORK.getImmediateContext()->ClearRenderTargetView(render_target_view.Get(), ClearColor);
	/*if (dsvFlg)*/
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//デプスステンシルステート設定
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(DepthStencilState.Get(), 1);

	//ブレンドステート設定
	//static blender blender(FRAMEWORK.device);
	//FRAMEWORK.getImmediateContext()->OMSetBlendState(blend->states[blend_type].Get(), nullptr, 0xFFFFFFFF);
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(blend_type), nullptr, 0xFFFFFFFF);

	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = vp_size.x;//ラスタライズする画面の幅
	vp.Height = vp_size.y;//高さ
	vp.MinDepth = 0.0f;//最低深度
	vp.MaxDepth = 1.0f;//最高深度
	vp.TopLeftX = 0;//左座標
	vp.TopLeftY = 0;//

	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);
}


void frame_buffer::swapRenderTargetViewEnd()
{

	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(), cached_depth_stencil_view.Get());//FRAMEWORKのRTVに設定しなおす
	//FRAMEWORK.getImmediateContext()->ClearDepthStencilView(DepthStencilView2.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);//2は常に何も入れないようにしておく

	//コンパイルは通る//2020/7/14
	//dsv.Get()->Release();
	//rtv.Get()->Release();

	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = 1280;//ラスタライズする画面の幅
	vp.Height = 720;//高さ
	vp.MinDepth = 0.0f;//最低深度
	vp.MaxDepth = 1.0f;//最高深度
	vp.TopLeftX = 0;//左座標
	vp.TopLeftY = 0;//

	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);

}

//void  frame_buffer::createSsaoBufferBegine(std::unique_ptr<Texture> texture[2], DirectX::XMFLOAT4 light, int blend_type, DirectX::XMFLOAT4 clear_color)
//{
//	//今セットされているレンダーターゲットを保存
//	FRAMEWORK.getImmediateContext()->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());
//	//書き込み先の設定
//
//	depth_stencil_view = cached_depth_stencil_view;//FRAMEWORKのRTVの深度情報いるならこっち
//
//	//書き込み先の設定
//	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtvs[2] = { texture[0]->GetRenderTarget(),texture[1]->GetRenderTarget()};//g_buffer
//	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(2, rtvs->GetAddressOf(), depth_stencil_view.Get());//dsv更新許可
//
//	// 画面とZステンシルバッファのクリア
//	float ClearColor[4] = { clear_color.x,clear_color.y,clear_color.z,clear_color.w }; //red,green,blue,alpha
//	for (int i = 0; i < 2; i++)
//	{
//		FRAMEWORK.getImmediateContext()->ClearRenderTargetView(rtvs[i].Get(), ClearColor);
//	}
//	/*if (dsvFlg)*/
//	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//
//	//デプスステンシルステート設定
//	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(DepthStencilState.Get(), 1);
//
//	//ブレンドステート設定
//	//static blender blender(FRAMEWORK.device);
//	//FRAMEWORK.getImmediateContext()->OMSetBlendState(blend->states[blend_type].Get(), nullptr, 0xFFFFFFFF);
//	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(blend_type), nullptr, 0xFFFFFFFF);
//
//	//ライト位置にViewを設定
//	DirectX::XMFLOAT3 t(0, 0, 0);
//	DirectX::XMFLOAT3 p;
//	p.x = t.x - light.x * 30.0f;
//	p.y = t.y - light.y * 30.0f;
//	p.z = t.z - light.z * 30.0f;
//
//	// ビューポート設定
//	D3D11_VIEWPORT vp;
//	vp.Width = sCREEN_WIDTH;//ラスタライズする画面の幅
//	vp.Height = sCREEN_HEIGHT;//高さ
//	vp.MinDepth = 0.0f;//最低深度
//	vp.MaxDepth = 1.0f;//最高深度
//	vp.TopLeftX = 0;//左座標
//	vp.TopLeftY = 0;//
//
//	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);
//
//	//// ビューの設定
//	//View v;
//	//v.Set(p, DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
//	//// 平行投影
//	//v.SetOrtho(10.f, 10.0f, 1.0f, 100.0f);
//	//v.Activate();
//	////ライト空間PV行列
//	//XMStoreFloat4x4(&LightViewProjection, v.GetView()*v.GetProjection());
//}
//
//void frame_buffer::createSsaoBufferEnd()
//{
//	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(), cached_depth_stencil_view.Get());//FRAMEWORKのRTVに設定しなおす
//	//FRAMEWORK.getImmediateContext()->ClearDepthStencilView(DepthStencilView2.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);//2は常に何も入れないようにしておく
//
//	//コンパイルは通る//2020/7/14
//	//dsv.Get()->Release();
//	//rtv.Get()->Release();
//}
//
//void frame_buffer::createSsrBufferBegine(std::unique_ptr<Texture> texture[3], DirectX::XMFLOAT4 light , int blend_type , DirectX::XMFLOAT4 clear_color )
//{
//	//今セットされているレンダーターゲットを保存
//	FRAMEWORK.getImmediateContext()->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());
//	//書き込み先の設定
//	depth_stencil_view = cached_depth_stencil_view;//FRAMEWORKのRTVの深度情報いるならこっち
//
//	//書き込み先の設定
//	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtvs[3] = { texture[0]->GetRenderTarget(),texture[1]->GetRenderTarget(),texture[2]->GetRenderTarget() };//g_buffer
//	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(3, rtvs->GetAddressOf(), depth_stencil_view.Get());//dsv更新許可
//
//	// 画面とZステンシルバッファのクリア
//	float ClearColor[4] = { clear_color.x,clear_color.y,clear_color.z,clear_color.w }; //red,green,blue,alpha
//	for (int i = 0; i < 3; i++)
//	{
//		FRAMEWORK.getImmediateContext()->ClearRenderTargetView(rtvs[i].Get(), ClearColor);
//	}
//	/*if (dsvFlg)*/
//	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//
//	//デプスステンシルステート設定
//	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(DepthStencilState.Get(), 1);
//
//	//ブレンドステート設定
//	//static blender blender(FRAMEWORK.device);
//	//FRAMEWORK.getImmediateContext()->OMSetBlendState(blend->states[blend_type].Get(), nullptr, 0xFFFFFFFF);
//	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(blend_type), nullptr, 0xFFFFFFFF);
//
//	// ビューポート設定
//	D3D11_VIEWPORT vp;
//	vp.Width = sCREEN_WIDTH;//ラスタライズする画面の幅
//	vp.Height = sCREEN_HEIGHT;//高さ
//	vp.MinDepth = 0.0f;//最低深度
//	vp.MaxDepth = 1.0f;//最高深度
//	vp.TopLeftX = 0;//左座標
//	vp.TopLeftY = 0;//
//
//	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);
//}
//
//void frame_buffer::createSsrBufferEnd()
//{
//	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(), cached_depth_stencil_view.Get());//FRAMEWORKのRTVに設定しなおす
//}

void frame_buffer::createGBufferBegin(std::unique_ptr<Texture> texture[GBUFFER_MAX], DirectX::XMFLOAT4 light,  DirectX::XMFLOAT4 clear_color, bool dsvFlg)
{
	//今セットされているレンダーターゲットを保存
	FRAMEWORK.getImmediateContext()->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());
	//書き込み先の設定
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtvs[GBUFFER_MAX] = { texture[0]->GetRenderTarget(),texture[1]->GetRenderTarget(),texture[2]->GetRenderTarget(),texture[3]->GetRenderTarget() };//g_buffer
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = cached_depth_stencil_view.Get();

	//ID3D11RenderTargetView* nullViews[] = { nullptr };
	 //FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, nullViews, nullptr);

	//WARNING対策でnullセット
	ID3D11ShaderResourceView* dummySRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	FRAMEWORK.getImmediateContext()->PSSetShaderResources(0, _countof(dummySRVs), dummySRVs);
	FRAMEWORK.getImmediateContext()->DSSetShaderResources(0, _countof(dummySRVs), dummySRVs);

	if (dsvFlg)
	{
		FRAMEWORK.getImmediateContext()->OMSetRenderTargets(GBUFFER_MAX, rtvs->GetAddressOf(), dsv.Get());//dsv更新許可//9/7/変更
	}
	if (!dsvFlg)
	{
		dsv = DepthStencilView2.Get();                                                                    /*9/7/変更*/
		FRAMEWORK.getImmediateContext()->OMSetRenderTargets(GBUFFER_MAX, rtvs->GetAddressOf(), dsv.Get());//深度情報は使用しない(仮の深度ビューを与えておく)
	}

	// 画面とZステンシルバッファのクリア
	float c_color[4] = { clear_color.x,clear_color.y,clear_color.z,clear_color.w }; //red,green,blue,alpha
	for (int i = 0; i < GBUFFER_MAX; i++)
	{
		FRAMEWORK.getImmediateContext()->ClearRenderTargetView(rtvs[i].Get(), c_color);
	}

	/*if (dsvFlg)*/
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//デプスステンシルステート設定
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(DepthStencilState.Get(), 1);

	//ブレンドステート設定
	//static blender blender(FRAMEWORK.device);
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0xFFFFFFFF);

	//ライト位置にViewを設定
	DirectX::XMFLOAT3 t(0, 0, 0);
	DirectX::XMFLOAT3 p;
	p.x = t.x - light.x * 30.0f;
	p.y = t.y - light.y * 30.0f;
	p.z = t.z - light.z * 30.0f;

	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = sCREEN_WIDTH;//ラスタライズする画面の幅
	vp.Height = sCREEN_HEIGHT;//高さ
	vp.MinDepth = 0.0f;//最低深度
	vp.MaxDepth = 1.0f;//最高深度
	vp.TopLeftX = 0;//左座標
	vp.TopLeftY = 0;//

	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);

	//// ビューの設定
	//View v;
	//v.Set(p, DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
	//// 平行投影
	//v.SetOrtho(10.f, 10.0f, 1.0f, 100.0f);
	//v.Activate();
	////ライト空間PV行列
	//XMStoreFloat4x4(&LightViewProjection, v.GetView()*v.GetProjection());
}

void frame_buffer::createGBufferEnd()
{
	//nullセットして解放
	//ID3D11RenderTargetView* dummyRTVs[GBUFFER_MAX] = { nullptr };
	//FRAMEWORK.getImmediateContext()->OMSetRenderTargets(GBUFFER_MAX, dummyRTVs, nullptr);

	//元のレンダーターゲットにセットしなおす
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = cached_depth_stencil_view.Get();
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv = cached_render_target_view.Get();
	
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());//FRAMEWORKのRTVに設定しなおす
	//FRAMEWORK.getImmediateContext()->ClearDepthStencilView(DepthStencilView2.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);//2は常に何も入れないようにしておく
}

void frame_buffer::createGBufferBegin2(std::unique_ptr<Texture> texture[2], DirectX::XMFLOAT4 light, DirectX::XMFLOAT4 clear_color, bool dsvFlg)
{
	//今セットされているレンダーターゲットを保存
	FRAMEWORK.getImmediateContext()->OMGetRenderTargets(1, cached_render_target_view.GetAddressOf(), cached_depth_stencil_view.GetAddressOf());
	//書き込み先の設定
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtvs[2] = { texture[0]->GetRenderTarget(),texture[1]->GetRenderTarget()};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = cached_depth_stencil_view.Get();//FRAMEWORKのRTVの深度情報いるならこっち

	if (dsvFlg)
	{
		FRAMEWORK.getImmediateContext()->OMSetRenderTargets(2, rtvs->GetAddressOf(), dsv.Get());//dsv更新許可//9/7/変更
	}
	if (!dsvFlg)
	{
		dsv = DepthStencilView2.Get();                                                                    /*9/7/変更*/
		FRAMEWORK.getImmediateContext()->OMSetRenderTargets(2, rtvs->GetAddressOf(), dsv.Get());//深度情報は使用しない(仮の深度ビューを与えておく)
	}

	// 画面とZステンシルバッファのクリア
	float c_color[4] = { clear_color.x,clear_color.y,clear_color.z,clear_color.w }; //red,green,blue,alpha
	for (int i = 0; i < 2; i++)
	{
		FRAMEWORK.getImmediateContext()->ClearRenderTargetView(rtvs[i].Get(), c_color);
	}

	/*if (dsvFlg)*/
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//デプスステンシルステート設定
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(DepthStencilState.Get(), 1);

	//ブレンドステート設定
	//static blender blender(FRAMEWORK.device);
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0xFFFFFFFF);

	//ライト位置にViewを設定
	//DirectX::XMFLOAT3 t(0, 0, 0);
	//DirectX::XMFLOAT3 p;
	//p.x = t.x - light.x * 30.0f;
	//p.y = t.y - light.y * 30.0f;
	//p.z = t.z - light.z * 30.0f;

	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = sCREEN_WIDTH;//ラスタライズする画面の幅
	vp.Height = sCREEN_HEIGHT;//高さ
	vp.MinDepth = 0.0f;//最低深度
	vp.MaxDepth = 1.0f;//最高深度
	vp.TopLeftX = 0;//左座標
	vp.TopLeftY = 0;//

	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);

	//// ビューの設定
	//View v;
	//v.Set(p, DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
	//// 平行投影
	//v.SetOrtho(10.f, 10.0f, 1.0f, 100.0f);
	//v.Activate();
	////ライト空間PV行列
	//XMStoreFloat4x4(&LightViewProjection, v.GetView()*v.GetProjection());
}

void frame_buffer::createGBufferEnd2()
{
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = cached_depth_stencil_view.Get();
	//元のレンダーターゲットにセットしなおす
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv = cached_render_target_view.Get();
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());//FRAMEWORKのRTVに設定しなおす
}

//深度ビューから深度マップ作成(RTV使わないのでピクセルシェーダー不要)
void frame_buffer::createDepthMapBegin(Texture_d *texture_d, DirectX::XMFLOAT4 light, DirectX::XMFLOAT2 vp_size)
{
	//今セットされているレンダーターゲットを保存
	FRAMEWORK.getImmediateContext()->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());
	
	//warning潰しのnullset
	ID3D11ShaderResourceView* dummySRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	FRAMEWORK.getImmediateContext()->PSSetShaderResources(0, _countof(dummySRVs), dummySRVs);
	FRAMEWORK.getImmediateContext()->DSSetShaderResources(0, _countof(dummySRVs), dummySRVs);

	//書き込み先の設定
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> dummy_rtv = nullptr;//WARNING対策
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = texture_d->GetDepthStencilView();
	//ID3D11DepthStencilView* dsv = texture_d->GetDepthStencilView();
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, render_target_view.GetAddressOf(), dsv.Get());//WARNING潰し
	//FRAMEWORK.getImmediateContext()->OMSetRenderTargets(0, nullptr, dsv.Get());//dsvだけ設定


	// 画面とZステンシルバッファのクリア
	//float color[4] = { 0,0,0,0 };
	//FRAMEWORK.getImmediateContext()->ClearRenderTargetView(rtv, color);
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//デプスステンシルステート設定
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(DepthStencilState.Get(), 1);

	//ブレンドステート設定
	static blender blender(FRAMEWORK.device);
	FRAMEWORK.getImmediateContext()->OMSetBlendState(blender.states[blender::BS_ALPHA].Get(), nullptr, 0xFFFFFFFF);

	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = vp_size.x;//ラスタライズする画面の幅
	vp.Height = vp_size.y;//高さ
	vp.MinDepth = 0.0f;//最低深度
	vp.MaxDepth = 1.0f;//最高深度
	vp.TopLeftX = 0;//左座標
	vp.TopLeftY = 0;//

	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);
}

void frame_buffer::createDepthMapEnd()
{
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = cached_depth_stencil_view.Get();
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv = cached_render_target_view.Get();
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());//FRAMEWORKのRTVに設定しなおす
	D3D11_VIEWPORT vp;
	vp.Width = sCREEN_WIDTH;//ラスタライズする画面の幅
	vp.Height = sCREEN_HEIGHT;//高さ
	vp.MinDepth = 0.0f;//最低深度
	vp.MaxDepth = 1.0f;//最高深度
	vp.TopLeftX = 0;//左座標
	vp.TopLeftY = 0;//

	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);
}

void frame_buffer::release()
{
	/*if (ShaderResourceView)
	{
		ShaderResourceView->Release();
		ShaderResourceView = NULL;
	}
	if (ShaderResourceView_d)
	{
		ShaderResourceView_d->Release();
		ShaderResourceView_d = NULL;
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
	}
	if (DepthStencilView2)
	{
		DepthStencilView2->Release();
		DepthStencilView2 = NULL;
	}
	if (Sampler)
	{
		Sampler->Release();
		Sampler = NULL;
	}
	if (DepthStencilState)
	{
		DepthStencilState->Release();
		DepthStencilState = NULL;
	}*/
}

//元画像のテクスチャは小さい必要がある//(入力,出力,使用するシェーダー)
Texture* frame_buffer::createDownSample(Texture  *tex_input, Texture *tex_output, shader *shader, float width, float height)
{
	DirectX::XMFLOAT2 size = DirectX::XMFLOAT2(tex_input->GetWidth(), tex_input->GetHeight());//元画像のサイズを取得
	tex_output->Create(size.x, size.y);//出力先のテクスチャのサイズを入力元のサイズと合わせる
	std::unique_ptr<sprite> tmpSpr;
	tmpSpr = std::make_unique<sprite>();
	swapRenderTargetViewBegin(tex_output, Light::LightDir, blender::BLEND_STATE::BS_ALPHA, DirectX::XMFLOAT2(1280,720),DirectX::XMFLOAT4(0, 0, 0, 1), false);//RTVに描画
	tmpSpr->render(shader, tex_input, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
	swapRenderTargetViewEnd();
	tex_output->SetSize(DirectX::XMFLOAT2(width, height));//小さくした画像を(未設定なら)画面サイズまで拡大処理

	return tex_output;
}
