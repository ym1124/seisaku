#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <string>
//#include "texture.h"
//#include "shader.h"

//class Texture;
class shader;

class sprite
{
public:
	DirectX::XMFLOAT4X4 world;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state;

	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;//GPU側にシェーダーの情報(テクスチャ)を渡す
	Texture *tex = nullptr;//ロードして使う時用

	D3D11_TEXTURE2D_DESC texture2dDesc;
	//ID3D11SamplerState* sampler_state;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;//深度ステンシル
	//ID3D11BlendState *blend_state;//ブレンド

	Texture* texture;

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	sprite();
	~sprite();
	sprite(ID3D11Device *);
	sprite(const wchar_t*);
	//描画開始位置,画像全体の大きさ,画像の左上の位置?,描画する範囲,色,透過
	void render(ID3D11DeviceContext*, float, float, float, float, float, float, float, float, float angle, float r, float g, float b, float a);
	void render(shader* shader, float dx = 0.0f, float dy = 0.0f, float dw = 1280.0f, float dh = 720.0f, float sx = 0.0f, float sy = 0.0f, float sw = 1280.0f, float sh = 720.0f, float alpha = 1.0f, int tex_slot = 0);
	void render(shader* shader,Texture* tex, float dx = 0.0f, float dy = 0.0f, float dw = 1280.0f, float dh = 720.0f, float sx = 0.0f, float sy = 0.0f, float sw = 1280.0f, float sh = 720.0f, float alpha = 1.0f, int tex_slot = 0, int sampler_type = 1/*LINEAR_BORDER*/);
	void render(shader* shader, Texture_d* tex, float dx = 0.0f, float dy = 0.0f, float dw = 1280.0f, float dh = 720.0f, float sx = 0.0f, float sy = 0.0f, float sw = 1280.0f, float sh = 720.0f, float alpha = 1.0f, int tex_slot = 0, int sampler_type = 1);
	void render(shader* shader, ID3D11ShaderResourceView *srv, float dx = 0.0f, float dy = 0.0f, float dw = 1280.0f, float dh = 720.0f, float sx = 0.0f, float sy = 0.0f, float sw = 1280.0f, float sh = 720.0f, float alpha = 1.0f, int tex_slot = 0, int sampler_type = 1/*LINEAR_BORDER*/);
	ID3D11RenderTargetView* GetRenderTarget() { return texture->GetRenderTarget(); }
	
	void TextOut(ID3D11DeviceContext*immediateContext, std::string s, float x, float y, float w, float h, float r = 1, float g = 1, float b = 1, float a = 1) /*const*/;
};