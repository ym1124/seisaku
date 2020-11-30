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
//	シェーダー単体コンパイル
//------------------------------------------------
HRESULT BASE_SHADING::Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
{
	DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ShaderFlags |= D3DCOMPILE_DEBUG;
	ShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

	Microsoft::WRL::ComPtr<ID3DBlob> BlobError = NULL;
	// コンパイル
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

	// エラー出力
	if (BlobError != NULL)
	{
		MessageBoxA(0, (char*)BlobError->GetBufferPointer(), NULL, MB_OK);
	}

	return hr;
}

// 初期化
HRESULT BASE_SHADING::Init(ID3D11Device* pD3DDevice,WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob = NULL;
	// 頂点シェーダ
	hr = Compile(filename, VSFunc, "vs_5_0", VSBlob.GetAddressOf());
	assert(SUCCEEDED(hr));


	// 頂点シェーダ生成
	hr = FRAMEWORK.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, VS.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// 入力レイアウト生成
	hr = FRAMEWORK.getDevice()->CreateInputLayout(layout, numElements, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), VertexLayout.GetAddressOf());
	assert(SUCCEEDED(hr));


	// 入力レイアウト設定
	FRAMEWORK.getImmediateContext()->IASetInputLayout(VertexLayout.Get());


	// ピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob = NULL;
	hr = Compile(filename, PSFunc, "ps_5_0", &PSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	// ピクセルシェーダ生成
	hr = FRAMEWORK.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), NULL, PS.GetAddressOf());
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
	if (FAILED(hr)) goto EXIT;

	// 頂点シェーダーで使用する定数バッファオブジェクトを作成する
	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.ByteWidth = sizeof(CBUFFER1);        // バッファサイズ
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // リソース使用法を特定する
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// バッファの種類
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU アクセス
	BufferDesc.MiscFlags = 0;                         // その他のフラグも設定しない
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
//	// 行列を列優先で設定し、古い形式の記述を許可しないようにする
//	UINT Flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
//	// 最適化レベルを設定する
//#if defined(DEBUG) || defined(_DEBUG)
//	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
//#else
//	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
//#endif
//
//	// 頂点シェーダーをコンパイルする
//	hr = D3DX11CompileFromFile(pSrcFile, NULL, NULL, pVSMain, "vs_5_0", Flag1, 0, NULL, &pVSBlob, NULL, NULL);
//	if (FAILED(hr)) goto EXIT;
//
//	// ピクセルシェーダーをコンパイルする
//	hr = D3DX11CompileFromFile(pSrcFile, NULL, NULL, pPSMain, "ps_5_0", Flag1, 0, NULL, &pPSBlob, NULL, NULL);
//	if (FAILED(hr)) goto EXIT;
//
//	// 初期化する
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

// 頂点シェーダー用の定数バッファを設定する
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

// ハーフランバート用の定数バッファを設定
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

	//コンスタントバッファに設定
	cbuffer.vecLight0 = *p_vecLight;
	FRAMEWORK.getImmediateContext()->UpdateSubresource(m_pConstantBuffers[0], 0, NULL, &cbuffer, 0, 0);

	// シェーダーリソースビューを設定
	pD3DDeviceContext->PSSetShaderResources(0, 1, &pDecalMap);

	hr = S_OK;
EXIT:
	return hr;
}

// フォンシェーディング用の定数バッファを設定
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


	//コンスタントバッファに設定
	cbuffer.vecLight1 = *p_vecLight;
	cbuffer.EyePos = *p_EyePos;
	FRAMEWORK.getImmediateContext()->UpdateSubresource(m_pConstantBuffers[0], 0, NULL, &cbuffer, 0, 0);
	// シェーダーリソースビューを設定
	pD3DDeviceContext->PSSetShaderResources(0, 1, &pDecalMap);

	hr = S_OK;
EXIT:
	return hr;
}

// セルシェーディング用の定数バッファを設定
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

	// シェーダーリソースビューを設定
	pD3DDeviceContext->PSSetShaderResources(0, 1, &pDecalMap);
	pD3DDeviceContext->PSSetShaderResources(1, 1, &pCelMap);

	hr = S_OK;
EXIT:
	return hr;
}

HRESULT BASE_SHADING::Begin(ID3D11DeviceContext* pD3DDeviceContext, LPCSTR pClassInstanceName)
{
	HRESULT hr = E_FAIL;

	// 指定された HLSL クラスを表すクラスインスタンス オブジェクトを取得する
	// http://msdn.microsoft.com/ja-jp/library/ee419541(v=vs.85).aspx
	hr = m_pPSClassLinkage->GetClassInstance(pClassInstanceName, 0, &m_pClassInstance);
	if (FAILED(hr)) goto EXIT;

	// レイアウト設定
	pD3DDeviceContext->IASetInputLayout(VertexLayout.Get());

	// 頂点シェーダーをデバイスに設定する。
	pD3DDeviceContext->VSSetShader(VS.Get(), NULL, 0);

	// 頂点シェーダーに定数バッファを設定する
	pD3DDeviceContext->VSSetConstantBuffers(1, 1, &m_pConstantBuffers[1]);

	// ハルシェーダーを無効にする。
	pD3DDeviceContext->HSSetShader(NULL, NULL, 0);

	// ドメインシェーダーを無効にする。
	pD3DDeviceContext->DSSetShader(NULL, NULL, 0);

	// ジオメトリシェーダーを無効にする。
	pD3DDeviceContext->GSSetShader(NULL, NULL, 0);

	// ピクセルシェーダーを動的シェーダーリンクとして設定する
	pD3DDeviceContext->PSSetShader(PS.Get(), &m_pClassInstance, 1);

	// ピクセルシェーダーに定数バッファを設定する
	pD3DDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffers[0]);

	// コンピュートシェーダーを無効にする。
	pD3DDeviceContext->CSSetShader(NULL, NULL, 0);

	hr = S_OK;
EXIT:
	return hr;
}

void BASE_SHADING::End()
{
	delete m_pClassInstance;
}
