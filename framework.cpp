//#include <memory>

#include "framework.h"
//#include "input.h"
//#include "multi_thread.h"
//#include "view.h"
#include "scene_manager.h"

bool framework::initialize()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hwnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT cdev_flags = 0;
#ifdef _DEBUG
	cdev_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;//Shader Model 5.0
	g_DriverType = D3D_DRIVER_TYPE_HARDWARE;

	// �X���b�v�`�F�C���ݒ�
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;//��ʕ�
	sd.BufferDesc.Height = height;//��ʍ���
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;//fps//���q
	sd.BufferDesc.RefreshRate.Denominator = 1;//����//60/1=1�b��60�Ƃ����Ƃ���1�t���[��
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;//�o�͂���E�B���h�E
	sd.SampleDesc.Count = 1;//�}���`�T���v�����O�̐�
	sd.SampleDesc.Quality = 0;//�}���`�T���v�����O�̎�
	sd.Windowed = TRUE;//�E�B���h�E���[�h���t���X�N���[����
	//sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// DirectX11�f�o�C�X�ƃX���b�v�`�F�C���쐬
	hr = D3D11CreateDeviceAndSwapChain(NULL, g_DriverType, NULL,
		/*D3D11_CREATE_DEVICE_DEBUG*/cdev_flags, &feature_level, 1, D3D11_SDK_VERSION, &sd,
		&swap_chain, &device, &g_FeatureLevel, &immediate_context);
	if (FAILED(hr)) {
		return hr;
	}

	//imgui
#if USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, immediate_context);
	//ImGui::StyleColorsClassic();
	//ImGui::StyleColorsLight();
	ImGui::StyleColorsDark();
#endif


	//�������ݐ�Ƃ��Ẵo�b�N�o�b�t�@���m�ۂ���?//�����_�[�^�[�Q�b�g�r���[�쐬
	ID3D11Texture2D* back_buff = NULL;
	hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buff);
	if (FAILED(hr)) {
		return hr;
	}

	//�����_�[�^�[�Q�b�g�r���[(�������ݐ�)
	hr = device->CreateRenderTargetView(back_buff, NULL, &render_target_view);
	back_buff->Release();//GetBuffer�Ŏ擾�A�g�p���Release
	if (FAILED(hr)) {
		return hr;
	}

	//�e�N�X�`���쐬
	ID3D11Texture2D* depth_stencil_buffer;
	D3D11_TEXTURE2D_DESC txDesc;
	ZeroMemory(&txDesc, sizeof(txDesc));
	txDesc.Width = width;
	txDesc.Height = height;
	txDesc.MipLevels = 1;
	txDesc.ArraySize = 1;
	txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	txDesc.SampleDesc.Count = 1;
	txDesc.SampleDesc.Quality = 0;
	txDesc.Usage = D3D11_USAGE_DEFAULT;
	txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	txDesc.CPUAccessFlags = 0;
	txDesc.MiscFlags = 0;
	hr = device->CreateTexture2D(&txDesc, NULL, &depth_stencil_buffer);
	if (FAILED(hr))
		return hr;


	//�[�x�X�e���V���r���[
	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.Format = txDesc.Format;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(depth_stencil_buffer, &dsDesc, &depth_stencil_view);
	if (FAILED(hr))
		return hr;

	depth_stencil_buffer->Release();

	createRasterizerStates();
	createDephtStencilStates();
	createBlendStates();
	createSamplerStates();

	/*if (!blend)
		blend = new blender(getDevice());*/

	// �^�C�g���V�[����
	scene_manager::getInstance().ChangeScene(/*new scene_load(*/scene_manager::SELECT_SCENE::S75/*)*/);
	return true;
}

void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
#if USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
	game_timer++;

	// �L�[���X�V
	KeyInput::Update();

	// �V�[���X�V
	scene_manager::getInstance().Update(elapsed_time);
}

void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{

#if USE_IMGUI
	ImGui::Render();
	//sceneManager��Render�̉��H
#endif

	// �V�[���`��
	scene_manager::getInstance().Render(elapsed_time);

#if USE_IMGUI
	//�G���[�̌���?
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	//�v���[���g
	swap_chain->Present(0, 0);//�������ŌŒ�A��FPS�ύX
}

void framework::release()
{

	if (immediate_context)
		immediate_context->ClearState();

	if (immediate_context)
	{
		immediate_context->Release();
		immediate_context = NULL;
	}
	if (swap_chain)
	{
		swap_chain->Release();
		swap_chain = NULL;
	}
	if (render_target_view)
	{
		render_target_view->Release();
		render_target_view = NULL;
	}
	if (depth_stencil_view)
	{
		depth_stencil_view->Release();
		depth_stencil_view = NULL;
	}

	if (debug)
	{
		debug->Release();
		debug = NULL;
	}

	if (device)
	{
		device->Release();
		device = NULL;
	}

	if (sampler_states[0])
	{
		for (int i = 0; i < SAMPLER_TYPE; i++)
		{
			delete sampler_states[i];
			sampler_states[i] = nullptr;
		}
		//delete[] sampler_states;
	}

	/*if (blend)
	{
		delete blend;
		blend = nullptr;
	}*/
}

HRESULT framework::createRasterizerStates()
{
	D3D11_RASTERIZER_DESC rd;
	for (int type = 0; type < RASTERIZE_TYPE; type++) {
		ZeroMemory(&rd, sizeof(rd));
		switch (type) {
		case RS::RS_CULL_BACK:
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = /*D3D11_CULL_BACK*/D3D11_CULL_NONE;
			rd.FrontCounterClockwise = FALSE;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = TRUE;
			rd.ScissorEnable = FALSE;
			rd.MultisampleEnable = FALSE;
			rd.AntialiasedLineEnable = FALSE;

			break;

		case RS::RS_WIRE:
			rd.FillMode = D3D11_FILL_WIREFRAME;
			rd.CullMode = D3D11_CULL_BACK;
			rd.FrontCounterClockwise = FALSE;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = TRUE;
			rd.ScissorEnable = FALSE;
			rd.MultisampleEnable = FALSE;
			rd.AntialiasedLineEnable = FALSE;
			break;

		case RS::RS_CULL_FRONT:
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_FRONT;
			rd.FrontCounterClockwise = FALSE;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = TRUE;
			rd.ScissorEnable = FALSE;
			rd.MultisampleEnable = FALSE;
			rd.AntialiasedLineEnable = FALSE;

			break;

		case RS::RS_CULL_NONE:
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_NONE;
			rd.FrontCounterClockwise = FALSE;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = TRUE;
			rd.ScissorEnable = FALSE;
			rd.MultisampleEnable = FALSE;
			rd.AntialiasedLineEnable = FALSE;

			break;
		}
		HRESULT hr = device->CreateRasterizerState(&rd, rasterizer_states[type].GetAddressOf());

		if (FAILED(hr))
		{
			assert("���X�^���C�U�X�e�[�g�̐���");
			return false;
		}
	}

	return true;
}

HRESULT framework::createDephtStencilStates()
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;

	for (int type = 0; type < DEPTH_STENCIL_TYPE; type++)
	{
		ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
		switch (type)
		{
		case DS::DS_FALSE://sprite�N���X�̐ݒ莝���Ă���
			//Z�o�b�t�@�̃I���I�t(TRUE : on,FALSE : off)
			depth_stencil_desc.DepthEnable = FALSE;
			//Z�o�b�t�@�ւ̏�������(ZERO : off,ALL : on)
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			//Z�e�X�g�̏���
			/*depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			depth_stencil_desc.StencilEnable = FALSE;
			depth_stencil_desc.StencilReadMask = 0xFF;
			depth_stencil_desc.StencilWriteMask = 0xFF;
			depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;*/
			break;

		case DS::DS_TRUE:
			depth_stencil_desc.DepthEnable = TRUE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
			break;

		case DS::DS_WRITE_FALSE:
			depth_stencil_desc.DepthEnable = TRUE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
			break;
		}

		HRESULT hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[type].GetAddressOf());

		if (FAILED(hr))
		{
			assert("�f�v�X�X�e���V���X�e�[�g���s");
			return false;
		}
	}
	return true;
}

HRESULT framework::createBlendStates()
{
	D3D11_BLEND_DESC bd;

	for (int type = 0; type < BLEND_TYPE; type++)
	{
		ZeroMemory(&bd, sizeof(bd));
		switch (type) {
		case BS::BS_NONE:
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = false;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS::BS_ALPHA:
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS::BS_ADD:
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS::BS_SUBTRACT:
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS_REPLACE:
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;
		case BS::BS_MULTIPLY:
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;
		case BS::BS_LIGHTEN:
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS::BS_DARKEN:
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;
		case BS::BS_SCREEN:
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;
		}
		//�u�����h�X�e�[�g�̍쐬
		HRESULT hr = device->CreateBlendState(&bd, blend_states[type].GetAddressOf());
		//assert(FAILED(hr));

		if (FAILED(hr))
		{
			assert("�u�����h�X�e�[�g�̍쐬���s");
			return false;
		}
	}
	return true;
}

void framework::createSamplerStates(float r, float g, float b,float a)
{
	D3D11_SAMPLER_DESC sd;
	float border_colour[4] = { r, g, b, a };
	for (int type = 0; type < SAMPLER_TYPE; type++)
	{
		switch (type)
		{
		case SM::POINT_BORDER:
			ZeroMemory(&sd, sizeof(sd));
			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;//�����r?
			memcpy(sd.BorderColor, border_colour, sizeof(float) * 4);
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;

			break;

		case SM::LINEAR_BORDER:
			ZeroMemory(&sd, sizeof(sd));
			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;//�����r?
			memcpy(sd.BorderColor, border_colour, sizeof(float) * 4);
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;

			break;

		case SM::ANISOTROPIC_BORDER:
			ZeroMemory(&sd, sizeof(sd));
			sd.Filter = D3D11_FILTER_ANISOTROPIC;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;//�����r?
			sd.MipLODBias = 0;
			sd.MaxAnisotropy = 16;
			memcpy(sd.BorderColor, border_colour, sizeof(float) * 4);
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;

			break;

		case SM::POINT_CLAMP:
			ZeroMemory(&sd, sizeof(sd));
			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;//�����r?
			memcpy(sd.BorderColor, border_colour, sizeof(float) * 4);
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;

			break;

		case SM::LINEAR_CLAMP:
			ZeroMemory(&sd, sizeof(sd));
			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;//�����r?
			memcpy(sd.BorderColor, border_colour, sizeof(float) * 4);
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;

			break;

		case SM::ANISOTROPIC_CLAMP:
			ZeroMemory(&sd, sizeof(sd));
			sd.Filter = D3D11_FILTER_ANISOTROPIC;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;//�����r?
			memcpy(sd.BorderColor, border_colour, sizeof(float) * 4);
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;

			break;

		case SM::LINEAR_WRAP:
			ZeroMemory(&sd, sizeof(sd));
			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;//�����r?
			memcpy(sd.BorderColor, border_colour, sizeof(float) * 4);
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;

			break;

		case SM::LINER_BORDER_WHITE:
	ZeroMemory(&sd, sizeof(sd));
			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;//�����r?
			float border_white[4] = { 1.0f,1.0f,1.0f,1.0f };
			memcpy(sd.BorderColor, border_white, sizeof(float) * 4);
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;

			break;
		}
		sampler_states[type] = new sampler(&sd);
	}
}

HRESULT framework::checkLeak()
{
	//���̎��_�Ő����Ă���I�u�W�F�N�g��\�����邩��I�����ɌĂ΂Ȃ��ƃ_��

	//HRESULT hr;
	//// ���[�N�m�F�p
	//hr = device.Get()->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(debug.GetAddressOf()));
	//if (FAILED(hr))
	//{
	//	//return E_FAIL;
	//}

	//// �ڍו\��
	//hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	//return hr;

	/////////////////////////////////////////////////////////////////////////////
	IDXGIDebug *pDxgiDebug = nullptr;

	//OS���ׂ�
	OSVERSIONINFOEX OSver;
	ULONGLONG condition = 0;
	OSver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	OSver.dwMajorVersion = 6;
	OSver.dwMinorVersion = 2;
	VER_SET_CONDITION(condition, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(condition, VER_MINORVERSION, VER_GREATER_EQUAL);

	if (VerifyVersionInfo(&OSver, VER_MAJORVERSION | VER_MINORVERSION, condition))
	{
		// Widows8.0�ȏ�Ȃ�
		if (pDxgiDebug == nullptr)
		{
			// �쐬
			typedef HRESULT(__stdcall *fPtr)(const IID&, void**);
			HMODULE hDll = GetModuleHandleW(L"dxgidebug.dll");
			fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");

			DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&pDxgiDebug);

			// �o��
			pDxgiDebug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
		}
		else
			pDxgiDebug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
	}
	else
	{
		return debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
}