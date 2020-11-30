#pragma once

//#include <d3d11.h>
#include "Math.h"
#include <vector>
#include <string>
#include "shader_ex.h"

class s_mesh
{
private:
	int init(ID3D11Device*, const wchar_t *obj_filename);
	void release();

public:
	struct size_data
	{
		int vertex;
		int material;
		int subset;
	} size;

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;

		DirectX::XMFLOAT2 texcoord;
	};

	struct cbuffer0
	{
		DirectX::XMFLOAT4X4 world_view_projection;	//ワールド・ビュー・プロジェクション合成行列 
		DirectX::XMFLOAT4X4 world;					//ワールド変換行列 
		DirectX::XMFLOAT4 material_color;			//材質色 
		DirectX::XMFLOAT4 light_direction;			//ライトの方向ベクトル
		DirectX::XMFLOAT4 camera_pos;			//カメラ位置
		DirectX::XMFLOAT4 timer;			//
	};

	struct cbuffer1
	{
		DirectX::XMFLOAT4 Ambient = { 0, 0, 0, 0 };//アンビエント光
		DirectX::XMFLOAT4 Diffuse = { 1, 0, 0, 0 }; //拡散反射(色）
		DirectX::XMFLOAT4 Specular = { 1, 1, 1, 1 };//鏡面反射
	};

	shader_ex					normal;

	ID3D11VertexShader		*vertex_shader;
	ID3D11PixelShader		*pixel_shader;
	ID3D11InputLayout		*input_layout;

	ID3D11Buffer			*vertex_buffer;
	ID3D11Buffer			*index_buffer;
	ID3D11Buffer			*const_buffer0;
	ID3D11Buffer			*const_buffer1;


	ID3D11RasterizerState	*wire_rasterizer_state;
	ID3D11RasterizerState	*solid_rasterizer_state;

	ID3D11DepthStencilState	*depth_stencil;
	ID3D11SamplerState		*sampler_state;

	struct subset
	{
		std::wstring usemtl;
		u_int index_start = 0;
		u_int index_count = 0;
	};

	std::vector<subset> subsets;

	struct material
	{
		std::wstring newmtl;
		DirectX::XMFLOAT3 Ka = { 0.2f,0.2f,0.2f };
		DirectX::XMFLOAT3 Kd = { 0.8f,0.8f,0.8f };
		DirectX::XMFLOAT3 Ks = { 1.0f,1.0f,1.0f };
		u_int illum = 1;
		std::wstring map_Kd;

		ID3D11ShaderResourceView *shader_resource_view;
		D3D11_TEXTURE2D_DESC	*tex2d_desc;
	};

	std::vector<material> materials;

	struct SaveSubset
	{
		u_int index_start = 0;
		u_int index_count = 0;
	}save_subset;

	struct SaveMaterial
	{
		DirectX::XMFLOAT3 Ka = { 0.2f,0.2f,0.2f };
		DirectX::XMFLOAT3 Kd = { 0.8f,0.8f,0.8f };
		DirectX::XMFLOAT3 Ks = { 1.0f,1.0f,1.0f };
		u_int illum = 1;
	}save_material;

	s_mesh(ID3D11Device* device, const wchar_t *obj_filename) {
		if (init(device, obj_filename) != 0) {
			OutputDebugString(L"lol");
			exit(EXIT_FAILURE);
		}
	}
	~s_mesh() { release(); }

	void createBuffers(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices);

	void loadOBJ(ID3D11Device *device, std::vector<subset> *psubset, const wchar_t *obj_filename);
	void loadMTL(ID3D11Device *device, std::vector<material> *pmaterial, const wchar_t *mtl_filename);

	void render(ID3D11DeviceContext *immediate_context, const DirectX::XMFLOAT4X4 &world_view_projection, const DirectX::XMFLOAT4X4 &world, const DirectX::XMFLOAT4 &light_direction, const DirectX::XMFLOAT4 &material_color, const DirectX::XMFLOAT4 &camera_pos, const float timer, bool wireframe);
};


