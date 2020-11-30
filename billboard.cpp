#include "billboard.h"
#include "load_file.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include "framework.h"


billboard::billboard(const wchar_t* file_name)
{
	HRESULT hr = S_OK;

	//�C���v�b�g���C�A�E�g//���_�̍\�����K�肷��//�e�Z�}���e�B�N�X�̋K��
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	vs_Load(FRAMEWORK.getInstance().getDevice(), "billboard_vs.cso", vertexShader.GetAddressOf(), input_layout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	ps_Load(FRAMEWORK.getInstance().getDevice(), "billboard_ps.cso", pixelShader.GetAddressOf());

	//���X�^���C�U�ݒ�
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;//���߂�
	rasterizerDesc.CullMode = D3D11_CULL_BACK;//�o�b�N�J�����O
	rasterizerDesc.FrontCounterClockwise = FALSE;//���_�̕\�����v���
	rasterizerDesc.DepthBias = 0;//
	rasterizerDesc.DepthBiasClamp = 0;//
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	hr = FRAMEWORK.getInstance().getDevice()->CreateRasterizerState(&rasterizerDesc, rasterizer_state.GetAddressOf());
	if (FAILED(hr))
	{
		assert("Failed to CreateRasterizerState");
	}

	hr = load_texture_from_file(FRAMEWORK.getInstance().getDevice(), file_name, shader_resource_view.GetAddressOf(), &texture2dDesc);
	if (FAILED(hr))
	{
		assert("Failed to CreateTextureFromFile");
	}

	//�T���v���[�X�e�[�g�ɕK�v
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //�e�N�X�`���T���v�����O�Ŏg���t�B���^
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;//0~1�͈̔͊O�ɂ���U�e�N�X�`�����W�̕`����@
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;//0~1�͈̔͊O�ɂ���V�e�N�X�`�����W�̕`����@
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;//0~1�͈̔͊O�ɂ���W�e�N�X�`�����W�̕`����@
	samplerDesc.MipLODBias = 0;//LoD�̃o�C�A�X�BLoD�ɂ���𑫂����l���ŏI�I��LoD�̃��x��(Level of Detail)
	samplerDesc.MaxAnisotropy = 16;//�T���v�����O�Ɉٕ����⊮���̗p���Ă���ꍇ�̍ő�l
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;//�T���v���Ƃ̔�r�I�v�V����
	memcpy(samplerDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	samplerDesc.MinLOD = 0;//LoD�̉����B0���ő�ōł�����
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;//LoD�̏���BMaxLoD>=MinLoD�łȂ���΂Ȃ�Ȃ��B
	hr = FRAMEWORK.getInstance().getDevice()->CreateSamplerState(&samplerDesc, sampler_state.GetAddressOf());
	if (FAILED(hr))
	{
		assert("Failed to CreateSamplerState");
	}

	//�[�x�X�e���V���X�e�[�g�L�q
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	depth_stencil_desc.DepthEnable = TRUE;//�[�x�e�X�g���邩
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
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
	blend = new blender(FRAMEWORK.getInstance().getDevice());

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
	//hr = FRAMEWORK.getInstance().getDevice()->CreateBlendState(&blendDesc, &blend_state);
	//if (FAILED(hr))
	//{
	//	assert("Failed to CreateBlendState");
	//}

	//���_���̃Z�b�g
	vertex vertices[4] = {};
	unsigned int indices[3 * 2] = {};

	vertices[0].pos = DirectX::XMFLOAT3(-5.0f, +5.0f, .0f);
	vertices[1].pos = DirectX::XMFLOAT3(+5.0f, +5.0f, .0f);
	vertices[2].pos = DirectX::XMFLOAT3(-5.0f, -5.0f, .0f);
	vertices[3].pos = DirectX::XMFLOAT3(+5.0f, -5.0f, .0f);
	vertices[0].normal = vertices[1].normal =
		vertices[2].normal =
		vertices[3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
	vertices[0].texcoord.x = 0;
	vertices[0].texcoord.y = 0;
	vertices[1].texcoord.x = 1;
	vertices[1].texcoord.y = 0;
	vertices[2].texcoord.x = 0;
	vertices[2].texcoord.y = 1;
	vertices[3].texcoord.x = 1;
	vertices[3].texcoord.y = 1;
	indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
	indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

	create_buffer(FRAMEWORK.getInstance().getDevice(), vertices, ARRAYSIZE(vertices), indices, ARRAYSIZE(indices));
}

billboard::~billboard()
{
	delete blend;
	blend = nullptr;
}


//void billboard::update(Camera* m_camera)
//{
//	//�r���[�s��̉�]��ł������s��̎擾
//	DirectX::XMMATRIX inv_view = getInvMatrixForBillBoard(m_camera);
//
//	//�r���[�v���W�F�N�V�����s��쐬
//	DirectX::XMMATRIX view_projection = DirectX::XMLoadFloat4x4(&m_camera->GetView())*DirectX::XMLoadFloat4x4(&m_camera->GetProjection());
//
//	//���[���h�ϊ��s��쐬
//	DirectX::XMMATRIX S, T, R;
//	//�g��k���s��
//	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
//	//�ʒu�s��
//	T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
//	//��]�s��
//	//R = DirectX::XMMatrixRotationRollPitchYaw(angle.x*(DirectX::XM_PI / 180.0f), angle.y*(DirectX::XM_PI / 180.0f), angle.z*(DirectX::XM_PI / 180.0f));
//	//����]
//	DirectX::XMVECTOR focus_vec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&m_camera->GetFocus()), DirectX::XMLoadFloat3(&m_camera->GetEye()));
//	R = DirectX::XMMatrixRotationAxis(DirectX::XMVector3Normalize(focus_vec), angle.z * (DirectX::XM_PI / 180.0f));
//
//	//���[���h�s��֍���
//	DirectX::XMMATRIX world = S*R*T;
//	//���[���h�s��4x4�Ŋi�[�p
//	DirectX::XMFLOAT4X4 world4x4;
//	//���[���h�s���4x4�ł֕ϊ�
//	DirectX::XMStoreFloat4x4(&world4x4, world);
//
//	//���[���h�r���[�v���W�F�N�V����
//	DirectX::XMMATRIX customWVP = inv_view*world*view_projection;
//	DirectX::XMFLOAT4X4 customWVP4x4;
//	DirectX::XMStoreFloat4x4(&customWVP4x4, customWVP);
//
//	this->world = world4x4;
//	this->wvp = customWVP4x4;
//}

////�r���[�s��̉�]�݂̂�ł������t�s�񐶐�
//DirectX::XMMATRIX billboard::getInvMatrixForBillBoard(Camera* m_camera)
//{
//	DirectX::XMFLOAT4X4 	view;
//	DirectX::XMMATRIX		inv_view;
//	view = m_camera->GetView();
//	//��view�s���R*T������T��0�ɂ���R�̋t�s��ɂ���Ή�]���E�ł���
//	view._41 = 0.0f; view._42 = 0.0f;//�ʒu��񂾂����폜
//	view._43 = 0.0f; view._44 = 1.0f;//	
//	inv_view = DirectX::XMLoadFloat4x4(&view);//	Matrix�^�֍ĕϊ�
//	inv_view = DirectX::XMMatrixInverse(nullptr, inv_view);//	view�s��̋t�s��쐬
//
//	return inv_view;
//}

//void billboard::move()
//{
//	//static DirectX::XMFLOAT3 position(0, 0, 10);
//	if (GetAsyncKeyState(VK_SHIFT) < 0)
//	{
//		if (GetAsyncKeyState(VK_UP) < 0)
//			position.z += 3;
//		if (GetAsyncKeyState(VK_DOWN) < 0)
//			position.z -= 3;
//		if (GetAsyncKeyState(VK_LEFT) < 0)
//			position.x -= 3;
//		if (GetAsyncKeyState(VK_RIGHT) < 0)
//			position.x += 3;
//	}
//
//	//DirectX::XMFLOAT3 dimensions(1, 1, 1);
//
//	//static DirectX::XMFLOAT3 angles(0, 0, 0);
//	if (GetAsyncKeyState(VK_CONTROL) < 0)
//	{
//		if (GetAsyncKeyState(VK_UP) < 0)
//			angle.x++;
//		if (GetAsyncKeyState(VK_DOWN) < 0)
//			angle.x--;
//		if (GetAsyncKeyState(VK_LEFT) < 0)
//			angle.y++;
//		if (GetAsyncKeyState(VK_RIGHT) < 0)
//			angle.y--;
//		//if (GetAsyncKeyState('UP') & 1)
//		angle.z = 0;
//	}
//}

void billboard::render(ID3D11DeviceContext* context, DirectX::XMFLOAT4X4 world, Camera *camera, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color
	)
{
	//�萔�o�b�t�@�̍쐬
	DirectX::XMMATRIX viewM, projectionM, worldM;
	viewM = DirectX::XMLoadFloat4x4(&camera->GetView());
	projectionM = DirectX::XMLoadFloat4x4(&camera->GetProjection());
	worldM = DirectX::XMLoadFloat4x4(&world);

	cbuffer cb;
	DirectX::XMStoreFloat4x4(&cb.wvp, worldM*viewM*projectionM);
	cb.world = world;
	cb.light_direction = light_direction;
	cb.material_color = material_color;
	context->UpdateSubresource(cs_buffer.Get(), 0, 0, &cb, 0, 0);
	context->VSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());

	// ���_�o�b�t�@�̃o�C���h
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, /*&vertex_buffe*/vertex_buffer.GetAddressOf(), &stride, &offset);

	//	�C���f�b�N�X�o�b�t�@�̃o�C���h
	context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//	�v���~�e�B�u���[�h�̐ݒ�
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	���̓��C�A�E�g�̃o�C���h
	context->IASetInputLayout(input_layout.Get());

	//	�V�F�[�_�[(2��)�̐ݒ�
	context->VSSetShader(vertexShader.Get(), nullptr, 0);
	context->PSSetShader(pixelShader.Get(), nullptr, 0);

	//�V�F�[�_�[���\�[�X�̐ݒ�
	//PixelShader�Ƀe�N�X�`��(deffuse_map)�𑗂�
	context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());//(resister(t0)�̔ԍ�,�z��̏ꍇ�����邩)

																			 //�T���v���[�̐ݒ�
																			 //PixelShader�ɃT���v���[(diffuse_map_sampler_state)�𑗂�
	context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());//(sampler(s0)�̔ԍ�,�z��̏ꍇ�����邩)

															   //�R���X�^���g�o�b�t�@�[�ݒ�
	context->VSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());
	context->PSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());

	//	�[�x�e�X�g�̐ݒ�
	context->OMSetDepthStencilState(depth_stencil_state.Get(), 1);

	//�u�����h�X�e�[�g
	context->OMSetBlendState(blend->states[blender::BS_ADD].Get(), nullptr, 0xFFFFFFFF);

	//	���X�^���C�U�[�̐ݒ�
	context->RSSetState(rasterizer_state.Get());

	//	�v���~�e�B�u�̕`��(index�t��)
	context->DrawIndexed(numIndices, 0, 0);
}

void billboard::create_buffer(ID3D11Device *pDevice, vertex *vertices, u_int num_vertices, u_int *indices, u_int num_indices)
{
	HRESULT hr;
	//���_�o�b�t�@�̍쐬
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(vertex)*num_vertices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.

		hr = pDevice->CreateBuffer(&buffer_desc, &subresource_data, /*&vertex_buffe*/vertex_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//�C���f�b�N�X�o�b�t�@
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		numIndices = num_indices;

		buffer_desc.ByteWidth = sizeof(u_int) * num_indices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = indices;
		subresource_data.SysMemPitch = 0; //Not use for index buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
		hr = pDevice->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
		if (FAILED(hr)) {
			return;
		}
	}

	//�萔�o�b�t�@
	{
		D3D11_BUFFER_DESC buffer_desc;
		ZeroMemory(&buffer_desc, sizeof(buffer_desc));

		buffer_desc.ByteWidth = sizeof(cbuffer);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		hr = pDevice->CreateBuffer(&buffer_desc, nullptr, &cs_buffer);
		if (FAILED(hr)) {
			return;
		}
	}
}

billboard_obj::billboard_obj(std::shared_ptr<billboard> billboard)
{
	bb = billboard;
	setPosition(DirectX::XMFLOAT3(.0f, .0f, .0f));
	setAngle(DirectX::XMFLOAT3(.0f, .0f, .0f));
	setScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
}

void billboard_obj::update(Camera * m_camera)
{
	//�r���[�s��̉�]��ł������s��̎擾
	DirectX::XMMATRIX inv_view = getInvMatrixForBillBoard(m_camera);

	//�r���[�v���W�F�N�V�����s��쐬
	DirectX::XMMATRIX view_projection = DirectX::XMLoadFloat4x4(&m_camera->GetView())*DirectX::XMLoadFloat4x4(&m_camera->GetProjection());

	//���[���h�ϊ��s��쐬
	DirectX::XMMATRIX S, T, R;
	//�g��k���s��
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//�ʒu�s��
	T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//��]�s��
	//R = DirectX::XMMatrixRotationRollPitchYaw(angle.x*(DirectX::XM_PI / 180.0f), angle.y*(DirectX::XM_PI / 180.0f), angle.z*(DirectX::XM_PI / 180.0f));
	//����]
	DirectX::XMVECTOR focus_vec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&m_camera->GetFocus()), DirectX::XMLoadFloat3(&m_camera->GetEye()));
	R = DirectX::XMMatrixRotationAxis(DirectX::XMVector3Normalize(focus_vec), angle.z * (DirectX::XM_PI / 180.0f));

	//���[���h�s��֍���
	DirectX::XMMATRIX world = S*R*T;
	//���[���h�s��4x4�Ŋi�[�p
	DirectX::XMFLOAT4X4 world4x4;
	//���[���h�s���4x4�ł֕ϊ�
	DirectX::XMStoreFloat4x4(&world4x4, world);

	//���[���h�r���[�v���W�F�N�V����
	DirectX::XMMATRIX customWVP = inv_view*world*view_projection;
	DirectX::XMFLOAT4X4 customWVP4x4;
	DirectX::XMStoreFloat4x4(&customWVP4x4, customWVP);

	this->world = world4x4;
	this->wvp = customWVP4x4;
}

void billboard_obj::render(ID3D11DeviceContext * context, Camera *camera, const DirectX::XMFLOAT4 & light_direction, const DirectX::XMFLOAT4 & material_color)
{
	bb->render(context, world, camera, light_direction, material_color);
}

DirectX::XMMATRIX billboard_obj::getInvMatrixForBillBoard(Camera* m_camera)
{
	DirectX::XMFLOAT4X4 	view;
	DirectX::XMMATRIX		inv_view;
	view = m_camera->GetView();
	//��view�s���R*T������T��0�ɂ���R�̋t�s��ɂ���Ή�]���E�ł���
	view._41 = 0.0f; view._42 = 0.0f;//�ʒu��񂾂����폜
	view._43 = 0.0f; view._44 = 1.0f;//	
	inv_view = DirectX::XMLoadFloat4x4(&view);//	Matrix�^�֍ĕϊ�
	inv_view = DirectX::XMMatrixInverse(nullptr, inv_view);//	view�s��̋t�s��쐬

	return inv_view;
}

void billboard_obj::move()
{
	//static DirectX::XMFLOAT3 position(0, 0, 10);
	if (GetAsyncKeyState(VK_SHIFT) < 0)
	{
		if (GetAsyncKeyState(VK_UP) < 0)
			position.z += 3;
		if (GetAsyncKeyState(VK_DOWN) < 0)
			position.z -= 3;
		if (GetAsyncKeyState(VK_LEFT) < 0)
			position.x -= 3;
		if (GetAsyncKeyState(VK_RIGHT) < 0)
			position.x += 3;
	}

	//DirectX::XMFLOAT3 dimensions(1, 1, 1);

	//static DirectX::XMFLOAT3 angles(0, 0, 0);
	if (GetAsyncKeyState(VK_CONTROL) < 0)
	{
		if (GetAsyncKeyState(VK_UP) < 0)
			angle.x++;
		if (GetAsyncKeyState(VK_DOWN) < 0)
			angle.x--;
		if (GetAsyncKeyState(VK_LEFT) < 0)
			angle.y++;
		if (GetAsyncKeyState(VK_RIGHT) < 0)
			angle.y--;
		//if (GetAsyncKeyState('UP') & 1)
		angle.z = 0;
	}
}