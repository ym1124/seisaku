#pragma once

#include "gpu_particle.h"
#include "framework.h"
#include "mesh.h"
#include "static_object.h"

gpu_particle::gpu_particle()//���[�N
{
	_shader = std::make_unique<shader>();
	//�萔�o�b�t�@
	{
		HRESULT hr;
		D3D11_BUFFER_DESC cBufferDesc = {};
		cBufferDesc.ByteWidth = sizeof(cbuffer);
		cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cBufferDesc.CPUAccessFlags = 0;
		cBufferDesc.MiscFlags = 0;
		cBufferDesc.StructureByteStride = 0;
		hr = FRAMEWORK.getInstance().getDevice()->CreateBuffer(&cBufferDesc, nullptr, &cs_buffer);
		if (FAILED(hr))
			return;
	}
}

gpu_particle::~gpu_particle()
{
	release();
}

void gpu_particle::setInitData(int pattern, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 size, float range,float timer)
{
	pd = new particle_data[NUM_ELEMENTS];//�z�񐶐�
	ZeroMemory(pd, sizeof(particle_data)*NUM_ELEMENTS);

	init_pos = pos;
	type = pattern;
	this->timer = timer;

	switch (pattern)
	{
	case gpu_particle::PATTERN::RADIAL:
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			float rng_x = (rand() % 1000 / 1000.0f - 0.5f) / 10.0f;
			float rng_y = (rand() % 1000 / 1000.0f - 0.5f) / 10.0f;
			float rng_z = (rand() % 1000 / 1000.0f - 0.5f) / 10.0f;
			float rng_t = (rand() % 100 / 100.0f) * timer;

			pd[i].pos = init_pos;
			pd[i].velocity = DirectX::XMFLOAT3(rng_x, rng_y, rng_z);
			pd[i].size = size;
			pd[i].timer = rng_t;
		}
		break;
	case gpu_particle::PATTERN::SNOW:
		this->range = range;
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			//float rng_x = (rand() % 1000 / 1000.0f - 0.5f) / 10.0f;
			//float rng_y = (rand() % 1000 / 1000.0f);
			//float rng_z = (rand() % 1000 / 1000.0f - 0.5f) / 10.0f;
			//float rng_t = (rand() % 100 / 100.0f) * 100.0f;
			//float r;
			//if (rng_x > 0)
			//	r = range / 2.0f;
			//else
			//	r = -range / 2.0f;
			//pd[i].pos = DirectX::XMFLOAT3(init_pos.x + r, init_pos.y, init_pos.z + r);
			//pd[i].velocity = DirectX::XMFLOAT3(rng_x, -rng_y, rng_z);
			//pd[i].size = size;
			//pd[i].timer = rng_t;

			int rng, rng2;
			float r, r2;
			float rng_x = (rand() % 1000 / 1000.0f - 0.5f) / 10.0f;
			float rng_y = (rand() % 1000 / 1000.0f);
			float rng_z = (rand() % 1000 / 1000.0f - 0.5f) / 10.0f;
			rng = rand() % 2;
			rng2 = rand() % 2;
			if (!rng)
				rng = -1;
			if (!rng2)
				rng2 = -1;
			r = (rand() % (int)range) / range;
			r2 = (rand() % (int)range) / range;
			pd[i].pos.x = init_pos.x + r*range*rng;
			pd[i].pos.y = init_pos.y;
			pd[i].pos.z = init_pos.z + r2*range*rng2;
			pd[i].size = size;
			pd[i].velocity = DirectX::XMFLOAT3(rng_x, -rng_y, rng_z);
			pd[i].timer = rand() % (int)timer;
		}
		break;
	case gpu_particle::PATTERN::TORNADO:
		this->range = range;
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			float rng_t = (rand() % 100 / 100.0f);
			pd[i].timer = rng_t* timer;
			pd[i].pos = DirectX::XMFLOAT3(init_pos.x + cosf(pd[i].timer)*range, init_pos.y, init_pos.z + sinf(pd[i].timer)*range);
			float angle = ((rng_t * 360.0f) + 90.0f)*(DirectX::XM_PI / 180.0f);
			pd[i].velocity = DirectX::XMFLOAT3(cosf(angle) / 10.0f, 0.01f, sinf(angle) / 10.0f);
			pd[i].size = size;
		}
		break;
	case gpu_particle::PATTERN::FIREWORK:
		for (int i = 0; i < NUM_ELEMENTS; i++)
		{
			float rng_x = (rand() % 1000 / 1000.0f - 0.5f);
			float rng_y = (rand() % 1000 / 1000.0f - 0.5f);
			float rng_z = (rand() % 1000 / 1000.0f - 0.5f);
			//�X�s�[�h�𐳋K�����邱�Ƃŋ���ɂ���
			float speed_length = sqrtf(rng_x*rng_x + rng_y*rng_y + rng_z*rng_z);

			float rng_t = (rand() % 100 / 100.0f) * 100.0f;

			pd[i].pos = init_pos;
			pd[i].velocity = DirectX::XMFLOAT3(rng_x / speed_length / 10.0f, rng_y / speed_length / 10.0f, rng_z / speed_length / 10.0f);
			pd[i].size = size;
			pd[i].timer = rng_t;
		}
		break;
	}


	//����
	//�ʒu�@�@���@�T�C�Y�@�F
	//VERTEX v = { DirectX::XMFLOAT3(0.0f, -20.0f, 0.0f), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(1,1), DirectX::XMFLOAT4(1,1,1,0), };

	//	���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX)* NUM_ELEMENTS; //�꒸�_���̃o�b�t�@
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = FRAMEWORK.getDevice()->CreateBuffer(&bd, NULL, &vertex_buffer);
	assert(SUCCEEDED(hr));

	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBufferForPerFrame);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&cbd, nullptr, &constant_buffer);
	assert(SUCCEEDED(hr));

}

void gpu_particle::setInitData(my_mesh *model, DirectX::XMFLOAT2 size, float range)
{
	pd = new particle_data[NUM_ELEMENTS];//�z�񐶐�
	ZeroMemory(pd, sizeof(particle_data)*NUM_ELEMENTS);

	type = gpu_particle::PATTERN::MODEL;
	const float TIME_MAX = 100.0f;


	for (int i = 0; i < NUM_ELEMENTS; i++)
	{
		float rng_x = (rand() % 1000 / 1000.0f - 0.5f) / 10.0f;
		float rng_y = (rand() % 1000 / 1000.0f - 0.5f) / 10.0f;
		float rng_z = (rand() % 1000 / 1000.0f - 0.5f) / 10.0f;
		float rng_t = (rand() % 100 / 100.0f) * 100.0f;

		pd[i].pos = model->_model->getModelResource()->getModelData()->meshes.at(i%model->_model->getModelResource()->getModelData()->meshes.size()).vertices.at(i%model->_model->getModelResource()->getModelData()->meshes.at(i%model->_model->getModelResource()->getModelData()->meshes.size()).vertices.size()).position;
		pd[i].velocity = DirectX::XMFLOAT3(rng_x, rng_y, rng_z);
		pd[i].size = size;
		pd[i].timer = rng_t;
	}

	mdl = model;

	//����
	//�ʒu�@�@���@�T�C�Y�@�F
	//VERTEX v = { DirectX::XMFLOAT3(0.0f, -20.0f, 0.0f), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(1,1), DirectX::XMFLOAT4(1,1,1,0), };

	//	���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX)* NUM_ELEMENTS; //�꒸�_���̃o�b�t�@
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = FRAMEWORK.getDevice()->CreateBuffer(&bd, NULL, &vertex_buffer);
	assert(SUCCEEDED(hr));

	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBufferForPerFrame);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&cbd, nullptr, &constant_buffer);
	assert(SUCCEEDED(hr));
}

void gpu_particle::setInitData(static_object *model, DirectX::XMFLOAT2 size, float range)
{
	pd = new particle_data[NUM_ELEMENTS];//�z�񐶐�
	ZeroMemory(pd, sizeof(particle_data)*NUM_ELEMENTS);

	type = gpu_particle::PATTERN::STATIC_MODEL;
	const float TIME_MAX = 100.0f;


	for (int i = 0; i < NUM_ELEMENTS; i++)
	{
		float rng_x = (rand() % 1000 / 1000.0f - 0.5f) / 10.0f;
		float rng_y = (rand() % 1000 / 1000.0f - 0.5f) / 10.0f;
		float rng_z = (rand() % 1000 / 1000.0f - 0.5f) / 10.0f;
		float rng_t = (rand() % 100 / 100.0f) * 100.0f;
		pd[i].pos = model->getMesh()->vertices.at(i%model->getMesh()->vertices.size()).position;
		pd[i].velocity = DirectX::XMFLOAT3(rng_x, rng_y, rng_z);
		pd[i].size = size;
		pd[i].timer = rng_t;
	}

	s_obj = model;

	HRESULT hr;
	//	���_�o�b�t�@�쐬
	if (vertex_buffer == nullptr)
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX)* NUM_ELEMENTS; //�꒸�_���̃o�b�t�@
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		hr = FRAMEWORK.getDevice()->CreateBuffer(&bd, NULL, &vertex_buffer);
		assert(SUCCEEDED(hr));
	}

	// �萔�o�b�t�@����
	if (constant_buffer == nullptr)
	{
		D3D11_BUFFER_DESC cbd;
		ZeroMemory(&cbd, sizeof(cbd));
		cbd.Usage = D3D11_USAGE_DEFAULT;
		cbd.ByteWidth = sizeof(ConstantBufferForPerFrame);
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = 0;

		hr = FRAMEWORK.getDevice()->CreateBuffer(&cbd, nullptr, &constant_buffer);
		assert(SUCCEEDED(hr));
	}
}

void gpu_particle::resetInitPosition(DirectX::XMFLOAT3 pos)
{
	init_pos = pos;
}

DirectX::XMFLOAT3 gpu_particle::resetInitPosition(my_mesh *model)
{
	int i = rand() % NUM_ELEMENTS;
	model->_model->getModelResource()->getModelData()->meshes.at(i%model->_model->getModelResource()->getModelData()->meshes.size()).vertices.at(i%model->_model->getModelResource()->getModelData()->meshes.at(i%model->_model->getModelResource()->getModelData()->meshes.size()).vertices.size()).position;
	return DirectX::XMFLOAT3(1, 1, 1);
}

//�摜�L���
void gpu_particle::create(wchar_t* srcFile, char* functionName, wchar_t* texture_filename)
{
	_shader->Create(L"create_particle.fx", "VSMain", "PSMain", "GSMain");
	_shader->CreateCS(srcFile, functionName);

	tex = std::make_shared<Texture>();
	tex->Load(texture_filename);

	//���X�^���C�U�[�X�e�[�g�I�u�W�F�N�g(���C���[�t���[��)
	D3D11_RASTERIZER_DESC rDesc;
	ZeroMemory(&rDesc, sizeof(rDesc));
	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.CullMode = D3D11_CULL_NONE;
	rDesc.FrontCounterClockwise = FALSE;
	rDesc.DepthClipEnable = TRUE;//����H
	HRESULT hr = FRAMEWORK.getInstance().getDevice()->CreateRasterizerState(&rDesc, &rasterizer_states);
	if (FAILED(hr)) {
		return;
	}

	//�[�x�X�e���V���X�e�[�g�I�u�W�F�N�g
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = FRAMEWORK.getInstance().getDevice()->CreateDepthStencilState(&dsDesc, &depth_stencil_state);
	if (FAILED(hr)) {
		return;
	}
}

//�摜������
void gpu_particle::create(wchar_t* srcFile, char* functionName)
{
	_shader->Create(L"Shader/create_particle_noimage.fx", "VSMain", "PSMain", "GSMain");
	_shader->CreateCS(srcFile, functionName);

	//���X�^���C�U�[�X�e�[�g�I�u�W�F�N�g(���C���[�t���[��)
	D3D11_RASTERIZER_DESC rDesc;
	ZeroMemory(&rDesc, sizeof(rDesc));
	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.CullMode = D3D11_CULL_NONE;
	rDesc.FrontCounterClockwise = FALSE;
	rDesc.DepthClipEnable = TRUE;//����H
	HRESULT hr = FRAMEWORK.getInstance().getDevice()->CreateRasterizerState(&rDesc, &rasterizer_states);
	if (FAILED(hr)) {
		return;
	}

	//�[�x�X�e���V���X�e�[�g�I�u�W�F�N�g
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = FRAMEWORK.getInstance().getDevice()->CreateDepthStencilState(&dsDesc, &depth_stencil_state);
	if (FAILED(hr)) {
		return;
	}
}

void gpu_particle::update()
{
	if (type == MODEL)
	{
		for (int i = 0; i < NUM_ELEMENTS; ++i)
		{
			DirectX::XMVECTOR v_pos = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(pd[i].pos.x, pd[i].pos.y, pd[i].pos.z, 1.0f));
			DirectX::XMMATRIX wm = DirectX::XMLoadFloat4x4(&mdl->getWorldMatrix());
			DirectX::XMVECTOR res = DirectX::XMVector4Transform(v_pos, wm);
			DirectX::XMFLOAT4 tmp;
			DirectX::XMStoreFloat4(&tmp, res);
			pd[i].pos = DirectX::XMFLOAT3(tmp.x, tmp.y, tmp.z);
		}
	}

	else if(type == STATIC_MODEL)
	{
		for (int i = 0; i < NUM_ELEMENTS; ++i)
		{
			DirectX::XMVECTOR v_pos = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(pd[i].pos.x, pd[i].pos.y, pd[i].pos.z, 1.0f));
			DirectX::XMMATRIX wm = DirectX::XMLoadFloat4x4(&s_obj->getWorld());
			DirectX::XMVECTOR res = DirectX::XMVector4Transform(v_pos, wm);
			DirectX::XMFLOAT4 tmp;
			DirectX::XMStoreFloat4(&tmp, res);
			pd[i].pos = DirectX::XMFLOAT3(tmp.x, tmp.y, tmp.z);
		}
	}


	// �\�����o�b�t�@�𐶐�//�f�[�^�̓��͂Əo�͂Ɏg��
	createStructureBuffers();

	//�\�����o�b�t�@����o�^���邽�߂�SRV��UAV�ɕϊ�
	createSRVBuffers();
	createUAVBuffers();

	ID3D11ShaderResourceView* pSRVs[1] = { srv };
	runComputeShader(1, pSRVs, nullptr, nullptr, 0, getUAVBuffer(), /*NUM_ELEMENTS/*/100, 1, 1);//�X���b�h�̍��v��Dispatch*numthreads
	pSRVs[0]->Release();

	// �o�b�t�@�𐶐��ƃR�s�[.
	ID3D11Buffer* pBufDbg = createAndCopyToBuffer(FRAMEWORK.getDevice(), FRAMEWORK.getImmediateContext(), getResultBuffer());

	D3D11_MAPPED_SUBRESOURCE subRes;
	//BufType* pBufType;
	//particle_data *temp;

	//if (!temp)
	//	temp = new particle_data();

	// �}�b�v
	HRESULT hr = FRAMEWORK.getImmediateContext()->Map(pBufDbg, 0, D3D11_MAP_READ, 0, &subRes);
	assert(SUCCEEDED(hr));
	temp = (particle_data*)subRes.pData;
	// �A���}�b�v
	FRAMEWORK.getImmediateContext()->Unmap(pBufDbg, 0);

	// �������
	pBufDbg->Release();


	//�f�[�^�X�V
	for (int i = 0; i < NUM_ELEMENTS; ++i)
	{
		pd[i].pos = temp[i].pos;
		pd[i].timer = temp[i].timer;
		pd[i].normal = temp[i].normal;
	}

	//���
	if (buffer)
	{
		buffer->Release();
		buffer = NULL;
	}
	if (result_buffer)
	{
		result_buffer->Release();
		result_buffer = NULL;
	}
	if (uav_buffer)
	{
		uav_buffer->Release();
		uav_buffer = NULL;
	}
}

void gpu_particle::render(DirectX::XMFLOAT3 color)
{
	//�V�F�[�_�L����
	_shader->Activate();

	//nullset
	ID3D11ShaderResourceView* dummySRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	FRAMEWORK.getImmediateContext()->PSSetShaderResources(0, _countof(dummySRVs), dummySRVs);
	FRAMEWORK.getImmediateContext()->DSSetShaderResources(0, _countof(dummySRVs), dummySRVs);

	if (tex != nullptr)
		FRAMEWORK.getImmediateContext()->PSSetShaderResources(0, 1, tex->GetShaderResourceViewPointer());
	//FRAMEWORK.getImmediateContext()->PSSetSamplers(0, 1, tex->getSamplerState());

	//�u�����h�X�e�[�g�ݒ�
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0xFFFFFFFF);

	//���X�^���C�U�\�ݒ�
	FRAMEWORK.getImmediateContext()->RSSetState(rasterizer_states);

	//�f�v�X�X�e���V���X�e�[�g�ݒ�
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(depth_stencil_state, 1);

	//�v���~�e�B�u�g�|���W�[���Z�b�g
	FRAMEWORK.getImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	VERTEX *v = new VERTEX[NUM_ELEMENTS];
	int n = 0; //�p�[�e�B�N��������
	static int dead_cnt = 0;
	int rng, rng2;
	float r,r2;
	DirectX::XMVECTOR v_pos, res;
	DirectX::XMMATRIX wm;
	DirectX::XMFLOAT4 tmp;
	for (int i = 0; i < NUM_ELEMENTS; i++)
	{
		if (pd[i].timer <= 0)
		{
			switch (type)
			{
			case PATTERN::RADIAL:
				pd[i].pos = init_pos;
				pd[i].timer = rand() % 100;
				break;
			case PATTERN::SNOW:
				rng = rand() % 2;
				rng2 = rand() % 2;
				if (!rng)
					rng = -1;
				if (!rng2)
					rng2 = -1;
				r = (rand() % (int)range) / range;
				r2 = (rand() % (int)range) / range;
				pd[i].pos.x = init_pos.x + r*range*rng;
				pd[i].pos.y = init_pos.y;
				pd[i].pos.z = init_pos.z + r2*range*rng2;
				pd[i].timer = rand() % (int)timer;
				break;
			case PATTERN::TORNADO:
				pd[i].pos = DirectX::XMFLOAT3(init_pos.x + cosf(pd[i].timer / 100.0f*360.0f*(DirectX::XM_PI / 180.0f))*range, init_pos.y, init_pos.z + sinf(pd[i].timer / 100.0f*360.0f*(DirectX::XM_PI / 180.0f))*range);
				break;
			case PATTERN::FIREWORK:
				if (pd[i].timer == 0)
					dead_cnt++;
				if (dead_cnt >= NUM_ELEMENTS - 1000)
				{
					for (int i = 0; i < NUM_ELEMENTS; i++) {
						pd[i].pos = init_pos;
						pd[i].timer = rand() % 100;
					}
					dead_cnt = 0;
				}
				break;
			case PATTERN::MODEL:
				pd[i].timer = rand() % 50 + 50;
				pd[i].pos = mdl->_model->getModelResource()->getModelData()->meshes.at(i%mdl->_model->getModelResource()->getModelData()->meshes.size()).vertices.at(i%mdl->_model->getModelResource()->getModelData()->meshes.at(i%mdl->_model->getModelResource()->getModelData()->meshes.size()).vertices.size()).position;
				v_pos = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(pd[i].pos.x, pd[i].pos.y, pd[i].pos.z, 1.0f));
				wm = DirectX::XMLoadFloat4x4(&mdl->getWorldMatrix());
				res = DirectX::XMVector4Transform(v_pos, wm);
				DirectX::XMStoreFloat4(&tmp, res);
				pd[i].pos = DirectX::XMFLOAT3(tmp.x, tmp.y, tmp.z);
				break;
			case PATTERN::STATIC_MODEL:
				pd[i].timer = rand() % 50 + 50;
				pd[i].pos = s_obj->getMesh()->vertices.at(i%s_obj->getMesh()->vertices.size()).position;
				v_pos = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(pd[i].pos.x, pd[i].pos.y, pd[i].pos.z, 1.0f));
				wm = DirectX::XMLoadFloat4x4(&s_obj->getWorld());
				res = DirectX::XMVector4Transform(v_pos, wm);
				DirectX::XMStoreFloat4(&tmp, res);
				pd[i].pos = DirectX::XMFLOAT3(tmp.x, tmp.y, tmp.z);
				break;
			default:
				break;
			}
			v[n].Pos = pd[i].pos;
			continue;
		}

		v[n].Pos = pd[i].pos;
		v[n].Normal = pd[i].normal;
		v[n].Tex = pd[i].size;

		//v[n].Color.x = 1;//type����
		//v[n].Color.y = 1;
		//v[n].Color.z = 1;
		//v[n].Color.w = 1.0f;
		++n;
	}

	cbuffer data;
	data.init_pos = DirectX::XMFLOAT4(init_pos.x, init_pos.y, init_pos.z, 1.0f);
	data.color = DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f);
	FRAMEWORK.getImmediateContext()->UpdateSubresource(cs_buffer, 0, 0, &data, 0, 0);
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(2, 1, &cs_buffer);

	//���_�f�[�^�X�V
	FRAMEWORK.getImmediateContext()->UpdateSubresource(vertex_buffer, 0, nullptr, v, 0, 0);

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	FRAMEWORK.getImmediateContext()->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	//�����_�����O(�C���f�b�N�X�t��)
	FRAMEWORK.getImmediateContext()->Draw(n, 0);

	//�V�F�[�_������
	_shader->Deactivate();

	delete[] v;
}

void gpu_particle::release()
{
	//update�Ŗ��񃊃��[�X���Ă�
	/*if (srv)
	{
		srv->Release();
		srv = NULL;
	}
*/
//delete temp;
	delete[] pd;

	if (vertex_buffer)
	{
		vertex_buffer->Release();
		vertex_buffer = NULL;
	}

	if (constant_buffer)
	{
		constant_buffer->Release();
		constant_buffer = NULL;
	}

	if (cs_buffer)
	{
		cs_buffer->Release();
		cs_buffer = NULL;
	}

	if (buffer)
	{
		buffer->Release();
		buffer = NULL;
	}

	if (result_buffer)
	{
		result_buffer->Release();
		result_buffer = NULL;
	}

	if (uav_buffer)
	{
		uav_buffer->Release();
		uav_buffer = NULL;
	}

	if (rasterizer_states)
	{
		rasterizer_states->Release();
		rasterizer_states = NULL;
	}

	if (depth_stencil_state)
	{
		depth_stencil_state->Release();
		depth_stencil_state = NULL;
	}
}

//------------------------------
//      �\�����o�b�t�@�𐶐�
//------------------------------
HRESULT gpu_particle::createStructuredBuffer(ID3D11Device* pDevice, UINT elementSize, UINT count, void* pInitData, ID3D11Buffer**  ppBufferOut)
{
	(*ppBufferOut) = nullptr;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = elementSize * count;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = elementSize;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pInitData;

		return pDevice->CreateBuffer(&desc, &initData, ppBufferOut);
	}

	return pDevice->CreateBuffer(&desc, nullptr, ppBufferOut);
}

//------------------------------------------------------------------------------------------
//      SRV�o�b�t�@�𐶐�//�\�����o�b�t�@����͂��邽�߂�SRV�ɕϊ�����
//------------------------------------------------------------------------------------------
HRESULT gpu_particle::createSRVBuffer(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	pBuffer->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));

	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;

	if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		srvDesc.BufferEx.NumElements = desc.ByteWidth / 4;
	}
	else if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.BufferEx.NumElements = desc.ByteWidth / desc.StructureByteStride;
	}
	else
	{
		return E_INVALIDARG;
	}

	return pDevice->CreateShaderResourceView(pBuffer, &srvDesc, ppSRVOut);
}


//---------------------------------------------------------------------------------------------
//      �o�C�g�A�h���X�o�b�t�@�𐶐�
//---------------------------------------------------------------------------------------------
HRESULT gpu_particle::createRawBuffer(ID3D11Device* pDevice, UINT size, void* pInitData, ID3D11Buffer** ppBufferOut)
{
	(*ppBufferOut) = nullptr;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_INDEX_BUFFER | D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = size;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pInitData;

		return pDevice->CreateBuffer(&desc, &initData, ppBufferOut);
	}

	return pDevice->CreateBuffer(&desc, nullptr, ppBufferOut);
}

//---------------------------------------------------------------------------------------------
//      ���s���A�N�Z�X�r���[�𐶐�
//---------------------------------------------------------------------------------------------
HRESULT gpu_particle::createUAVBuffer(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	pBuffer->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	memset(&uavDesc, 0, sizeof(uavDesc));

	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;

	if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		uavDesc.Buffer.NumElements = desc.ByteWidth / 4;
	}
	else if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
	}
	else
	{
		return E_INVALIDARG;
	}

	return pDevice->CreateUnorderedAccessView(pBuffer, &uavDesc, ppUAVOut);
}




void gpu_particle::createStructureBuffers()
{
	createStructuredBuffer(FRAMEWORK.getDevice(), sizeof(particle_data), NUM_ELEMENTS, &pd[0], &buffer);//���͐�
	createStructuredBuffer(FRAMEWORK.getDevice(), sizeof(particle_data), NUM_ELEMENTS, nullptr, &result_buffer);//�o�͐悾����null�ŗǂ�
}

void gpu_particle::createSRVBuffers()
{
	createSRVBuffer(FRAMEWORK.getDevice(), buffer, &srv);
}

void gpu_particle::createUAVBuffers()
{
	createUAVBuffer(FRAMEWORK.getDevice(), result_buffer, &uav_buffer);
}

//----------------------------------------------------------------------------------------------
//      �R���s���[�g�V�F�[�_�����s
//----------------------------------------------------------------------------------------------
void gpu_particle::runComputeShader
(
	/*ID3D11DeviceContext*        pContext,*/
	/*ID3D11ComputeShader*        pComputeShader,*/
	UINT                        numViews,
	ID3D11ShaderResourceView**  pSRVs,
	ID3D11Buffer*               pCBCS,
	void*                       pCSData,
	DWORD                       numDataBytes,
	ID3D11UnorderedAccessView*  pUAV,
	UINT                        x,
	UINT                        y,
	UINT                        z
	)
{
	FRAMEWORK.getImmediateContext()->CSSetShader(_shader->cs.Get(), nullptr, 0);
	FRAMEWORK.getImmediateContext()->CSSetShaderResources(0, numViews, pSRVs);
	FRAMEWORK.getImmediateContext()->CSSetUnorderedAccessViews(0, 1, &pUAV, nullptr);

	if (pCBCS)
	{
		D3D11_MAPPED_SUBRESOURCE res;

		FRAMEWORK.getImmediateContext()->Map(pCBCS, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
		memcpy(res.pData, pCSData, numDataBytes);
		FRAMEWORK.getImmediateContext()->Unmap(pCBCS, 0);

		//�萔�o�b�t�@�ݒ�
		ID3D11Buffer* ppCB[1] = { pCBCS };
		FRAMEWORK.getImmediateContext()->CSSetConstantBuffers(0, 1, ppCB);
	}

	//�X���b�h�O���[�v���������Ă邩�w��
	FRAMEWORK.getImmediateContext()->Dispatch(x, y, z);

	ID3D11UnorderedAccessView*  pNullUAVs[1] = { nullptr };
	ID3D11ShaderResourceView*   pNullSRVs[1] = { nullptr };
	ID3D11Buffer* pNullCBs[1] = { nullptr };

	FRAMEWORK.getImmediateContext()->CSSetShader(nullptr, nullptr, 0);
	FRAMEWORK.getImmediateContext()->CSSetUnorderedAccessViews(0, 1, pNullUAVs, nullptr);
	FRAMEWORK.getImmediateContext()->CSSetShaderResources(0, 1, pNullSRVs);
	FRAMEWORK.getImmediateContext()->CSSetConstantBuffers(0, 1, pNullCBs);
}

//----------------------------------------------------------------------------------------------
//      �o�b�t�@�̓��e���R�s�[
//----------------------------------------------------------------------------------------------
ID3D11Buffer* gpu_particle::createAndCopyToBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer)
{
	//	ID3D11Buffer* debugbuf = nullptr;
	//
	//	D3D11_BUFFER_DESC desc = {};
	//	pBuffer->GetDesc(&desc);
	//	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	//	desc.Usage = D3D11_USAGE_STAGING;
	//	desc.BindFlags = 0;
	//	desc.MiscFlags = 0;
	//	if (SUCCEEDED(pDevice->CreateBuffer(&desc, nullptr, &debugbuf)))
	//	{
	//#if defined(_DEBUG) || defined(PROFILE)
	//		debugbuf->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("Debug") - 1, "Debug");
	//#endif
	//
	//		pd3dImmediateContext->CopyResource(debugbuf, pBuffer);
	//	}
	//
	//	return debugbuf;

	ID3D11Buffer* pCloneBuf = nullptr;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	pBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;

	if (SUCCEEDED(pDevice->CreateBuffer(&desc, nullptr, &pCloneBuf)))
	{
		FRAMEWORK.getImmediateContext()->CopyResource(pCloneBuf, pBuffer);
	}

	return pCloneBuf;
}

firework_emitter::firework_emitter()
{
}

void firework_emitter::init(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 size, int range)
{
	for (int i = 0; i < 10; i++)
	{
		//gpu_particle�̐錾
		gp[i] = std::make_unique<gpu_particle>();
		gp[i]->create(L"particle_cs.hlsl", "CSFunc");
		//�����f�[�^�ݒ�
		float x = rand() % (2 * range + 1) - range;
		float y = rand() % (2 * range + 1) - range;
		float z = rand() % (2 * range + 1) - range;
		gp[i]->setInitData(gpu_particle::PATTERN::FIREWORK, DirectX::XMFLOAT3(pos.x + x, pos.y + y, pos.z + z), size, 0);
	}
}

void firework_emitter::update()
{
	for (int i = 0; i < 10; i++)
	{
		gp[i]->update();
	}
}

void firework_emitter::render()
{
	for (int i = 0; i < 10; i++)
	{
		gp[i]->render();
	}
}