#pragma once

#include <d3d11.h>
#include "math.h"

#include <sstream>
#include <vector>

#include "light.h"//

#define MAX_BONE_INFLUENCES 4  //１つの頂点が影響を受けるボーンの最大数 
#define MAX_BONES 128


#define LIGHTS_MAX 16

class skin_mesh_ex
{
public:
	struct bone
	{
		DirectX::XMFLOAT4X4 transform;
	};

	typedef std::vector<bone> skeletal;
	struct skeletal_animation :public std::vector<skeletal>
	{
		float sampling_time = 1 / 24.0f;    float animation_tick = 0.0f;//経過時間
	};

protected:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;

		FLOAT bone_weights[MAX_BONE_INFLUENCES] = { 1, 0, 0, 0 };
		INT bone_indices[MAX_BONE_INFLUENCES] = {};
	};

	struct cbuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;	//ワールド・ビュー・プロジェクション合成行列 
		DirectX::XMFLOAT4X4 world;					//ワールド変換行列 
		DirectX::XMFLOAT4 material_color;			//材質色 
		DirectX::XMFLOAT4 light_direction;			//ライト進行方向
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES];
	};

	struct cbuffer_light
	{
		DirectX::XMFLOAT4X4 world_view_projection;		//ワールド・ビュー・プロジェクション合成行列 
		DirectX::XMFLOAT4X4 world;						//ワールド変換行列 
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4 light_point[LIGHTS_MAX];	//ライトの位置
		DirectX::XMFLOAT4 attenution[LIGHTS_MAX];		//明りの減衰の調整

		DirectX::XMFLOAT4 light_color[LIGHTS_MAX];		//ライトの色
		DirectX::XMFLOAT4 obj_color;					//objの色
		DirectX::XMFLOAT4 light_directional;
		DirectX::XMINT4	  light_exist[LIGHTS_MAX];
	};

	struct material
	{
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		ID3D11ShaderResourceView *shader_resource_view;
		D3D11_TEXTURE2D_DESC	*tex2d_desc;
	};

	//material diffuse;
	struct subset
	{
		u_int index_start = 0; // start number of index buffer
		u_int index_count = 0; // number of vertices (indices)
		material diffuse;
	};

	struct mesh
	{
		ID3D11Buffer			*vertex_buffer;
		ID3D11Buffer			*index_buffer;
		std::vector<subset> subsets;
		DirectX::XMFLOAT4X4 global_transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		//std::vector<SkinMesh::bone> skeletal;
		skeletal_animation skeletal_animation;
	};

	std::vector<mesh> meshes;

	ID3D11VertexShader		*vertex_shader;
	ID3D11VertexShader		*no_bone_vertex_shader;

	ID3D11PixelShader		*pixel_shader;
	ID3D11PixelShader		*no_texture_pixel_shader;

	ID3D11InputLayout		*input_layout;
	ID3D11InputLayout		*no_bone_input_layout;

	ID3D11Buffer			*const_buffer;

	ID3D11RasterizerState	*wire_rasterizer_state;
	ID3D11RasterizerState	*solid_rasterizer_state;

	ID3D11DepthStencilState	*depth_stencil;

	ID3D11SamplerState		*sampler_state;

public:

	bool is_bone;
	bool culling_clockwise;

	DirectX::XMFLOAT4X4 coordinate_conversion =
	{
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1
	};

	skin_mesh_ex(ID3D11Device*, const std::string& fbx_filename);
	~skin_mesh_ex() { release(); }

	void render(
		ID3D11DeviceContext *device_context,				//デバイスコンテキスト 
		const DirectX::XMFLOAT4X4 & world_view_projection,	//ワールド・ビュー・プロジェクション合成行列 
		const DirectX::XMFLOAT4X4 & world,					//ワールド変換行列 
		const DirectX::XMFLOAT4 & light_direction,			//ライト進行方向 
		const DirectX::XMFLOAT4 & material_color,			//材質色 
		float elapsed_time,
		bool mode											//線・塗りつぶし描画フラグ 
		);

	void release();

	//void CreateBuffers(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices);
	void createBuffers(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices, mesh &mesh);

	void loadFBX(ID3D11Device*, const std::string& fbx_filename);
};