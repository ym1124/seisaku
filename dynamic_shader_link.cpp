#pragma once
//#include <d3dcompiler.h>
#include "framework.h"
#include "dynamic_shader_link.h"
#include "shader.h"


BASE_SHADING::BASE_SHADING()
{
	m_pLayout = NULL;
	for (int i = 0; i<_countof(m_pConstantBuffers); i++)
		m_pConstantBuffers[i] = NULL;
	m_pVertexShader = NULL;
	m_pPSClassLinkage = NULL;
	m_pPixelShader = NULL;
}

BASE_SHADING::~BASE_SHADING()
{
	m_pLayout->Release();
	m_pLayout = nullptr;
	for (int i = 0; i < _countof(m_pConstantBuffers); i++)
	{
		m_pConstantBuffers[i]->Release();
		m_pConstantBuffers[i] = nullptr;
	}
	m_pVertexShader->Release();
	m_pVertexShader = nullptr;
	m_pPSClassLinkage->Release();
	m_pPSClassLinkage = nullptr;
	m_pPixelShader->Release();
	m_pPixelShader = nullptr;
}

//// ������
//HRESULT BASE_SHADING::Init(ID3D11Device* pD3DDevice
//	, const BYTE* pVS, size_t VSSize
//	, const BYTE* pPS, size_t PSSize
//	)
//{
//	HRESULT hr = E_FAIL;
//
//	if (pD3DDevice == NULL) goto EXIT;
//
//	// *****************************************************************************************************************
//	// ���_�V�F�[�_�[�̍쐬
//	// *****************************************************************************************************************
//
//	hr = pD3DDevice->CreateVertexShader(pVS, VSSize, NULL, &m_pVertexShader);
//	if (FAILED(hr)) goto EXIT;
//
//	// ���̓��C�A�E�g�͌Œ�ݒ�ɂ��Ă���
//	D3D11_INPUT_ELEMENT_DESC layout[] = {
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	hr = pD3DDevice->CreateInputLayout(layout, _countof(layout), pVS, VSSize, &m_pLayout);
//	if (FAILED(hr)) goto EXIT;
//
//	// *****************************************************************************************************************
//	// �s�N�Z���F�[�_�[�̍쐬
//	// *****************************************************************************************************************
//
//	// �s�N�Z���V�F�[�_�[�p�̓��I�V�F�[�_�[ �����N��L���ɂ���N���X �����N ���C�u�������쐬����
//	// http://msdn.microsoft.com/ja-jp/library/ee419783(v=vs.85).aspx
//	pD3DDevice->CreateClassLinkage(&m_pPSClassLinkage);
//
//	hr = pD3DDevice->CreatePixelShader(pPS, PSSize, m_pPSClassLinkage, &m_pPixelShader);
//	if (FAILED(hr)) goto EXIT;
//
//	// *****************************************************************************************************************
//	// �萔�o�b�t�@���쐬
//	// *****************************************************************************************************************
//
//	D3D11_BUFFER_DESC BufferDesc;
//
//	// ���I�V�F�[�_�[�����P�[�W���Ŏg�p����萔�o�b�t�@�I�u�W�F�N�g���쐬����
//	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
//	BufferDesc.ByteWidth = sizeof(CBUFFER0);        // �o�b�t�@�T�C�Y
//	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // ���\�[�X�g�p�@����肷��
//	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// �o�b�t�@�̎��
//	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU �A�N�Z�X
//	BufferDesc.MiscFlags = 0;                         // ���̑��̃t���O���ݒ肵�Ȃ�
//	hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, &m_pConstantBuffers[0]);
//	if (FAILED(hr)) goto EXIT;
//
//	// ���_�V�F�[�_�[�Ŏg�p����萔�o�b�t�@�I�u�W�F�N�g���쐬����
//	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
//	BufferDesc.ByteWidth = sizeof(CBUFFER1);        // �o�b�t�@�T�C�Y
//	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // ���\�[�X�g�p�@����肷��
//	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// �o�b�t�@�̎��
//	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU �A�N�Z�X
//	BufferDesc.MiscFlags = 0;                         // ���̑��̃t���O���ݒ肵�Ȃ�
//	hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, &m_pConstantBuffers[1]);
//	if (FAILED(hr)) goto EXIT;
//
//	hr = S_OK;
//
//EXIT:
//	return hr;
//}

HRESULT BASE_SHADING::Init(ID3D11Device* pD3DDevice, WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc)
{
	HRESULT hr = E_FAIL;

	ID3D10Blob* pVSBlob = NULL, *pPSBlob = NULL;

	// �s����D��Őݒ肵�A�Â��`���̋L�q�������Ȃ��悤�ɂ���
	UINT Flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
	// �œK�����x����ݒ肷��
#if defined(DEBUG) || defined(_DEBUG)
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
#else
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

	// ���_�V�F�[�_�[���R���p�C������
	//hr = D3DX11CompileFromFile(pSrcFile, NULL, NULL, pVSMain, "vs_5_0", Flag1, 0, NULL, &pVSBlob, NULL, NULL);
	//if (FAILED(hr)) goto EXIT;

	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob = NULL;
	// ���_�V�F�[�_
	hr = shader::Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ���_�V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &m_pVertexShader);
	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g�͌Œ�ݒ�ɂ��Ă���
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	//hr = pD3DDevice->CreateInputLayout(layout, _countof(layout), pVS, VSSize, &m_pLayout);
	//if (FAILED(hr)) goto EXIT;
	hr = FRAMEWORK.getDevice()->CreateInputLayout(layout, _countof(layout), VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &m_pLayout);
	assert(SUCCEEDED(hr));

	// �s�N�Z���V�F�[�_�[���R���p�C������
	//hr = D3DX11CompileFromFile(pSrcFile, NULL, NULL, pPSMain, "ps_5_0", Flag1, 0, NULL, &pPSBlob, NULL, NULL);
	//if (FAILED(hr)) goto EXIT;

	// �s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
	hr = shader::Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// �s�N�Z���V�F�[�_�[�p�̓��I�V�F�[�_�[ �����N��L���ɂ���N���X �����N ���C�u�������쐬����
	pD3DDevice->CreateClassLinkage(&m_pPSClassLinkage);

	// �s�N�Z���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), m_pPSClassLinkage, &m_pPixelShader);
	//PSBlob->Release();
	assert(SUCCEEDED(hr));

	// *****************************************************************************************************************
	// �萔�o�b�t�@���쐬
	// *****************************************************************************************************************

	D3D11_BUFFER_DESC BufferDesc;

	// ���I�V�F�[�_�[�����P�[�W���Ŏg�p����萔�o�b�t�@�I�u�W�F�N�g���쐬����
	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.ByteWidth = sizeof(CBUFFER0);        // �o�b�t�@�T�C�Y
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // ���\�[�X�g�p�@����肷��
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// �o�b�t�@�̎��
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU �A�N�Z�X
	BufferDesc.MiscFlags = 0;                         // ���̑��̃t���O���ݒ肵�Ȃ�
	hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, &m_pConstantBuffers[0]);
	if (FAILED(hr))
		return hr;

	// ���_�V�F�[�_�[�Ŏg�p����萔�o�b�t�@�I�u�W�F�N�g���쐬����
	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.ByteWidth = sizeof(CBUFFER1);        // �o�b�t�@�T�C�Y
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // ���\�[�X�g�p�@����肷��
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// �o�b�t�@�̎��
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU �A�N�Z�X
	BufferDesc.MiscFlags = 0;                         // ���̑��̃t���O���ݒ肵�Ȃ�
	hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, &m_pConstantBuffers[1]);
	if (FAILED(hr))
		return hr;

	hr = S_OK;

	return hr;
}

// ���_�V�F�[�_�[�p�̒萔�o�b�t�@��ݒ肷��
HRESULT BASE_SHADING::SetCBVertexShader(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMMATRIX* World ,DirectX::XMMATRIX* matWVP)
{
	HRESULT hr = E_FAIL;

	if (pD3DDeviceContext == NULL) goto EXIT;
	if (matWVP == NULL) goto EXIT;
	if (World == NULL) goto EXIT;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CBUFFER1* cbuffer;

	hr = pD3DDeviceContext->Map(m_pConstantBuffers[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) goto EXIT;
	cbuffer = (CBUFFER1*)(mappedResource.pData);

	std::memcpy(&cbuffer->World, World, sizeof(DirectX::XMMATRIX));
	std::memcpy(&cbuffer->matWVP, matWVP, sizeof(DirectX::XMMATRIX));

	pD3DDeviceContext->Unmap(m_pConstantBuffers[1], 0);

	hr = S_OK;
EXIT:
	return hr;
}

// �����o�[�g�p�̒萔�o�b�t�@��ݒ�
HRESULT BASE_SHADING::SetCBLambert(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight)
{
	HRESULT hr = E_FAIL;

	if (pD3DDeviceContext == NULL) goto EXIT;
	if (p_vecLight == NULL) goto EXIT;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CBUFFER0* cbuffer;

	hr = pD3DDeviceContext->Map(m_pConstantBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) goto EXIT;
	cbuffer = (CBUFFER0*)(mappedResource.pData);

	std::memcpy(&cbuffer->vecLight0, p_vecLight, sizeof(DirectX::XMFLOAT4));

	pD3DDeviceContext->Unmap(m_pConstantBuffers[0], 0);

	//// �V�F�[�_�[���\�[�X�r���[��ݒ�
	//pD3DDeviceContext->PSSetShaderResources(0, 1, &pDecalMap);

	hr = S_OK;
EXIT:
	return hr;
}

// �n�[�t�����o�[�g�p�̒萔�o�b�t�@��ݒ�
HRESULT BASE_SHADING::SetCBHalfLambert(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight)
{
	HRESULT hr = E_FAIL;

	if (pD3DDeviceContext == NULL) goto EXIT;
	if (p_vecLight == NULL) goto EXIT;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CBUFFER0* cbuffer;

	hr = pD3DDeviceContext->Map(m_pConstantBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) goto EXIT;
	cbuffer = (CBUFFER0*)(mappedResource.pData);

	std::memcpy(&cbuffer->vecLight1, p_vecLight, sizeof(DirectX::XMFLOAT4));

	pD3DDeviceContext->Unmap(m_pConstantBuffers[0], 0);

	//// �V�F�[�_�[���\�[�X�r���[��ݒ�
	//pD3DDeviceContext->PSSetShaderResources(0, 1, &pDecalMap);

	hr = S_OK;
EXIT:
	return hr;
}

// �t�H���V�F�[�f�B���O�p�̒萔�o�b�t�@��ݒ�
HRESULT BASE_SHADING::SetCBPhongShading(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight, DirectX::XMFLOAT4* p_EyePos)
{
	HRESULT hr = E_FAIL;

	if (pD3DDeviceContext == NULL) goto EXIT;
	if (p_vecLight == NULL) goto EXIT;
	if (p_EyePos == NULL) goto EXIT;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CBUFFER0* cbuffer;

	hr = pD3DDeviceContext->Map(m_pConstantBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) goto EXIT;
	cbuffer = (CBUFFER0*)(mappedResource.pData);

	std::memcpy(&cbuffer->vecLight2, p_vecLight, sizeof(DirectX::XMFLOAT4));
	std::memcpy(&cbuffer->EyePos, p_EyePos, sizeof(DirectX::XMFLOAT4));

	pD3DDeviceContext->Unmap(m_pConstantBuffers[0], 0);

	//// �V�F�[�_�[���\�[�X�r���[��ݒ�
	//pD3DDeviceContext->PSSetShaderResources(0, 1, &pDecalMap);

	hr = S_OK;
EXIT:
	return hr;
}

// �V�F�[�f�B���O�p�̒萔�o�b�t�@��ݒ�
HRESULT BASE_SHADING::SetCBNoShading(ID3D11DeviceContext* pD3DDeviceContext/*, DirectX::XMFLOAT4* p_vecLight*/)
{
HRESULT hr = E_FAIL;

	//if (pD3DDeviceContext == NULL) goto EXIT;
	//if (p_vecLight == NULL) goto EXIT;

	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//CBUFFER0* cbuffer;

	//hr = pD3DDeviceContext->Map(m_pConstantBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//if (FAILED(hr)) goto EXIT;
	//cbuffer = (CBUFFER0*)(mappedResource.pData);

	//std::memcpy(&cbuffer->vecLight2, p_vecLight, sizeof(DirectX::XMFLOAT4));

	//pD3DDeviceContext->Unmap(m_pConstantBuffers[0], 0);

	////// �V�F�[�_�[���\�[�X�r���[��ݒ�
	////pD3DDeviceContext->PSSetShaderResources(0, 1, &pDecalMap);
	////pD3DDeviceContext->PSSetShaderResources(1, 1, &pCelMap);

	hr = S_OK;
//EXIT:
	return hr;
}

HRESULT BASE_SHADING::Begin(ID3D11DeviceContext* pD3DDeviceContext, LPCSTR pClassInstanceName)
{
	HRESULT hr = E_FAIL;

	// �w�肳�ꂽ HLSL �N���X��\���N���X�C���X�^���X �I�u�W�F�N�g���擾����
	// http://msdn.microsoft.com/ja-jp/library/ee419541(v=vs.85).aspx
	//ID3D11ClassLinkage::GetClassInstance �Ƃ����֐��͎w�肳�ꂽHLSL�N���X��\���N���X�C���X�^���X�I�u�W�F�N�g���擾���邽�߂̊֐��ł��邪�A 
	//���̊֐��̓N���X���Ƀf�[�^�����o�[(�����o�ϐ�)�����݂���ꍇ�Ɏg�p�ł��A���݂��Ȃ��ꍇ��ID3D11ClassLinkage::CreateClassInstance���g�p����K�v������(�R���p�C���͒ʂ邪PS�����f����Ȃ��Ȃ�)
	hr = m_pPSClassLinkage->GetClassInstance(pClassInstanceName, 0, &m_pClassInstance);
	if (FAILED(hr)) goto EXIT;

	// ���C�A�E�g�ݒ�
	pD3DDeviceContext->IASetInputLayout(m_pLayout);

	// ���_�V�F�[�_�[���f�o�C�X�ɐݒ肷��B
	pD3DDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);

	// ���_�V�F�[�_�[�ɒ萔�o�b�t�@��ݒ肷��
	pD3DDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffers[1]);

	// �n���V�F�[�_�[�𖳌��ɂ���B
	pD3DDeviceContext->HSSetShader(NULL, NULL, 0);

	// �h���C���V�F�[�_�[�𖳌��ɂ���B
	pD3DDeviceContext->DSSetShader(NULL, NULL, 0);

	// �W�I���g���V�F�[�_�[�𖳌��ɂ���B
	pD3DDeviceContext->GSSetShader(NULL, NULL, 0);

	// �s�N�Z���V�F�[�_�[�𓮓I�V�F�[�_�[�����N�Ƃ��Đݒ肷��
	pD3DDeviceContext->PSSetShader(m_pPixelShader, &m_pClassInstance, 1);

	// �s�N�Z���V�F�[�_�[�ɒ萔�o�b�t�@��ݒ肷��
	pD3DDeviceContext->PSSetConstantBuffers(2, 1, &m_pConstantBuffers[0]);

	// �R���s���[�g�V�F�[�_�[�𖳌��ɂ���B
	pD3DDeviceContext->CSSetShader(NULL, NULL, 0);

	hr = S_OK;
EXIT:
	return hr;
}

//HLSL���ɃN���X�������o���Ȃ��o�[�W����
HRESULT BASE_SHADING::BeginNM(ID3D11DeviceContext* pD3DDeviceContext, LPCSTR pClassTypeName)
{
	HRESULT hr = E_FAIL;

	// �w�肳�ꂽ HLSL �N���X��\���N���X�C���X�^���X �I�u�W�F�N�g���擾����
	// http://msdn.microsoft.com/ja-jp/library/ee419541(v=vs.85).aspx
	//ID3D11ClassLinkage::GetClassInstance �Ƃ����֐��͎w�肳�ꂽHLSL�N���X��\���N���X�C���X�^���X�I�u�W�F�N�g���擾���邽�߂̊֐��ł��邪�A 
	//���̊֐��̓N���X���Ƀf�[�^�����o�[(�����o�ϐ�)�����݂���ꍇ�Ɏg�p�ł��A���݂��Ȃ��ꍇ��ID3D11ClassLinkage::CreateClassInstance���g�p����K�v������(�R���p�C���͒ʂ邪PS�����f����Ȃ��Ȃ�)
	hr = m_pPSClassLinkage->CreateClassInstance(pClassTypeName, 0, 0, 0, 0, &m_pClassInstance);
	if (FAILED(hr)) goto EXIT;                                 //��"HLSL�̃N���X�̌^�̖��O"

	// ���C�A�E�g�ݒ�
	pD3DDeviceContext->IASetInputLayout(m_pLayout);

	// ���_�V�F�[�_�[���f�o�C�X�ɐݒ肷��B
	pD3DDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);

	// ���_�V�F�[�_�[�ɒ萔�o�b�t�@��ݒ肷��
	pD3DDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffers[1]);

	// �n���V�F�[�_�[�𖳌��ɂ���B
	pD3DDeviceContext->HSSetShader(NULL, NULL, 0);

	// �h���C���V�F�[�_�[�𖳌��ɂ���B
	pD3DDeviceContext->DSSetShader(NULL, NULL, 0);

	// �W�I���g���V�F�[�_�[�𖳌��ɂ���B
	pD3DDeviceContext->GSSetShader(NULL, NULL, 0);

	// �s�N�Z���V�F�[�_�[�𓮓I�V�F�[�_�[�����N�Ƃ��Đݒ肷��
	pD3DDeviceContext->PSSetShader(m_pPixelShader, &m_pClassInstance, 1);

	// �s�N�Z���V�F�[�_�[�ɒ萔�o�b�t�@��ݒ肷��
	pD3DDeviceContext->PSSetConstantBuffers(2, 1, &m_pConstantBuffers[0]);

	// �R���s���[�g�V�F�[�_�[�𖳌��ɂ���B
	pD3DDeviceContext->CSSetShader(NULL, NULL, 0);

	hr = S_OK;
EXIT:
	return hr;
}

void BASE_SHADING::End()
{
	if (m_pClassInstance != nullptr)
		m_pClassInstance->Release();
}

