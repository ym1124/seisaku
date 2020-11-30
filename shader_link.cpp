#include "..\Header\shader_link.h"
#include "..\Header\framework.h"

BASE_SHADING::BASE_SHADING()
{
	for (int i = 0; i<_countof(m_pConstantBuffers); i++)
		m_pConstantBuffers[i] = NULL;
	m_pPSClassLinkage = NULL;
}

BASE_SHADING::~BASE_SHADING()
{
	for (int i = 0; i<_countof(m_pConstantBuffers); i++)
		delete m_pConstantBuffers[i];
	delete m_pPSClassLinkage;
}

//------------------------------------------------
//	�V�F�[�_�[�P�̃R���p�C��
//------------------------------------------------
HRESULT BASE_SHADING::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
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

// ������
HRESULT BASE_SHADING::Init(ID3D11Device* pD3DDevice,WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob = NULL;
	// ���_�V�F�[�_
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));


	// ���_�V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
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
	hr = FRAMEWORK.getDevice()->CreateInputLayout(layout, numElements, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), VertexLayout.GetAddressOf());
	assert(SUCCEEDED(hr));


	// ���̓��C�A�E�g�ݒ�
	FRAMEWORK.getImmediateContext()->IASetInputLayout(VertexLayout.Get());


	// �s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// �s�N�Z���V�F�[�_����
	hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
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
	if (FAILED(hr)) goto EXIT;

	// ���_�V�F�[�_�[�Ŏg�p����萔�o�b�t�@�I�u�W�F�N�g���쐬����
	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.ByteWidth = sizeof(CBUFFER1);        // �o�b�t�@�T�C�Y
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // ���\�[�X�g�p�@����肷��
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// �o�b�t�@�̎��
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU �A�N�Z�X
	BufferDesc.MiscFlags = 0;                         // ���̑��̃t���O���ݒ肵�Ȃ�
	hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, &m_pConstantBuffers[1]);
	if (FAILED(hr)) goto EXIT;

	hr = S_OK;

EXIT:
	return hr;
}

//HRESULT BASE_SHADING::Init(ID3D11Device* pD3DDevice, TCHAR pSrcFile[], LPCSTR pVSMain, LPCSTR pPSMain)
//{
//	HRESULT hr = E_FAIL;
//
//	ID3D10Blob* pVSBlob = NULL, *pPSBlob = NULL;
//
//	// �s����D��Őݒ肵�A�Â��`���̋L�q�������Ȃ��悤�ɂ���
//	UINT Flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
//	// �œK�����x����ݒ肷��
//#if defined(DEBUG) || defined(_DEBUG)
//	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
//#else
//	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
//#endif
//
//	// ���_�V�F�[�_�[���R���p�C������
//	hr = D3DX11CompileFromFile(pSrcFile, NULL, NULL, pVSMain, "vs_5_0", Flag1, 0, NULL, &pVSBlob, NULL, NULL);
//	if (FAILED(hr)) goto EXIT;
//
//	// �s�N�Z���V�F�[�_�[���R���p�C������
//	hr = D3DX11CompileFromFile(pSrcFile, NULL, NULL, pPSMain, "ps_5_0", Flag1, 0, NULL, &pPSBlob, NULL, NULL);
//	if (FAILED(hr)) goto EXIT;
//
//	// ����������
//	hr = Init(pD3DDevice
//		, reinterpret_cast<BYTE*>(pVSBlob->GetBufferPointer()), pVSBlob->GetBufferSize()
//		, reinterpret_cast<BYTE*>(pPSBlob->GetBufferPointer()), pPSBlob->GetBufferSize()
//		);
//	if (FAILED(hr)) goto EXIT;
//
//	hr = S_OK;
//EXIT:
//	SAFE_RELEASE(pVSBlob);
//	SAFE_RELEASE(pPSBlob);
//
//	return hr;
//}

// ���_�V�F�[�_�[�p�̒萔�o�b�t�@��ݒ肷��
HRESULT BASE_SHADING::SetCBVertexShader(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMMATRIX* p_matWVP)
{
	HRESULT hr = E_FAIL;

	if (pD3DDeviceContext == NULL) goto EXIT;
	if (p_matWVP == NULL) goto EXIT;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CBUFFER1 cbuffer;

	/*hr = pD3DDeviceContext->Map(m_pConstantBuffers[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) goto EXIT;
	cbuffer = (CBUFFER1*)(mappedResource.pData);

	::CopyMemory(&cbuffer->matWVP, p_matWVP, sizeof(D3DXMATRIX));

	pD3DDeviceContext->Unmap(m_pConstantBuffers[1], 0);*/

	cbuffer.matWVP = *p_matWVP;
	FRAMEWORK.getImmediateContext()->UpdateSubresource(m_pConstantBuffers[1], 0, NULL, &cbuffer, 0, 0);

	hr = S_OK;
EXIT:
	return hr;
}

// �n�[�t�����o�[�g�p�̒萔�o�b�t�@��ݒ�
HRESULT BASE_SHADING::SetCBHalfLambert(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight, ID3D11ShaderResourceView* pDecalMap)
{
	HRESULT hr = E_FAIL;

	if (pD3DDeviceContext == NULL) goto EXIT;
	if (p_vecLight == NULL) goto EXIT;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CBUFFER0 cbuffer;

	/*hr = pD3DDeviceContext->Map(m_pConstantBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) goto EXIT;
	cbuffer = (CBUFFER0*)(mappedResource.pData);

	::CopyMemory(&cbuffer->vecLight0, p_vecLight, sizeof(D3DXVECTOR4));

	pD3DDeviceContext->Unmap(m_pConstantBuffers[0], 0);*/

	//�R���X�^���g�o�b�t�@�ɐݒ�
	cbuffer.vecLight0 = *p_vecLight;
	FRAMEWORK.getImmediateContext()->UpdateSubresource(m_pConstantBuffers[0], 0, NULL, &cbuffer, 0, 0);

	// �V�F�[�_�[���\�[�X�r���[��ݒ�
	pD3DDeviceContext->PSSetShaderResources(0, 1, &pDecalMap);

	hr = S_OK;
EXIT:
	return hr;
}

// �t�H���V�F�[�f�B���O�p�̒萔�o�b�t�@��ݒ�
HRESULT BASE_SHADING::SetCBPhongShading(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight, DirectX::XMFLOAT4* p_EyePos, ID3D11ShaderResourceView* pDecalMap)
{
	HRESULT hr = E_FAIL;

	if (pD3DDeviceContext == NULL) goto EXIT;
	if (p_vecLight == NULL) goto EXIT;
	if (p_EyePos == NULL) goto EXIT;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CBUFFER0 cbuffer;

	//hr = pD3DDeviceContext->Map(m_pConstantBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//if (FAILED(hr)) goto EXIT;
	//cbuffer = (CBUFFER0*)(mappedResource.pData);

	//::CopyMemory(&cbuffer->vecLight1, p_vecLight, sizeof(D3DXVECTOR4));
	//::CopyMemory(&cbuffer->EyePos, p_EyePos, sizeof(D3DXVECTOR4));

	//pD3DDeviceContext->Unmap(m_pConstantBuffers[0], 0);


	//�R���X�^���g�o�b�t�@�ɐݒ�
	cbuffer.vecLight1 = *p_vecLight;
	cbuffer.EyePos = *p_EyePos;
	FRAMEWORK.getImmediateContext()->UpdateSubresource(m_pConstantBuffers[0], 0, NULL, &cbuffer, 0, 0);
	// �V�F�[�_�[���\�[�X�r���[��ݒ�
	pD3DDeviceContext->PSSetShaderResources(0, 1, &pDecalMap);

	hr = S_OK;
EXIT:
	return hr;
}

// �Z���V�F�[�f�B���O�p�̒萔�o�b�t�@��ݒ�
HRESULT BASE_SHADING::SetCBCelShading(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight, ID3D11ShaderResourceView* pDecalMap, ID3D11ShaderResourceView* pCelMap)
{
	HRESULT hr = E_FAIL;

	if (pD3DDeviceContext == NULL) goto EXIT;
	if (p_vecLight == NULL) goto EXIT;
	if (pCelMap == NULL) goto EXIT;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CBUFFER0 cbuffer;

	/*hr = pD3DDeviceContext->Map(m_pConstantBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) goto EXIT;
	cbuffer = (CBUFFER0*)(mappedResource.pData);

	::CopyMemory(&cbuffer->vecLight2, p_vecLight, sizeof(D3DXVECTOR4));

	pD3DDeviceContext->Unmap(m_pConstantBuffers[0], 0);*/

	// �V�F�[�_�[���\�[�X�r���[��ݒ�
	pD3DDeviceContext->PSSetShaderResources(0, 1, &pDecalMap);
	pD3DDeviceContext->PSSetShaderResources(1, 1, &pCelMap);

	hr = S_OK;
EXIT:
	return hr;
}

HRESULT BASE_SHADING::Begin(ID3D11DeviceContext* pD3DDeviceContext, LPCSTR pClassInstanceName)
{
	HRESULT hr = E_FAIL;

	// �w�肳�ꂽ HLSL �N���X��\���N���X�C���X�^���X �I�u�W�F�N�g���擾����
	// http://msdn.microsoft.com/ja-jp/library/ee419541(v=vs.85).aspx
	hr = m_pPSClassLinkage->GetClassInstance(pClassInstanceName, 0, &m_pClassInstance);
	if (FAILED(hr)) goto EXIT;

	// ���C�A�E�g�ݒ�
	pD3DDeviceContext->IASetInputLayout(VertexLayout.Get());

	// ���_�V�F�[�_�[���f�o�C�X�ɐݒ肷��B
	pD3DDeviceContext->VSSetShader(VS.Get(), NULL, 0);

	// ���_�V�F�[�_�[�ɒ萔�o�b�t�@��ݒ肷��
	pD3DDeviceContext->VSSetConstantBuffers(1, 1, &m_pConstantBuffers[1]);

	// �n���V�F�[�_�[�𖳌��ɂ���B
	pD3DDeviceContext->HSSetShader(NULL, NULL, 0);

	// �h���C���V�F�[�_�[�𖳌��ɂ���B
	pD3DDeviceContext->DSSetShader(NULL, NULL, 0);

	// �W�I���g���V�F�[�_�[�𖳌��ɂ���B
	pD3DDeviceContext->GSSetShader(NULL, NULL, 0);

	// �s�N�Z���V�F�[�_�[�𓮓I�V�F�[�_�[�����N�Ƃ��Đݒ肷��
	pD3DDeviceContext->PSSetShader(PS.Get(), &m_pClassInstance, 1);

	// �s�N�Z���V�F�[�_�[�ɒ萔�o�b�t�@��ݒ肷��
	pD3DDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffers[0]);

	// �R���s���[�g�V�F�[�_�[�𖳌��ɂ���B
	pD3DDeviceContext->CSSetShader(NULL, NULL, 0);

	hr = S_OK;
EXIT:
	return hr;
}

void BASE_SHADING::End()
{
	delete m_pClassInstance;
}
