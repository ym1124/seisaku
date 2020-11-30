//#include "logger.h"
#include "model_renderer.h"
#include "misc.h"
//#include "System.h"
#include "framework.h"

model_renderer::model_renderer(ID3D11Device* device)
{
	////シェーダーの設定
	//{
	//	normal.Set("Shader/CSO/skin_mesh_vs.cso", "Shader/CSO/skin_mesh_ps.cso");
	//}

	// 定数バッファ
	{
		// シーン用バッファ
		D3D11_BUFFER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(cbScene);
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, 0, m_cb_scene.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// メッシュ用バッファ
		desc.ByteWidth = sizeof(cbMesh);

		hr = device->CreateBuffer(&desc, 0, m_cb_mesh.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// サブセット用バッファ
		desc.ByteWidth = sizeof(cbSubset);

		hr = device->CreateBuffer(&desc, 0, m_cb_subset.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// ブレンドステート
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, m_blend_state.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// 深度ステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT hr = device->CreateDepthStencilState(&desc, m_depth_stencil_state.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// ラスタライザーステート
	{
		D3D11_RASTERIZER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.AntialiasedLineEnable = false;

		HRESULT hr = device->CreateRasterizerState(&desc, false_rasterizer_state.GetAddressOf());

		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.AntialiasedLineEnable = false;

		hr = device->CreateRasterizerState(&desc, true_rasterizer_state.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// サンプラステート
	{
		D3D11_SAMPLER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		HRESULT hr = device->CreateSamplerState(&desc, m_sampler_state.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// ダミーテクスチャ
	{
		const int width = 8;
		const int height = 8;
		UINT pixels[width * height];
		::memset(pixels, 0xFF, sizeof(pixels));

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA data;
		::memset(&data, 0, sizeof(data));
		data.pSysMem = pixels;
		data.SysMemPitch = width;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
		HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = device->CreateShaderResourceView(texture.Get(), nullptr, m_dummy_srv.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

// 描画開始
void model_renderer::begin(ID3D11DeviceContext* context, shader_ex* shader, const XMFLOAT4X4& world, const XMFLOAT4X4& view_projection, const XMFLOAT4& light_direction, const XMFLOAT4& eye, const bool& clock_wise)
{
	shader->activate();

	ID3D11Buffer* constant_buffers[] =
	{
		m_cb_scene.Get(),
		m_cb_mesh.Get(),
		m_cb_subset.Get()
	};
	context->VSSetConstantBuffers(5, ARRAYSIZE(constant_buffers), constant_buffers);
	context->GSSetConstantBuffers(5, ARRAYSIZE(constant_buffers), constant_buffers);
	context->PSSetConstantBuffers(5, ARRAYSIZE(constant_buffers), constant_buffers);

	//const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//context->OMSetBlendState(m_blend_state.Get(), blend_factor, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_depth_stencil_state.Get(), 0);

	if (clock_wise) {
		context->RSSetState(true_rasterizer_state.Get());
	}
	else {
		context->RSSetState(false_rasterizer_state.Get());
	}

	if (cullNone) {
		context->RSSetState(FRAMEWORK.getRasterizerState(FRAMEWORK.RS_CULL_NONE));
	}

	context->PSSetSamplers(0, 1, m_sampler_state.GetAddressOf());

	// シーン用定数バッファ更新
	cbScene cb_scene;
	cb_scene.eye = eye;
	cb_scene.world = world;
	cb_scene.view_projection = view_projection;
	cb_scene.light_direction = light_direction;


	context->UpdateSubresource(m_cb_scene.Get(), 0, 0, &cb_scene, 0, 0);
}

void model_renderer::begin(ID3D11DeviceContext* context, shader *shader, const XMFLOAT4X4& world, const XMFLOAT4X4& view_projection, const XMFLOAT4& light_direction, const XMFLOAT4& eye, const bool& clock_wise)
{
	shader->Activate();

	ID3D11Buffer* constant_buffers[] =
	{
		m_cb_scene.Get(),
		m_cb_mesh.Get(),
		m_cb_subset.Get()
	};
	context->VSSetConstantBuffers(5, ARRAYSIZE(constant_buffers), constant_buffers);
	context->PSSetConstantBuffers(5, ARRAYSIZE(constant_buffers), constant_buffers);

	//const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//context->OMSetBlendState(m_blend_state.Get(), blend_factor, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_depth_stencil_state.Get(), 0);

	if (clock_wise) {
		context->RSSetState(true_rasterizer_state.Get());
	}
	else {
		context->RSSetState(false_rasterizer_state.Get());
	}

	if (cullNone) {
		context->RSSetState(FRAMEWORK.getRasterizerState(FRAMEWORK.RS_CULL_NONE));
	}

	context->PSSetSamplers(0, 1, m_sampler_state.GetAddressOf());

	// シーン用定数バッファ更新
	cbScene cb_scene;
	cb_scene.eye = eye;
	cb_scene.world = world;
	cb_scene.view_projection = view_projection;
	cb_scene.light_direction = light_direction;


	context->UpdateSubresource(m_cb_scene.Get(), 0, 0, &cb_scene, 0, 0);
}

// 描画
void model_renderer::draw(ID3D11DeviceContext* context, shader_ex* shader, model& _model, const DirectX::XMFLOAT4& color, float timer)
{
	const model_resource* model_resource = _model.getModelResource();
	const std::vector<model::node>& nodes = _model.getNodes();

	for (const model_resource::mesh& mesh : model_resource->getMeshes())
	{
		// メッシュ用定数バッファ更新
		cbMesh cb_mesh;
		::memset(&cb_mesh, 0, sizeof(cb_mesh));
		if (mesh.node_indices.size() > 0)
		{
			for (size_t i = 0; i < mesh.node_indices.size(); ++i)
			{
				//直接ぶちこむとメモリエラーでるからいったん変数に突っ込む
				DirectX::XMMATRIX inverse_transform = mesh.inverse_transforms.at(i);
				DirectX::XMMATRIX world_transform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_indices.at(i)).world_transform);

				//inverseとworld_transformのサイズ?
				XMMATRIX bone_transform = inverse_transform*world_transform;
				//XMMATRIX bone_transform = mesh.inverse_transforms.at(i) * DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_indices.at(i)).world_transform);
				//XMMATRIX bone_transform = DirectX::XMMatrixMultiply(mesh.inverse_transforms.at(i), DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_indices.at(i)).world_transform));
				
				DirectX::XMStoreFloat4x4(&cb_mesh.bone_transforms[i], bone_transform);
			}
		}
		else
		{
			DirectX::XMStoreFloat4x4(&cb_mesh.bone_transforms[0], DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_index).world_transform));
		}
		context->UpdateSubresource(m_cb_mesh.Get(), 0, 0, &cb_mesh, 0, 0);

		UINT stride = sizeof(ModelData::Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(shader->input_layout);

		for (const model_resource::subset& subset : mesh.subsets)
		{

			cbSubset cb_subset;
			cb_subset.material_color.x = subset._material->color.x * color.x;
			cb_subset.material_color.y = subset._material->color.y * color.y;
			cb_subset.material_color.z = subset._material->color.z * color.z;
			cb_subset.material_color.w = subset._material->color.w * color.w;

			cb_subset.tex_timer = timer;
			context->UpdateSubresource(m_cb_subset.Get(), 0, 0, &cb_subset, 0, 0);
			context->PSSetShaderResources(0, 1, subset._material->shader_resource_view.Get() ? subset._material->shader_resource_view.GetAddressOf() : m_dummy_srv.GetAddressOf());
			context->PSSetSamplers(0, 1, m_sampler_state.GetAddressOf());
			context->DrawIndexed(subset.index_count, subset.start_index, 0);
		}
	}

}

void model_renderer::draw(ID3D11DeviceContext* context, model& _model, const DirectX::XMFLOAT4& color, float timer)
{
	const model_resource* model_resource = _model.getModelResource();
	const std::vector<model::node>& nodes = _model.getNodes();

	for (const model_resource::mesh& mesh : model_resource->getMeshes())
	{
		// メッシュ用定数バッファ更新
		cbMesh cb_mesh;
		::memset(&cb_mesh, 0, sizeof(cb_mesh));
		if (mesh.node_indices.size() > 0)
		{
			for (size_t i = 0; i < mesh.node_indices.size(); ++i)
			{
				//DirectX::XMMATRIX inverse_transform = DirectX::XMLoadFloat4x4(mesh.inverse_transforms.at(i));
				//DirectX::XMMATRIX world_transform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_indices.at(i)).world_transform);

				//XMMATRIX bone_transform = mesh.inverse_transforms.at(i) * DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_indices.at(i)).world_transform);
				XMMATRIX bone_transform = DirectX::XMMatrixMultiply(mesh.inverse_transforms.at(i), DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_indices.at(i)).world_transform));
				DirectX::XMStoreFloat4x4(&cb_mesh.bone_transforms[i], bone_transform);
			}
		}
		else
		{
			DirectX::XMStoreFloat4x4(&cb_mesh.bone_transforms[0], DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_index).world_transform));
		}
		context->UpdateSubresource(m_cb_mesh.Get(), 0, 0, &cb_mesh, 0, 0);

		UINT stride = sizeof(ModelData::Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		for (const model_resource::subset& subset : mesh.subsets)
		{
			cbSubset cb_subset;
			cb_subset.material_color.x = subset._material->color.x * color.x;
			cb_subset.material_color.y = subset._material->color.y * color.y;
			cb_subset.material_color.z = subset._material->color.z * color.z;
			cb_subset.material_color.w = subset._material->color.w * color.w;

			cb_subset.tex_timer = timer;
			context->UpdateSubresource(m_cb_subset.Get(), 0, 0, &cb_subset, 0, 0);
			context->PSSetShaderResources(0, 1, subset._material->shader_resource_view.Get() ? subset._material->shader_resource_view.GetAddressOf() : m_dummy_srv.GetAddressOf());
			context->PSSetSamplers(0, 1, m_sampler_state.GetAddressOf());
			context->DrawIndexed(subset.index_count, subset.start_index, 0);
		}
	}

}

// 描画終了
void model_renderer::end(ID3D11DeviceContext* context)
{

}

void model_renderer::release()
{

}
