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

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;//�[�x�X�e���V��

	triangle();
	~triangle() {};
	//�`��J�n�ʒu,�摜�S�̂̑傫��,�摜�̍���̈ʒu?,�`�悷��͈�,�F,����
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

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;//�[�x�X�e���V��

public:
	ts_triangle();
	~ts_triangle() {};
	//�`��J�n�ʒu,�摜�S�̂̑傫��,�摜�̍���̈ʒu?,�`�悷��͈�,�F,����
	void render(bool wire_frame = false, float cx = 1280 / 2, float cy = 720 / 2, float size = 300, float alpha = 1.0f, bool tess_flg = false, int tess_factor = 1, int inside_tess_factor = 1);
};