#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "light.h"

class BASE_SHADING
{
private:
	// 動的シェーダーリンケージ( サンプルではピクセルシェーダーのみ )用の定数バッファ定義
	// この並び順はシェーダーソース内で宣言している定数バッファの並び順と同じくなるようにすること
	typedef struct _CBUFFER0
	{
		// ハーフランバート用
		DirectX::XMFLOAT4 vecLight0;

		// フォンシェーディング用
		DirectX::XMFLOAT4 vecLight1;
		DirectX::XMFLOAT4 EyePos;

		// セルシェーディング用
		DirectX::XMFLOAT4 vecLight2;
	}CBUFFER0;

	// 頂点シェーダー用の定数バッファ定義
	typedef struct _CBUFFER1
	{
		// ワールドビュープロジェクション行列
		DirectX::XMMATRIX  matWVP;
	}CBUFFER1;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		VS = nullptr; // 頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		PS = nullptr; // ピクセルシェーダ

	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		VertexLayout;

	// 定数バッファ
	ID3D11Buffer* m_pConstantBuffers[2];

	// 動的シェーダーリンク
	// ID3D11ClassLinkage
	ID3D11ClassLinkage* m_pPSClassLinkage;

	// ピクセルシェーダーに設定時に使用するクラスインスタンス
	// ID3D11ClassInstance
	ID3D11ClassInstance* m_pClassInstance;

public:

	BASE_SHADING();
	~BASE_SHADING();

	HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);

	// 初期化
	HRESULT Init(ID3D11Device* pD3DDevice, WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc);
	/*HRESULT Init(ID3D11Device* pD3DDevice, WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc);*/

	// 頂点シェーダー用の定数バッファを設定する
	HRESULT SetCBVertexShader(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMMATRIX* p_matWVP);
	// ハーフランバート用の定数バッファを設定
	HRESULT SetCBHalfLambert(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight, ID3D11ShaderResourceView* pDecalMap);
	// フォンシェーディング用の定数バッファを設定
	HRESULT SetCBPhongShading(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight, DirectX::XMFLOAT4* p_EyePos, ID3D11ShaderResourceView* pDecalMap);
	// セルシェーディング用の定数バッファを設定
	HRESULT SetCBCelShading(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight, ID3D11ShaderResourceView* pDecalMap, ID3D11ShaderResourceView* pCelMap);
	// 描画開始処理
	HRESULT Begin(ID3D11DeviceContext* pD3DDeviceContext, LPCSTR pClassInstanceName);
	// 描画終了処理
	void End();
};