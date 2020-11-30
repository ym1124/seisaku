//#include <direct.h>
//#include <DirectXMath.h>
//#include <iostream>
//#include <string>
//#include <stdio.h>
#include "sprite.h"
#include "load_file.h"
#include "framework.h"

sprite::sprite() :texture(nullptr)
{
	HRESULT hr = S_OK;

	VERTEX v[] = {
		DirectX::XMFLOAT3(-0.5f, 0.5f,0),DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(0,0), DirectX::XMFLOAT4(1,1,1,1), //����
		DirectX::XMFLOAT3(0.5f, 0.5f,0),  DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(1,0), DirectX::XMFLOAT4(1,1,1,1), //�E��
		DirectX::XMFLOAT3(-0.5f,-0.5f,0), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(0,1), DirectX::XMFLOAT4(1,1,1,1), //����
		DirectX::XMFLOAT3(0.5f,-0.5f,0),  DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(1,1), DirectX::XMFLOAT4(1,1,1,1), //�E��
	};

	//	���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(v);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA res;
	ZeroMemory(&res, sizeof(res));
	res.pSysMem = v;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&bd, &res, vertex_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		assert("Failed to CreateVertexBuffer");
	}

	//���X�^���C�U�ݒ�
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;//���߂�
	rasterizerDesc.CullMode = D3D11_CULL_BACK;//�o�b�N�J�����O
	rasterizerDesc.FrontCounterClockwise = false;//���_�̕\�����v���
	rasterizerDesc.DepthBias = 0;//
	rasterizerDesc.DepthBiasClamp = 0;//
	rasterizerDesc.SlopeScaledDepthBias = false;
	rasterizerDesc.DepthClipEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;
	hr = FRAMEWORK.getDevice()->CreateRasterizerState(&rasterizerDesc, rasterizer_state.GetAddressOf());
	if (FAILED(hr))
	{
		assert("Failed to CreateRasterizerState");
	}

	//�f�v�X�X�e���V���X�e�[�g
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	//Z�o�b�t�@�̃I���I�t(TRUE : on,FALSE : off)
	depth_stencil_desc.DepthEnable = FALSE;
	//Z�o�b�t�@�ւ̏�������(ZERO : off,ALL : on)
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	//Z�e�X�g�̏���
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
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
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = FRAMEWORK.getDevice()->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());
	if (FAILED(hr))
	{
		assert("Failed to CreateDepthStencilState");
	}

	////�T���v���[�X�e�[�g
	//D3D11_SAMPLER_DESC samplerDesc;
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //�e�N�X�`���T���v�����O�Ŏg���t�B���^
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;//0~1�͈̔͊O�ɂ���U�e�N�X�`�����W�̕`����@
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;//0~1�͈̔͊O�ɂ���V�e�N�X�`�����W�̕`����@
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;//0~1�͈̔͊O�ɂ���W�e�N�X�`�����W�̕`����@
	//samplerDesc.MipLODBias = 0;//LoD�̃o�C�A�X�BLoD�ɂ���𑫂����l���ŏI�I��LoD�̃��x��(Level of Detail)
	//samplerDesc.MaxAnisotropy = 16;//�T���v�����O�Ɉٕ����⊮���̗p���Ă���ꍇ�̍ő�l
	//samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;//�T���v���Ƃ̔�r�I�v�V����
	//memcpy(samplerDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	//samplerDesc.MinLOD = 0;//LoD�̉����B0���ő�ōł�����
	//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;//LoD�̏���BMaxLoD>=MinLoD�łȂ���΂Ȃ�Ȃ��B
	//hr = FRAMEWORK.getDevice()->CreateSamplerState(&samplerDesc, &sampler_state);
	//if (FAILED(hr))
	//{
	//	assert("Failed to CreateSamplerState");
	//}
}

sprite::sprite(const wchar_t* file_name/*Texture file name*/)
{
	HRESULT hr = S_OK;

	//ID3D11Device *device = FRAMEWORK.getInstance().getDevice();

	//���_���̃Z�b�g
	vertex vertices[] = {
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
	};

	//���_�o�b�t�@�̍쐬
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//�T�u���\�[�X//�e�N�X�`���Ƃ��e�N�X�`���z��
	D3D11_SUBRESOURCE_DATA subResourceData;
	ZeroMemory(&subResourceData, sizeof(subResourceData));
	subResourceData.pSysMem = vertices;//����������f�[�^�ւ̃|�C���^

	//���_�o�b�t�@
	hr = FRAMEWORK.getInstance().getDevice()->CreateBuffer(&bufferDesc, &subResourceData, vertex_buffer.GetAddressOf());
	if (FAILED(hr))
		return;

	//�C���v�b�g���C�A�E�g//���_�̍\�����K�肷��//�e�Z�}���e�B�N�X�̋K��
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	vs_Load(FRAMEWORK.getInstance().getDevice(), "Shader/sprite_vs.cso", vertexShader.GetAddressOf(), input_layout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	ps_Load(FRAMEWORK.getInstance().getDevice(), "Shader/sprite_ps.cso", pixelShader.GetAddressOf());

	//���X�^���C�U�ݒ�
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;//���߂�
	rasterizerDesc.CullMode = D3D11_CULL_BACK;//�o�b�N�J�����O
	rasterizerDesc.FrontCounterClockwise = false;//���_�̕\�����v���
	rasterizerDesc.DepthBias = 0;//
	rasterizerDesc.DepthBiasClamp = 0;//
	rasterizerDesc.SlopeScaledDepthBias = false;
	rasterizerDesc.DepthClipEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	hr = FRAMEWORK.getInstance().getDevice()->CreateRasterizerState(&rasterizerDesc, rasterizer_state.GetAddressOf());
	if (FAILED(hr))
	{
		assert("Failed to CreateRasterizerState");
	}

	//hr = load_texture_from_file(device, file_name, shader_resource_view.GetAddressOf(), &texture2dDesc);
	//if (FAILED(hr))
	//{
	//	assert("Failed to CreateTextureFromFile");
	//}

	//tex�g���ĉ摜�o���Ȃ火�g��

	tex = new Texture();
	tex->Create();
	hr = load_texture_from_file(FRAMEWORK.getInstance().getDevice(), file_name, tex->GetShaderResourceViewPointer(), &texture2dDesc);
	if (FAILED(hr))
	{
		assert("Failed to CreateTextureFromFile");
	}

	////�T���v���[�X�e�[�g�ɕK�v
	//D3D11_SAMPLER_DESC samplerDesc;
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //�e�N�X�`���T���v�����O�Ŏg���t�B���^
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;//0~1�͈̔͊O�ɂ���U�e�N�X�`�����W�̕`����@
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;//0~1�͈̔͊O�ɂ���V�e�N�X�`�����W�̕`����@
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;//0~1�͈̔͊O�ɂ���W�e�N�X�`�����W�̕`����@
	//samplerDesc.MipLODBias = 0;//LoD�̃o�C�A�X�BLoD�ɂ���𑫂����l���ŏI�I��LoD�̃��x��(Level of Detail)
	//samplerDesc.MaxAnisotropy = 16;//�T���v�����O�Ɉٕ����⊮���̗p���Ă���ꍇ�̍ő�l
	//samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;//�T���v���Ƃ̔�r�I�v�V����
	//memcpy(samplerDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	//samplerDesc.MinLOD = 0;//LoD�̉����B0���ő�ōł�����
	//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;//LoD�̏���BMaxLoD>=MinLoD�łȂ���΂Ȃ�Ȃ��B
	//hr = FRAMEWORK.getInstance().getDevice()->CreateSamplerState(&samplerDesc, &sampler_state);
	//if (FAILED(hr))
	//{
	//	assert("Failed to CreateSamplerState");
	//}

	//�[�x�X�e���V���X�e�[�g�L�q
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	depth_stencil_desc.DepthEnable = FALSE;//�[�x�e�X�g���邩
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.StencilEnable = FALSE;//�X�e���V���e�X�g���邩���Ȃ���
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	FRAMEWORK.getInstance().getDevice()->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());
	if (FAILED(hr))
	{
		assert("Failed to CreateDepthStencilState");
	}

	//�u�����h�X�e�[�g
	//D3D11_BLEND_DESC blendDesc;
	//ZeroMemory(&blendDesc, sizeof(blendDesc));
	//blendDesc.AlphaToCoverageEnable = FALSE;//�}���`�T���v�����O�̎��Ƀ��l���g����
	//blendDesc.IndependentBlendEnable = FALSE; //FALSE�Ȃ�RenderTarget��[0]�݂̂�ݒ肷��
	//blendDesc.RenderTarget[0].BlendEnable = TRUE;//�u�����h���邩
	//blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;//�\�[�X��RGB�l
	//blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;//�v�Z���@
	//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;//�\�[�X�̃��l
	//blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;//�v�Z���@
	//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;//RenderTarget�ɂǂ̐F���������ނ�
	//hr = device->CreateBlendState(&blendDesc, &blend_state);
	//if (FAILED(hr))
	//{
	//	assert("Failed to CreateBlendState");
	//}
}

sprite::sprite(ID3D11Device* device)
{
	HRESULT hr = S_OK;

	//���_���̃Z�b�g
	vertex vertices[] = {
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
	};

	//���_�o�b�t�@�̍쐬
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//�T�u���\�[�X//�e�N�X�`���Ƃ��e�N�X�`���z��
	D3D11_SUBRESOURCE_DATA subResourceData;
	ZeroMemory(&subResourceData, sizeof(subResourceData));
	subResourceData.pSysMem = vertices;//����������f�[�^�ւ̃|�C���^

	//���_�o�b�t�@
	hr = device->CreateBuffer(&bufferDesc, &subResourceData, vertex_buffer.GetAddressOf());
	if (FAILED(hr))
		return;

	//�C���v�b�g���C�A�E�g//���_�̍\�����K�肷��//�e�Z�}���e�B�N�X�̋K��
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	vs_Load(device, "sprite_vs.cso", vertexShader.GetAddressOf(), input_layout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	ps_Load(device, "sprite_ps.cso", pixelShader.GetAddressOf());

	//���X�^���C�U�ݒ�
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;//���߂�
	rasterizerDesc.CullMode = D3D11_CULL_BACK;//�o�b�N�J�����O
	rasterizerDesc.FrontCounterClockwise = false;//���_�̕\�����v���
	rasterizerDesc.DepthBias = 0;//
	rasterizerDesc.DepthBiasClamp = 0;//
	rasterizerDesc.SlopeScaledDepthBias = false;
	rasterizerDesc.DepthClipEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizer_state.GetAddressOf());
	if (FAILED(hr))
	{
		assert("Failed to CreateRasterizerState");
	}

	////�T���v���[�X�e�[�g�ɕK�v
	//D3D11_SAMPLER_DESC samplerDesc;
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //�e�N�X�`���T���v�����O�Ŏg���t�B���^
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;//0~1�͈̔͊O�ɂ���U�e�N�X�`�����W�̕`����@
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;//0~1�͈̔͊O�ɂ���V�e�N�X�`�����W�̕`����@
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;//0~1�͈̔͊O�ɂ���W�e�N�X�`�����W�̕`����@
	//samplerDesc.MipLODBias = 0;//LoD�̃o�C�A�X�BLoD�ɂ���𑫂����l���ŏI�I��LoD�̃��x��(Level of Detail)
	//samplerDesc.MaxAnisotropy = 16;//�T���v�����O�Ɉٕ����⊮���̗p���Ă���ꍇ�̍ő�l
	//samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;//�T���v���Ƃ̔�r�I�v�V����
	//memcpy(samplerDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	//samplerDesc.MinLOD = 0;//LoD�̉����B0���ő�ōł�����
	//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;//LoD�̏���BMaxLoD>=MinLoD�łȂ���΂Ȃ�Ȃ��B
	//hr = device->CreateSamplerState(&samplerDesc, &sampler_state);
	//if (FAILED(hr))
	//{
	//	assert("Failed to CreateSamplerState");
	//}

	//�[�x�X�e���V���X�e�[�g�L�q
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	depth_stencil_desc.DepthEnable = FALSE;//�[�x�e�X�g���邩
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.StencilEnable = FALSE;//�X�e���V���e�X�g���邩���Ȃ���
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());
	if (FAILED(hr))
	{
		assert("Failed to CreateDepthStencilState");
	}

	//�u�����h�X�e�[�g
	//D3D11_BLEND_DESC blendDesc;
	//ZeroMemory(&blendDesc, sizeof(blendDesc));
	//blendDesc.AlphaToCoverageEnable = FALSE;//�}���`�T���v�����O�̎��Ƀ��l���g����
	//blendDesc.IndependentBlendEnable = FALSE; //FALSE�Ȃ�RenderTarget��[0]�݂̂�ݒ肷��
	//blendDesc.RenderTarget[0].BlendEnable = TRUE;//�u�����h���邩
	//blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;//�\�[�X��RGB�l
	//blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;//�v�Z���@
	//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;//�\�[�X�̃��l
	//blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;//�v�Z���@
	//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;//RenderTarget�ɂǂ̐F���������ނ�
	//hr = device->CreateBlendState(&blendDesc, &blend_state);
	//if (FAILED(hr))
	//{
	//	assert("Failed to CreateBlendState");
	//}
}

sprite::~sprite()
{
	if (tex != nullptr)
		delete tex;
}

void sprite::render(ID3D11DeviceContext* device,
	float dx, float dy, // dx, dy : Coordinate of sprite's left-top corner in screen space
	float dw, float dh, // dw, dh : Size of sprite in screen space
	float sx, float sy, // sx, sy : Coordinate of sprite's left-top corner in texture space
	float sw, float sh, // sw, sh : Size of sprite in texture space 
	float angle, // angle : Raotation angle (Rotation centre is sprite's centre), Unit is degree
	float r, float g, float b, float a // r, g, b, a : Color of sprite's each vertices 
	)
{
	HRESULT hr;

	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	device->RSGetViewports(&num_viewports, &viewport);
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	//��ʍ��W�Ɋe���_���Z�b�g����
	//���� 
	float x0 = dx;
	float y0 = dy;
	//�E��
	float x1 = dx + dw;
	float y1 = dy;
	//����
	float x2 = dx;
	float y2 = dy + dh;
	//�E��
	float x3 = dx + dw;
	float y3 = dy + dh;

	// �X�v���C�g�̒��S�����_�Ɉړ�������
	float mx = dx + dw*0.5f;
	float my = dy + dh*0.5f;
	x0 -= mx;
	y0 -= my;
	x1 -= mx;
	y1 -= my;
	x2 -= mx;
	y2 -= my;
	x3 -= mx;
	y3 -= my;

	//�e���_���p�x�ɉ����ĉ�]������
	float rx, ry;
	float cos = cosf(angle*0.01745f);
	float sin = sinf(angle*0.01745f);
	rx = x0;
	ry = y0;
	x0 = cos*rx + -sin*ry;
	y0 = sin*rx + cos*ry;
	rx = x1;
	ry = y1;
	x1 = cos*rx + -sin*ry;
	y1 = sin*rx + cos*ry;
	rx = x2;
	ry = y2;
	x2 = cos*rx + -sin*ry;
	y2 = sin*rx + cos*ry;
	rx = x3;
	ry = y3;
	x3 = cos*rx + -sin*ry;
	y3 = sin*rx + cos*ry;

	//�X�v���C�g�̒��S�����̍��W�ɖ߂�
	x0 += mx;
	y0 += my;
	x1 += mx;
	y1 += my;
	x2 += mx;
	y2 += my;
	x3 += mx;
	y3 += my;

	//NDC��Ԃɕϊ�����
	x0 = 2.0f*x0 / screen_width - 1.0f;
	y0 = 1.0f - 2.0f*y0 / screen_height;
	x1 = 2.0f*x1 / screen_width - 1.0f;
	y1 = 1.0f - 2.0f*y1 / screen_height;
	x2 = 2.0f*x2 / screen_width - 1.0f;
	y2 = 1.0f - 2.0f*y2 / screen_height;
	x3 = 2.0f*x3 / screen_width - 1.0f;
	y3 = 1.0f - 2.0f*y3 / screen_height;

	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = device->Map(vertex_buffer.Get(), 0, map, 0, &mapped_buffer);
	if (FAILED(hr))
	{
		assert("failed to Map");
	}

	vertex *vertices = static_cast<vertex*>(mapped_buffer.pData);
	vertices[0].position.x = x0;
	vertices[0].position.y = y0;
	vertices[1].position.x = x1;
	vertices[1].position.y = y1;
	vertices[2].position.x = x2;
	vertices[2].position.y = y2;
	vertices[3].position.x = x3;
	vertices[3].position.y = y3;
	vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = 0.0f;

	DirectX::XMFLOAT4 color(r, g, b, a);
	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;

	//vertices[0].texcoord.x = 0;
	//vertices[0].texcoord.y = 0;
	//vertices[1].texcoord.x = 1;
	//vertices[1].texcoord.y = 0;
	//vertices[2].texcoord.x = 0;
	//vertices[2].texcoord.y = 1;
	//vertices[3].texcoord.x = 1;
	//vertices[3].texcoord.y = 1;
	vertices[0].texcoord.x = static_cast<FLOAT>(sx) / texture2dDesc.Width;
	vertices[0].texcoord.y = static_cast<FLOAT>(sy) / texture2dDesc.Height;
	vertices[1].texcoord.x = static_cast<FLOAT>(sx + sw) / texture2dDesc.Width;
	vertices[1].texcoord.y = static_cast<FLOAT>(sy) / texture2dDesc.Height;
	vertices[2].texcoord.x = static_cast<FLOAT>(sx) / texture2dDesc.Width;
	vertices[2].texcoord.y = static_cast<FLOAT>(sy + sh) / texture2dDesc.Height;
	vertices[3].texcoord.x = static_cast<FLOAT>(sx + sw) / texture2dDesc.Width;
	vertices[3].texcoord.y = static_cast<FLOAT>(sy + sh) / texture2dDesc.Height;

	device->Unmap(vertex_buffer.Get(), 0);

	// ���̓A�Z���u���ɒ��_�o�b�t�@��ݒ�
	UINT stride = sizeof(vertex);//���_�f�[�^�̃T�C�Y
	UINT offset = 0;

	//IA(Input Assembler)!!
	//���_�o�b�t�@�ݒ�
	device->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);//(�X���b�g�ԍ�,�ݒ肷��o�b�t�@�̐�,�ݒ肷��o�b�t�@�̔z��,���_�̃T�C�Y,�I�t�Z�b�g)

	//�`�悷��v���~�e�B�u�̎�ނ��w�肷��
	device->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�C���v�b�g���C�A�E�g��ݒ�
	//���_���̑����
	device->IASetInputLayout(input_layout.Get());

	//RS(Rasterizer State)
	//���X�^���C�U�̐ݒ�
	device->RSSetState(rasterizer_state.Get());

	//���_�V�F�[�_�[�̃Z�b�g
	device->VSSetShader(vertexShader.Get(), NULL, 0);//(VS�ւ̃|�C���^,�N���X�C���X�^���X�C���^�[�t�F�C�X�̔z��ւ̃|�C���^(�s�v�Ȃ�null),���̃T�C�Y)

	//�s�N�Z���V�F�[�_�[�̃Z�b�g
	device->PSSetShader(pixelShader.Get(), NULL, 0);

	//�V�F�[�_�[���\�[�X�̐ݒ�
	//PixelShader�Ƀe�N�X�`��(deffuse_map)�𑗂�
	//device->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());//(resister(t0)�̔ԍ�,�z��̏ꍇ�����邩)

	device->PSSetShaderResources(0, 1, tex->GetShaderResourceViewPointer());//(resister(t0)�̔ԍ�,�z��̏ꍇ�����邩)

	//�T���v���[�̐ݒ�
	//PixelShader�ɃT���v���[(diffuse_map_sampler_state)�𑗂�
	device->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::POINT_BORDER));//(sampler(s0)�̔ԍ�,�z��̏ꍇ�����邩)

	//�[�x�X�e���V��(Z�o�b�t�@�ւ̏�������)
	device->OMSetDepthStencilState(depth_stencil_state.Get(), 1);

	//�u�����h�̐ݒ�
	//device->OMSetBlendState(blend_state, nullptr, 0xFFFFFFFF);

	//�`��
	device->Draw(4, 0);
}

void sprite::render(shader* shader, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float alpha, int tex_slot)
{
	shader->Activate();
	//���_�f�[�^�ݒ�
	VERTEX data[4];

	data[0].Pos.x = dx;
	data[0].Pos.y = dy;
	data[0].Pos.z = 0.0f;

	data[1].Pos.x = dx + dw;
	data[1].Pos.y = dy;
	data[1].Pos.z = 0.0f;

	data[2].Pos.x = dx;
	data[2].Pos.y = dy + dh;
	data[2].Pos.z = 0.0f;

	data[3].Pos.x = dx + dw;
	data[3].Pos.y = dy + dh;
	data[3].Pos.z = 0.0f;


	//���K���f�o�C�X���W�n
	for (int i = 0; i < 4; i++) {
		data[i].Pos.x = 2.0f * data[i].Pos.x / FRAMEWORK.SCREEN_WIDTH - 1.0f;
		data[i].Pos.y = 1.0f - 2.0f * data[i].Pos.y / FRAMEWORK.SCREEN_HEIGHT;
		data[i].Pos.z = 0.0f;
	}


	//�e�N�X�`�����W�ݒ�
	data[0].Tex.x = sx;
	data[0].Tex.y = sy;
	data[1].Tex.x = sx + sw;
	data[1].Tex.y = sy;
	data[2].Tex.x = sx;
	data[2].Tex.y = sy + sh;
	data[3].Tex.x = sx + sw;
	data[3].Tex.y = sy + sh;

	//UV���W
	for (int i = 0; i < 4; i++) {
		data[i].Tex.x = data[i].Tex.x / dw;
		data[i].Tex.y = data[i].Tex.y / dh;
	}
	//���_�J���[
	data[0].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	data[1].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	data[2].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	data[3].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	//�@��
	data[0].Normal = DirectX::XMFLOAT3(0, 0, 1);
	data[1].Normal = DirectX::XMFLOAT3(0, 0, 1);
	data[2].Normal = DirectX::XMFLOAT3(0, 0, 1);
	data[3].Normal = DirectX::XMFLOAT3(0, 0, 1);

	//���_�f�[�^�X�V
	FRAMEWORK.getImmediateContext()->UpdateSubresource(vertex_buffer.Get(), 0, NULL, data, 0, 0);

	//���_�o�b�t�@�̎w��
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	FRAMEWORK.getImmediateContext()->IASetVertexBuffers(
		0, 1, vertex_buffer.GetAddressOf(), // �X���b�g, ��, �o�b�t�@
		&stride,		// �P���_�̃T�C�Y
		&offset			// �J�n�ʒu
		);
	FRAMEWORK.getImmediateContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		);
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(depth_stencil_state.Get(), 1);

	//�萔�o�b�t�@
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(0, 1, &shader->ConstantBuffer);
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(0, 1, &shader->ConstantBuffer);
	FRAMEWORK.getImmediateContext()->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::POINT_BORDER));

	FRAMEWORK.getImmediateContext()->Draw(4, 0);
}

void sprite::render(shader* shader, Texture* tex, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float alpha, int tex_slot,int sampler_type )
{
	shader->Activate();

	//���_�f�[�^�ݒ�
	VERTEX data[4];

	data[0].Pos.x = dx;
	data[0].Pos.y = dy;
	data[0].Pos.z = 0.0f;

	data[1].Pos.x = dx + dw;
	data[1].Pos.y = dy;
	data[1].Pos.z = 0.0f;

	data[2].Pos.x = dx;
	data[2].Pos.y = dy + dh;
	data[2].Pos.z = 0.0f;

	data[3].Pos.x = dx + dw;
	data[3].Pos.y = dy + dh;
	data[3].Pos.z = 0.0f;


	//���K���f�o�C�X���W�n
	for (int i = 0; i < 4; i++) {
		data[i].Pos.x = 2.0f * data[i].Pos.x / FRAMEWORK.SCREEN_WIDTH - 1.0f;
		data[i].Pos.y = 1.0f - 2.0f * data[i].Pos.y / FRAMEWORK.SCREEN_HEIGHT;
		data[i].Pos.z = 0.0f;
	}


	//�e�N�X�`�����W�ݒ�
	data[0].Tex.x = sx;
	data[0].Tex.y = sy;
	data[1].Tex.x = sx + sw;
	data[1].Tex.y = sy;
	data[2].Tex.x = sx;
	data[2].Tex.y = sy + sh;
	data[3].Tex.x = sx + sw;
	data[3].Tex.y = sy + sh;

	//UV���W
	for (int i = 0; i < 4; i++) {
		data[i].Tex.x = data[i].Tex.x / tex->GetWidth();
		data[i].Tex.y = data[i].Tex.y / tex->GetHeight();
	}
	//���_�J���[
	data[0].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	data[1].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	data[2].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	data[3].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	//�@��
	data[0].Normal = DirectX::XMFLOAT3(0, 0, 1);
	data[1].Normal = DirectX::XMFLOAT3(0, 0, 1);
	data[2].Normal = DirectX::XMFLOAT3(0, 0, 1);
	data[3].Normal = DirectX::XMFLOAT3(0, 0, 1);

	//���_�f�[�^�X�V
	FRAMEWORK.getImmediateContext()->UpdateSubresource(vertex_buffer.Get(), 0, NULL, data, 0, 0);

	//���_�o�b�t�@�̎w��
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	FRAMEWORK.getImmediateContext()->IASetVertexBuffers(
		0, 1, vertex_buffer.GetAddressOf(), // �X���b�g, ��, �o�b�t�@
		&stride,		// �P���_�̃T�C�Y
		&offset			// �J�n�ʒu
		);
	FRAMEWORK.getImmediateContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		);
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(FRAMEWORK.getDephtStencilState(FRAMEWORK.DS::DS_FALSE), 1);

	//�e�N�X�`���̐ݒ�
	if (tex) tex->Set(tex_slot,0);
	if (tex) tex->Set(tex_slot);

	//�萔�o�b�t�@
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(0, 1, &shader->ConstantBuffer);
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(0, 1, &shader->ConstantBuffer);
	FRAMEWORK.getImmediateContext()->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(sampler_type));

	FRAMEWORK.getImmediateContext()->Draw(4, 0);

	//if (tex) tex->Set(tex_slot,0);
	shader->Deactivate();
}

void sprite::render(shader* shader, Texture_d* tex, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float alpha, int tex_slot, int sampler_type)
{
	shader->Activate();
	//���_�f�[�^�ݒ�
	VERTEX data[4];

	data[0].Pos.x = dx;
	data[0].Pos.y = dy;
	data[0].Pos.z = 0.0f;

	data[1].Pos.x = dx + dw;
	data[1].Pos.y = dy;
	data[1].Pos.z = 0.0f;

	data[2].Pos.x = dx;
	data[2].Pos.y = dy + dh;
	data[2].Pos.z = 0.0f;

	data[3].Pos.x = dx + dw;
	data[3].Pos.y = dy + dh;
	data[3].Pos.z = 0.0f;


	//���K���f�o�C�X���W�n
	for (int i = 0; i < 4; i++) {
		data[i].Pos.x = 2.0f * data[i].Pos.x / FRAMEWORK.SCREEN_WIDTH - 1.0f;
		data[i].Pos.y = 1.0f - 2.0f * data[i].Pos.y / FRAMEWORK.SCREEN_HEIGHT;
		data[i].Pos.z = 0.0f;
	}


	//�e�N�X�`�����W�ݒ�
	data[0].Tex.x = sx;
	data[0].Tex.y = sy;
	data[1].Tex.x = sx + sw;
	data[1].Tex.y = sy;
	data[2].Tex.x = sx;
	data[2].Tex.y = sy + sh;
	data[3].Tex.x = sx + sw;
	data[3].Tex.y = sy + sh;

	//UV���W
	for (int i = 0; i < 4; i++) {
		data[i].Tex.x = data[i].Tex.x / tex->GetWidth();
		data[i].Tex.y = data[i].Tex.y / tex->GetHeight();
	}
	//���_�J���[
	data[0].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	data[1].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	data[2].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	data[3].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	//�@��
	data[0].Normal = DirectX::XMFLOAT3(0, 0, 1);
	data[1].Normal = DirectX::XMFLOAT3(0, 0, 1);
	data[2].Normal = DirectX::XMFLOAT3(0, 0, 1);
	data[3].Normal = DirectX::XMFLOAT3(0, 0, 1);

	//���_�f�[�^�X�V
	FRAMEWORK.getImmediateContext()->UpdateSubresource(vertex_buffer.Get(), 0, NULL, data, 0, 0);

	//���_�o�b�t�@�̎w��
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	FRAMEWORK.getImmediateContext()->IASetVertexBuffers(
		0, 1, vertex_buffer.GetAddressOf(), // �X���b�g, ��, �o�b�t�@
		&stride,		// �P���_�̃T�C�Y
		&offset			// �J�n�ʒu
		);
	FRAMEWORK.getImmediateContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		);
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(depth_stencil_state.Get(), 1);

	//�e�N�X�`���̐ݒ�
	if (tex) tex->Set(tex_slot);

	//�萔�o�b�t�@
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(0, 1, &shader->ConstantBuffer);
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(0, 1, &shader->ConstantBuffer);
	//FRAMEWORK.getImmediateContext()->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(sampler_type));

	FRAMEWORK.getImmediateContext()->Draw(4, 0);
}

void sprite::render(shader* shader, ID3D11ShaderResourceView *srv, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float alpha, int tex_slot, int sampler_type)
{
	shader->Activate();

	//���_�f�[�^�ݒ�
	VERTEX data[4];

	data[0].Pos.x = dx;
	data[0].Pos.y = dy;
	data[0].Pos.z = 0.0f;

	data[1].Pos.x = dx + dw;
	data[1].Pos.y = dy;
	data[1].Pos.z = 0.0f;

	data[2].Pos.x = dx;
	data[2].Pos.y = dy + dh;
	data[2].Pos.z = 0.0f;

	data[3].Pos.x = dx + dw;
	data[3].Pos.y = dy + dh;
	data[3].Pos.z = 0.0f;


	//���K���f�o�C�X���W�n
	for (int i = 0; i < 4; i++) {
		data[i].Pos.x = 2.0f * data[i].Pos.x / FRAMEWORK.SCREEN_WIDTH - 1.0f;
		data[i].Pos.y = 1.0f - 2.0f * data[i].Pos.y / FRAMEWORK.SCREEN_HEIGHT;
		data[i].Pos.z = 0.0f;
	}


	//�e�N�X�`�����W�ݒ�
	data[0].Tex.x = sx;
	data[0].Tex.y = sy;
	data[1].Tex.x = sx + sw;
	data[1].Tex.y = sy;
	data[2].Tex.x = sx;
	data[2].Tex.y = sy + sh;
	data[3].Tex.x = sx + sw;
	data[3].Tex.y = sy + sh;

	//UV���W
	for (int i = 0; i < 4; i++) {
		data[i].Tex.x = data[i].Tex.x / 1280;
		data[i].Tex.y = data[i].Tex.y / 720;
	}
	//���_�J���[
	data[0].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	data[1].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	data[2].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	data[3].Color = DirectX::XMFLOAT4(1, 1, 1, alpha);
	//�@��
	data[0].Normal = DirectX::XMFLOAT3(0, 0, 1);
	data[1].Normal = DirectX::XMFLOAT3(0, 0, 1);
	data[2].Normal = DirectX::XMFLOAT3(0, 0, 1);
	data[3].Normal = DirectX::XMFLOAT3(0, 0, 1);

	//���_�f�[�^�X�V
	FRAMEWORK.getImmediateContext()->UpdateSubresource(vertex_buffer.Get(), 0, NULL, data, 0, 0);

	//���_�o�b�t�@�̎w��
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	FRAMEWORK.getImmediateContext()->IASetVertexBuffers(
		0, 1, vertex_buffer.GetAddressOf(), // �X���b�g, ��, �o�b�t�@
		&stride,		// �P���_�̃T�C�Y
		&offset			// �J�n�ʒu
		);
	FRAMEWORK.getImmediateContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		);
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(FRAMEWORK.getDephtStencilState(FRAMEWORK.DS::DS_FALSE), 1);

	//�e�N�X�`���̐ݒ�
	if (srv)
	{
		ID3D11ShaderResourceView* srv[1] = { nullptr };
		ID3D11SamplerState*ss[1] = { nullptr };
		FRAMEWORK.getImmediateContext()->PSSetShaderResources(tex_slot, 1, srv);
		FRAMEWORK.getImmediateContext()->PSSetSamplers(tex_slot, 1, ss);
	}
	if (srv)
	{
		FRAMEWORK.getImmediateContext()->PSSetShaderResources(tex_slot, 1, &srv);//load�֐��Ȃ����rtv�̓��e�����f�����
		FRAMEWORK.getImmediateContext()->PSSetSamplers(tex_slot, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::LINEAR_BORDER));
	}

	//�萔�o�b�t�@
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(0, 1, &shader->ConstantBuffer);
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(0, 1, &shader->ConstantBuffer);
	FRAMEWORK.getImmediateContext()->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(sampler_type));

	FRAMEWORK.getImmediateContext()->Draw(4, 0);

	//if (tex) tex->Set(tex_slot,0);
	shader->Deactivate();
}

void sprite::TextOut(ID3D11DeviceContext *immediateContext, std::string s, float x, float y, float w, float h, float r, float g, float b, float a) /*const*/
{
	float sw = static_cast<float>(texture2dDesc.Width / 16);
	float sh = static_cast<float>(texture2dDesc.Height / 16);
	float cursor = 0;
	for (auto c : s)
	{
		LONG sx = c % 0x0F;
		render(immediateContext, x + cursor, y, w, h, sw*(c & 0x0F), sh*(c >> 4), sw, sh, 0, r, g, b, a);
		cursor += w;
	}
}