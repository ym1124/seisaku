#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

class BASE_SHADING
{
private:
	// 動的シェーダーリンケージ( サンプルではピクセルシェーダーのみ )用の定数バッファ定義
	// この並び順はシェーダーソース内で宣言している定数バッファの並び順と同じくなるようにすること
	typedef struct _CBUFFER0
	{
		//ランバート
		DirectX::XMFLOAT4 vecLight0;

		// ハーフランバート用
		DirectX::XMFLOAT4 vecLight1;

		// フォンシェーディング用
		DirectX::XMFLOAT4 vecLight2;
		DirectX::XMFLOAT4 EyePos;

		// シェーディング用
		//DirectX::XMFLOAT4 vecLight3;
	}CBUFFER0;

	// 頂点シェーダー用の定数バッファ定義
	typedef struct _CBUFFER1
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX  matWVP;
	}CBUFFER1;

	// 入力レイアウト
	ID3D11InputLayout* m_pLayout;

	// 定数バッファ// [0]にライティング用[1]にVS用定数
	ID3D11Buffer* m_pConstantBuffers[2];


	// 頂点シェーダー
	ID3D11VertexShader* m_pVertexShader;

	// ピクセルシェーダー
	ID3D11PixelShader* m_pPixelShader;

	// 動的シェーダーリンク
	// ID3D11ClassLinkage
	ID3D11ClassLinkage* m_pPSClassLinkage;

	// ピクセルシェーダーに設定時に使用するクラスインスタンス
	// ID3D11ClassInstance
	ID3D11ClassInstance* m_pClassInstance;

public:

	BASE_SHADING();
	~BASE_SHADING();//解放処理

	// 初期化
	//HRESULT Init(ID3D11Device* pD3DDevice, const BYTE* pVS, size_t VSSize, const BYTE* pPS, size_t PSSize);
	HRESULT Init(ID3D11Device* pD3DDevice, WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc);

	// 頂点シェーダー用の定数バッファを設定する
	HRESULT SetCBVertexShader(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMMATRIX* World, DirectX::XMMATRIX* p_matWVP);
	//ランバート用
	HRESULT SetCBLambert(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight);
	// ハーフランバート用の定数バッファを設定
	HRESULT SetCBHalfLambert(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight);
	// フォンシェーディング用の定数バッファを設定
	HRESULT SetCBPhongShading(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLigh, DirectX::XMFLOAT4* p_EyePos);
	// ノーシェーディング用の定数バッファを設定
	HRESULT SetCBNoShading(ID3D11DeviceContext* pD3DDeviceContext/*, DirectX::XMFLOAT4* p_vecLight*/);
	// 描画開始処理
	HRESULT Begin(ID3D11DeviceContext* pD3DDeviceContext, LPCSTR pClassInstanceName);
	HRESULT BeginNM(ID3D11DeviceContext* pD3DDeviceContext, LPCSTR pClassTypeName);
	//getter
	ID3D11Buffer** getConstantBufferAddress(int num) { return &m_pConstantBuffers[num]; }
	// 描画終了処理
	void End();
};
