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

//// 初期化
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
//	// 頂点シェーダーの作成
//	// *****************************************************************************************************************
//
//	hr = pD3DDevice->CreateVertexShader(pVS, VSSize, NULL, &m_pVertexShader);
//	if (FAILED(hr)) goto EXIT;
//
//	// 入力レイアウトは固定設定にしておく
//	D3D11_INPUT_ELEMENT_DESC layout[] = {
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	hr = pD3DDevice->CreateInputLayout(layout, _countof(layout), pVS, VSSize, &m_pLayout);
//	if (FAILED(hr)) goto EXIT;
//
//	// *****************************************************************************************************************
//	// ピクセルェーダーの作成
//	// *****************************************************************************************************************
//
//	// ピクセルシェーダー用の動的シェーダー リンクを有効にするクラス リンク ライブラリを作成する
//	// http://msdn.microsoft.com/ja-jp/library/ee419783(v=vs.85).aspx
//	pD3DDevice->CreateClassLinkage(&m_pPSClassLinkage);
//
//	hr = pD3DDevice->CreatePixelShader(pPS, PSSize, m_pPSClassLinkage, &m_pPixelShader);
//	if (FAILED(hr)) goto EXIT;
//
//	// *****************************************************************************************************************
//	// 定数バッファを作成
//	// *****************************************************************************************************************
//
//	D3D11_BUFFER_DESC BufferDesc;
//
//	// 動的シェーダーリンケージ内で使用する定数バッファオブジェクトを作成する
//	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
//	BufferDesc.ByteWidth = sizeof(CBUFFER0);        // バッファサイズ
//	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // リソース使用法を特定する
//	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// バッファの種類
//	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU アクセス
//	BufferDesc.MiscFlags = 0;                         // その他のフラグも設定しない
//	hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, &m_pConstantBuffers[0]);
//	if (FAILED(hr)) goto EXIT;
//
//	// 頂点シェーダーで使用する定数バッファオブジェクトを作成する
//	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
//	BufferDesc.ByteWidth = sizeof(CBUFFER1);        // バッファサイズ
//	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // リソース使用法を特定する
//	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// バッファの種類
//	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU アクセス
//	BufferDesc.MiscFlags = 0;                         // その他のフラグも設定しない
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

	// 行列を列優先で設定し、古い形式の記述を許可しないようにする
	UINT Flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
	// 最適化レベルを設定する
#if defined(DEBUG) || defined(_DEBUG)
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
#else
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

	// 頂点シェーダーをコンパイルする
	//hr = D3DX11CompileFromFile(pSrcFile, NULL, NULL, pVSMain, "vs_5_0", Flag1, 0, NULL, &pVSBlob, NULL, NULL);
	//if (FAILED(hr)) goto EXIT;

	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob = NULL;
	// 頂点シェーダ
	hr = shader::Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 頂点シェーダ生成
	hr = FRAMEWORK.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &m_pVertexShader);
	assert(SUCCEEDED(hr));

	// 入力レイアウトは固定設定にしておく
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	//hr = pD3DDevice->CreateInputLayout(layout, _countof(layout), pVS, VSSize, &m_pLayout);
	//if (FAILED(hr)) goto EXIT;
	hr = FRAMEWORK.getDevice()->CreateInputLayout(layout, _countof(layout), VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &m_pLayout);
	assert(SUCCEEDED(hr));

	// ピクセルシェーダーをコンパイルする
	//hr = D3DX11CompileFromFile(pSrcFile, NULL, NULL, pPSMain, "ps_5_0", Flag1, 0, NULL, &pPSBlob, NULL, NULL);
	//if (FAILED(hr)) goto EXIT;

	// ピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
	hr = shader::Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// ピクセルシェーダー用の動的シェーダー リンクを有効にするクラス リンク ライブラリを作成する
	pD3DDevice->CreateClassLinkage(&m_pPSClassLinkage);

	// ピクセルシェーダ生成
	hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), m_pPSClassLinkage, &m_pPixelShader);
	//PSBlob->Release();
	assert(SUCCEEDED(hr));

	// *****************************************************************************************************************
	// 定数バッファを作成
	// *****************************************************************************************************************

	D3D11_BUFFER_DESC BufferDesc;

	// 動的シェーダーリンケージ内で使用する定数バッファオブジェクトを作成する
	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.ByteWidth = sizeof(CBUFFER0);        // バッファサイズ
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // リソース使用法を特定する
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// バッファの種類
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU アクセス
	BufferDesc.MiscFlags = 0;                         // その他のフラグも設定しない
	hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, &m_pConstantBuffers[0]);
	if (FAILED(hr))
		return hr;

	// 頂点シェーダーで使用する定数バッファオブジェクトを作成する
	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.ByteWidth = sizeof(CBUFFER1);        // バッファサイズ
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // リソース使用法を特定する
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// バッファの種類
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU アクセス
	BufferDesc.MiscFlags = 0;                         // その他のフラグも設定しない
	hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, &m_pConstantBuffers[1]);
	if (FAILED(hr))
		return hr;

	hr = S_OK;

	return hr;
}

// 頂点シェーダー用の定数バッファを設定する
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

// ランバート用の定数バッファを設定
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

	//// シェーダーリソースビューを設定
	//pD3DDeviceContext->PSSetShaderResources(0, 1, &pDecalMap);

	hr = S_OK;
EXIT:
	return hr;
}

// ハーフランバート用の定数バッファを設定
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

	//// シェーダーリソースビューを設定
	//pD3DDeviceContext->PSSetShaderResources(0, 1, &pDecalMap);

	hr = S_OK;
EXIT:
	return hr;
}

// フォンシェーディング用の定数バッファを設定
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

	//// シェーダーリソースビューを設定
	//pD3DDeviceContext->PSSetShaderResources(0, 1, &pDecalMap);

	hr = S_OK;
EXIT:
	return hr;
}

// シェーディング用の定数バッファを設定
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

	////// シェーダーリソースビューを設定
	////pD3DDeviceContext->PSSetShaderResources(0, 1, &pDecalMap);
	////pD3DDeviceContext->PSSetShaderResources(1, 1, &pCelMap);

	hr = S_OK;
//EXIT:
	return hr;
}

HRESULT BASE_SHADING::Begin(ID3D11DeviceContext* pD3DDeviceContext, LPCSTR pClassInstanceName)
{
	HRESULT hr = E_FAIL;

	// 指定された HLSL クラスを表すクラスインスタンス オブジェクトを取得する
	// http://msdn.microsoft.com/ja-jp/library/ee419541(v=vs.85).aspx
	//ID3D11ClassLinkage::GetClassInstance という関数は指定されたHLSLクラスを表すクラスインスタンスオブジェクトを取得するための関数であるが、 
	//この関数はクラス内にデータメンバー(メンバ変数)が存在する場合に使用でき、存在しない場合はID3D11ClassLinkage::CreateClassInstanceを使用する必要がある(コンパイルは通るがPSが反映されなくなる)
	hr = m_pPSClassLinkage->GetClassInstance(pClassInstanceName, 0, &m_pClassInstance);
	if (FAILED(hr)) goto EXIT;

	// レイアウト設定
	pD3DDeviceContext->IASetInputLayout(m_pLayout);

	// 頂点シェーダーをデバイスに設定する。
	pD3DDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);

	// 頂点シェーダーに定数バッファを設定する
	pD3DDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffers[1]);

	// ハルシェーダーを無効にする。
	pD3DDeviceContext->HSSetShader(NULL, NULL, 0);

	// ドメインシェーダーを無効にする。
	pD3DDeviceContext->DSSetShader(NULL, NULL, 0);

	// ジオメトリシェーダーを無効にする。
	pD3DDeviceContext->GSSetShader(NULL, NULL, 0);

	// ピクセルシェーダーを動的シェーダーリンクとして設定する
	pD3DDeviceContext->PSSetShader(m_pPixelShader, &m_pClassInstance, 1);

	// ピクセルシェーダーに定数バッファを設定する
	pD3DDeviceContext->PSSetConstantBuffers(2, 1, &m_pConstantBuffers[0]);

	// コンピュートシェーダーを無効にする。
	pD3DDeviceContext->CSSetShader(NULL, NULL, 0);

	hr = S_OK;
EXIT:
	return hr;
}

//HLSL内にクラス内メンバがないバージョン
HRESULT BASE_SHADING::BeginNM(ID3D11DeviceContext* pD3DDeviceContext, LPCSTR pClassTypeName)
{
	HRESULT hr = E_FAIL;

	// 指定された HLSL クラスを表すクラスインスタンス オブジェクトを取得する
	// http://msdn.microsoft.com/ja-jp/library/ee419541(v=vs.85).aspx
	//ID3D11ClassLinkage::GetClassInstance という関数は指定されたHLSLクラスを表すクラスインスタンスオブジェクトを取得するための関数であるが、 
	//この関数はクラス内にデータメンバー(メンバ変数)が存在する場合に使用でき、存在しない場合はID3D11ClassLinkage::CreateClassInstanceを使用する必要がある(コンパイルは通るがPSが反映されなくなる)
	hr = m_pPSClassLinkage->CreateClassInstance(pClassTypeName, 0, 0, 0, 0, &m_pClassInstance);
	if (FAILED(hr)) goto EXIT;                                 //↑"HLSLのクラスの型の名前"

	// レイアウト設定
	pD3DDeviceContext->IASetInputLayout(m_pLayout);

	// 頂点シェーダーをデバイスに設定する。
	pD3DDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);

	// 頂点シェーダーに定数バッファを設定する
	pD3DDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffers[1]);

	// ハルシェーダーを無効にする。
	pD3DDeviceContext->HSSetShader(NULL, NULL, 0);

	// ドメインシェーダーを無効にする。
	pD3DDeviceContext->DSSetShader(NULL, NULL, 0);

	// ジオメトリシェーダーを無効にする。
	pD3DDeviceContext->GSSetShader(NULL, NULL, 0);

	// ピクセルシェーダーを動的シェーダーリンクとして設定する
	pD3DDeviceContext->PSSetShader(m_pPixelShader, &m_pClassInstance, 1);

	// ピクセルシェーダーに定数バッファを設定する
	pD3DDeviceContext->PSSetConstantBuffers(2, 1, &m_pConstantBuffers[0]);

	// コンピュートシェーダーを無効にする。
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

