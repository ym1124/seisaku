#pragma once

#include <d3d11.h>
#include "Math.h"
#include "shader_ex.h"
#include <memory>

class shader_ex;
class geometric_primitive
{
protected:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};

	struct cbuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;	//ワールド・ビュー・プロジェクション合成行列 
		DirectX::XMFLOAT4X4 world;					//ワールド変換行列 
		DirectX::XMFLOAT4 material_color;			//材質色 
		DirectX::XMFLOAT4 light_direction;			//ライト進行方向 
	};

	//ID3D11VertexShader		*vertex_shader;
	//ID3D11PixelShader		*pixel_shader;
	//ID3D11InputLayout		*input_layout;

	shader_ex					normal;

	ID3D11Buffer			*vertex_buffer;
	ID3D11Buffer			*index_buffer;
	ID3D11Buffer			*constant_buffer;

	ID3D11RasterizerState	*wireframe_rasterizer_state;
	ID3D11RasterizerState	*solid_rasterizer_state;

	ID3D11DepthStencilState	*depth_stencil_state;

	ID3D11SamplerState		*sampler_state;

	ID3D11ShaderResourceView *shader_resource_view;
	D3D11_TEXTURE2D_DESC	*tex2d_desc;

	int						numIndices;
	bool					isMakeBuffer;

	bool	createBuffers(ID3D11Device* device,
		vertex* vertices, int numV,
		unsigned int* indices, int numI);

	void	_init(ID3D11Device* device,
		const char* vsName, D3D11_INPUT_ELEMENT_DESC *inputElementDescs, int numElement,
		const char* psName);

protected:
	geometric_primitive() :
		normal(),
		vertex_buffer(nullptr), index_buffer(nullptr), constant_buffer(nullptr),
		wireframe_rasterizer_state(nullptr), solid_rasterizer_state(nullptr),
		depth_stencil_state(nullptr), sampler_state(nullptr),
		shader_resource_view(nullptr), tex2d_desc(nullptr),
		numIndices(0), isMakeBuffer(false)
	{}

public:
	geometric_primitive(ID3D11Device* device) :
		normal(),
		vertex_buffer(nullptr), index_buffer(nullptr), constant_buffer(nullptr),
		wireframe_rasterizer_state(nullptr), solid_rasterizer_state(nullptr),
		depth_stencil_state(nullptr), sampler_state(nullptr), shader_resource_view(nullptr),
		numIndices(0), isMakeBuffer(false)
	{
		// 入力レイアウトの定義
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                         D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		const char* vsName = "Shader/CSO/geometric_primitive_vs.cso";
		const char* psName = "Shader/CSO/geometric_primitive_ps.cso";

		//	初期化
		_init(device, vsName, layout, ARRAYSIZE(layout), psName);
	}
	geometric_primitive(ID3D11Device* device,
		const char* vsName, D3D11_INPUT_ELEMENT_DESC *inputElementDescs, int numElement,
		const char* psName) :
		normal(),
		vertex_buffer(nullptr), index_buffer(nullptr), constant_buffer(nullptr),
		wireframe_rasterizer_state(nullptr), solid_rasterizer_state(nullptr),
		depth_stencil_state(nullptr), numIndices(0), isMakeBuffer(false)

	{
		_init(device, vsName, inputElementDescs, numElement, psName);
	}
	virtual ~geometric_primitive() { release(); }

	virtual void render(
		ID3D11DeviceContext* device_context,
		const DirectX::XMFLOAT4X4& world_view_projection,
		const DirectX::XMFLOAT4X4& world,
		const DirectX::XMFLOAT4& light_direction,
		const DirectX::XMFLOAT4& material_color = DirectX::XMFLOAT4(1, 1, 1, 1),
		bool bSolid = true);

	////////////////////////自作///////////////////////

	//GeometricPrimitive(ID3D11Device*);
	//GeometricPrimitive(ID3D11Device*,const char* vs_filename,const char* ps_filename);
	//
	//~GeometricPrimitive() { Release(); }
	//
	//void Render(
	//	ID3D11DeviceContext *device_context,				//デバイスコンテキスト 
	//	const DirectX::XMFLOAT4X4 & world_view_projection,	//ワールド・ビュー・プロジェクション合成行列 
	//	const DirectX::XMFLOAT4X4 & world,					//ワールド変換行列 
	//	const DirectX::XMFLOAT4 & light_direction,			//ライト進行方向 
	//	const DirectX::XMFLOAT4 & material_color,			//材質色 
	//	bool mode											//線・塗りつぶし描画フラグ 
	//);

	/////////////////////////////////////////////////

	void release();
};

class geometric_rect : public geometric_primitive
{
public:
	geometric_rect(ID3D11Device* device);
	~geometric_rect() {};
};


class geometric_board : public geometric_primitive
{
public:
	geometric_board(ID3D11Device* device, const wchar_t *tex_filename);
	~geometric_board() {};

	virtual void render(
		ID3D11DeviceContext* device_context,
		const DirectX::XMFLOAT4X4& world_view_projection,
		const DirectX::XMFLOAT4X4& world,
		const DirectX::XMFLOAT4& light_direction,
		const DirectX::XMFLOAT4& material_color = DirectX::XMFLOAT4(1, 1, 1, 1),
		bool bSolid = true);
};


class geometric_cube : public geometric_primitive
{
public:
	geometric_cube(ID3D11Device* device);
	~geometric_cube() {};
};


class geometric_sphere : public geometric_primitive
{
public:
	geometric_sphere(ID3D11Device* device, u_int slices = 16, u_int stacks = 16);
	~geometric_sphere() {};
};


class geometric_sphere2 : public geometric_primitive
{
private:
	int		numVertices;
	inline	vertex _makeVertex(const DirectX::XMFLOAT3& p);
public:
	//	形状は slices = (div-1) * 4, stacks = (div-1) * 2 と同じ
	geometric_sphere2(ID3D11Device* device, u_int div = 8);
	~geometric_sphere2() {};

	virtual void render(ID3D11DeviceContext* context,
		const DirectX::XMFLOAT4X4& wvp,
		const DirectX::XMFLOAT4X4& world,
		const DirectX::XMFLOAT4& light_direction,
		const DirectX::XMFLOAT4& material_color = DirectX::XMFLOAT4(1, 1, 1, 1),
		bool bSolid = true);
};