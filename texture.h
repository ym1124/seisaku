#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "define.h"

class Texture
{
protected:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView = nullptr;

	// テクスチャ情報
	D3D11_TEXTURE2D_DESC texture2d_desc;

public:
	Texture(bool selfCreateFlg = false);
	~Texture();
	bool Load(const wchar_t* filename = nullptr);
	bool Create(u_int width = sCREEN_WIDTH, u_int height = sCREEN_HEIGHT, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);//////
	
	//getter
	UINT GetWidth() { return texture2d_desc.Width; }
	UINT GetHeight() { return texture2d_desc.Height; }
	ID3D11Texture2D* getTexture2D() { return Texture2D.Get(); }
	ID3D11RenderTargetView* GetRenderTarget() { return RenderTargetView.Get(); }
	ID3D11ShaderResourceView* GetShaderResourceView() { return ShaderResourceView.Get(); }
	ID3D11ShaderResourceView** GetShaderResourceViewPointer() { return ShaderResourceView.GetAddressOf(); }
	
	//setter
	void Set(UINT Slot = 0, BOOL flg = true);
	void SetShaderResourceView(ID3D11ShaderResourceView* srv) { ShaderResourceView = srv; }
	void SetSize(DirectX::XMFLOAT2 size)
	{ 
		texture2d_desc.Width = UINT(size.x);//警告のためキャスト
		texture2d_desc.Height = UINT(size.y);
	}
	void release();
};

class Texture_d
{
protected:
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
	//Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler = nullptr;
	//Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView = nullptr;

	//ComptrじゃないとSRV上書きされる
	ID3D11ShaderResourceView *ShaderResourceView = nullptr;
	ID3D11DepthStencilView *DepthStencilView = nullptr;

	// テクスチャ情報
	D3D11_TEXTURE2D_DESC texture2d_desc;

public:
	Texture_d(bool self_create = false);
	~Texture_d() 
	{
		ShaderResourceView->Release();
		ShaderResourceView = nullptr;
		DepthStencilView->Release();
		DepthStencilView = nullptr;
	};
	void Create(u_int width = sCREEN_WIDTH, u_int height = sCREEN_HEIGHT, int tex_format = 44, int dsv_format = 45, int srv_format = 46);////////
	void Create2(u_int width = sCREEN_WIDTH, u_int height = sCREEN_HEIGHT);
	void Set(UINT Slot = 0, BOOL flg = true);

	UINT GetWidth() { return texture2d_desc.Width; }
	UINT GetHeight() { return texture2d_desc.Height; }
	//getter
	ID3D11DepthStencilView* GetDepthStencilView() { return DepthStencilView; }
	ID3D11ShaderResourceView* GetShaderResourceView() { return ShaderResourceView; }
	//setter
	void SetDepthStencilView(ID3D11DepthStencilView* dsv) { DepthStencilView = dsv; }
	void SetShaderResourceView(ID3D11ShaderResourceView* srv) { ShaderResourceView = srv; }
};

//texture2dRTV,DSV,SRVのフォーマット組み合わせ
//{ DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_R24_UNORM_X8_TYPELESS }44,45,46
//{ DXGI_FORMAT_R32_TYPELESS,  DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_R32_FLOAT }39,40,41
//{ DXGI_FORMAT_R16_TYPELESS,  DXGI_FORMAT_D16_UNORM , DXGI_FORMAT_R16_UNORM }