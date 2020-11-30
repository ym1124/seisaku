#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <string>
#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "blend.h"

class billboard
{
	//private:
	//	DirectX::XMFLOAT3 position;
	//	DirectX::XMFLOAT3 scale;
	//	DirectX::XMFLOAT3 angle;
	//	DirectX::XMFLOAT4X4 world;
	//	DirectX::XMFLOAT4X4 wvp;
public:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;//nullになる！？？？？？
													   //ID3D11Buffer *vertex_buffe;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cs_buffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;//GPU側にシェーダーの情報(テクスチャ)を渡す(?)
	D3D11_TEXTURE2D_DESC texture2dDesc;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;//マルチサンプリングに必要(?)

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;//深度ステンシル
																		//Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;//ブレンド
	blender* blend;

	int numIndices;

	struct vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};

	struct cbuffer
	{
		DirectX::XMFLOAT4X4	world;
		DirectX::XMFLOAT4X4	wvp;				//	world_view_projectio		//	
		DirectX::XMFLOAT4	material_color;		//	
		DirectX::XMFLOAT4	light_direction;	//	
	};

	billboard(const wchar_t* /*Texture file name*/);
	~billboard();

	//回転の中心座標
	//virtual void update(Camera* m_camera);
	virtual void render(ID3D11DeviceContext* context, DirectX::XMFLOAT4X4 world, camera *camera, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color);
	//void render(shader* shader, Texture* tex, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float alpha = 1.0f);
	void create_buffer(ID3D11Device *device, vertex *vertices, u_int num_vertices, u_int *indices, u_int num_indices);
	/*DirectX::XMMATRIX billboard::getInvMatrixForBillBoard(Camera* m_camera);*/
	//void move();

	//void setPosition(DirectX::XMFLOAT3 pos) { position = pos; };
	//void setAngle(DirectX::XMFLOAT3 angle) { this->angle = angle; };
	//void setScale(DirectX::XMFLOAT3 scale) { this->scale = scale; };

	//DirectX::XMFLOAT3 getPosition() { return position; };
};

class billboard_obj
{
public:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 wvp;
	std::shared_ptr<billboard> bb;

	billboard_obj(std::shared_ptr<billboard> bb);
	~billboard_obj() {};
	void update(camera* m_camera);
	void render(ID3D11DeviceContext* context, camera *camera, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color);

	DirectX::XMMATRIX getInvMatrixForBillBoard(camera* camera);
	void move();

	void setPosition(DirectX::XMFLOAT3 pos) { position = pos; };
	void setAngle(DirectX::XMFLOAT3 angle) { this->angle = angle; };
	void setScale(DirectX::XMFLOAT3 scale) { this->scale = scale; };

	DirectX::XMFLOAT3 getPosition() { return position; };
};