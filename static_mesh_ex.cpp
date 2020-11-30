#include "static_mesh_ex.h"

#include <memory>
#include "resource_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>

int s_mesh::init(ID3D11Device* device, const wchar_t *obj_filename)
{
	normal.init();

	vertex_buffer = nullptr;
	index_buffer = nullptr;
	const_buffer0 = nullptr;
	const_buffer1 = nullptr;

	wire_rasterizer_state = nullptr;
	solid_rasterizer_state = nullptr;

	depth_stencil = nullptr;

	sampler_state = nullptr;

	loadOBJ(device, &subsets, obj_filename);

	//{
	//	std::wstring file_name = obj_filename;
	//	std::wstring add = file_name.substr(0, file_name.find_last_of(L"/") + 1);
	//	std::string add_name(add.begin(), add.end());

	//	std::string file_size = add_name + "size.dat";
	//	std::string file_material = add_name + "material.dat";
	//	std::string file_mtl_name = add_name + "mtl_name.dat";
	//	std::string file_subset = add_name + "subset.dat";
	//	std::string file_subset_name = add_name + "subset_name.dat";
	//	std::string file_vertex = add_name + "vertex.dat";

	//	{
	//		std::fstream file(file_size, std::ios::binary | std::ios::in);

	//		file.read((char*)&size.vertex, sizeof(int));
	//		file.read((char*)&size.material, sizeof(int));
	//		file.read((char*)&size.subset, sizeof(int));
	//		file.close();
	//	}

	//	{
	//		std::vector<vertex> vertices;
	//		std::vector<u_int> indices;
	//		std::fstream file01(file_vertex, std::ios::binary | std::ios::in);

	//		for (int i = 0; i < size.vertex; i++) {
	//			vertex ver;
	//			file01.read((char*)&ver, sizeof(vertex));
	//			ver.normal.x = -ver.normal.x;
	//			ver.normal.y = -ver.normal.y;
	//			ver.normal.z = -ver.normal.z;

	//			vertices.push_back(ver);
	//			indices.push_back(i);
	//		}
	//		file01.close();

	//		CreateBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
	//	}

	//	{
	//		std::fstream file03(file_subset, std::ios::binary | std::ios::in);
	//		std::wfstream use_sub(file_subset_name, std::ios::binary | std::ios::in);

	//		for (int i = 0; i < size.subset; i++) {
	//			subset sub;
	//			file03.read((char*)&save_subset, sizeof(SaveSubset));

	//			sub.index_count = save_subset.index_count;
	//			sub.index_start = save_subset.index_start;

	//			use_sub >> sub.usemtl;
	//			subsets.push_back(sub);
	//		}

	//		use_sub.close();
	//		file03.close();
	//	}

	//	{
	//		std::fstream file02(file_material, std::ios::binary | std::ios::in);
	//		std::wfstream use_mtl(file_mtl_name, std::ios::binary | std::ios::in);

	//		for (int i = 0; i < size.material; i++) {
	//			material mat;
	//			file02.read((char*)&save_material, sizeof(SaveMaterial));

	//			mat.illum = save_material.illum;
	//			mat.Ka = save_material.Ka;
	//			mat.Kd = save_material.Kd;
	//			mat.Ks = save_material.Ks;

	//			use_mtl >> mat.newmtl;

	//			std::wstring check(L"!");

	//			std::wstring tex;
	//			use_mtl >> tex;
	//			if (tex.size() != check.size())
	//			{
	//				mat.map_Kd = tex;
	//			}

	//			mat.shader_resource_view = nullptr;
	//			mat.tex2d_desc = nullptr;

	//			materials.push_back(mat);
	//		}
	//		use_mtl.close();
	//		file02.close();
	//	}
	//}

	HRESULT hr = S_OK;

	////入力レイアウトオブジェクトの生成 
	//D3D11_INPUT_ELEMENT_DESC VertexDesc[] = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};

	//シェーダーの設定
	if (!normal.create("Shader/CSO/static_mesh_vs.cso", "Shader/CSO/static_mesh_ps.cso")) return 8;

	// create rasterizer state : solid mode
	{
		D3D11_RASTERIZER_DESC rasterizer_desc = {};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc, &solid_rasterizer_state);
		if (FAILED(hr)) return 1;
	}
	// create rasterizer state : wireframe mode
	{
		D3D11_RASTERIZER_DESC rasterizer_desc = {};
		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc, &wire_rasterizer_state);
		if (FAILED(hr)) return 2;
	}

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
	if (FAILED(hr)) return 3;

	for (auto &material : materials) {
		//hr = device->CreateSamplerState(&smpDesc, &material.sampler_state);
		//if (FAILED(hr)) return;
		std::wstring null_check;

		if (material.map_Kd.size() == null_check.size()) {
			resource_manager::loadShaderResourceView(device, L"OBJ/dummy.png", &material.shader_resource_view, &material.tex2d_desc);
			continue;
		}
		else if (!resource_manager::loadShaderResourceView(device, material.map_Kd.c_str(), &material.shader_resource_view, &material.tex2d_desc))
		{
			return 4;
		}
	}

	// create depth stencil state
	{
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.StencilEnable = FALSE;
		depth_stencil_desc.StencilReadMask = 0xFF;
		depth_stencil_desc.StencilWriteMask = 0xFF;
		depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil);
		if (FAILED(hr)) return 5;
	}
	// create constant buffer
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(cbuffer0);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&buffer_desc, nullptr, &const_buffer0);
		if (FAILED(hr)) return 6;

		buffer_desc.ByteWidth = sizeof(cbuffer1);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&buffer_desc, nullptr, &const_buffer1);
		if (FAILED(hr)) return 7;
	}

	return 0;
}

void s_mesh::render(ID3D11DeviceContext *immediate_context, const DirectX::XMFLOAT4X4 &world_view_projection, const DirectX::XMFLOAT4X4 &world, const DirectX::XMFLOAT4 &light_direction, const DirectX::XMFLOAT4 &material_color, const DirectX::XMFLOAT4 &camera_pos, const float timer, bool wireframe)
{
	//D3D11_VIEWPORT vp;
	//vp.TopLeftX = 200.0f;
	//vp.TopLeftY = 200.0f;
	//vp.Width = 500.0f;
	//vp.Height = 500.0f;
	//vp.MinDepth = 0.0f;
	//vp.MaxDepth = 1.0f;
	//immediate_context->RSSetViewports(2, &vp);

	u_int stride = sizeof(vertex);
	u_int offset = 0;

	immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	immediate_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	normal.activate();

	immediate_context->OMSetDepthStencilState(depth_stencil, 0);

	if (wireframe)
	{
		immediate_context->RSSetState(wire_rasterizer_state);
	}
	else
	{
		immediate_context->RSSetState(solid_rasterizer_state);
	}

	cbuffer0 data = {};
	data.world_view_projection = world_view_projection;
	data.world = world;
	data.light_direction = light_direction;
	data.material_color.x = material_color.x;
	data.material_color.y = material_color.y;
	data.material_color.z = material_color.z;
	data.material_color.w = material_color.w;
	data.camera_pos = camera_pos;
	data.timer.x = timer;

	immediate_context->UpdateSubresource(const_buffer0, 0, 0, &data, 0, 0);

	immediate_context->VSSetConstantBuffers(0, 1, &const_buffer0);
	immediate_context->PSSetConstantBuffers(0, 1, &const_buffer0);

	float constant = 0.0f;

	for (unsigned int i = 0; i < materials.size(); i++) {
		cbuffer1 cb = {};
		cb.Ambient.x = materials[i].Ka.x;
		cb.Ambient.y = materials[i].Ka.y;
		cb.Ambient.z = materials[i].Ka.z;
		cb.Ambient.w = 1.0f;

		cb.Diffuse.x = materials[i].Kd.x;
		cb.Diffuse.y = materials[i].Kd.y;
		cb.Diffuse.z = materials[i].Kd.z;
		cb.Diffuse.w = 1.0f;

		cb.Specular.x = materials[i].Ks.x;
		cb.Specular.y = materials[i].Ks.y;
		cb.Specular.z = materials[i].Ks.z;
		cb.Specular.w = 1.0f;

		//cb.Ambient.x = constant;
		//cb.Ambient.y = constant;
		//cb.Ambient.z = constant;
		//cb.Ambient.w = constant;

		//cb.Diffuse.x = constant;
		//cb.Diffuse.y = constant;
		//cb.Diffuse.z = constant;
		//cb.Diffuse.w = constant;

		//cb.Specular.x = constant;
		//cb.Specular.y = constant;
		//cb.Specular.z = constant;
		//cb.Specular.w = constant;

		immediate_context->UpdateSubresource(const_buffer1, 0, 0, &cb, 0, 0);

		immediate_context->VSSetConstantBuffers(1, 1, &const_buffer1);
		immediate_context->PSSetConstantBuffers(1, 1, &const_buffer1);

		//if (materials[i].tex2d_desc->Width == 225)
		//	immediate_context->PSSetShaderResources(0, 0, nullptr);
		//else
		immediate_context->PSSetShaderResources(0, 1, &materials[i].shader_resource_view);
		immediate_context->PSSetSamplers(0, 1, &sampler_state);

		for (auto &subset : subsets) {
			if (materials[i].newmtl == subset.usemtl) {
				immediate_context->DrawIndexed(subset.index_count, subset.index_start, 0);
			}
		}
	}

	normal.inactivate();
}

void s_mesh::release()
{
	if (const_buffer0)				const_buffer0->Release();
	if (const_buffer1)				const_buffer1->Release();

	if (index_buffer)				index_buffer->Release();
	if (vertex_buffer)				vertex_buffer->Release();

	if (depth_stencil)				depth_stencil->Release();
	if (solid_rasterizer_state)		solid_rasterizer_state->Release();
	if (wire_rasterizer_state)		wire_rasterizer_state->Release();
}

void s_mesh::createBuffers(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices)
{
	HRESULT hr;
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(vertex)*num_vertices;
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		//buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.

		hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
		if (FAILED(hr)) return;
	}
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(u_int)*num_indices;
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		//.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = indices;
		subresource_data.SysMemPitch = 0; //Not use for index buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, &index_buffer);
		if (FAILED(hr)) return;
	}
}

void s_mesh::loadOBJ(ID3D11Device* device, std::vector<subset>* psubset, const wchar_t* obj_filename)
{
	std::wifstream fin(obj_filename);
	_ASSERT_EXPR(fin, L"'OBJ file not fuund.");

	std::vector<vertex> vertices;
	std::vector<u_int> indices;
	u_int current_index = 0;

	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> texcoords;

	bool is_obj = false;

	subset sub = {};
	std::wstring file_name = obj_filename;
	std::wstring add = file_name.substr(0, file_name.find_last_of(L"/") + 1);
	std::string add_name(add.begin(), add.end());
	//add_name += save_filename;

	std::string file_vertex = add_name + "vertex.dat";

	std::fstream file(file_vertex, std::ios::binary | std::ios::out);

	wchar_t command[256];
	while (fin)
	{
		fin >> command;

		if (0 == wcscmp(command, L"mtllib"))
		{
			std::wstring mtl;

			wchar_t mtl_name[256];
			fin >> mtl_name;
			std::wstring file_name = obj_filename;

			mtl = file_name.substr(0, file_name.find_last_of(L"/") + 1);
			mtl += mtl_name;

			loadMTL(device, &materials, mtl.c_str());

		}
		else if (0 == wcscmp(command, L"usemtl"))
		{
			if (is_obj) {
				psubset->push_back(sub);
				sub.index_start += sub.index_count;
				sub.index_count = 0;
			}

			is_obj = true;

			wchar_t usemtl[256];
			fin >> usemtl;
			sub.usemtl = usemtl;

			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"v"))
		{
			float x, y, z;
			fin >> x >> y >> z;
			positions.push_back(DirectX::XMFLOAT3(x, y, z));
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"vt"))
		{
			float u = 0.0f;
			float v = 0.0f;
			fin >> u >> v;
			texcoords.push_back(DirectX::XMFLOAT2(u, 1 - v));
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"vn"))
		{
			FLOAT i, j, k;
			fin >> i >> j >> k;
			normals.push_back(DirectX::XMFLOAT3(i, j, k));
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"f"))
		{
			for (u_int i = 0; i < 3; i++)
			{
				vertex vertex;
				u_int v, vt, vn;

				fin >> v;
				vertex.position = positions[v - 1];
				if (L'/' == fin.peek())
				{
					fin.ignore();
					if (L'/' != fin.peek())
					{
						fin >> vt;
						vertex.texcoord = texcoords[vt - 1];
					}
					if (L'/' == fin.peek())
					{
						fin.ignore();
						fin >> vn;
						vertex.normal = normals[vn - 1];
					}
				}

				file.write((char*)&vertex, sizeof(vertex));

				vertices.push_back(vertex);
				indices.push_back(current_index++);
			}
			sub.index_count = current_index;
			fin.ignore(1024, L'\n');
		}
		else
		{
			fin.ignore(1024, L'\n');
		}
	}

	file.close();
	fin.close();
	if (is_obj) psubset->push_back(sub);

	//ver = vertices.size();

	createBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}

void s_mesh::loadMTL(ID3D11Device* device, std::vector<material>* pmaterial, const wchar_t* mtl_filename)
{
	std::wifstream mfin(mtl_filename);
	_ASSERT_EXPR(mfin, L"'MTL file not fuund.");
	wchar_t tex_name[256];
	wchar_t com[256];

	bool is_material = false;

	material mat = {};

	while (mfin)
	{
		mfin >> com;	//キーワード読み込み

		if (0 == wcscmp(com, L"newmtl"))	//マテリアル名
		{
			if (is_material) pmaterial->push_back(mat);

			is_material = true;

			wchar_t newmtl[256];
			mfin >> newmtl;
			mat.newmtl = newmtl;

			mfin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(com, L"Ka"))	//Ka　アンビエント
		{
			FLOAT i, j, k;
			mfin >> i >> j >> k;

			mat.Ka = DirectX::XMFLOAT3(i, j, k);

			mfin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(com, L"Kd"))	//Kd　ディフューズ
		{
			FLOAT r, g, b;
			mfin >> r >> g >> b;

			mat.Kd = DirectX::XMFLOAT3(r, g, b);

			mfin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(com, L"Ks"))	//Ks　スペキュラー
		{
			FLOAT i, j, k;
			mfin >> i >> j >> k;

			mat.Ks = DirectX::XMFLOAT3(i, j, k);

			mfin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(com, L"illum"))
		{
			u_int i;
			mfin >> i;

			mat.illum = i;

			mfin.ignore(1024, L'\n');

		}
		else if (0 == wcscmp(com, L"map_Kd"))	//map_Kd　テクスチャ
		{
			mfin >> tex_name;

			std::wstring file_name = mtl_filename;

			mat.map_Kd = file_name.substr(0, file_name.find_last_of(L"/") + 1);
			mat.map_Kd += tex_name;

			mfin.ignore(1024, L'\n');
		}
		else
		{
			mfin.ignore(1024, L'\n');
		}
	}
	mfin.close();
	if (is_material) pmaterial->push_back(mat);
}
