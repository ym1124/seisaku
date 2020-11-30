#pragma once
#include "shader.h"
#include "framework.h"
#include "load_file.h"
#include "sprite.h"
#include "mesh.h"
#include "frame_buffer.h"


shader::shader()
{
}

shader::~shader()
{
	release();
}

//------------------------------------------------
//	�V�F�[�_�[�P�̃R���p�C��
//------------------------------------------------
HRESULT shader::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
{
	DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ShaderFlags |= D3DCOMPILE_DEBUG;
	ShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

	Microsoft::WRL::ComPtr<ID3DBlob> BlobError = NULL;
	// �R���p�C��
	HRESULT hr = D3DCompileFromFile(
		filename,
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		method,
		shaderModel,
		ShaderFlags,
		0,
		ppBlobOut,
		BlobError.GetAddressOf()
		);

	// �G���[�o��
	if (BlobError != NULL)
	{
		MessageBoxA(0, (char*)BlobError->GetBufferPointer(), NULL, MB_OK);
	}

	return hr;
}

////------------------------------------------------
////	�t�@�C������R���s���[�g�V�F�[�_�[�R���p�C��//�ėp���Ⴂ�̂ō폜
////------------------------------------------------
//HRESULT shader::CompileCS(const wchar_t* szFileName,const char* szEntryPoint,const char* szShaderModel,ID3DBlob** ppBlobOut)
//{
//	// ���^�[���R�[�h��������.
//	HRESULT hr = S_OK;
//
//	// �R���p�C���t���O.
//	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
//
//#if defined(DEBUG) || defined(_DEBUG)
//	dwShaderFlags |= D3DCOMPILE_DEBUG;
//#endif//defiend(DEBUG) || defined(_DEBUG)
//
//#if defined(NDEBUG) || defined(_NDEBUG)
//	dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
//#endif//defined(NDEBUG) || defined(_NDEBUG)
//
//	ID3DBlob* pErrorBlob = nullptr;
//
//	// �t�@�C������V�F�[�_���R���p�C��.
//	hr = D3DCompileFromFile(
//		szFileName,
//		NULL,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,
//		szEntryPoint,
//		szShaderModel,
//		dwShaderFlags,
//		0,
//		ppBlobOut,
//		&pErrorBlob
//		);
//
//	// �G���[�`�F�b�N.
//	if (FAILED(hr))
//	{
//		// �G���[���b�Z�[�W���o��.
//		if (pErrorBlob != nullptr)
//		{
//			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
//		}
//	}
//
//	// �������.
//	pErrorBlob->Release();
//
//	// ���^�[���R�[�h��ԋp.
//	return hr;
//}

//------------------------------------------------
//	�V�F�[�_�[�Z�b�g�R���p�C��
//------------------------------------------------
bool shader::Create(const wchar_t* shader_name)
{
	HRESULT hr = S_OK;

	//�C���v�b�g���C�A�E�g
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	for (int i = 0; i < 2; i++)
	{
		wchar_t str[8];
		//�V�F�[�_�[�̓ǂݍ���
		switch (i)
		{
		case 0:
			wcscpy_s(str, L"_vs.cso");
			break;
		case 1:
			wcscpy_s(str, L"_ps.cso");
			break;
		}

		wchar_t src[255];
		wcscpy_s(src, shader_name);
		wcscat_s(src, sizeof(src) / sizeof(src[0]), str);
		size_t *num = nullptr;
		char tmp[255];
		wcstombs_s(num, tmp, sizeof(tmp), src, sizeof(src) / sizeof(src[0]));

		switch (i)
		{
		case 0:
			vs_Load(FRAMEWORK.getInstance().getDevice(), tmp, vs.GetAddressOf(), vertex_layout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
			break;
		case 1:
			ps_Load(FRAMEWORK.getInstance().getDevice(), tmp, ps.GetAddressOf());
			break;
		}
	}

	// ���̓��C�A�E�g�ݒ�
	FRAMEWORK.getImmediateContext()->IASetInputLayout(vertex_layout.Get());

	//�R���X�^���g�o�b�t�@�[�쐬�@�ϊ��s��n���p
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(D3D11_BUFFER_DESC));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(ConstantBufferWorld);
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DEFAULT;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&cb, NULL, ConstantBuffer_w.GetAddressOf());
	assert(SUCCEEDED(hr));

	CreateConstantBuffer(sizeof(ConstantBufferForPhong));

	return true;
}

//------------------------------------------------
//	�V�F�[�_�[�Z�b�g�R���p�C��
//------------------------------------------------
bool shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob = NULL;
	// ���_�V�F�[�_
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ���_�V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vs);
	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// ���̓��C�A�E�g����
	hr = FRAMEWORK.getDevice()->CreateInputLayout(layout, numElements, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &vertex_layout);
	assert(SUCCEEDED(hr));


	// ���̓��C�A�E�g�ݒ�
	FRAMEWORK.getImmediateContext()->IASetInputLayout(vertex_layout.Get());


	// �s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// �s�N�Z���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &ps);
	//PSBlob->Release();
	assert(SUCCEEDED(hr));

	//�R���X�^���g�o�b�t�@�[�쐬�@�ϊ��s��n���p
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(D3D11_BUFFER_DESC));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(ConstantBufferWorld);
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DEFAULT;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&cb, NULL, ConstantBuffer_w.GetAddressOf());
	assert(SUCCEEDED(hr));

	CreateConstantBuffer(sizeof(ConstantBufferForPhong));

	return true;
}

//------------------------------------------------
//	�V�F�[�_�[�Z�b�g�R���p�C��
//------------------------------------------------
bool shader::CreateForTangent(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob = NULL;
	// ���_�V�F�[�_
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));


	// ���_�V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vs);
	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// ���̓��C�A�E�g����
	hr = FRAMEWORK.getDevice()->CreateInputLayout(layout, numElements, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &vertex_layout);
	assert(SUCCEEDED(hr));


	// ���̓��C�A�E�g�ݒ�
	FRAMEWORK.getImmediateContext()->IASetInputLayout(vertex_layout.Get());


	// �s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// �s�N�Z���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &ps);
	//PSBlob->Release();
	assert(SUCCEEDED(hr));

	//�R���X�^���g�o�b�t�@�[�쐬�@�ϊ��s��n���p
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(D3D11_BUFFER_DESC));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(ConstantBufferWorld);
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DEFAULT;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&cb, NULL, &ConstantBuffer_w);
	assert(SUCCEEDED(hr));

	CreateConstantBuffer(sizeof(ConstantBufferForPhong));

	return true;
}

//bool shader::Create(WCHAR* filename, LPCSTR Func, shader::SHADER_TYPE type)
//{
//	HRESULT hr = S_OK;
//
//	if (type == shader::SHADER_TYPE::VERTEX)
//	{
//		Microsoft::WRL::ComPtr<ID3DBlob> VSBlob = NULL;
//		// ���_�V�F�[�_
//		hr = Compile(filename, Func, "vs_5_0", VSBlob.GetAddressOf());
//		assert(SUCCEEDED(hr));
//
//		// ���_�V�F�[�_����
//		hr = FRAMEWORK.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vs);
//		assert(SUCCEEDED(hr));
//
//		// ���̓��C�A�E�g
//		D3D11_INPUT_ELEMENT_DESC layout[] = {
//			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		};
//		UINT numElements = ARRAYSIZE(layout);
//
//		// ���̓��C�A�E�g����
//		hr = FRAMEWORK.getDevice()->CreateInputLayout(
//			layout,
//			numElements,
//			VSBlob->GetBufferPointer(),
//			VSBlob->GetBufferSize(),
//			&vertex_layout
//			);
//
//		assert(SUCCEEDED(hr));
//
//		// ���̓��C�A�E�g�ݒ�
//		FRAMEWORK.getImmediateContext()->IASetInputLayout(vertex_layout);
//	}
//
//	if (type == shader::SHADER_TYPE::PIXEL)
//	{
//		// �s�N�Z���V�F�[�_
//		Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
//		hr = Compile(filename, Func, "ps_5_0", &PSBlob);
//		if (FAILED(hr))
//		{
//			return false;
//		}
//
//		// �s�N�Z���V�F�[�_����
//		hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &ps);
//		//PSBlob->Release();
//		assert(SUCCEEDED(hr));
//
//		//�R���X�^���g�o�b�t�@�[�쐬�@�ϊ��s��n���p
//		D3D11_BUFFER_DESC cb;
//		ZeroMemory(&cb, sizeof(D3D11_BUFFER_DESC));
//		cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//		cb.ByteWidth = sizeof(ConstantBufferWorld);
//		cb.CPUAccessFlags = 0;
//		cb.MiscFlags = 0;
//		cb.Usage = D3D11_USAGE_DEFAULT;
//
//		hr = FRAMEWORK.getDevice()->CreateBuffer(&cb, NULL, &ConstantBuffer_w);
//		assert(SUCCEEDED(hr));
//	}
//
//	return true;
//}


//------------------------------------------------
//	�V�F�[�_�[�Z�b�g�R���p�C��2
//------------------------------------------------
bool shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob = NULL;
	// ���_�V�F�[�_
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ���_�V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vs);
	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// ���̓��C�A�E�g����
	hr = FRAMEWORK.getDevice()->CreateInputLayout(
		layout,
		numElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		&vertex_layout
		);

	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g�ݒ�
	FRAMEWORK.getImmediateContext()->IASetInputLayout(vertex_layout.Get());

	// �s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", PSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �s�N�Z���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &ps);
	//PSBlob.Get()->Release();
	assert(SUCCEEDED(hr));
	// �W�I���g���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> GSBlob = NULL;
	hr = Compile(filename, GSFunc, "gs_5_0", GSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �W�I���g���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, &gs);
	assert(SUCCEEDED(hr));

	//�R���X�^���g�o�b�t�@�[�쐬�@�ϊ��s��n���p
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(D3D11_BUFFER_DESC));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(ConstantBufferWorld);
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DEFAULT;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&cb, NULL, &ConstantBuffer_w);
	assert(SUCCEEDED(hr));

	CreateConstantBuffer(sizeof(ConstantBufferForPhong));

	return true;
}

bool shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR HSFunc, LPCSTR DSFunc)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob = NULL;
	// ���_�V�F�[�_
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ���_�V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vs);
	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// ���̓��C�A�E�g����
	hr = FRAMEWORK.getDevice()->CreateInputLayout(layout, numElements, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &vertex_layout);
	assert(SUCCEEDED(hr));


	// ���̓��C�A�E�g�ݒ�
	FRAMEWORK.getImmediateContext()->IASetInputLayout(vertex_layout.Get());


	// �s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// �s�N�Z���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &ps);
	//PSBlob->Release();
	assert(SUCCEEDED(hr));

	// �n���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> HSBlob = NULL;
	hr = Compile(filename, HSFunc, "hs_5_0", HSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �n���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateHullShader(HSBlob->GetBufferPointer(), HSBlob->GetBufferSize(), NULL, &hs);
	assert(SUCCEEDED(hr));

	// �h���C���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> DSBlob = NULL;
	hr = Compile(filename, DSFunc, "ds_5_0", DSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �h���C���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateDomainShader(DSBlob->GetBufferPointer(), DSBlob->GetBufferSize(), NULL, &ds);
	assert(SUCCEEDED(hr));

	//�R���X�^���g�o�b�t�@�[�쐬�@�ϊ��s��n���p
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(D3D11_BUFFER_DESC));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(ConstantBufferWorld);
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DEFAULT;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&cb, NULL, &ConstantBuffer_w);
	assert(SUCCEEDED(hr));

	CreateConstantBuffer(sizeof(ConstantBufferForPhong));

	return true;
}

//------------------------------------------------
//	�V�F�[�_�[�Z�b�g�R���p�C��4
//------------------------------------------------
bool shader::Create(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc, LPCSTR HSFunc, LPCSTR DSFunc)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob = NULL;
	// ���_�V�F�[�_
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ���_�V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vs);
	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// ���̓��C�A�E�g����
	hr = FRAMEWORK.getDevice()->CreateInputLayout(
		layout,
		numElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		&vertex_layout
		);

	assert(SUCCEEDED(hr));
	VSBlob.Get()->Release();

	// ���̓��C�A�E�g�ݒ�
	FRAMEWORK.getImmediateContext()->IASetInputLayout(vertex_layout.Get());

	// �s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", PSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �s�N�Z���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &ps);
	assert(SUCCEEDED(hr));

	// �W�I���g���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> GSBlob = NULL;
	hr = Compile(filename, GSFunc, "gs_5_0", GSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));
	// �W�I���g���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, &gs);
	assert(SUCCEEDED(hr));

	// �n���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> HSBlob = NULL;
	hr = Compile(filename, HSFunc, "hs_5_0", HSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �n���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateHullShader(HSBlob->GetBufferPointer(), HSBlob->GetBufferSize(), NULL, &hs);
	assert(SUCCEEDED(hr));

	// �h���C���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> DSBlob = NULL;
	hr = Compile(filename, DSFunc, "ds_5_0", DSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �h���C���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateDomainShader(DSBlob->GetBufferPointer(), DSBlob->GetBufferSize(), NULL, &ds);
	assert(SUCCEEDED(hr));

	return true;
}

//skin_mesh�p��shader���
bool shader::CreateForSkin(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob = NULL;
	// ���_�V�F�[�_
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));


	// ���_�V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vs);
	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// ���̓��C�A�E�g����
	hr = FRAMEWORK.getDevice()->CreateInputLayout(
		layout,
		numElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		&vertex_layout
		);
	assert(SUCCEEDED(hr));


	// ���̓��C�A�E�g�ݒ�
	FRAMEWORK.getImmediateContext()->IASetInputLayout(vertex_layout.Get());


	// �s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// �s�N�Z���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &ps);
	//PSBlob->Release();
	assert(SUCCEEDED(hr));

	//�R���X�^���g�o�b�t�@�[�쐬�@�ϊ��s��n���p
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(D3D11_BUFFER_DESC));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(ConstantBufferWorld_s);
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DEFAULT;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&cb, NULL, &ConstantBuffer_w);
	assert(SUCCEEDED(hr));

	CreateConstantBuffer(sizeof(ConstantBufferForPhong));

	return true;
}

//skin_mesh�p��shader���(GS�Ή�)
bool shader::CreateForSkin(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob = NULL;
	// ���_�V�F�[�_
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));


	// ���_�V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vs);
	assert(SUCCEEDED(hr));

	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// ���̓��C�A�E�g����
	hr = FRAMEWORK.getDevice()->CreateInputLayout(
		layout,
		numElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		&vertex_layout
		);
	assert(SUCCEEDED(hr));


	// ���̓��C�A�E�g�ݒ�
	FRAMEWORK.getImmediateContext()->IASetInputLayout(vertex_layout.Get());


	// �s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// �s�N�Z���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &ps);
	//PSBlob->Release();
	assert(SUCCEEDED(hr));

	// �W�I���g���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> GSBlob = NULL;
	hr = Compile(filename, GSFunc, "gs_5_0", GSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �W�I���g���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), NULL, &gs);
	assert(SUCCEEDED(hr));

	//�R���X�^���g�o�b�t�@�[�쐬�@�ϊ��s��n���p
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(D3D11_BUFFER_DESC));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(ConstantBufferWorld_s);
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DEFAULT;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&cb, NULL, &ConstantBuffer_w);
	assert(SUCCEEDED(hr));

	return true;
}

//shader���P�������Ȃ�shader�N���X�̃����o�֐��g��
bool shader::CreateConstantBuffer(u_int size)
{
	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = size;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = FRAMEWORK.getDevice()->CreateBuffer(&bd, NULL, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;
}

//�Ǝ��p�����[�^�[�p
bool shader::CreateConstantBufferOriginal(u_int size)
{
	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = size;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = FRAMEWORK.getDevice()->CreateBuffer(&bd, NULL, ConstantBufferOriginal.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;
}

//HRESULT shader::CreateVS_POINT(wchar_t* srcFile, char* functionName)
//{
//	HRESULT hr = S_OK;
//
//	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob = NULL;
//	// ���_�V�F�[�_
//	hr = Compile(srcFile, functionName, "vs_5_0", VSBlob.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//
//	// ���_�V�F�[�_����
//	hr = FRAMEWORK.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vs);
//	assert(SUCCEEDED(hr));
//
//	// ���̓��C�A�E�g
//	D3D11_INPUT_ELEMENT_DESC layout[] = {
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	UINT numElements = ARRAYSIZE(layout);
//
//	// ���̓��C�A�E�g����
//	hr = FRAMEWORK.getDevice()->CreateInputLayout(layout, numElements, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &vertex_layout);
//	assert(SUCCEEDED(hr));
//
//
//	// ���̓��C�A�E�g�ݒ�
//	FRAMEWORK.getImmediateContext()->IASetInputLayout(vertex_layout.Get());
//
//	//���_�o�b�t�@�̍쐬
//	{
//		D3D11_BUFFER_DESC buffer_desc = {};
//		D3D11_SUBRESOURCE_DATA subresource_data = {};
//
//		buffer_desc.ByteWidth = sizeof(vertex) * 2;//��������2
//		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
//		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
//		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		buffer_desc.CPUAccessFlags = 0;
//		buffer_desc.MiscFlags = 0;
//		buffer_desc.StructureByteStride = 0;
//		subresource_data.pSysMem = vertices;
//		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
//		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
//
//		hr = FRAMEWORK.getDevice()->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
//		if (FAILED(hr)) {
//			return hr;
//		}
//	}
//
//	//�萔�o�b�t�@
//	{
//		D3D11_BUFFER_DESC cb;
//		ZeroMemory(&cb, sizeof(D3D11_BUFFER_DESC));
//		cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//		cb.ByteWidth = sizeof(ConstantBufferWorld);
//		cb.CPUAccessFlags = 0;
//		cb.MiscFlags = 0;
//		cb.Usage = D3D11_USAGE_DEFAULT;
//
//		hr = FRAMEWORK.getDevice()->CreateBuffer(&cb, NULL, &ConstantBuffer_w);
//		assert(SUCCEEDED(hr));
//	}
//
//	return hr;
//}

//HRESULT shader::CreatePS_POINT(wchar_t* srcFile, char* functionName)
//{
//	HRESULT hr = S_OK;
//
//	//// �s�N�Z���V�F�[�_
//	//Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
//	//hr = Compile(srcFile, functionName, " ps_5_0", &PSBlob);
//	//assert(SUCCEEDED(hr));
//
//	//// �s�N�Z���V�F�[�_����
//	//hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &ps);
//	////PSBlob->Release();
//	//assert(SUCCEEDED(hr));
//
//	// �s�N�Z���V�F�[�_
//	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
//	hr = Compile(srcFile, functionName, "ps_5_0", &PSBlob);
//	if (FAILED(hr))
//	{
//		return false;
//	}
//
//	// �s�N�Z���V�F�[�_����
//	hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &ps);
//	//PSBlob->Release();
//	assert(SUCCEEDED(hr));
//
//	return hr;
//}

//�R���s���[�g�V�F�[�_�[�P�̍쐬
HRESULT shader::CreateCS(wchar_t* srcFile, char* functionName)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif//defined(DEBUG) || defined(_DEBUG)

	ID3DBlob* pBlob = nullptr;

	// �R���p�C��
	//hr = CompileShaderFromFile(srcFile, functionName, "cs_4_0", &pBlob);
	//if (FAILED(hr))
	//{
	//	return hr;
	//}

	//�V�F�[�_�[�R���p�C��
	hr = Compile(srcFile, functionName, "cs_5_0", &pBlob);//cs_4_0����ύX/2020/7/14

	// �V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &cs);

	//�I�u�W�F�N�g��j��
	pBlob->Release();

	return hr;
}

HRESULT shader::CreateGS(wchar_t* srcFile, char* functionName)
{
	HRESULT hr = E_FAIL;

	ID3D10Blob* pBlob = NULL;

	UINT Flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
#else
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

	//�V�F�[�_�[�R���p�C��
	hr = Compile(srcFile, functionName, "gs_4_0", &pBlob);

	// �R���p�C���ς݃V�F�[�_�[����A�W�I���g���V�F�[�_�[ �I�u�W�F�N�g���쐬����
	// ID3D11Device::CreateGeometryShader
	hr = FRAMEWORK.getDevice()->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &gs);
	if (FAILED(hr))
		assert("Creating GS is Failed");

	hr = S_OK;
	//�I�u�W�F�N�g��j��
	pBlob->Release();
	return hr;
}

HRESULT shader::CreateHS(wchar_t* srcFile, char* functionName)
{
	HRESULT hr = E_FAIL;

	ID3D10Blob* pBlob = NULL;

	UINT Flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
#else
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

	//�V�F�[�_�[�R���p�C��
	hr = Compile(srcFile, functionName, "hs_5_0", &pBlob);

	// �R���p�C���ς݃V�F�[�_�[����A�W�I���g���V�F�[�_�[ �I�u�W�F�N�g���쐬����
	// ID3D11Device::CreateGeometryShader
	hr = FRAMEWORK.getDevice()->CreateHullShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &hs);
	if (FAILED(hr))
		assert("Creating HS is Failed");

	hr = S_OK;
	//�I�u�W�F�N�g��j��
	pBlob->Release();
	return hr;
}

//�e���v���[�g
template <typename T>
void UpdateConstantBuffer(shader* shader, T cb, UINT slot_num)
{
	//�R���X�^���g�o�b�t�@�X�V
	FRAMEWORK.getImmediateContext()->UpdateSubresource(shader->ConstantBufferOriginal.Get(), 0, NULL, &cb, 0, 0);
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(slot_num, 1, shader->ConstantBufferOriginal.GetAddressOf());
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(slot_num, 1, shader->ConstantBufferOriginal.GetAddressOf());
	FRAMEWORK.getImmediateContext()->GSSetConstantBuffers(slot_num, 1, shader->ConstantBufferOriginal.GetAddressOf());
	FRAMEWORK.getImmediateContext()->HSSetConstantBuffers(slot_num, 1, shader->ConstantBufferOriginal.GetAddressOf());
	FRAMEWORK.getImmediateContext()->DSSetConstantBuffers(slot_num, 1, shader->ConstantBufferOriginal.GetAddressOf());
}
template void UpdateConstantBuffer<ConstantBufferForSSR>(shader* shader, ConstantBufferForSSR cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForSSAO>(shader* shader, ConstantBufferForSSAO cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForFog>(shader* shader, ConstantBufferForFog cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForConstantFog>(shader* shader, ConstantBufferForConstantFog cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForHeightFog>(shader* shader, ConstantBufferForHeightFog cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForVoroNoise>(shader* shader, ConstantBufferForVoroNoise cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForWaver>(shader* shader, ConstantBufferForWaver cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForHightToNormal>(shader* shader, ConstantBufferForHightToNormal cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForBlur>(shader* shader, ConstantBufferForBlur cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForVelocityMap>(shader* shader, ConstantBufferForVelocityMap cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForMotionBlur>(shader* shader, ConstantBufferForMotionBlur cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForGaussianBlur>(shader* shader, ConstantBufferForGaussianBlur cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForIrradiation>(shader* shader, ConstantBufferForIrradiation cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForBrightnessExtraction>(shader* shader, ConstantBufferForBrightnessExtraction cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForToneMap>(shader* shader, ConstantBufferForToneMap cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForPBR>(shader* shader, ConstantBufferForPBR cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForPBRv2>(shader* shader, ConstantBufferForPBRv2 cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForPBREX>(shader* shader, ConstantBufferForPBREX cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForEnvironmentMap>(shader* shader, ConstantBufferForEnvironmentMap cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForRayTracing>(shader* shader, ConstantBufferForRayTracing cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForPathTracing1>(shader* shader, ConstantBufferForPathTracing1 cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForBoxVolumeLight>(shader* shader, ConstantBufferForBoxVolumeLight cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForBoxVolumeLightSphere>(shader* shader, ConstantBufferForBoxVolumeLightSphere cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForFieldRaymarching>(shader* shader, ConstantBufferForFieldRaymarching cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForSeaRaymarching>(shader* shader, ConstantBufferForSeaRaymarching cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForCloudRaymarching>(shader* shader, ConstantBufferForCloudRaymarching cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForSkyRaymarching>(shader* shader, ConstantBufferForSkyRaymarching cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForDoF>(shader* shader, ConstantBufferForDoF cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForLensGhost>(shader* shader, ConstantBufferForLensGhost cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForTessellation>(shader* shader, ConstantBufferForTessellation cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForLight>(shader* shader, ConstantBufferForLight cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForNormal>(shader* shader, ConstantBufferForNormal cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForLinerDepth>(shader* shader, ConstantBufferForLinerDepth cb, UINT slot_num);
template void UpdateConstantBuffer<ConstantBufferForShadowMap>(shader* shader, ConstantBufferForShadowMap cb, UINT slot_num);

//��t���V�F�[�_�[�X�V
void UpdateConstantBuffer(shader* shader, ConstantBufferForPhong cb, UINT slot_num)
{
	//�R���X�^���g�o�b�t�@�X�V
	FRAMEWORK.getImmediateContext()->UpdateSubresource(shader->ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(slot_num, 1, shader->ConstantBuffer.GetAddressOf());
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(slot_num, 1, shader->ConstantBuffer.GetAddressOf());
}

void UpdateConstantBuffer(shader* shader, ConstantBufferForDepth cb, UINT slot_num)
{
	//�R���X�^���g�o�b�t�@�X�V
	FRAMEWORK.getImmediateContext()->UpdateSubresource(shader->ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(slot_num, 1, shader->ConstantBuffer.GetAddressOf());
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(slot_num, 1, shader->ConstantBuffer.GetAddressOf());
	FRAMEWORK.getImmediateContext()->GSSetConstantBuffers(slot_num, 1, shader->ConstantBuffer.GetAddressOf());
}



//****************************************************************
//
//
//
//****************************************************************
//------------------------------------------------
//	�L����
//------------------------------------------------
void shader::Activate()
{
	FRAMEWORK.getImmediateContext()->VSSetShader(vs.Get(), NULL, 0);
	FRAMEWORK.getImmediateContext()->PSSetShader(ps.Get(), NULL, 0);
	FRAMEWORK.getImmediateContext()->GSSetShader(gs.Get(), NULL, 0);
	FRAMEWORK.getImmediateContext()->HSSetShader(hs.Get(), NULL, 0);
	FRAMEWORK.getImmediateContext()->DSSetShader(ds.Get(), NULL, 0);
}

void shader::Deactivate()
{
	FRAMEWORK.getImmediateContext()->VSSetShader(NULL, NULL, 0);
	FRAMEWORK.getImmediateContext()->PSSetShader(NULL, NULL, 0);
	FRAMEWORK.getImmediateContext()->GSSetShader(NULL, NULL, 0);
	FRAMEWORK.getImmediateContext()->HSSetShader(NULL, NULL, 0);
	FRAMEWORK.getImmediateContext()->DSSetShader(NULL, NULL, 0);
}

void shader::release()
{
	//if (vs)
	//{
	//	vs->Release(); 
	//	vs = nullptr;
	//}
	//if (ps)
	//{
	//	ps->Release(); 
	//	ps = nullptr;
	//}
	//if (gs)
	//{
	//	gs->Release(); 
	//	gs = nullptr;
	//}
	//if (hs)
	//{
	//	hs->Release();
	//	hs = nullptr;
	//}
	//if (ds)
	//{
	//	ds->Release(); 
	//	ds = nullptr;
	//}
	//if (cs)
	//{
	//	cs->Release(); 
	//	cs = nullptr;
	//}
	//if (vertex_layout)
	//{
	//	vertex_layout->Release(); 
	//	vertex_layout = nullptr;
	//}
}


//---------------------------------------------------------------------------------------------
//      �R���X�^���g�o�b�t�@�쐬
//---------------------------------------------------------------------------------------------
bool CreateConstantBuffer(ID3D11Buffer**ppCB, u_int size)
{
	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = size;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = FRAMEWORK.getDevice()->CreateBuffer(&bd, NULL, ppCB);
	assert(SUCCEEDED(hr));

	return true;
}


//------------------------------
//      �\�����o�b�t�@�𐶐�
//------------------------------
HRESULT CreateStructuredBuffer(ID3D11Device* pDevice, UINT elementSize, UINT count, void* pInitData, ID3D11Buffer**  ppBufferOut)
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
HRESULT CreateSRVBuffer(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
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
HRESULT CreateRawBuffer(ID3D11Device* pDevice, UINT size, void* pInitData, ID3D11Buffer** ppBufferOut)
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
HRESULT CreateUAVBuffer(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
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
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS; /*DXGI_FORMAT_R8G8B8A8_TYPELESS*/
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

//----------------------------------------------------------------------------------------------
//      �R���s���[�g�V�F�[�_�����s
//----------------------------------------------------------------------------------------------
void shader::RunComputeShader
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
	FRAMEWORK.getImmediateContext()->CSSetShader(cs.Get(), nullptr, 0);
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
	ID3D11ShaderResourceView*   pNullSRVs[2] = { nullptr, nullptr };
	ID3D11Buffer*               pNullCBs[1] = { nullptr };

	FRAMEWORK.getImmediateContext()->CSSetShader(nullptr, nullptr, 0);
	FRAMEWORK.getImmediateContext()->CSSetUnorderedAccessViews(0, 1, pNullUAVs, nullptr);
	FRAMEWORK.getImmediateContext()->CSSetShaderResources(0, 2, pNullSRVs);
	FRAMEWORK.getImmediateContext()->CSSetConstantBuffers(0, 1, pNullCBs);
}

//----------------------------------------------------------------------------------------------
//      �o�b�t�@�̓��e���R�s�[
//----------------------------------------------------------------------------------------------
ID3D11Buffer* CreateAndCopyToBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer)
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

//CS�Ōv�Z����1���_����GS�łS���_�ɂ��ăp�[�e�B�N���ɂ���
//void shader::CreateGPUParticleShader()
//{
//	//VSGSCSPS�S�����
//
//	HRESULT hr = S_OK;
//
//	//VS
//	{
//		Microsoft::WRL::ComPtr<ID3DBlob> VSBlob = NULL;
//		// ���_�V�F�[�_
//		hr = Compile(L"create_particle_vs.hlsl", "VSMain", "vs_4_0", VSBlob.GetAddressOf());
//		assert(SUCCEEDED(hr));
//
//
//		// ���_�V�F�[�_����
//		hr = FRAMEWORK.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &vs);
//		assert(SUCCEEDED(hr));
//
//		// ���̓��C�A�E�g
//		D3D11_INPUT_ELEMENT_DESC layout[] = {
//			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		};
//		UINT numElements = ARRAYSIZE(layout);
//
//		// ���̓��C�A�E�g����
//		hr = FRAMEWORK.getDevice()->CreateInputLayout(layout, numElements, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &vertex_layout);
//		assert(SUCCEEDED(hr));
//
//		// ���̓��C�A�E�g�ݒ�
//		FRAMEWORK.getImmediateContext()->IASetInputLayout(vertex_layout.Get());
//	}
//
//	//PS
//	{
//		// �s�N�Z���V�F�[�_
//		Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
//		hr = Compile(L"create_particle_ps.hlsl", "PSMain", " ps_4_0", &PSBlob);
//		assert(SUCCEEDED(hr));
//
//		// �s�N�Z���V�F�[�_����
//		hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, &ps);
//		//PSBlob->Release();
//		assert(SUCCEEDED(hr));
//	}
//
//	//�R���X�^���g�o�b�t�@�[�쐬�@�ϊ��s��n���p
//	D3D11_BUFFER_DESC cb;
//	ZeroMemory(&cb, sizeof(D3D11_BUFFER_DESC));
//	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	cb.ByteWidth = sizeof(ConstantBufferWorld);
//	cb.CPUAccessFlags = 0;
//	cb.MiscFlags = 0;
//	cb.Usage = D3D11_USAGE_DEFAULT;
//
//	hr = FRAMEWORK.getDevice()->CreateBuffer(&cb, NULL, &ConstantBuffer_w);
//	assert(SUCCEEDED(hr));
//
//
//	//GS
//	{
//		ID3D10Blob* pBlob = NULL;
//		//�V�F�[�_�[�R���p�C��
//		hr = Compile(L"create_particle_gs.hlsl", "GSMain", "gs_4_0", &pBlob);
//		// �R���p�C���ς݃V�F�[�_�[����A�W�I���g���V�F�[�_�[ �I�u�W�F�N�g���쐬����
//		// ID3D11Device::CreateGeometryShader
//		hr = FRAMEWORK.getDevice()->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &gs);
//		if (FAILED(hr))
//			assert("Creating GS is Failed");
//		hr = S_OK;
//		//�I�u�W�F�N�g��j��
//		pBlob->Release();
//	}
//
//	//CS
//	{
//		ID3DBlob* pBlob = nullptr;
//		//�V�F�[�_�[�R���p�C��
//		hr = Compile(L"particle_cs.hlsl", "CSMain", "cs_4_0", &pBlob);
//		// �V�F�[�_����
//		hr = FRAMEWORK.getDevice()->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &cs);
//		//�I�u�W�F�N�g��j��
//		pBlob->Release();
//	}
//
//	//���X�^���C�U�[�X�e�[�g�I�u�W�F�N�g
//	D3D11_RASTERIZER_DESC rasterizer_desc;
//	rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
//	rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
//	rasterizer_desc.FrontCounterClockwise = FALSE;
//	rasterizer_desc.DepthBias = 0;
//	rasterizer_desc.DepthBiasClamp = 0;
//	rasterizer_desc.SlopeScaledDepthBias = 0;
//	rasterizer_desc.DepthClipEnable = TRUE;
//	rasterizer_desc.ScissorEnable = FALSE;
//	rasterizer_desc.MultisampleEnable = FALSE;
//	rasterizer_desc.AntialiasedLineEnable = FALSE;
//	hr = FRAMEWORK.getDevice()->CreateRasterizerState(&rasterizer_desc, &rasterizer_states);
//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//
//	//�T���v���[�̋L�q
//	D3D11_SAMPLER_DESC sampler_desc;
//	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //D3D11_FILTER_ANISOTROPIC
//	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
//	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
//	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
//	sampler_desc.MipLODBias = 0;
//	sampler_desc.MaxAnisotropy = 16;
//	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
//	memcpy(sampler_desc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
//	sampler_desc.MinLOD = 0;
//	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
//
//	//�T���v���[�X�e�[�g����
//	hr = FRAMEWORK.getDevice()->CreateSamplerState(&sampler_desc, &sampler_state);
//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//	//�[�x�X�e���V���X�e�[�g�I�u�W�F�N�g
//	D3D11_DEPTH_STENCIL_DESC dsDesc;
//	dsDesc.DepthEnable = TRUE;
//	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
//	dsDesc.StencilEnable = FALSE;
//	dsDesc.StencilReadMask = 0xFF;
//	dsDesc.StencilWriteMask = 0xFF;
//	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
//	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
//	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//
//	hr = FRAMEWORK.getDevice()->CreateDepthStencilState(&dsDesc, &depth_stencil_state);
//	if (FAILED(hr)) {
//		return;
//	}
//
//	//���_�o�b�t�@�̍쐬
//	{
//		D3D11_BUFFER_DESC buffer_desc = {};
//		D3D11_SUBRESOURCE_DATA subresource_data = {};
//
//		buffer_desc.ByteWidth = sizeof(vertex) * NUM_ELEMENTS;
//		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
//		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
//		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		buffer_desc.CPUAccessFlags = 0;
//		buffer_desc.MiscFlags = 0;
//		buffer_desc.StructureByteStride = 0;
//		subresource_data.pSysMem = Buf;
//		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
//		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
//
//		hr = FRAMEWORK.getDevice()->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
//		if (FAILED(hr)) {
//			return;
//		}
//	}
//
//	//�C���f�b�N�X�o�b�t�@
//	{
//		D3D11_BUFFER_DESC buffer_desc = {};
//		D3D11_SUBRESOURCE_DATA subresource_data = {};
//
//		buffer_desc.ByteWidth = sizeof(u_int) * NUM_ELEMENTS;
//		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
//		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
//		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//		buffer_desc.CPUAccessFlags = 0;
//		buffer_desc.MiscFlags = 0;
//		buffer_desc.StructureByteStride = 0;
//		subresource_data.pSysMem = Buf;
//		subresource_data.SysMemPitch = 0; //Not use for index buffers.
//		subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
//		hr = FRAMEWORK.getDevice()->CreateBuffer(&buffer_desc, &subresource_data, &index_buffer);
//		if (FAILED(hr)) {
//			return;
//		}
//	}
//}

////CS�X�V
//void shader::UpdateGPUParticleShader()
//{
//	{
//		// �\�����o�b�t�@�𐶐�//�f�[�^�̓��͂Əo�͂Ɏg��
//		CreateStructuredBuffer(FRAMEWORK.getDevice(), sizeof(vertex), NUM_ELEMENTS, &Buf[0], &g_pBuf);//���͐�
//		CreateStructuredBuffer(FRAMEWORK.getDevice(), sizeof(vertex), NUM_ELEMENTS, nullptr, &g_pBufResult);//�o�͐悾����null�ŗǂ�
//
//		//�\�����o�b�t�@����o�^���邽�߂�SRV��UAV�ɕϊ�
//		CreateSRVBuffer(FRAMEWORK.getDevice(), g_pBuf, &g_pBufSRV);
//		CreateUAVBuffer(FRAMEWORK.getDevice(), g_pBufResult, &g_pBufUAV);
//
//		//ID3D11ShaderResourceView* pSRVs = g_pBufSRV;
//		RunComputeShader(1, &g_pBufSRV, nullptr, nullptr, 0, g_pBufUAV, 64, 1, 1);
//
//		{
//			// �o�b�t�@�𐶐��ƃR�s�[.
//			ID3D11Buffer* pBufDbg = CreateAndCopyToBuffer(FRAMEWORK.getDevice(), FRAMEWORK.getImmediateContext(), g_pBufResult);
//
//			D3D11_MAPPED_SUBRESOURCE subRes;
//			//BufType* pBufType;
//
//			// �}�b�v
//			FRAMEWORK.getImmediateContext()->Map(pBufDbg, 0, D3D11_MAP_READ, 0, &subRes);
//			pData = (vertex*)subRes.pData;
//			// �A���}�b�v
//			FRAMEWORK.getImmediateContext()->Unmap(pBufDbg, 0);
//
//			// �������
//			pBufDbg->Release();
//		}
//
//		//�f�[�^�X�V
//		for (int i = 0; i < NUM_ELEMENTS; ++i)
//		{
//			Buf[i].position = pData[i].position;
//			Buf[i].color = pData[i].color;
//		}
//	}
//}
//
////���o�����p�[�e�B�N����`��
//void shader::RenderGPUParticleShader(DirectX::XMFLOAT4X4 world, Camera* m_camera)
//{
//	u_int stride = sizeof(vertex);
//	u_int offset = 0;
//
//	//���[���h�r���[�v���W�F�N�V�����쐬
//	DirectX::XMMATRIX wrl = DirectX::XMLoadFloat4x4(&world);
//	DirectX::XMFLOAT4X4 world_view_projection;
//	DirectX::XMMATRIX wvp = wrl*DirectX::XMLoadFloat4x4(&m_camera->GetView())*DirectX::XMLoadFloat4x4(&m_camera->GetProjection());
//	DirectX::XMStoreFloat4x4(&world_view_projection, wvp);
//
//	//�萔�o�b�t�@�ݒ�
//	ConstantBufferWorld cb;
//	cb.world = DirectX::XMLoadFloat4x4(&world);
//	cb.matWVP = wvp;
//	FRAMEWORK.getImmediateContext()->UpdateSubresource(ConstantBuffer_w.Get(), 0, NULL, &cb, 0, 0);//Create�����H
//	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(0, 1, ConstantBuffer_w.());
//	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(0, 1, ConstantBuffer_w.());
//	FRAMEWORK.getImmediateContext()->GSSetConstantBuffers(0, 1, ConstantBuffer_w.());
//
//	//�V�F�[�_�[�Z�b�g
//	Activate(FRAMEWORK.getImmediateContext());
//
//	static blender* blend = new blender(FRAMEWORK.getInstance().getDevice());
//
//	FRAMEWORK.getImmediateContext()->IASetVertexBuffers(0, 1, vertex_buffer.(), &stride, &offset);
//	FRAMEWORK.getImmediateContext()->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//	FRAMEWORK.getImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	FRAMEWORK.getImmediateContext()->IASetInputLayout(vertex_layout);
//	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(depth_stencil_state.Get(), 1);
//	FRAMEWORK.getImmediateContext()->OMSetBlendState(blend->states[blender::BS_ALPHA].Get(), nullptr, 0xFFFFFFFF);
//	FRAMEWORK.getImmediateContext()->RSSetState(rasterizer_states.Get());
//
//	FRAMEWORK.getImmediateContext()->DrawIndexed(NUM_ELEMENTS , 0, 0);
//}

//�K�E�X�̏d�݂��v�Z
float GaussianDistribution(DirectX::XMFLOAT2 &pos, float rho)
{
	return exp(-(pos.x * pos.x + pos.y * pos.y) / (2.0f * rho * rho));
}


//�K�E�V�A���u���[�p�����[�^���v�Z
ConstantBufferForBlur CalcBlurParam(int width, int height, DirectX::XMFLOAT2 dir, float deviation)
{
	ConstantBufferForBlur result;
	result.SampleCount.x = 14;
	auto tu = 1.0f / float(width);
	auto tv = 1.0f / float(height);
	result.Offset[0].z = GaussianDistribution(DirectX::XMFLOAT2(0.0f, 0.0f), deviation);
	auto total_weight = result.Offset[0].z;

	result.Offset[0].x = 0.0f;
	result.Offset[0].y = 0.0f;

	for (auto i = 1; i < 8; ++i)
	{
		result.Offset[i].x = dir.x * i * tu;
		result.Offset[i].y = dir.y * i * tv;
		result.Offset[i].z = GaussianDistribution(DirectX::XMFLOAT2(dir.x * float(i), dir.y*float(i)), deviation);
		total_weight += result.Offset[i].z * 2.0f;
	}

	for (auto i = 0; i < 8; ++i)
	{
		result.Offset[i].z /= total_weight;
	}
	for (auto i = 8; i < 15; ++i)
	{
		result.Offset[i].x = -result.Offset[i - 7].x;
		result.Offset[i].y = -result.Offset[i - 7].y;
		result.Offset[i].z = result.Offset[i - 7].z;
	}

	return result;
}

//�K�E�X�u���[�v�Z//(���U,�E�F�C�g�i�[�p�o�b�t�@,�E�F�C�g�̐�)
void CalcGaussWeights(float dispersion, float *weights, int weight_num)
{
	float total = 0.0f;

	for (int i = 0; i < weight_num; ++i)
	{
		float pos = 1.0f + 2.0f * (float)i;
		weights[i] = expf(-0.5f * (pos * pos) / dispersion);
		total += 2.0f * weights[i];
	}

	float invTotal = 1.0f / total;
	for (int i = 0; i < weight_num; ++i)
	{
		weights[i] *= invTotal;
	}
}

//�_�E���T���v���p�e�N�X�`���쐬
void CreateDownSampleTextures(std::unique_ptr<Texture> texes[], std::unique_ptr<Texture> temp[], int size)
{
	for (int i = 0; i < size; i++)
	{
		int num = pow(2, i);
		texes[i] = std::make_unique<Texture>();
		texes[i]->Create(sCREEN_WIDTH / num, sCREEN_HEIGHT / num);//�e�N�X�`���̑傫�������������Ă���
		temp[i] = std::make_unique<Texture>();
		temp[i]->Create(sCREEN_WIDTH / num, sCREEN_HEIGHT / num);//�e�N�X�`���̑傫�������������Ă���
	}
}

//�_�E���T���v�����O�����摜�Ƀu���[�����ĕԂ�
void DownSamplingAndBlur(std::unique_ptr<Texture> texes[], std::unique_ptr<Texture> temp[], Texture* src/*�u���[��������摜*/, sprite* spr, std::unique_ptr<shader> *blur, frame_buffer* f_buffer, ConstantBufferForGaussianBlur cbgb, int size)
{
	for (int i = 0; i < size; i++)
	{
		int num = pow(2, i);
		//�摜���Z�b�g//���摜�ŏ㏑�������Ⴄ�̂ŕs�v
		//texes[i]->SetShaderResourceView(src->GetShaderResourceView());
		//�摜��1280,720�ɖ߂��u���[�����ĕ`��
		//FRAMEWORK.getImmediateContext()->OMSetBlendState(blend.states[blender::BS_ADD].Get(), nullptr, 0xFFFFFFFF);

		texes[i]->SetSize(DirectX::XMFLOAT2(sCREEN_WIDTH, sCREEN_HEIGHT));
		temp[i]->SetSize(DirectX::XMFLOAT2(sCREEN_WIDTH, sCREEN_HEIGHT));

		UpdateConstantBuffer(blur[0].get(), cbgb);
		f_buffer->swapRenderTargetViewBegin(temp[i].get(), DirectX::XMFLOAT4(1, 1, 1, 1), FRAMEWORK.BS::BS_ALPHA);
		spr->render(blur[0].get(), src, 0, 0, sCREEN_WIDTH / num, sCREEN_HEIGHT / num, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
		f_buffer->swapRenderTargetViewEnd();

		f_buffer->swapRenderTargetViewBegin(texes[i].get(), DirectX::XMFLOAT4(1, 1, 1, 1), FRAMEWORK.BS::BS_ALPHA);
		spr->render(blur[1].get(), temp[i].get(), 0, 0, sCREEN_WIDTH / num, sCREEN_HEIGHT / num, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
		f_buffer->swapRenderTargetViewEnd();

	}
}

void ReleaseDownSampleTextures(Texture* texes[])
{
	for (int i = 0; i < sizeof(texes) / sizeof(Texture*); i++)
	{
		delete texes[i];
		texes[i] = nullptr;
	}
}

//�Â��s��Z�b�g
void setOldMBMatrix(MotionBlurMatrix &mbm)
{
	mbm.OldMatWorld = mbm.NewMatWorld;
	mbm.OldMatView = mbm.NewMatView;
	mbm.OldMatProjection = mbm.NewMatProjection;
	for (int i = 0; i < 4; i++)
	{
		mbm.OldBoneTransforms[i] = mbm.NewBoneTransforms[i];
	}
}