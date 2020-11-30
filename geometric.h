#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

class shader;

class geometric
{
public:
	DirectX::XMFLOAT4X4 world;

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};
	struct cbuffer
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[2];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;

	std::unique_ptr<shader> geo_shader;

public:
	geometric(ID3D11Device *device, bool pbr_flg = false);
	virtual ~geometric() {}

	void update(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 angle, DirectX::XMFLOAT3 scale);
	void render(ID3D11DeviceContext *immediate_context, const DirectX::XMFLOAT4 &material_color, bool wireframe);
	void render(ID3D11DeviceContext *immediate_context, shader* sh, const DirectX::XMFLOAT4X4 &world_view_projection, const DirectX::XMFLOAT4X4 &world, const DirectX::XMFLOAT4 &light_direction, const DirectX::XMFLOAT4 &material_color, bool wireframe = false);

protected:
	void create_buffers(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices);
};

class geo_cube : public geometric
{
public:
	geo_cube(ID3D11Device *device);
};

class geo_cylinder : public geometric
{
public:
	geo_cylinder(ID3D11Device *device, u_int slices);
};

class geo_sphere : public geometric
{
public:
	geo_sphere(ID3D11Device *device, u_int slices, u_int stacks, bool pbr_flg);
};

class geo_capsule : public geometric
{
public:
	geo_capsule(ID3D11Device *device);
};

