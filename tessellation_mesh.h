#pragma once
#include <d3d11.h>
#include <vector>
#include <wrl.h>
#include "shader.h"
#include "texture.h"
#include "static_mesh.h"

//struct material
//{
//public:
//	char Name[255];
//	DirectX::XMFLOAT4 Ka;//アンビエント
//	DirectX::XMFLOAT4 Kd;//ディフューズ
//	DirectX::XMFLOAT4 Ks;//スペキュラー
//	wchar_t TextureName[255];//テクスチャファイル名
//	ID3D11ShaderResourceView* pTexture;//テクスチャデータ//リーク？
//	D3D11_TEXTURE2D_DESC texture2d_desc;
//	DWORD MaxFace;//ポリゴン数
//};

class tessellation_mesh
{
public:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;
	Microsoft::WRL::ComPtr<ID3D11HullShader> hs;
	Microsoft::WRL::ComPtr<ID3D11DomainShader> ds;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cs_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cs_buffer2;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[2];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;

	std::vector<material> Materials;
	DirectX::XMVECTOR focusVec;

	struct subset
	{
		std::wstring usemtl;
		u_int index_start = 0; // start number of index buffer
		u_int index_count = 0; // number of vertices (indices)
	};
	std::vector<subset> subsets;

	tessellation_mesh(const wchar_t* file_name);
	tessellation_mesh(const wchar_t* file_name, const wchar_t* shader_name);
	~tessellation_mesh();

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};

	struct vertex_tangent
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT3 bitangent;
		DirectX::XMFLOAT2 texcoord;
	};

	struct cbuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
		DirectX::XMFLOAT4 light_direction;
	};

	struct Face
	{
		DirectX::XMFLOAT3 position[3];
		int materialIndex;
	};
	std::vector<Face> faces;
	std::vector<vertex> vertices;
	std::vector<u_int> indices;

public:
	void create_buffer(ID3D11Device *pDevice, vertex *vertices, u_int num_vertices, u_int *indices, u_int num_indices);
	void create_buffer(ID3D11Device *pDevice, vertex_tangent *vertices, u_int num_vertices, u_int *indices, u_int num_indices);
	void render(ID3D11DeviceContext *, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4&, const DirectX::XMFLOAT4&,bool wire_frame, int tess_factor, int inside_tess_factor,float depth = 1.0f);
	void render(ID3D11DeviceContext *, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4&, const DirectX::XMFLOAT4&, bool wire_frame, int tess_factor, int inside_tess_factor, float depth, ID3D11ShaderResourceView *srv);
	HRESULT LoadMaterialFromFile(ID3D11Device *pDevice, const wchar_t* filename);
};