#pragma once

#include <memory>
#include <d3d11.h>
#include "model.h"
#include "shader_ex.h"
#include "shader.h"

class model_renderer
{
public:
	model_renderer(ID3D11Device* device);
	~model_renderer() { release(); }

	void begin(ID3D11DeviceContext* context,shader_ex* shader, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& eye, const bool& clock_wise);
	void begin(ID3D11DeviceContext* context, shader *shader, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& eye, const bool& clock_wise);
	void draw(ID3D11DeviceContext* context, shader_ex* shader,model& _model, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }, float timer = 0.0f);
	void draw(ID3D11DeviceContext* context, model& _model, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }, float timer = 0.0f);
	void end(ID3D11DeviceContext* context);

	bool cullNone = false;
private:

	void release();

	//static const int MaxBones = 128;

	struct cbScene
	{
		XMFLOAT4X4	world;
		XMFLOAT4X4	view_projection;
		XMFLOAT4	light_direction;
		XMFLOAT4	eye;
	};

	struct cbMesh
	{
		XMFLOAT4X4	bone_transforms[MAX_BONES];
	};

	struct cbSubset
	{
		XMFLOAT4	material_color;
		float		tex_timer;
		float		dammy1;
		float		dammy2;
		float		dammy3;
	};


	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_scene;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_mesh;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb_subset;

	//Shader	normal;

	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blend_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> true_rasterizer_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> false_rasterizer_state;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depth_stencil_state;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler_state;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_dummy_srv;
};

