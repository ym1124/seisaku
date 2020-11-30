#pragma once

#include "misc.h"
#include "high_resolution_timer.h"
#include "framework.h"
#include "shader.h"
#include "tessellation_mesh.h"
#include "load_file.h"
#include "blend.h"



tessellation_mesh::tessellation_mesh(const wchar_t *file_name)
{
	HRESULT hr;

	//インプットレイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//シェーダーの読み込み
	//vs_Load(FRAMEWORK.getInstance().getDevice(), "static_mesh_vs.cso", &vs, &input_layout, inputElementDesc, ARRAYSIZE(inputElementDesc));
	//ps_Load(FRAMEWORK.getInstance().getDevice(), "static_mesh_ps.cso", &ps);

	vs_Load(FRAMEWORK.getInstance().getDevice(), "tessellation_mesh_vs.cso", vs.GetAddressOf(), input_layout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	ps_Load(FRAMEWORK.getInstance().getDevice(), "tessellation_mesh_ps.cso", ps.GetAddressOf());
	hs_Load(FRAMEWORK.getInstance().getDevice(), "tessellation_mesh_hs.cso", hs.GetAddressOf());
	ds_Load(FRAMEWORK.getInstance().getDevice(), "tessellation_mesh_ds.cso", ds.GetAddressOf());


	//深度ステンシルステートオブジェクト
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = FRAMEWORK.getInstance().getDevice()->CreateDepthStencilState(&dsDesc, &depth_stencil_state);
	if (FAILED(hr)) {
		return;
	}

	//定数バッファ
	{
		D3D11_BUFFER_DESC cBufferDesc = {};
		cBufferDesc.ByteWidth = sizeof(cbuffer);
		cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cBufferDesc.CPUAccessFlags = 0;
		cBufferDesc.MiscFlags = 0;
		cBufferDesc.StructureByteStride = 0;
		hr = FRAMEWORK.getInstance().getDevice()->CreateBuffer(&cBufferDesc, nullptr, cs_buffer.GetAddressOf());
		if (FAILED(hr))
			return;

		D3D11_BUFFER_DESC cBufferDesc2 = {};
		cBufferDesc2.ByteWidth = sizeof(ConstantBufferForTessellation);
		cBufferDesc2.Usage = D3D11_USAGE_DEFAULT;
		cBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cBufferDesc2.CPUAccessFlags = 0;
		cBufferDesc2.MiscFlags = 0;
		cBufferDesc2.StructureByteStride = 0;
		hr = FRAMEWORK.getInstance().getDevice()->CreateBuffer(&cBufferDesc2, nullptr, cs_buffer2.GetAddressOf());
		if (FAILED(hr))
			return;
	}


	//OBJ読み込み
	//ファイル名探索
	wchar_t temp_file_name[255];
	int fname_sz = 0;
	wcscpy_s(temp_file_name, L"OBJ/");//[3]まで入ってる([4]が0)
	for (;;)
	{
		if (file_name[fname_sz] == 0)
			break;
		fname_sz++;
	}
	int i = 0;
	for (;;)
	{
		temp_file_name[4 + i] = file_name[i];
		i++;
		if (i == fname_sz)
		{
			temp_file_name[4 + i] = L'/';
			i++;
			break;
		}
	}
	int js = 0;
	for (;;)
	{
		temp_file_name[4 + i + js] = file_name[js];
		js++;
		if (js == fname_sz)
		{
			temp_file_name[4 + i + js] = L'.';
			js++;
			temp_file_name[4 + i + js] = L'o';
			js++;
			temp_file_name[4 + i + js] = L'b';
			js++;
			temp_file_name[4 + i + js] = L'j';
			js++;
			temp_file_name[4 + i + js] = 0;
			break;
		}
	}

	//std::vector<vertex> vertices;
	//std::vector<u_int> indices;
	subset subtemp;
	u_int current_index = 0;
	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> texcoord;
	std::wifstream fin(temp_file_name);
	_ASSERT_EXPR(fin, L"OBJ file not found.");
	wchar_t command[256];
	static int j = -1;
	Face f;
	while (fin)
	{
		fin >> command;
		if (0 == wcscmp(command, L"v"))
		{
			float x, y, z;
			fin >> x >> y >> z;
			positions.push_back(DirectX::XMFLOAT3(x, y, z));
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"vn"))
		{
			FLOAT i, j, k;
			fin >> i >> j >> k;
			normals.push_back(DirectX::XMFLOAT3(i, j, k));
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"vt"))
		{
			FLOAT u, v;
			fin >> u >> v;
			texcoord.push_back(DirectX::XMFLOAT2(u, 1 - v));
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"f"))
		{
			static u_int index = 0;
			for (u_int i = 0; i < 3; i++)
			{
				vertex vertex;
				u_int v, vt, vn;
				fin >> v;
				vertex.position = positions[v - 1];
				f.position[i] = positions[v - 1];
				if (L'/' == fin.peek())
				{
					fin.ignore();
					if (L'/' != fin.peek())
					{
						fin >> vt;
						vertex.texcoord = texcoord[vt - 1];
					}
					if (L'/' == fin.peek())
					{
						fin.ignore();
						fin >> vn;
						vertex.normal = normals[vn - 1];
					}
				}
				vertices.push_back(vertex);
				indices.push_back(current_index++);
				subsets[j].index_count++;
			}
			faces.push_back(f);
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"usemtl"))
		{
			j++;
			subsets.push_back(subtemp);
			std::wstring st_temp;
			fin >> st_temp;
			subsets[j].usemtl = st_temp;
			subsets[j].index_start = current_index;
		}
		else
		{
			fin.ignore(1024, L'\n');
		}
	}
	fin.close();
	j = -1;

	LoadMaterialFromFile(FRAMEWORK.getInstance().getDevice(), file_name);

	create_buffer(FRAMEWORK.getInstance().getDevice(), vertices.data(), vertices.size(), indices.data(), indices.size());

}

tessellation_mesh::tessellation_mesh(const wchar_t* file_name, const wchar_t* shader_name)
{
	{
		HRESULT hr;

		//インプットレイアウト
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		for (int i = 0; i < 4;i++)
		{
			wchar_t str[8];
			//シェーダーの読み込み
			switch(i)
			{
			case 0 :
				wcscpy_s(str, L"_vs.cso");
				break;
			case 1:
				wcscpy_s(str, L"_ps.cso");
				break;
			case 2:
				wcscpy_s(str, L"_hs.cso");
				break;
			case 3:
				wcscpy_s(str, L"_ds.cso");
				break;
			}
			
			wchar_t src[255];
			wcscpy_s(src, shader_name);
			wcscat_s(src, sizeof(src) / sizeof(src[0]), str);
			size_t *num = nullptr;
			char tmp[255];
			wcstombs_s(num, tmp, sizeof(tmp), src, sizeof(src) / sizeof(src[0]));

			switch (i)
			{
			case 0:
				vs_Load(FRAMEWORK.getInstance().getDevice(), tmp, vs.GetAddressOf(), input_layout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
				break;
			case 1:
				ps_Load(FRAMEWORK.getInstance().getDevice(), tmp, ps.GetAddressOf());
				break;
			case 2:
				hs_Load(FRAMEWORK.getInstance().getDevice(), tmp, hs.GetAddressOf());
				break;
			case 3:
				ds_Load(FRAMEWORK.getInstance().getDevice(), tmp, ds.GetAddressOf());
				break;
			}
		}

		//深度ステンシルステートオブジェクト
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsDesc.StencilEnable = FALSE;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		hr = FRAMEWORK.getInstance().getDevice()->CreateDepthStencilState(&dsDesc, &depth_stencil_state);
		if (FAILED(hr)) {
			return;
		}

		//定数バッファ
		{
			D3D11_BUFFER_DESC cBufferDesc = {};
			cBufferDesc.ByteWidth = sizeof(cbuffer);
			cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cBufferDesc.CPUAccessFlags = 0;
			cBufferDesc.MiscFlags = 0;
			cBufferDesc.StructureByteStride = 0;
			hr = FRAMEWORK.getInstance().getDevice()->CreateBuffer(&cBufferDesc, nullptr, cs_buffer.GetAddressOf());
			if (FAILED(hr))
				return;

			D3D11_BUFFER_DESC cBufferDesc2 = {};
			cBufferDesc2.ByteWidth = sizeof(ConstantBufferForTessellation);
			cBufferDesc2.Usage = D3D11_USAGE_DEFAULT;
			cBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cBufferDesc2.CPUAccessFlags = 0;
			cBufferDesc2.MiscFlags = 0;
			cBufferDesc2.StructureByteStride = 0;
			hr = FRAMEWORK.getInstance().getDevice()->CreateBuffer(&cBufferDesc2, nullptr, cs_buffer2.GetAddressOf());
			if (FAILED(hr))
				return;
		}


		//OBJ読み込み
		//ファイル名探索
		wchar_t temp_file_name[255];
		int fname_sz = 0;
		wcscpy_s(temp_file_name, L"OBJ/");//[3]まで入ってる([4]が0)
		for (;;)
		{
			if (file_name[fname_sz] == 0)
				break;
			fname_sz++;
		}
		int i = 0;
		for (;;)
		{
			temp_file_name[4 + i] = file_name[i];
			i++;
			if (i == fname_sz)
			{
				temp_file_name[4 + i] = L'/';
				i++;
				break;
			}
		}
		int js = 0;
		for (;;)
		{
			temp_file_name[4 + i + js] = file_name[js];
			js++;
			if (js == fname_sz)
			{
				temp_file_name[4 + i + js] = L'.';
				js++;
				temp_file_name[4 + i + js] = L'o';
				js++;
				temp_file_name[4 + i + js] = L'b';
				js++;
				temp_file_name[4 + i + js] = L'j';
				js++;
				temp_file_name[4 + i + js] = 0;
				break;
			}
		}

		//std::vector<vertex> vertices;
		//std::vector<u_int> indices;
		subset subtemp;
		u_int current_index = 0;
		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<DirectX::XMFLOAT2> texcoord;
		std::wifstream fin(temp_file_name);
		_ASSERT_EXPR(fin, L"OBJ file not found.");
		wchar_t command[256];
		static int j = -1;
		Face f;
		while (fin)
		{
			fin >> command;
			if (0 == wcscmp(command, L"v"))
			{
				float x, y, z;
				fin >> x >> y >> z;
				positions.push_back(DirectX::XMFLOAT3(x, y, z));
				fin.ignore(1024, L'\n');
			}
			else if (0 == wcscmp(command, L"vn"))
			{
				FLOAT i, j, k;
				fin >> i >> j >> k;
				normals.push_back(DirectX::XMFLOAT3(i, j, k));
				fin.ignore(1024, L'\n');
			}
			else if (0 == wcscmp(command, L"vt"))
			{
				FLOAT u, v;
				fin >> u >> v;
				texcoord.push_back(DirectX::XMFLOAT2(u, 1 - v));
				fin.ignore(1024, L'\n');
			}
			else if (0 == wcscmp(command, L"f"))
			{
				static u_int index = 0;
				for (u_int i = 0; i < 3; i++)
				{
					vertex vertex;
					u_int v, vt, vn;
					fin >> v;
					vertex.position = positions[v - 1];
					f.position[i] = positions[v - 1];
					if (L'/' == fin.peek())
					{
						fin.ignore();
						if (L'/' != fin.peek())
						{
							fin >> vt;
							vertex.texcoord = texcoord[vt - 1];
						}
						if (L'/' == fin.peek())
						{
							fin.ignore();
							fin >> vn;
							vertex.normal = normals[vn - 1];
						}
					}
					vertices.push_back(vertex);
					indices.push_back(current_index++);
					subsets[j].index_count++;
				}
				faces.push_back(f);
				fin.ignore(1024, L'\n');
			}
			else if (0 == wcscmp(command, L"usemtl"))
			{
				j++;
				subsets.push_back(subtemp);
				std::wstring st_temp;
				fin >> st_temp;
				subsets[j].usemtl = st_temp;
				subsets[j].index_start = current_index;
			}
			else
			{
				fin.ignore(1024, L'\n');
			}
		}
		fin.close();
		j = -1;

		LoadMaterialFromFile(FRAMEWORK.getInstance().getDevice(), file_name);

		create_buffer(FRAMEWORK.getInstance().getDevice(), vertices.data(), vertices.size(), indices.data(), indices.size());

	}
}

tessellation_mesh::~tessellation_mesh()
{
	//Materials.clear();

	for (auto &it : Materials)
	{
		if (it.pTexture)
		{
			it.pTexture->Release();
			it.pTexture = NULL;
		}
	}
	//if (vs)
	//{
	//	vs->Release();
	//	vs = NULL;
	//}
	//if (ps)
	//{
	//	ps->Release();
	//	ps = NULL;
	//}
	//if (gs)
	//{
	//	gs->Release();
	//	gs = NULL;
	//}
	//if (input_layout)
	//{
	//	input_layout->Release();
	//	input_layout = NULL;
	//}
	//if (vertex_buffer)
	//{
	//	vertex_buffer->Release();
	//	vertex_buffer = NULL;
	//}
	//if (index_buffer)
	//{
	//	index_buffer->Release();
	//	index_buffer = NULL;
	//}
	//if (cs_buffer)
	//{
	//	cs_buffer->Release();
	//	cs_buffer = NULL;
	//}

	//if (depth_stencil_state)
	//{
	//	depth_stencil_state->Release();
	//	depth_stencil_state = NULL;
	//}
	//if (sampler_state)
	//{
	//	sampler_state->Release();
	//	sampler_state = NULL;
	//}
}

void tessellation_mesh::render(ID3D11DeviceContext *pDeviceContext, const DirectX::XMFLOAT4X4&world_view_projection, const DirectX::XMFLOAT4X4 &world, const DirectX::XMFLOAT4 &light_direction, const DirectX::XMFLOAT4 &material_color, bool wireframe, int tess_factor, int inside_tess_factor,float depth)
{
	u_int stride = sizeof(vertex);
	u_int offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	pDeviceContext->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	/*pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);*/
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	pDeviceContext->IASetInputLayout(input_layout.Get());
	pDeviceContext->VSSetShader(vs.Get(), nullptr, 0);
	pDeviceContext->HSSetShader(hs.Get(), nullptr, 0);
	pDeviceContext->DSSetShader(ds.Get(), nullptr, 0);
	pDeviceContext->PSSetShader(ps.Get(), nullptr, 0);

	pDeviceContext->OMSetDepthStencilState(depth_stencil_state.Get(), 1);

	if(wireframe)
		FRAMEWORK.getImmediateContext()->RSSetState(FRAMEWORK.getRasterizerState(FRAMEWORK.RS::RS_WIRE));
	else
		FRAMEWORK.getImmediateContext()->RSSetState(FRAMEWORK.getRasterizerState(FRAMEWORK.RS::RS_CULL_BACK));

	cbuffer data;
	data.world_view_projection = world_view_projection;
	data.world = world;
	data.light_direction = light_direction;
	//data.material_color.x = material_color.x * material.Kd.x;
	//data.material_color.y = material_color.y * material.Kd.y;
	//data.material_color.z = material_color.z * material.Kd.z;
	//data.material_color.w = material_color.w;
	data.material_color.x = 1;
	data.material_color.y = 1;
	data.material_color.z = 1;
	data.material_color.w = 1;
	pDeviceContext->UpdateSubresource(cs_buffer.Get(), 0, 0, &data, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());
	pDeviceContext->HSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());
	pDeviceContext->DSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());
	pDeviceContext->PSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());

	ConstantBufferForTessellation cbt;
	cbt.tessFactor = tess_factor;
	cbt.insideTessFactor = inside_tess_factor;
	cbt.depth = depth;
	pDeviceContext->UpdateSubresource(cs_buffer2.Get(), 0, 0, &cbt, 0, 0);
	pDeviceContext->VSSetConstantBuffers(2, 1, cs_buffer2.GetAddressOf());
	pDeviceContext->HSSetConstantBuffers(2, 1, cs_buffer2.GetAddressOf());
	pDeviceContext->DSSetConstantBuffers(2, 1, cs_buffer2.GetAddressOf());
	pDeviceContext->PSSetConstantBuffers(2, 1, cs_buffer2.GetAddressOf());

	for (auto &material : Materials)
	{
		pDeviceContext->PSSetShaderResources(0, 1, &material.pTexture);
		pDeviceContext->DSSetShaderResources(0, 1, &material.pTexture);
		pDeviceContext->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::LINEAR_CLAMP));
		pDeviceContext->DSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::LINEAR_CLAMP));
		for (auto &subset : subsets)
		{
			wchar_t temp[256];
			size_t wLen = 0;
			mbstowcs_s(&wLen, temp, sizeof(temp) / sizeof(wchar_t), material.Name, sizeof(material.Name) / sizeof(char));
			if (temp == subset.usemtl)
			{
				pDeviceContext->DrawIndexed(subset.index_count, subset.index_start, 0);
				break;
			}
		}
	}
	pDeviceContext->VSSetShader(nullptr, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(nullptr, nullptr, 0);
}

void tessellation_mesh::render(ID3D11DeviceContext *pDeviceContext, const DirectX::XMFLOAT4X4&world_view_projection, const DirectX::XMFLOAT4X4 &world, const DirectX::XMFLOAT4 &light_direction, const DirectX::XMFLOAT4 &material_color, bool wireframe, int tess_factor, int inside_tess_factor, float depth,ID3D11ShaderResourceView *srv)
{
	u_int stride = sizeof(vertex);
	u_int offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	pDeviceContext->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	/*pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);*/
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	pDeviceContext->IASetInputLayout(input_layout.Get());
	pDeviceContext->VSSetShader(vs.Get(), nullptr, 0);
	pDeviceContext->HSSetShader(hs.Get(), nullptr, 0);
	pDeviceContext->DSSetShader(ds.Get(), nullptr, 0);
	pDeviceContext->PSSetShader(ps.Get(), nullptr, 0);

	pDeviceContext->OMSetDepthStencilState(depth_stencil_state.Get(), 1);

	if (wireframe)
		FRAMEWORK.getImmediateContext()->RSSetState(FRAMEWORK.getRasterizerState(FRAMEWORK.RS::RS_WIRE));
	else
		FRAMEWORK.getImmediateContext()->RSSetState(FRAMEWORK.getRasterizerState(FRAMEWORK.RS::RS_CULL_BACK));

	cbuffer data;
	data.world_view_projection = world_view_projection;
	data.world = world;
	data.light_direction = light_direction;
	//data.material_color.x = material_color.x * material.Kd.x;
	//data.material_color.y = material_color.y * material.Kd.y;
	//data.material_color.z = material_color.z * material.Kd.z;
	//data.material_color.w = material_color.w;
	data.material_color.x = 1;
	data.material_color.y = 1;
	data.material_color.z = 1;
	data.material_color.w = 1;
	pDeviceContext->UpdateSubresource(cs_buffer.Get(), 0, 0, &data, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());
	pDeviceContext->HSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());
	pDeviceContext->DSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());
	pDeviceContext->PSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());

	ConstantBufferForTessellation cbt;
	cbt.tessFactor = tess_factor;
	cbt.insideTessFactor = inside_tess_factor;
	cbt.depth = depth;
	pDeviceContext->UpdateSubresource(cs_buffer2.Get(), 0, 0, &cbt, 0, 0);
	pDeviceContext->VSSetConstantBuffers(2, 1, cs_buffer2.GetAddressOf());
	pDeviceContext->HSSetConstantBuffers(2, 1, cs_buffer2.GetAddressOf());
	pDeviceContext->DSSetConstantBuffers(2, 1, cs_buffer2.GetAddressOf());
	pDeviceContext->PSSetConstantBuffers(2, 1, cs_buffer2.GetAddressOf());

	pDeviceContext->PSSetShaderResources(0, 1, &srv);
	pDeviceContext->DSSetShaderResources(0, 1, &srv);
	pDeviceContext->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::LINEAR_CLAMP));
	pDeviceContext->DSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::LINEAR_CLAMP));

	for (auto &material : Materials)
	{
		for (auto &subset : subsets)
		{
			wchar_t temp[256];
			size_t wLen = 0;
			mbstowcs_s(&wLen, temp, sizeof(temp) / sizeof(wchar_t), material.Name, sizeof(material.Name) / sizeof(char));
			if (temp == subset.usemtl)
			{
				pDeviceContext->DrawIndexed(subset.index_count, subset.index_start, 0);
				break;
			}
		}
	}
	pDeviceContext->VSSetShader(nullptr, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(nullptr, nullptr, 0);
}


void tessellation_mesh::create_buffer(ID3D11Device *pDevice, vertex *vertices, u_int num_vertices, u_int *indices, u_int num_indices)
{
	HRESULT hr;
	//頂点バッファの作成
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(vertex) * num_vertices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.

		hr = pDevice->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
		if (FAILED(hr)) {
			return;
		}
	}

	//インデックスバッファ
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(u_int) * num_indices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = indices;
		subresource_data.SysMemPitch = 0; //Not use for index buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
		hr = pDevice->CreateBuffer(&buffer_desc, &subresource_data, &index_buffer);
		if (FAILED(hr)) {
			return;
		}
	}
}

void tessellation_mesh::create_buffer(ID3D11Device *pDevice, vertex_tangent *vertices, u_int num_vertices, u_int *indices, u_int num_indices)
{
	HRESULT hr;
	//頂点バッファの作成
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(vertex_tangent) * num_vertices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.

		hr = pDevice->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
		if (FAILED(hr)) {
			return;
		}
	}

	//インデックスバッファ
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(u_int) * num_indices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = indices;
		subresource_data.SysMemPitch = 0; //Not use for index buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
		hr = pDevice->CreateBuffer(&buffer_desc, &subresource_data, &index_buffer);
		if (FAILED(hr)) {
			return;
		}
	}
}


//マテリアルファイルを読み込む関数
HRESULT tessellation_mesh::LoadMaterialFromFile(ID3D11Device* pDevice, const wchar_t* FileName)
{
	//ファイル名探索
	wchar_t temp_file_name[255];
	int fname_sz = 0;
	wcscpy_s(temp_file_name, L"OBJ/");//[3]まで入ってる([4]が0)
	for (;;)
	{
		if (FileName[fname_sz] == 0)
			break;
		fname_sz++;
	}
	int i = 0;
	for (;;)
	{
		temp_file_name[4 + i] = FileName[i];
		i++;
		if (i == fname_sz)
		{
			temp_file_name[4 + i] = L'/';
			i++;
			break;
		}
	}
	int js = 0;
	for (;;)
	{
		temp_file_name[4 + i + js] = FileName[js];
		js++;
		if (js == fname_sz)
		{
			temp_file_name[4 + i + js] = L'.';
			js++;
			temp_file_name[4 + i + js] = L'm';
			js++;
			temp_file_name[4 + i + js] = L't';
			js++;
			temp_file_name[4 + i + js] = L'l';
			js++;
			temp_file_name[4 + i + js] = 0;
			break;
		}
	}

	material temp;
	std::wifstream fin(temp_file_name);
	_ASSERT_EXPR(fin, L"OBJ file not fuund.");
	wchar_t command[256];
	wchar_t name[256];
	bool flag = false;
	while (fin)
	{
		fin >> command;
		if (0 == wcscmp(command, L"newmtl"))
		{
			flag = true;
			fin >> name;
			int i = 0;
			for (;;)
			{
				temp.Name[i] = name[i];
				if (name[i] == 0)
					break;
				i++;
			}
			//strcpy_s(Material.Name, name);
			fin.ignore(1024, L'\n');
		}
		if (0 == wcscmp(command, L"Ka"))
		{
			float x, y, z;
			fin >> x >> y >> z;
			temp.Ka.x = x;
			temp.Ka.y = y;
			temp.Ka.z = z;
			fin.ignore(1024, L'\n');
		}
		if (0 == wcscmp(command, L"Kd"))
		{
			float x, y, z;
			fin >> x >> y >> z;
			temp.Kd.x = x;
			temp.Kd.y = y;
			temp.Kd.z = z;
			fin.ignore(1024, L'\n');
		}
		if (0 == wcscmp(command, L"Ks"))
		{
			float x, y, z;
			fin >> x >> y >> z;
			temp.Ks.x = x;
			temp.Ks.y = y;
			temp.Ks.z = z;
			fin.ignore(1024, L'\n');
		}
		if (0 == wcscmp(command, L"map_Kd"))
		{
			int i = 0;
			int offset = 0;
			fin >> name;
			//名前探査
			wchar_t tfile_name[255];
			int fname_sz = 0;
			wcscpy_s(tfile_name, L"OBJ/");//[3]まで入ってる([4]が0)
			for (;;)
			{
				if (FileName[fname_sz] == 0)
					break;
				fname_sz++;
			}
			i = 0;
			for (;;)
			{
				tfile_name[4 + i] = FileName[i];
				i++;
				if (i == fname_sz)
				{
					tfile_name[4 + i] = L'/';
					i++;
					tfile_name[4 + i] = 0;
					break;
				}
			}
			wcscpy_s(temp.TextureName, tfile_name);
			//wcscpy_s(temp.TextureName, L"OBJ/Cup/");
			for (;;)
			{
				static int cnt = 0;
				if (!offset)
				{
					for (;;)
					{
						if (temp.TextureName[cnt] == 0)
						{
							cnt = 0;
							break;
						}
						offset++;
						cnt++;
					}
				}
				temp.TextureName[offset + cnt] = name[cnt];
				if (name[cnt] == 0)
				{
					cnt = 0;
					break;
				}
				cnt++;
			}
			//テクスチャを作成
			if (FAILED(load_texture_from_file(pDevice, temp.TextureName, &temp.pTexture, &temp.texture2d_desc)))
			{
				return E_FAIL;
			}
			fin.ignore(1024, L'\n');
			Materials.push_back(temp);
		}
		//else
		//{
		//	fin.ignore(1024, L'\n');
		//}
	}
	fin.close();
	return S_OK;
}