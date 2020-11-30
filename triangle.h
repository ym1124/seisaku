#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <string>
//#include "texture.h"
//#include "shader.h"

class Texture;
class shader;

class triangle
{
public:
	struct TRI_VERTEX
	{
		DirectX::XMFLOAT3 position;
	};
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

	D3D11_TEXTURE2D_DESC texture2dDesc;
	//ID3D11SamplerState* sampler_state;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;//深度ステンシル

	triangle();
	~triangle() {};
	//描画開始位置,画像全体の大きさ,画像の左上の位置?,描画する範囲,色,透過
	void render(shader* shader, bool wire_frame = false, bool tess_flg = false ,float cx = 1280 / 2, float cy = 720 / 2, float size = 300, float alpha = 1.0f);
};

class ts_triangle
{
private:
	struct TRI_VERTEX
	{
		DirectX::XMFLOAT3 position;
	};
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11HullShader> hullShader;
	Microsoft::WRL::ComPtr<ID3D11DomainShader> domainShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	D3D11_TEXTURE2D_DESC texture2dDesc;
	//ID3D11SamplerState* sampler_state;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;//深度ステンシル

public:
	ts_triangle();
	~ts_triangle() {};
	//描画開始位置,画像全体の大きさ,画像の左上の位置?,描画する範囲,色,透過
	void render(bool wire_frame = false, float cx = 1280 / 2, float cy = 720 / 2, float size = 300, float alpha = 1.0f, bool tess_flg = false, int tess_factor = 1, int inside_tess_factor = 1);
};