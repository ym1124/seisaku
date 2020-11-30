
#include "frame_buffer.h"
#include "framework.h"
//#include "view.h"
#include "blend.h"
#include "sprite.h"
#include "texture.h"

//Resource being set to OM RenderTarget slot 0 is still bound on input
//�������\�[�X�ɓ�����read/write���悤�Ƃ���Əo��//https://ja.stackoverflow.com/questions/4737/d3d11%E3%81%AE%E3%83%87%E3%83%90%E3%83%83%E3%82%B0%E3%83%AC%E3%82%A4%E3%83%A4%E3%83%BC%E3%81%A7%E5%87%BA%E3%82%8B%E8%AD%A6%E5%91%8A%E3%81%8C%E5%8F%96%E3%82%8C%E3%81%AA%E3%81%84

//Texture2D�ӂ����
//RTV��O�̏�ԂŋL�����Ă����N���X
frame_buffer::frame_buffer()
{
	//if (blend == nullptr)
	//	blend =new blender(FRAMEWORK.getDevice());

	D3D11_TEXTURE2D_DESC RTV_SRV_Tex2d_Desc;
	D3D11_TEXTURE2D_DESC DSV_SRV_Tex2d_Desc;
	//RTV��SRV���C�p�e�N�X�`��
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
		HRESULT hr = S_OK;
		//	�e�N�X�`���쐬
		ZeroMemory(&RTV_SRV_Tex2d_Desc, sizeof(RTV_SRV_Tex2d_Desc));
		RTV_SRV_Tex2d_Desc.Width = FRAMEWORK.SCREEN_WIDTH;
		RTV_SRV_Tex2d_Desc.Height = FRAMEWORK.SCREEN_HEIGHT;
		RTV_SRV_Tex2d_Desc.MipLevels = 1;
		RTV_SRV_Tex2d_Desc.ArraySize = 1;
		RTV_SRV_Tex2d_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		RTV_SRV_Tex2d_Desc.SampleDesc.Count = 1;
		RTV_SRV_Tex2d_Desc.Usage = D3D11_USAGE_DEFAULT;
		RTV_SRV_Tex2d_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		RTV_SRV_Tex2d_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;//RTV�ł���SRV�Ƃ��Ă��g����悤�ɐݒ肵�Ă����I

		hr = FRAMEWORK.getDevice()->CreateTexture2D(&RTV_SRV_Tex2d_Desc, NULL, Texture2D.GetAddressOf());
		if (FAILED(hr))
		{
			assert("to create texture is failed.");
		}

		//	�����_�[�^�[�Q�b�g�r���[�쐬
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


		//	�V�F�[�_�[���\�[�X�r���[�쐬
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

	//DSV��SRV���C�p�e�N�X�`��
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
		HRESULT hr = S_OK;
		//	�e�N�X�`���쐬
		ZeroMemory(&DSV_SRV_Tex2d_Desc, sizeof(DSV_SRV_Tex2d_Desc));
		DSV_SRV_Tex2d_Desc.Width = FRAMEWORK.SCREEN_WIDTH;
		DSV_SRV_Tex2d_Desc.Height = FRAMEWORK.SCREEN_HEIGHT;
		DSV_SRV_Tex2d_Desc.MipLevels = 1;
		DSV_SRV_Tex2d_Desc.ArraySize = 1;
		DSV_SRV_Tex2d_Desc.Format = DXGI_FORMAT_R24G8_TYPELESS;//�[�x�X�e���V���p
		DSV_SRV_Tex2d_Desc.SampleDesc.Count = 1;
		DSV_SRV_Tex2d_Desc.SampleDesc.Quality = 0;
		DSV_SRV_Tex2d_Desc.Usage = D3D11_USAGE_DEFAULT;
		DSV_SRV_Tex2d_Desc.CPUAccessFlags = 0;
		DSV_SRV_Tex2d_Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;//SRV�����ƍ쐬���s

		hr = FRAMEWORK.getDevice()->CreateTexture2D(&DSV_SRV_Tex2d_Desc, NULL, Texture2D.GetAddressOf());
		if (FAILED(hr))
		{
			assert("to create texture is failed.");
		}

		//�[�x�X�e���V���r���[�쐬
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(dsvd));
		dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;//DSV�p
		dsvd.Texture2D.MipSlice = 0;
		hr = FRAMEWORK.getDevice()->CreateDepthStencilView(Texture2D.Get(), &dsvd, cached_depth_stencil_view.GetAddressOf());
		if (FAILED(hr))
		{
			assert("to create RTV is failed.");
		}

		//	�V�F�[�_�[���\�[�X�r���[�쐬
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

		//�[�x�X�e���V���X�e�[�g�쐬
		D3D11_DEPTH_STENCIL_DESC dsd;
		dsd.DepthEnable = TRUE;//�[�x�e�X�g���邩
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;//
		dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;//�X�V����
		dsd.StencilEnable = FALSE;//�X�e���V���e�X�g���邩���Ȃ���
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

	//�[�x�r���[�e�X�g
	//�e�N�X�`���쐬
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

	//�[�x�X�e���V���r���[
	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.Format = txDesc.Format;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;
	FRAMEWORK.getDevice()->CreateDepthStencilView(depth_stencil_buffer.Get(), &dsDesc, DepthStencilView2.GetAddressOf());
}

void frame_buffer::swapRenderTargetViewBegin(Texture *texture, DirectX::XMFLOAT4 light, int blend_type, DirectX::XMFLOAT2 vp_size, DirectX::XMFLOAT4 clear_color, bool dsvFlg)
{
	//���Z�b�g����Ă��郌���_�[�^�[�Q�b�g��ۑ�
	FRAMEWORK.getImmediateContext()->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());
	//�������ݐ�̐ݒ�
	render_target_view = texture->GetRenderTarget();//texture�̃����_�[�^�[�Q�b�g��ݒ肷��
	//ID3D11DepthStencilView* dsv = texture->GetDepthStencil();//texture�œ����傫���ō��//texture��dsv�����
	depth_stencil_view = cached_depth_stencil_view;//FRAMEWORK��RTV�̐[�x��񂢂�Ȃ炱����
	/////////////////////////////////2020/6/29


	//warning�ׂ���nullset
	ID3D11ShaderResourceView* dummySRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	FRAMEWORK.getImmediateContext()->PSSetShaderResources(0, _countof(dummySRVs), dummySRVs);
	FRAMEWORK.getImmediateContext()->DSSetShaderResources(0, _countof(dummySRVs), dummySRVs);

	if (dsvFlg)
	{
		FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());//dsv�X�V����
	}
	if (!dsvFlg)
	{
		depth_stencil_view = DepthStencilView2;
		FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());//�[�x���͎g�p���Ȃ�(���̐[�x�r���[��^���Ă���)
	}

	// ��ʂ�Z�X�e���V���o�b�t�@�̃N���A
	float ClearColor[4] = { clear_color.x,clear_color.y,clear_color.z,clear_color.w }; //red,green,blue,alpha
	FRAMEWORK.getImmediateContext()->ClearRenderTargetView(render_target_view.Get(), ClearColor);
	/*if (dsvFlg)*/
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//�f�v�X�X�e���V���X�e�[�g�ݒ�
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(DepthStencilState.Get(), 1);

	//�u�����h�X�e�[�g�ݒ�
	//static blender blender(FRAMEWORK.device);
	//FRAMEWORK.getImmediateContext()->OMSetBlendState(blend->states[blend_type].Get(), nullptr, 0xFFFFFFFF);
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(blend_type), nullptr, 0xFFFFFFFF);

	// �r���[�|�[�g�ݒ�
	D3D11_VIEWPORT vp;
	vp.Width = vp_size.x;//���X�^���C�Y�����ʂ̕�
	vp.Height = vp_size.y;//����
	vp.MinDepth = 0.0f;//�Œ�[�x
	vp.MaxDepth = 1.0f;//�ō��[�x
	vp.TopLeftX = 0;//�����W
	vp.TopLeftY = 0;//

	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);
}


void frame_buffer::swapRenderTargetViewEnd()
{

	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(), cached_depth_stencil_view.Get());//FRAMEWORK��RTV�ɐݒ肵�Ȃ���
	//FRAMEWORK.getImmediateContext()->ClearDepthStencilView(DepthStencilView2.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);//2�͏�ɉ�������Ȃ��悤�ɂ��Ă���

	//�R���p�C���͒ʂ�//2020/7/14
	//dsv.Get()->Release();
	//rtv.Get()->Release();

	// �r���[�|�[�g�ݒ�
	D3D11_VIEWPORT vp;
	vp.Width = 1280;//���X�^���C�Y�����ʂ̕�
	vp.Height = 720;//����
	vp.MinDepth = 0.0f;//�Œ�[�x
	vp.MaxDepth = 1.0f;//�ō��[�x
	vp.TopLeftX = 0;//�����W
	vp.TopLeftY = 0;//

	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);

}

//void  frame_buffer::createSsaoBufferBegine(std::unique_ptr<Texture> texture[2], DirectX::XMFLOAT4 light, int blend_type, DirectX::XMFLOAT4 clear_color)
//{
//	//���Z�b�g����Ă��郌���_�[�^�[�Q�b�g��ۑ�
//	FRAMEWORK.getImmediateContext()->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());
//	//�������ݐ�̐ݒ�
//
//	depth_stencil_view = cached_depth_stencil_view;//FRAMEWORK��RTV�̐[�x��񂢂�Ȃ炱����
//
//	//�������ݐ�̐ݒ�
//	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtvs[2] = { texture[0]->GetRenderTarget(),texture[1]->GetRenderTarget()};//g_buffer
//	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(2, rtvs->GetAddressOf(), depth_stencil_view.Get());//dsv�X�V����
//
//	// ��ʂ�Z�X�e���V���o�b�t�@�̃N���A
//	float ClearColor[4] = { clear_color.x,clear_color.y,clear_color.z,clear_color.w }; //red,green,blue,alpha
//	for (int i = 0; i < 2; i++)
//	{
//		FRAMEWORK.getImmediateContext()->ClearRenderTargetView(rtvs[i].Get(), ClearColor);
//	}
//	/*if (dsvFlg)*/
//	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//
//	//�f�v�X�X�e���V���X�e�[�g�ݒ�
//	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(DepthStencilState.Get(), 1);
//
//	//�u�����h�X�e�[�g�ݒ�
//	//static blender blender(FRAMEWORK.device);
//	//FRAMEWORK.getImmediateContext()->OMSetBlendState(blend->states[blend_type].Get(), nullptr, 0xFFFFFFFF);
//	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(blend_type), nullptr, 0xFFFFFFFF);
//
//	//���C�g�ʒu��View��ݒ�
//	DirectX::XMFLOAT3 t(0, 0, 0);
//	DirectX::XMFLOAT3 p;
//	p.x = t.x - light.x * 30.0f;
//	p.y = t.y - light.y * 30.0f;
//	p.z = t.z - light.z * 30.0f;
//
//	// �r���[�|�[�g�ݒ�
//	D3D11_VIEWPORT vp;
//	vp.Width = sCREEN_WIDTH;//���X�^���C�Y�����ʂ̕�
//	vp.Height = sCREEN_HEIGHT;//����
//	vp.MinDepth = 0.0f;//�Œ�[�x
//	vp.MaxDepth = 1.0f;//�ō��[�x
//	vp.TopLeftX = 0;//�����W
//	vp.TopLeftY = 0;//
//
//	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);
//
//	//// �r���[�̐ݒ�
//	//View v;
//	//v.Set(p, DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
//	//// ���s���e
//	//v.SetOrtho(10.f, 10.0f, 1.0f, 100.0f);
//	//v.Activate();
//	////���C�g���PV�s��
//	//XMStoreFloat4x4(&LightViewProjection, v.GetView()*v.GetProjection());
//}
//
//void frame_buffer::createSsaoBufferEnd()
//{
//	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(), cached_depth_stencil_view.Get());//FRAMEWORK��RTV�ɐݒ肵�Ȃ���
//	//FRAMEWORK.getImmediateContext()->ClearDepthStencilView(DepthStencilView2.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);//2�͏�ɉ�������Ȃ��悤�ɂ��Ă���
//
//	//�R���p�C���͒ʂ�//2020/7/14
//	//dsv.Get()->Release();
//	//rtv.Get()->Release();
//}
//
//void frame_buffer::createSsrBufferBegine(std::unique_ptr<Texture> texture[3], DirectX::XMFLOAT4 light , int blend_type , DirectX::XMFLOAT4 clear_color )
//{
//	//���Z�b�g����Ă��郌���_�[�^�[�Q�b�g��ۑ�
//	FRAMEWORK.getImmediateContext()->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());
//	//�������ݐ�̐ݒ�
//	depth_stencil_view = cached_depth_stencil_view;//FRAMEWORK��RTV�̐[�x��񂢂�Ȃ炱����
//
//	//�������ݐ�̐ݒ�
//	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtvs[3] = { texture[0]->GetRenderTarget(),texture[1]->GetRenderTarget(),texture[2]->GetRenderTarget() };//g_buffer
//	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(3, rtvs->GetAddressOf(), depth_stencil_view.Get());//dsv�X�V����
//
//	// ��ʂ�Z�X�e���V���o�b�t�@�̃N���A
//	float ClearColor[4] = { clear_color.x,clear_color.y,clear_color.z,clear_color.w }; //red,green,blue,alpha
//	for (int i = 0; i < 3; i++)
//	{
//		FRAMEWORK.getImmediateContext()->ClearRenderTargetView(rtvs[i].Get(), ClearColor);
//	}
//	/*if (dsvFlg)*/
//	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//
//	//�f�v�X�X�e���V���X�e�[�g�ݒ�
//	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(DepthStencilState.Get(), 1);
//
//	//�u�����h�X�e�[�g�ݒ�
//	//static blender blender(FRAMEWORK.device);
//	//FRAMEWORK.getImmediateContext()->OMSetBlendState(blend->states[blend_type].Get(), nullptr, 0xFFFFFFFF);
//	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(blend_type), nullptr, 0xFFFFFFFF);
//
//	// �r���[�|�[�g�ݒ�
//	D3D11_VIEWPORT vp;
//	vp.Width = sCREEN_WIDTH;//���X�^���C�Y�����ʂ̕�
//	vp.Height = sCREEN_HEIGHT;//����
//	vp.MinDepth = 0.0f;//�Œ�[�x
//	vp.MaxDepth = 1.0f;//�ō��[�x
//	vp.TopLeftX = 0;//�����W
//	vp.TopLeftY = 0;//
//
//	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);
//}
//
//void frame_buffer::createSsrBufferEnd()
//{
//	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(), cached_depth_stencil_view.Get());//FRAMEWORK��RTV�ɐݒ肵�Ȃ���
//}

void frame_buffer::createGBufferBegin(std::unique_ptr<Texture> texture[GBUFFER_MAX], DirectX::XMFLOAT4 light,  DirectX::XMFLOAT4 clear_color, bool dsvFlg)
{
	//���Z�b�g����Ă��郌���_�[�^�[�Q�b�g��ۑ�
	FRAMEWORK.getImmediateContext()->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());
	//�������ݐ�̐ݒ�
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtvs[GBUFFER_MAX] = { texture[0]->GetRenderTarget(),texture[1]->GetRenderTarget(),texture[2]->GetRenderTarget(),texture[3]->GetRenderTarget() };//g_buffer
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = cached_depth_stencil_view.Get();

	//ID3D11RenderTargetView* nullViews[] = { nullptr };
	 //FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, nullViews, nullptr);

	//WARNING�΍��null�Z�b�g
	ID3D11ShaderResourceView* dummySRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	FRAMEWORK.getImmediateContext()->PSSetShaderResources(0, _countof(dummySRVs), dummySRVs);
	FRAMEWORK.getImmediateContext()->DSSetShaderResources(0, _countof(dummySRVs), dummySRVs);

	if (dsvFlg)
	{
		FRAMEWORK.getImmediateContext()->OMSetRenderTargets(GBUFFER_MAX, rtvs->GetAddressOf(), dsv.Get());//dsv�X�V����//9/7/�ύX
	}
	if (!dsvFlg)
	{
		dsv = DepthStencilView2.Get();                                                                    /*9/7/�ύX*/
		FRAMEWORK.getImmediateContext()->OMSetRenderTargets(GBUFFER_MAX, rtvs->GetAddressOf(), dsv.Get());//�[�x���͎g�p���Ȃ�(���̐[�x�r���[��^���Ă���)
	}

	// ��ʂ�Z�X�e���V���o�b�t�@�̃N���A
	float c_color[4] = { clear_color.x,clear_color.y,clear_color.z,clear_color.w }; //red,green,blue,alpha
	for (int i = 0; i < GBUFFER_MAX; i++)
	{
		FRAMEWORK.getImmediateContext()->ClearRenderTargetView(rtvs[i].Get(), c_color);
	}

	/*if (dsvFlg)*/
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//�f�v�X�X�e���V���X�e�[�g�ݒ�
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(DepthStencilState.Get(), 1);

	//�u�����h�X�e�[�g�ݒ�
	//static blender blender(FRAMEWORK.device);
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0xFFFFFFFF);

	//���C�g�ʒu��View��ݒ�
	DirectX::XMFLOAT3 t(0, 0, 0);
	DirectX::XMFLOAT3 p;
	p.x = t.x - light.x * 30.0f;
	p.y = t.y - light.y * 30.0f;
	p.z = t.z - light.z * 30.0f;

	// �r���[�|�[�g�ݒ�
	D3D11_VIEWPORT vp;
	vp.Width = sCREEN_WIDTH;//���X�^���C�Y�����ʂ̕�
	vp.Height = sCREEN_HEIGHT;//����
	vp.MinDepth = 0.0f;//�Œ�[�x
	vp.MaxDepth = 1.0f;//�ō��[�x
	vp.TopLeftX = 0;//�����W
	vp.TopLeftY = 0;//

	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);

	//// �r���[�̐ݒ�
	//View v;
	//v.Set(p, DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
	//// ���s���e
	//v.SetOrtho(10.f, 10.0f, 1.0f, 100.0f);
	//v.Activate();
	////���C�g���PV�s��
	//XMStoreFloat4x4(&LightViewProjection, v.GetView()*v.GetProjection());
}

void frame_buffer::createGBufferEnd()
{
	//null�Z�b�g���ĉ��
	//ID3D11RenderTargetView* dummyRTVs[GBUFFER_MAX] = { nullptr };
	//FRAMEWORK.getImmediateContext()->OMSetRenderTargets(GBUFFER_MAX, dummyRTVs, nullptr);

	//���̃����_�[�^�[�Q�b�g�ɃZ�b�g���Ȃ���
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = cached_depth_stencil_view.Get();
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv = cached_render_target_view.Get();
	
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());//FRAMEWORK��RTV�ɐݒ肵�Ȃ���
	//FRAMEWORK.getImmediateContext()->ClearDepthStencilView(DepthStencilView2.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);//2�͏�ɉ�������Ȃ��悤�ɂ��Ă���
}

void frame_buffer::createGBufferBegin2(std::unique_ptr<Texture> texture[2], DirectX::XMFLOAT4 light, DirectX::XMFLOAT4 clear_color, bool dsvFlg)
{
	//���Z�b�g����Ă��郌���_�[�^�[�Q�b�g��ۑ�
	FRAMEWORK.getImmediateContext()->OMGetRenderTargets(1, cached_render_target_view.GetAddressOf(), cached_depth_stencil_view.GetAddressOf());
	//�������ݐ�̐ݒ�
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtvs[2] = { texture[0]->GetRenderTarget(),texture[1]->GetRenderTarget()};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = cached_depth_stencil_view.Get();//FRAMEWORK��RTV�̐[�x��񂢂�Ȃ炱����

	if (dsvFlg)
	{
		FRAMEWORK.getImmediateContext()->OMSetRenderTargets(2, rtvs->GetAddressOf(), dsv.Get());//dsv�X�V����//9/7/�ύX
	}
	if (!dsvFlg)
	{
		dsv = DepthStencilView2.Get();                                                                    /*9/7/�ύX*/
		FRAMEWORK.getImmediateContext()->OMSetRenderTargets(2, rtvs->GetAddressOf(), dsv.Get());//�[�x���͎g�p���Ȃ�(���̐[�x�r���[��^���Ă���)
	}

	// ��ʂ�Z�X�e���V���o�b�t�@�̃N���A
	float c_color[4] = { clear_color.x,clear_color.y,clear_color.z,clear_color.w }; //red,green,blue,alpha
	for (int i = 0; i < 2; i++)
	{
		FRAMEWORK.getImmediateContext()->ClearRenderTargetView(rtvs[i].Get(), c_color);
	}

	/*if (dsvFlg)*/
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//�f�v�X�X�e���V���X�e�[�g�ݒ�
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(DepthStencilState.Get(), 1);

	//�u�����h�X�e�[�g�ݒ�
	//static blender blender(FRAMEWORK.device);
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0xFFFFFFFF);

	//���C�g�ʒu��View��ݒ�
	//DirectX::XMFLOAT3 t(0, 0, 0);
	//DirectX::XMFLOAT3 p;
	//p.x = t.x - light.x * 30.0f;
	//p.y = t.y - light.y * 30.0f;
	//p.z = t.z - light.z * 30.0f;

	// �r���[�|�[�g�ݒ�
	D3D11_VIEWPORT vp;
	vp.Width = sCREEN_WIDTH;//���X�^���C�Y�����ʂ̕�
	vp.Height = sCREEN_HEIGHT;//����
	vp.MinDepth = 0.0f;//�Œ�[�x
	vp.MaxDepth = 1.0f;//�ō��[�x
	vp.TopLeftX = 0;//�����W
	vp.TopLeftY = 0;//

	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);

	//// �r���[�̐ݒ�
	//View v;
	//v.Set(p, DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
	//// ���s���e
	//v.SetOrtho(10.f, 10.0f, 1.0f, 100.0f);
	//v.Activate();
	////���C�g���PV�s��
	//XMStoreFloat4x4(&LightViewProjection, v.GetView()*v.GetProjection());
}

void frame_buffer::createGBufferEnd2()
{
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = cached_depth_stencil_view.Get();
	//���̃����_�[�^�[�Q�b�g�ɃZ�b�g���Ȃ���
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv = cached_render_target_view.Get();
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());//FRAMEWORK��RTV�ɐݒ肵�Ȃ���
}

//�[�x�r���[����[�x�}�b�v�쐬(RTV�g��Ȃ��̂Ńs�N�Z���V�F�[�_�[�s�v)
void frame_buffer::createDepthMapBegin(Texture_d *texture_d, DirectX::XMFLOAT4 light, DirectX::XMFLOAT2 vp_size)
{
	//���Z�b�g����Ă��郌���_�[�^�[�Q�b�g��ۑ�
	FRAMEWORK.getImmediateContext()->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());
	
	//warning�ׂ���nullset
	ID3D11ShaderResourceView* dummySRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	FRAMEWORK.getImmediateContext()->PSSetShaderResources(0, _countof(dummySRVs), dummySRVs);
	FRAMEWORK.getImmediateContext()->DSSetShaderResources(0, _countof(dummySRVs), dummySRVs);

	//�������ݐ�̐ݒ�
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> dummy_rtv = nullptr;//WARNING�΍�
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = texture_d->GetDepthStencilView();
	//ID3D11DepthStencilView* dsv = texture_d->GetDepthStencilView();
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, render_target_view.GetAddressOf(), dsv.Get());//WARNING�ׂ�
	//FRAMEWORK.getImmediateContext()->OMSetRenderTargets(0, nullptr, dsv.Get());//dsv�����ݒ�


	// ��ʂ�Z�X�e���V���o�b�t�@�̃N���A
	//float color[4] = { 0,0,0,0 };
	//FRAMEWORK.getImmediateContext()->ClearRenderTargetView(rtv, color);
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//�f�v�X�X�e���V���X�e�[�g�ݒ�
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(DepthStencilState.Get(), 1);

	//�u�����h�X�e�[�g�ݒ�
	static blender blender(FRAMEWORK.device);
	FRAMEWORK.getImmediateContext()->OMSetBlendState(blender.states[blender::BS_ALPHA].Get(), nullptr, 0xFFFFFFFF);

	// �r���[�|�[�g�ݒ�
	D3D11_VIEWPORT vp;
	vp.Width = vp_size.x;//���X�^���C�Y�����ʂ̕�
	vp.Height = vp_size.y;//����
	vp.MinDepth = 0.0f;//�Œ�[�x
	vp.MaxDepth = 1.0f;//�ō��[�x
	vp.TopLeftX = 0;//�����W
	vp.TopLeftY = 0;//

	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);
}

void frame_buffer::createDepthMapEnd()
{
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv = cached_depth_stencil_view.Get();
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv = cached_render_target_view.Get();
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());//FRAMEWORK��RTV�ɐݒ肵�Ȃ���
	D3D11_VIEWPORT vp;
	vp.Width = sCREEN_WIDTH;//���X�^���C�Y�����ʂ̕�
	vp.Height = sCREEN_HEIGHT;//����
	vp.MinDepth = 0.0f;//�Œ�[�x
	vp.MaxDepth = 1.0f;//�ō��[�x
	vp.TopLeftX = 0;//�����W
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

//���摜�̃e�N�X�`���͏������K�v������//(����,�o��,�g�p����V�F�[�_�[)
Texture* frame_buffer::createDownSample(Texture  *tex_input, Texture *tex_output, shader *shader, float width, float height)
{
	DirectX::XMFLOAT2 size = DirectX::XMFLOAT2(tex_input->GetWidth(), tex_input->GetHeight());//���摜�̃T�C�Y���擾
	tex_output->Create(size.x, size.y);//�o�͐�̃e�N�X�`���̃T�C�Y����͌��̃T�C�Y�ƍ��킹��
	std::unique_ptr<sprite> tmpSpr;
	tmpSpr = std::make_unique<sprite>();
	swapRenderTargetViewBegin(tex_output, Light::LightDir, blender::BLEND_STATE::BS_ALPHA, DirectX::XMFLOAT2(1280,720),DirectX::XMFLOAT4(0, 0, 0, 1), false);//RTV�ɕ`��
	tmpSpr->render(shader, tex_input, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
	swapRenderTargetViewEnd();
	tex_output->SetSize(DirectX::XMFLOAT2(width, height));//�����������摜��(���ݒ�Ȃ�)��ʃT�C�Y�܂Ŋg�又��

	return tex_output;
}
