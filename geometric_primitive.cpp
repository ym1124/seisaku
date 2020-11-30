#include "geometric_primitive.h"
#include "resource_manager.h"

void geometric_primitive::_init(ID3D11Device* device,const char* vsName, D3D11_INPUT_ELEMENT_DESC *inputElementDescs, int numElement,const char* psName)
{
	HRESULT hr;

	////	頂点シェーダーオブジェクト & 入力レイアウトの生成
	//ResourceManager::LoadVertexShaders(device, vsName, inputElementDescs, numElement, &vertex_shader, &input_layout);

	////	ピクセルシェーダーオブジェクトの生成
	//ResourceManager::LoadPixelShaders(device, psName, &pixel_shader);

	normal.create(vsName, psName);

	//	ラスタライザーステート(ワイヤーフレーム)の作成
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rsDesc, &wireframe_rasterizer_state);
	//if (FAILED(hr))	assert(0 && "ワイヤーフレームラスタライザー作成失敗");

	//	ラスタライザーステート(ソリッド)の作成
	//D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	hr = device->CreateRasterizerState(&rsDesc, &solid_rasterizer_state);
	//if (FAILED(hr))	assert(0 && "ソリッドラスタライザー作成失敗");


	//	深度ステンシルStateの作成
	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(dssDesc));
	dssDesc.DepthEnable = FALSE;						//	深度テスト有効
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dssDesc.StencilEnable = FALSE;					//	ステンシルは無効
	hr = device->CreateDepthStencilState(&dssDesc, &depth_stencil_state);
	//if (FAILED(hr))		assert(0 && "深度ステンシルState作成失敗");

	resource_manager::loadShaderResourceView(device, L"OBJ/dummy.png", &shader_resource_view, &tex2d_desc);

	D3D11_SAMPLER_DESC smpDesc = {};

	smpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //D3D11_FILTER_ANISOTROPIC
	smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	smpDesc.MipLODBias = 0;
	smpDesc.MaxAnisotropy = 16;
	smpDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//memcpy(smpDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	smpDesc.MinLOD = 0;
	smpDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&smpDesc, &sampler_state);
	if (FAILED(hr)) return;
}

bool geometric_primitive::createBuffers(ID3D11Device* device, vertex* vertices, int numV, unsigned int* indices, int numI)
{
	HRESULT hr;
	D3D11_BUFFER_DESC buffer_desc;
	D3D11_SUBRESOURCE_DATA subresource_data;

	//	Vertex Buffer
	ZeroMemory(&buffer_desc, sizeof(buffer_desc));
	ZeroMemory(&subresource_data, sizeof(subresource_data));

	buffer_desc.ByteWidth = numV * sizeof(vertex);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
	if (FAILED(hr))		return false;


	//	Index Buffer
	if (indices)
	{
		ZeroMemory(&buffer_desc, sizeof(buffer_desc));
		ZeroMemory(&subresource_data, sizeof(subresource_data));

		buffer_desc.ByteWidth = numI * sizeof(unsigned int);
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		subresource_data.pSysMem = indices;
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;

		numIndices = numI;
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, &index_buffer);
		if (FAILED(hr))		return false;
	}

	//	Constant Buffer
	ZeroMemory(&buffer_desc, sizeof(buffer_desc));

	buffer_desc.ByteWidth = sizeof(cbuffer);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&buffer_desc, nullptr, &constant_buffer);
	if (FAILED(hr))		return false;

	isMakeBuffer = true;
	return true;
}

void geometric_primitive::render(ID3D11DeviceContext* context,
	const DirectX::XMFLOAT4X4& world_view_projection, const DirectX::XMFLOAT4X4& world,
	const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color,
	bool bSolid)
{
	//	念の為のエラーチェック
	//assert(isMakeBuffer);

	//	定数バッファの作成
	cbuffer cb;
	cb.world_view_projection = world_view_projection;
	cb.world = world;
	cb.light_direction = light_direction;
	cb.material_color = material_color;
	context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
	context->VSSetConstantBuffers(0, 1, &constant_buffer);

	// 頂点バッファのバインド
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	//	インデックスバッファのバインド
	context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	//	プリミティブモードの設定
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	ラスタライザーの設定
	if (bSolid)	context->RSSetState(solid_rasterizer_state);
	else		context->RSSetState(wireframe_rasterizer_state);

	normal.activate();

	//	深度テストの設定
	context->OMSetDepthStencilState(depth_stencil_state, 0);

	context->PSSetShaderResources(0, 1, &shader_resource_view);
	context->PSSetSamplers(0, 1, &sampler_state);

	//	プリミティブの描画(index付き)
	context->DrawIndexed(numIndices, 0, 0);

	normal.inactivate();
}

void geometric_primitive::release()
{
	if (constant_buffer)			constant_buffer->Release();
	if (index_buffer)				index_buffer->Release();
	if (vertex_buffer)				vertex_buffer->Release();

	if (depth_stencil_state)		depth_stencil_state->Release();
	if (solid_rasterizer_state)		solid_rasterizer_state->Release();
	if (wireframe_rasterizer_state)	wireframe_rasterizer_state->Release();
	if (sampler_state)				sampler_state->Release();

	resource_manager::releaseShaderResourceView(shader_resource_view);
}

geometric_rect::geometric_rect(ID3D11Device* device) : geometric_primitive(device)
{
	vertex vertices[4] = {};
	unsigned int indices[3 * 2] = {};


	vertices[0].position = DirectX::XMFLOAT3(-0.5f, .0f, +0.5f);
	vertices[1].position = DirectX::XMFLOAT3(+0.5f, .0f, +0.5f);
	vertices[2].position = DirectX::XMFLOAT3(-0.5f, .0f, -0.5f);
	vertices[3].position = DirectX::XMFLOAT3(+0.5f, .0f, -0.5f);

	vertices[0].texcoord = DirectX::XMFLOAT2(-0.5f, +0.5f);
	vertices[1].texcoord = DirectX::XMFLOAT2(+0.5f, +0.5f);
	vertices[2].texcoord = DirectX::XMFLOAT2(-0.5f, -0.5f);
	vertices[3].texcoord = DirectX::XMFLOAT2(+0.5f, -0.5f);

	vertices[0].normal = vertices[1].normal =
		vertices[2].normal =
		vertices[3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
	indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

	createBuffers(device, vertices, ARRAYSIZE(vertices), indices, ARRAYSIZE(indices));
}


geometric_board::geometric_board(ID3D11Device* device, const wchar_t *tex_filename) : geometric_primitive(device)
{
	vertex vertices[4] = {};
	unsigned int indices[3 * 2] = {};

	shader_resource_view = nullptr;
	tex2d_desc = nullptr;
	resource_manager::loadShaderResourceView(device, tex_filename, &shader_resource_view, &tex2d_desc);

	vertices[0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, .0f);
	vertices[1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, .0f);
	vertices[2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, .0f);
	vertices[3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, .0f);

	vertices[0].texcoord = DirectX::XMFLOAT2(0.0f, 0.0f);
	vertices[1].texcoord = DirectX::XMFLOAT2(1.0f, 0.0f);
	vertices[2].texcoord = DirectX::XMFLOAT2(0.0f, 1.0f);
	vertices[3].texcoord = DirectX::XMFLOAT2(1.0f, 1.0f);

	vertices[0].normal = vertices[1].normal =
		vertices[2].normal =
		vertices[3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
	indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
	indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

	createBuffers(device, vertices, ARRAYSIZE(vertices), indices, ARRAYSIZE(indices));
}

void geometric_board::render(ID3D11DeviceContext * device_context,const DirectX::XMFLOAT4X4 & world_view_projection, const DirectX::XMFLOAT4X4 & world, const DirectX::XMFLOAT4 & light_direction,const DirectX::XMFLOAT4 & material_color, bool bSolid)
{
	//DirectX::XMFLOAT2 position[4] = {
	//	{ dx,		dy },	//left_top
	//	{ dx + dw,	dy },	//right_top
	//	{ dx,		dy + dh },	//left_down
	//	{ dx + dw,	dy + dh }	//right_down
	//};

	//const DirectX::XMFLOAT2 center = { dx + dw * 0.5f,dy + dh * 0.5f };

	//for (int i = 0; i < 4; i++) {
	//	position[i].x -= center.x;
	//	position[i].y -= center.y;
	//}

	//float rx, ry;
	//float cos = cosf(angle*0.01745f);
	//float sin = sinf(angle*0.01745f);

	//for (int i = 0; i < 4; i++) {
	//	rx = position[i].x;
	//	ry = position[i].y;
	//	position[i].x = cos*rx + -sin*ry;
	//	position[i].y = sin*rx + cos*ry;
	//}

	//for (int i = 0; i < 4; i++) {
	//	position[i].x += center.x;
	//	position[i].y += center.y;
	//}

	// Convert to NDC space
	//for (int i = 0; i < 4; i++) {
	//	position[i].x = 2.0f*position[i].x / screen_width - 1.0f;
	//	position[i].y = 1.0f - 2.0f*position[i].y / screen_height;
	//}

	//HRESULT hr = S_OK;
	//D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	//D3D11_MAPPED_SUBRESOURCE mapped_buffer = {};

	//hr = device_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
	//if (FAILED(hr)) return;

	//vertex *vertices = static_cast<vertex *>(mapped_buffer.pData);

	////for (int i = 0; i < 4; i++) {
	////	vertices[i].position.x = position[i].x;
	////	vertices[i].position.y = position[i].y;
	////	vertices[i].position.z = 0.0f;
	////}

	//vertices[0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, 0.0f);
	//vertices[1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, 0.0f);
	//vertices[2].position = DirectX::XMFLOAT3(-0.5f, +0.5f, 0.0f);
	//vertices[3].position = DirectX::XMFLOAT3(+0.5f, +0.5f, 0.0f);

	//vertices[0].texcoord.x = static_cast<FLOAT>(tex_dx) / static_cast<FLOAT>(tex2d_desc->Width);
	//vertices[0].texcoord.y = static_cast<FLOAT>(tex_dy) / static_cast<FLOAT>(tex2d_desc->Height);
	//vertices[1].texcoord.x = static_cast<FLOAT>(tex_dx + tex_dw) / static_cast<FLOAT>(tex2d_desc->Width);
	//vertices[1].texcoord.y = static_cast<FLOAT>(tex_dy) / static_cast<FLOAT>(tex2d_desc->Height);
	//vertices[2].texcoord.x = static_cast<FLOAT>(tex_dx) / static_cast<FLOAT>(tex2d_desc->Width);
	//vertices[2].texcoord.y = static_cast<FLOAT>(tex_dy + tex_dh) / static_cast<FLOAT>(tex2d_desc->Height);
	//vertices[3].texcoord.x = static_cast<FLOAT>(tex_dx + tex_dw) / static_cast<FLOAT>(tex2d_desc->Width);
	//vertices[3].texcoord.y = static_cast<FLOAT>(tex_dy + tex_dh) / static_cast<FLOAT>(tex2d_desc->Height);

	//device_context->Unmap(vertex_buffer, 0);

	//	定数バッファの作成
	cbuffer cb;
	cb.world_view_projection = world_view_projection;
	cb.world = world;
	cb.light_direction = light_direction;
	cb.material_color = material_color;
	device_context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
	device_context->VSSetConstantBuffers(0, 1, &constant_buffer);

	normal.activate();

	// 頂点バッファのバインド
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	//	インデックスバッファのバインド
	device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	//	プリミティブモードの設定
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	ラスタライザーの設定
	if (bSolid)	device_context->RSSetState(solid_rasterizer_state);
	else		device_context->RSSetState(wireframe_rasterizer_state);

	//	深度テストの設定
	device_context->OMSetDepthStencilState(depth_stencil_state, 0);

	device_context->PSSetShaderResources(0, 1, &shader_resource_view);
	device_context->PSSetSamplers(0, 1, &sampler_state);

	//	プリミティブの描画(index付き)
	device_context->DrawIndexed(numIndices, 0, 0);

	normal.inactivate();
}


geometric_cube::geometric_cube(ID3D11Device* device) : geometric_primitive(device)
{
	vertex vertices[4 * 6] = {};
	unsigned int indices[3 * 2 * 6] = {};

	int numV = 0, numI = 0;

	//	上面
	vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[numV + 3].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);

	vertices[numV + 0].texcoord = DirectX::XMFLOAT2(-0.5f, +0.5f);
	vertices[numV + 1].texcoord = DirectX::XMFLOAT2(+0.5f, +0.5f);
	vertices[numV + 2].texcoord = DirectX::XMFLOAT2(-0.5f, -0.5f);
	vertices[numV + 3].texcoord = DirectX::XMFLOAT2(+0.5f, -0.5f);

	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
	numV += 4;	numI += 6;

	//	下面
	vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[numV + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);

	vertices[numV + 0].texcoord = DirectX::XMFLOAT2(-0.5f, +0.5f);
	vertices[numV + 1].texcoord = DirectX::XMFLOAT2(+0.5f, +0.5f);
	vertices[numV + 2].texcoord = DirectX::XMFLOAT2(-0.5f, -0.5f);
	vertices[numV + 3].texcoord = DirectX::XMFLOAT2(+0.5f, -0.5f);

	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
	numV += 4;	numI += 6;

	//	右面
	vertices[numV + 0].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[numV + 2].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[numV + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);

	vertices[numV + 0].texcoord = DirectX::XMFLOAT2(-0.5f, +0.5f);
	vertices[numV + 1].texcoord = DirectX::XMFLOAT2(+0.5f, +0.5f);
	vertices[numV + 2].texcoord = DirectX::XMFLOAT2(-0.5f, -0.5f);
	vertices[numV + 3].texcoord = DirectX::XMFLOAT2(+0.5f, -0.5f);

	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+1.0f, +0.0f, +0.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
	numV += 4;	numI += 6;

	//	左面
	vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[numV + 1].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[numV + 3].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);

	vertices[numV + 0].texcoord = DirectX::XMFLOAT2(-0.5f, +0.5f);
	vertices[numV + 1].texcoord = DirectX::XMFLOAT2(+0.5f, +0.5f);
	vertices[numV + 2].texcoord = DirectX::XMFLOAT2(-0.5f, -0.5f);
	vertices[numV + 3].texcoord = DirectX::XMFLOAT2(+0.5f, -0.5f);

	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(-1.0f, +0.0f, +0.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
	numV += 4;	numI += 6;

	//	後面
	vertices[numV + 0].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[numV + 3].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);

	vertices[numV + 0].texcoord = DirectX::XMFLOAT2(-0.5f, +0.5f);
	vertices[numV + 1].texcoord = DirectX::XMFLOAT2(+0.5f, +0.5f);
	vertices[numV + 2].texcoord = DirectX::XMFLOAT2(-0.5f, -0.5f);
	vertices[numV + 3].texcoord = DirectX::XMFLOAT2(+0.5f, -0.5f);

	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, +1.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
	numV += 4;	numI += 6;

	//	前面
	vertices[numV + 0].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[numV + 3].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);

	vertices[numV + 0].texcoord = DirectX::XMFLOAT2(-0.5f, +0.5f);
	vertices[numV + 1].texcoord = DirectX::XMFLOAT2(+0.5f, +0.5f);
	vertices[numV + 2].texcoord = DirectX::XMFLOAT2(-0.5f, -0.5f);
	vertices[numV + 3].texcoord = DirectX::XMFLOAT2(+0.5f, -0.5f);

	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
	numV += 4;	numI += 6;

	createBuffers(device, vertices, numV, indices, numI);
}

#include <vector>

geometric_sphere::geometric_sphere(ID3D11Device* device, u_int slices, u_int stacks) :geometric_primitive(device)
{
	std::vector<vertex> vertices;
	std::vector<u_int> indices;


	float r = 0.5f;		//	半径 0.5f = 直径 1.0f

	//
	// Compute the vertices stating at the top pole and moving down the stacks.
	//

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	vertex top_vertex;
	top_vertex.position = DirectX::XMFLOAT3(0.0f, +r, 0.0f);
	top_vertex.texcoord = DirectX::XMFLOAT2(0.0f, +r);
	top_vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);

	vertex bottom_vertex;
	bottom_vertex.position = DirectX::XMFLOAT3(0.0f, -r, 0.0f);
	bottom_vertex.texcoord = DirectX::XMFLOAT2(0.0f, -r);
	bottom_vertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);

	vertices.push_back(top_vertex);

	float phi_step = DirectX::XM_PI / stacks;
	float theta_step = 2.0f*DirectX::XM_PI / slices;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (u_int i = 1; i <= stacks - 1; ++i)
	{
		float phi = i*phi_step;
		float rs_phi = r * sinf(phi), rc_phi = r * cosf(phi);

		// Vertices of ring.
		for (u_int j = 0; j <= slices; ++j)
		{
			float theta = j*theta_step;

			vertex v;

			// spherical to cartesian
			v.position.x = rs_phi*cosf(theta);
			v.position.y = rc_phi;
			v.position.z = rs_phi*sinf(theta);

			v.texcoord.x = rs_phi*cosf(theta);
			v.texcoord.y = rc_phi;

			DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.position);
			DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

			vertices.push_back(v);
		}
	}

	vertices.push_back(bottom_vertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//
	for (UINT i = 1; i <= slices; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	u_int base_index = 1;
	u_int ring_vertex_count = slices + 1;
	for (u_int i = 0; i < stacks - 2; ++i)
	{
		u_int i_rvc = i*ring_vertex_count;
		u_int i1_rvc = (i + 1)*ring_vertex_count;

		for (u_int j = 0; j < slices; ++j)
		{
			indices.push_back(base_index + i_rvc + j);
			indices.push_back(base_index + i_rvc + j + 1);
			indices.push_back(base_index + i1_rvc + j);

			indices.push_back(base_index + i1_rvc + j);
			indices.push_back(base_index + i_rvc + j + 1);
			indices.push_back(base_index + i1_rvc + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	u_int south_pole_index = (u_int)vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	base_index = south_pole_index - ring_vertex_count;

	for (u_int i = 0; i < slices; ++i)
	{
		indices.push_back(south_pole_index);
		indices.push_back(base_index + i);
		indices.push_back(base_index + i + 1);
	}
	createBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}

geometric_primitive::vertex geometric_sphere2::_makeVertex(const DirectX::XMFLOAT3& p)
{
	vertex v;
	float l = sqrtf(p.x*p.x + p.y*p.y + p.z*p.z);
	v.position = p;
	v.normal.x = p.x / l;
	v.normal.y = p.y / l;
	v.normal.z = p.z / l;
	return v;
}

geometric_sphere2::geometric_sphere2(ID3D11Device* device, u_int div) :geometric_primitive(device)
{
	//
	//	参考 : http://rudora7.blog81.fc2.com/blog-entry-390.html
	//

#ifdef D3DD_ASSERT
	if (device == nullptr)	return;
	if (div < 2)			return;
#else
	if (device == nullptr)	assert(0 && "D3D11Device ERROR!");
	if (div < 2)			assert(0 && "Sphere div ERROR!");
#endif // !D3DD_ASSERT


	std::vector<vertex> sphere;
	int TriangleNum = 0;

	FLOAT Radius = 0.5f;

	DirectX::XMFLOAT3 temp;
	std::vector<std::vector<DirectX::XMFLOAT3>> iList;
	std::vector<DirectX::XMFLOAT3> tTest;

	float *radians = new float[div];
	for (u_int i = 0; i <= div - 1; i++)
		radians[i] = DirectX::XMConvertToRadians(90.0f * (div - i - 1) / (div - 1));


	int tNum = div;
	int tP = ((tNum - 2) << 2) + 4;
	TriangleNum += ((((tP - 1) << 2) + tP) << 1);
	TriangleNum += ((tNum - 2) << 1) * (tP << 1);
	TriangleNum += (((tNum - 1) << 1) - 1) << 2;


	//一番上の頂点の定義
	temp.x = .0f;
	temp.y = Radius;
	temp.z = .0f;
	tTest.push_back(temp);
	iList.push_back(tTest);
	tTest.clear();

	//半球を定義する頂点を算出
	for (int i = 1, count = tNum; i < count; i++)
	{
		float tRadius = cosf(radians[i]) * Radius;	//円の半径
		float tHeight = sinf(radians[i]) * Radius;	//円のＹ座標
		for (int j = 0, count2 = tNum; j < count2; j++)
		{
			temp.x = tRadius * cosf(radians[j]);
			temp.y = tHeight;
			temp.z = tRadius * sinf(radians[j]);

			tTest.push_back(temp);
		}
		//1象限の頂点をｚ座標反転	（k = 0）
		//4象限の頂点をｘ座標反転	（k = 1）
		//3象限の頂点をｚ座標反転	（k = 2）
		for (int k = 0; k < 3; k++) {
			for (int L = 0, Now = tTest.size(), count3 = tNum - 1; L < count3; L++)
			{
				temp = tTest[Now - 2 - L];
				if (k % 2 == 0) {
					temp.z = -tTest[Now - 2 - L].z;
				}
				else {
					temp.x = -tTest[Now - 2 - L].x;
				}
				if (!(k == 2 && L == count3 - 1))
					tTest.push_back(temp);
			}
		}
		iList.push_back(tTest);
		tTest.clear();
	}
	delete[]	radians;

	//半球を定義する頂点を全てＹ座標反転。（境界球全体の頂点が出る）
	for (int i = 1, count = iList.size(); i < count; i++)
	{
		int revi = count - 1 - i;
		for (int j = 0, count2 = iList[revi].size(); j < count2; j++)
		{
			temp = iList[revi][j];
			temp.y = -temp.y;
			tTest.push_back(temp);
		}
		iList.push_back(tTest);
		tTest.clear();
	}


	//真上の頂点１つと、1段下の頂点をつなぐ
	for (int i = 0, count = iList[1].size(); i < count; i++)
	{
		sphere.push_back(_makeVertex(iList[0][0]));
		sphere.push_back(_makeVertex(iList[1][(i + (i & 1)) % count]));
		sphere.push_back(_makeVertex(iList[1][(i + ((i + 1) & 1)) % count]));
		sphere.push_back(_makeVertex(iList[1][(i + ((i + 1) & 1)) % count]));
		if (i != count - 1) {
			sphere.push_back(_makeVertex(iList[0][0]));
		}
	}

	//真上と真下以外の頂点をつなぐ
	for (int i = 1, count = iList.size(); i < count - 2; i++)
	{
		sphere.push_back(_makeVertex(iList[i][0]));
		for (int j = 0, count2 = iList[i].size(); j <= count2; j++)
		{
			sphere.push_back(_makeVertex(iList[i][j % count2]));
			sphere.push_back(_makeVertex(iList[i + 1][j % count2]));
		}
		sphere.push_back(_makeVertex(iList[i + 1][0]));
	}

	sphere.push_back(_makeVertex(iList[iList.size() - 1][0]));

	//真下の頂点１つと、1段上の頂点をつなぐ
	for (int i = 0, count = iList[iList.size() - 2].size(), Num = iList.size(); i < count; i++)
	{
		sphere.push_back(_makeVertex(iList[Num - 1][0]));
		sphere.push_back(_makeVertex(iList[Num - 2][(i + ((i + 1) & 1)) % count]));
		sphere.push_back(_makeVertex(iList[Num - 2][(i + (i & 1)) % count]));
		sphere.push_back(_makeVertex(iList[Num - 2][(i + (i & 1)) % count]));
		if (i != count - 1) {
			sphere.push_back(_makeVertex(iList[Num - 1][0]));
		}
	}

	createBuffers(device, sphere.data(), sphere.size(), nullptr, 0);
	numVertices = sphere.size();
}

void geometric_sphere2::render(ID3D11DeviceContext* context,
	const DirectX::XMFLOAT4X4& world_view_projection, const DirectX::XMFLOAT4X4& world,
	const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color,
	bool bSolid)
{
	//	念の為のエラーチェック
	//assert(isMakeBuffer);

	//	定数バッファの作成
	cbuffer cb;
	cb.world_view_projection = world_view_projection;
	cb.world = world;
	cb.light_direction = light_direction;
	cb.material_color = material_color;
	context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
	context->VSSetConstantBuffers(0, 1, &constant_buffer);

	// 頂点バッファのバインド
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	////	インデックスバッファのバインド
	//context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	//	プリミティブモードの設定(STRIPで行こう！)
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//	ラスタライザーの設定
	if (bSolid)	context->RSSetState(solid_rasterizer_state);
	else		context->RSSetState(wireframe_rasterizer_state);

	normal.activate();

	//	深度テストの設定
	context->OMSetDepthStencilState(depth_stencil_state, 0);

	//	プリミティブの描画(index付き)
	context->Draw(numVertices, 0);

	normal.inactivate();
}