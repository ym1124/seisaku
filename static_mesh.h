#pragma once
#include <d3d11.h>
#include <vector>
#include <wrl.h>
#include "shader.h"
#include "texture.h"
class camera;

struct material
{
public:
	char Name[255];
	DirectX::XMFLOAT4 Ka;//アンビエント
	DirectX::XMFLOAT4 Kd;//ディフューズ
	DirectX::XMFLOAT4 Ks;//スペキュラー
	wchar_t TextureName[255];//テクスチャファイル名
	ID3D11ShaderResourceView* pTexture;//テクスチャデータ//リーク？
	D3D11_TEXTURE2D_DESC texture2d_desc;
	DWORD MaxFace;//ポリゴン数
};

class static_mesh
{
public:
	ID3D11VertexShader* vs;
	ID3D11PixelShader* ps;
	ID3D11GeometryShader* gs = nullptr;
	ID3D11InputLayout* input_layout;
	ID3D11Buffer* vertex_buffer;
	ID3D11Buffer* index_buffer;
	ID3D11Buffer* cs_buffer;
	ID3D11RasterizerState* rasterizer_states[2];
	ID3D11DepthStencilState* depth_stencil_state;

	std::vector<material> Materials;

	DirectX::XMVECTOR focusVec;
	//DirectX::XMFLOAT3 pos;
	//DirectX::XMFLOAT3 scale;
	//DirectX::XMFLOAT3 angle;

	struct subset
	{
		std::wstring usemtl;
		u_int index_start = 0; // start number of index buffer
		u_int index_count = 0; // number of vertices (indices)
	};
	std::vector<subset> subsets;

	//static_mesh(const wchar_t* file_name, bool culloff = false);
	static_mesh(const wchar_t* file_name, bool culloff = false, bool tangent = false);
	~static_mesh();

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
	void render(ID3D11DeviceContext *, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4&, const DirectX::XMFLOAT4&, bool DSLflg, bool wire_frame, bool cull = true);
	void render(ID3D11DeviceContext *, shader *, camera* cam, const DirectX::XMFLOAT4X4 &world, const DirectX::XMFLOAT4& light_dir, bool wire_frame, bool cull = true);
	void render(ID3D11DeviceContext *pDeviceContext, shader *shader, Texture* pTex, const DirectX::XMFLOAT4X4&world_view_projection, const DirectX::XMFLOAT4X4 &world, const DirectX::XMFLOAT4 &light_direction, /*const DirectX::XMFLOAT4 &material_color,*/bool wireframe = false);
	void renderTessellation(ID3D11DeviceContext *pDeviceContext, shader *shader, const DirectX::XMFLOAT4X4&world_view_projection, const DirectX::XMFLOAT4X4 &world, const DirectX::XMFLOAT4 &light_direction,/* const DirectX::XMFLOAT4 &material_color,*/ bool wireframe);
	HRESULT LoadMaterialFromFile(ID3D11Device *pDevice, const wchar_t* filename);
	int RayPick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal, float* outLength);

	/*void setPos(DirectX::XMFLOAT3 pos) { this->pos = pos; }
	void setScale(DirectX::XMFLOAT3 scale) { this->scale = scale; }
	void setAngle(DirectX::XMFLOAT3 angle) { this->angle = angle; }*/
	/*DirectX::XMMATRIX getWorldMatrix();
	DirectX::XMFLOAT4X4 getWorld();
	DirectX::XMFLOAT4X4 getWorldViewProjection(const DirectX::XMMATRIX &ViewM, const DirectX::XMMATRIX &ProjectionM);*/
};