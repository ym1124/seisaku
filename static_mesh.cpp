#pragma once

//#include <windows.h>
//#include <tchar.h>
//#include <sstream>
//#include <memory>
//#include <d3d11.h>
//#include <wrl.h>
//#include <DirectXMath.h>
//#include <fstream>

#include "misc.h"
#include "high_resolution_timer.h"
#include "framework.h"
#include "shader.h"
#include "static_mesh.h"
#include "load_file.h"
#include "blend.h"

//#pragma comment(lib,"d3d11.lib")


//static_mesh::static_mesh(const wchar_t *file_name, bool culloff)
//{
//	HRESULT hr;
//
//	//インプットレイアウト
//	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//
//	//シェーダーの読み込み
//	vs_Load(FRAMEWORK.getInstance().getDevice(), "static_mesh_vs.cso", &vs, &input_layout, inputElementDesc, ARRAYSIZE(inputElementDesc));
//	ps_Load(FRAMEWORK.getInstance().getDevice(), "static_mesh_ps.cso", &ps);
//
//	//ラスタライザーステートオブジェクト(ソリッド)
//	D3D11_RASTERIZER_DESC rasterizer_desc;
//	rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
//	if (!culloff)
//		rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
//	else
//		rasterizer_desc.CullMode = D3D11_CULL_NONE;//←網用
//	rasterizer_desc.FrontCounterClockwise = FALSE;
//	rasterizer_desc.DepthBias = 0;
//	rasterizer_desc.DepthBiasClamp = 0;
//	rasterizer_desc.SlopeScaledDepthBias = 0;
//	rasterizer_desc.DepthClipEnable = TRUE;
//	rasterizer_desc.ScissorEnable = FALSE;
//	rasterizer_desc.MultisampleEnable = FALSE;
//	rasterizer_desc.AntialiasedLineEnable = FALSE;
//	hr = FRAMEWORK.getInstance().getDevice()->CreateRasterizerState(&rasterizer_desc, &rasterizer_states[0]);
//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//
//	//ラスタライザーステートオブジェクト(ワイヤーフレーム)
//	D3D11_RASTERIZER_DESC rDesc;
//	ZeroMemory(&rDesc, sizeof(rDesc));
//	rDesc.FillMode = D3D11_FILL_WIREFRAME;
//	rDesc.CullMode = D3D11_CULL_BACK;
//	rDesc.FrontCounterClockwise = FALSE;
//	rDesc.DepthClipEnable = TRUE;//いる？
//	hr = FRAMEWORK.getInstance().getDevice()->CreateRasterizerState(&rDesc, &rasterizer_states[1]);
//	if (FAILED(hr)) {
//		return;
//	}
//
//	////サンプラーの記述
//	//D3D11_SAMPLER_DESC sampler_desc;
//	//sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //D3D11_FILTER_ANISOTROPIC
//	//sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
//	//sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
//	//sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
//	//sampler_desc.MipLODBias = 0;
//	//sampler_desc.MaxAnisotropy = 16;
//	//sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
//	//memcpy(sampler_desc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
//	//sampler_desc.MinLOD = 0;
//	//sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
//
//	////サンプラーステート生成
//	//hr = FRAMEWORK.getInstance().getDevice()->CreateSamplerState(&sampler_desc, &sampler_state);
//	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//	//深度ステンシルステートオブジェクト
//	D3D11_DEPTH_STENCIL_DESC dsDesc;
//	dsDesc.DepthEnable = TRUE;
//	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
//	dsDesc.StencilEnable = FALSE;
//	dsDesc.StencilReadMask = 0xFF;
//	dsDesc.StencilWriteMask = 0xFF;
//	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
//	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
//	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//
//	hr = FRAMEWORK.getInstance().getDevice()->CreateDepthStencilState(&dsDesc, &depth_stencil_state);
//	if (FAILED(hr)) {
//		return;
//	}
//
//	//定数バッファ
//	{
//		D3D11_BUFFER_DESC cBufferDesc = {};
//		cBufferDesc.ByteWidth = sizeof(cbuffer);
//		cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//		cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//		cBufferDesc.CPUAccessFlags = 0;
//		cBufferDesc.MiscFlags = 0;
//		cBufferDesc.StructureByteStride = 0;
//		hr = FRAMEWORK.getInstance().getDevice()->CreateBuffer(&cBufferDesc, nullptr, &cs_buffer);
//		if (FAILED(hr))
//			return;
//	}
//
//	//OBJ読み込み
//	//ファイル名探索
//	wchar_t temp_file_name[255];
//	int fname_sz = 0;
//	wcscpy_s(temp_file_name, L"OBJ/");//[3]まで入ってる([4]が0)
//	for (;;)
//	{
//		if (file_name[fname_sz] == 0)
//			break;
//		fname_sz++;
//	}
//	int i = 0;
//	for (;;)
//	{
//		temp_file_name[4 + i] = file_name[i];
//		i++;
//		if (i == fname_sz)
//		{
//			temp_file_name[4 + i] = L'/';
//			i++;
//			break;
//		}
//	}
//	int js = 0;
//	for (;;)
//	{
//		temp_file_name[4 + i + js] = file_name[js];
//		js++;
//		if (js == fname_sz)
//		{
//			temp_file_name[4 + i + js] = L'.';
//			js++;
//			temp_file_name[4 + i + js] = L'o';
//			js++;
//			temp_file_name[4 + i + js] = L'b';
//			js++;
//			temp_file_name[4 + i + js] = L'j';
//			js++;
//			temp_file_name[4 + i + js] = 0;
//			break;
//		}
//	}
//
//	std::vector<vertex> vertices;
//	std::vector<u_int> indices;
//	subset subtemp;
//	u_int current_index = 0;
//	std::vector<DirectX::XMFLOAT3> positions;
//	std::vector<DirectX::XMFLOAT3> normals;
//	std::vector<DirectX::XMFLOAT2> texcoord;
//	std::wifstream fin(temp_file_name);
//	//std::wifstream fin(L"OBJ/Cup/cup.obj");
//	_ASSERT_EXPR(fin, L"OBJ file not found.");
//	wchar_t command[256];
//	static int j = -1;
//	Face f;
//	while (fin)
//	{
//		fin >> command;
//		if (0 == wcscmp(command, L"v"))
//		{
//			float x, y, z;
//			fin >> x >> y >> z;
//			positions.push_back(DirectX::XMFLOAT3(x, y, z));
//			fin.ignore(1024, L'\n');
//		}
//		else if (0 == wcscmp(command, L"vn"))
//		{
//			FLOAT i, j, k;
//			fin >> i >> j >> k;
//			normals.push_back(DirectX::XMFLOAT3(i, j, k));
//			fin.ignore(1024, L'\n');
//		}
//		else if (0 == wcscmp(command, L"vt"))
//		{
//			FLOAT u, v;
//			fin >> u >> v;
//			texcoord.push_back(DirectX::XMFLOAT2(u, 1 - v));
//			fin.ignore(1024, L'\n');
//		}
//		else if (0 == wcscmp(command, L"f"))
//		{
//			static u_int index = 0;
//			for (u_int i = 0; i < 3; i++)
//			{
//				vertex vertex;
//				u_int v, vt, vn;
//				fin >> v;
//				vertex.position = positions[v - 1];
//				f.position[i] = positions[v - 1];
//				if (L'/' == fin.peek())
//				{
//					fin.ignore();
//					if (L'/' != fin.peek())
//					{
//						fin >> vt;
//						vertex.texcoord = texcoord[vt - 1];
//					}
//					if (L'/' == fin.peek())
//					{
//						fin.ignore();
//						fin >> vn;
//						vertex.normal = normals[vn - 1];
//					}
//				}
//				vertices.push_back(vertex);
//				indices.push_back(current_index++);
//				subsets[j].index_count++;
//			}
//			faces.push_back(f);
//			fin.ignore(1024, L'\n');
//		}
//		else if (0 == wcscmp(command, L"usemtl"))
//		{
//			j++;
//			subsets.push_back(subtemp);
//			std::wstring st_temp;
//			fin >> st_temp;
//			subsets[j].usemtl = st_temp;
//			subsets[j].index_start = current_index;
//		}
//		else
//		{
//			fin.ignore(1024, L'\n');
//		}
//	}
//	fin.close();
//	j = -1;
//
//	LoadMaterialFromFile(FRAMEWORK.getInstance().getDevice(), file_name);
//
//	create_buffer(FRAMEWORK.getInstance().getDevice(), vertices.data(), vertices.size(), indices.data(), indices.size());
//}

static_mesh::static_mesh(const wchar_t *file_name, bool culloff, bool tangent)
{
	HRESULT hr;

	if (!tangent)
	{
		//インプットレイアウト
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		//シェーダーの読み込み
		vs_Load(FRAMEWORK.getInstance().getDevice(), "Shader/static_mesh_vs.cso", &vs, &input_layout, inputElementDesc, ARRAYSIZE(inputElementDesc));
		ps_Load(FRAMEWORK.getInstance().getDevice(), "Shader/static_mesh_ps.cso", &ps);
	}

	else
	{
		//インプットレイアウト
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		//シェーダーの読み込み
		vs_Load(FRAMEWORK.getInstance().getDevice(), "Shader/static_mesh_tangent_vs.cso", &vs, &input_layout, inputElementDesc, ARRAYSIZE(inputElementDesc));
		ps_Load(FRAMEWORK.getInstance().getDevice(), "Shader/static_mesh_tangent_ps.cso", &ps);
	}

	//ラスタライザーステートオブジェクト(ソリッド)
	D3D11_RASTERIZER_DESC rasterizer_desc;
	rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	if (!culloff)
		rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
	else
		rasterizer_desc.CullMode = D3D11_CULL_NONE;//←網用
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	hr = FRAMEWORK.getInstance().getDevice()->CreateRasterizerState(&rasterizer_desc, &rasterizer_states[0]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//ラスタライザーステートオブジェクト(ワイヤーフレーム)
	D3D11_RASTERIZER_DESC rDesc;
	ZeroMemory(&rDesc, sizeof(rDesc));
	rDesc.FillMode = D3D11_FILL_WIREFRAME;
	rDesc.CullMode = D3D11_CULL_BACK;
	rDesc.FrontCounterClockwise = FALSE;
	rDesc.DepthClipEnable = TRUE;//いる？
	hr = FRAMEWORK.getInstance().getDevice()->CreateRasterizerState(&rDesc, &rasterizer_states[1]);
	if (FAILED(hr)) {
		return;
	}

	////サンプラーの記述
	//D3D11_SAMPLER_DESC sampler_desc;
	//sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //D3D11_FILTER_ANISOTROPIC
	//sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	//sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	//sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	//sampler_desc.MipLODBias = 0;
	//sampler_desc.MaxAnisotropy = 16;
	//sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//memcpy(sampler_desc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	//sampler_desc.MinLOD = 0;
	//sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	////サンプラーステート生成
	//hr = FRAMEWORK.getInstance().getDevice()->CreateSamplerState(&sampler_desc, &sampler_state);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

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
		hr = FRAMEWORK.getInstance().getDevice()->CreateBuffer(&cBufferDesc, nullptr, &cs_buffer);
		if (FAILED(hr))
			return;
	}

	if (!tangent)
	{
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
						else
						{
							vertex.texcoord = DirectX::XMFLOAT2(0.5f, 0.5f);
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

	else
	{
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

		std::vector<vertex_tangent> vertices;
		std::vector<u_int> indices;
		subset subtemp;
		u_int current_index = 0;
		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<DirectX::XMFLOAT2> texcoord;
		std::wifstream fin(temp_file_name);
		//std::wifstream fin(L"OBJ/Cup/cup.obj");
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
					vertex_tangent vertex;
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

static_mesh::~static_mesh()
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
	if (vs)
	{
		vs->Release();
		vs = NULL;
	}
	if (ps)
	{
		ps->Release();
		ps = NULL;
	}
	if (gs)
	{
		gs->Release();
		gs = NULL;
	}
	if (input_layout)
	{
		input_layout->Release();
		input_layout = NULL;
	}
	if (vertex_buffer)
	{
		vertex_buffer->Release();
		vertex_buffer = NULL;
	}
	if (index_buffer)
	{
		index_buffer->Release();
		index_buffer = NULL;
	}
	if (cs_buffer)
	{
		cs_buffer->Release();
		cs_buffer = NULL;
	}
	if (rasterizer_states[0])
	{
		rasterizer_states[0]->Release();
		rasterizer_states[0] = NULL;
	}
	if (rasterizer_states[1])
	{
		rasterizer_states[1]->Release();
		rasterizer_states[1] = NULL;
	}
	if (depth_stencil_state)
	{
		depth_stencil_state->Release();
		depth_stencil_state = NULL;
	}
	//if (sampler_state)
	//{
	//	sampler_state->Release();
	//	sampler_state = NULL;
	//}
}

void static_mesh::render(ID3D11DeviceContext *pDeviceContext, const DirectX::XMFLOAT4X4&world_view_projection, const DirectX::XMFLOAT4X4 &world, const DirectX::XMFLOAT4 &light_direction, const DirectX::XMFLOAT4 &material_color, bool DSLflg, bool wireframe, bool cull)
{
	u_int stride = sizeof(vertex);
	u_int offset = 0;

	//if (tangent)
	//	stride = sizeof(vertex_tangent);

	pDeviceContext->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (!DSLflg)
	{
		pDeviceContext->IASetInputLayout(input_layout);
		pDeviceContext->VSSetShader(vs, nullptr, 0);
		pDeviceContext->PSSetShader(ps, nullptr, 0);
		pDeviceContext->GSSetShader(gs, nullptr, 0);
	}
	pDeviceContext->OMSetDepthStencilState(depth_stencil_state, 1);

	if (wireframe)
	{
		pDeviceContext->RSSetState(rasterizer_states[1]);
	}
	else
	{
		pDeviceContext->RSSetState(rasterizer_states[0]);
	}

	for (auto &material : Materials)
	{
		cbuffer data;
		data.world_view_projection = world_view_projection;
		data.world = world;
		data.light_direction = light_direction;
		data.material_color.x = material_color.x * material.Kd.x;
		data.material_color.y = material_color.y * material.Kd.y;
		data.material_color.z = material_color.z * material.Kd.z;
		data.material_color.w = material_color.w;
		/*data.material_color.x = 1;
		data.material_color.y = 1;
		data.material_color.z = 1;
		data.material_color.w = 1;*/
		pDeviceContext->UpdateSubresource(cs_buffer, 0, 0, &data, 0, 0);
		if (!DSLflg)
		{
			pDeviceContext->VSSetConstantBuffers(0, 1, &cs_buffer);
			pDeviceContext->PSSetConstantBuffers(0, 1, &cs_buffer);
		}
		pDeviceContext->PSSetShaderResources(0, 1, &material.pTexture);
		pDeviceContext->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::POINT_BORDER));
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
}

void static_mesh::render(ID3D11DeviceContext *pDeviceContext, shader *shader, camera* camera, const DirectX::XMFLOAT4X4 &world,const DirectX::XMFLOAT4 &light_direction,/* const DirectX::XMFLOAT4 &material_color,*/ bool wireframe,bool cull)
{
	//obj
	u_int stride = sizeof(vertex);
	u_int offset = 0;

	shader->Activate();
	blender* blend = new blender(FRAMEWORK.getInstance().getDevice());

	pDeviceContext->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetInputLayout(input_layout);
	pDeviceContext->OMSetDepthStencilState(depth_stencil_state, 1);
	pDeviceContext->OMSetBlendState(blend->states[blender::BS_ALPHA].Get(), nullptr, 0xFFFFFFFF);

	if (wireframe)
	{
		pDeviceContext->RSSetState(rasterizer_states[1]);
	}
	else if(cull)
	{
		FRAMEWORK.getImmediateContext()->RSSetState(FRAMEWORK.getRasterizerState(FRAMEWORK.RS::RS_CULL_BACK));
	}
	else
	{
		pDeviceContext->RSSetState(rasterizer_states[0]);
	}

	//ワールドビュープロジェクション作成
	DirectX::XMMATRIX wrl = DirectX::XMLoadFloat4x4(&world);
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMMATRIX wvp = wrl*DirectX::XMLoadFloat4x4(&camera->getView())*DirectX::XMLoadFloat4x4(&camera->getProjection());
	DirectX::XMStoreFloat4x4(&world_view_projection, wvp);

	//定数バッファ設定
	ConstantBufferWorld cb;
	cb.world = XMLoadFloat4x4(&world);
	cb.matWVP = wvp;

	//テクスチャー
	for (auto &material : Materials)
	{
		cb.material_color.x = material.Kd.x;
		cb.material_color.y = material.Kd.y;
		cb.material_color.z = material.Kd.z;
		cb.material_color.w = 1.0f;
		FRAMEWORK.getImmediateContext()->UpdateSubresource(shader->ConstantBuffer_w.Get(), 0, NULL, &cb, 0, 0);
		FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());
		//FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());//こいつが上書きしたりする
		FRAMEWORK.getImmediateContext()->GSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());
		if (shader->hs != nullptr)
			FRAMEWORK.getImmediateContext()->HSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());
		if (shader->ds != nullptr);
		FRAMEWORK.getImmediateContext()->DSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());

		pDeviceContext->PSSetShaderResources(0, 1, &material.pTexture);
		pDeviceContext->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::POINT_BORDER));
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
	delete blend;
	shader->Deactivate();
}

void static_mesh::renderTessellation(ID3D11DeviceContext *pDeviceContext, shader *shader, const DirectX::XMFLOAT4X4&world_view_projection, const DirectX::XMFLOAT4X4 &world, const DirectX::XMFLOAT4 &light_direction,/* const DirectX::XMFLOAT4 &material_color,*/ bool wireframe)
{
	//obj
	u_int stride = sizeof(vertex);
	u_int offset = 0;

	shader->Activate();

	blender* blend = new blender(FRAMEWORK.getInstance().getDevice());

	pDeviceContext->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	pDeviceContext->IASetInputLayout(input_layout);
	pDeviceContext->OMSetDepthStencilState(depth_stencil_state, 1);
	pDeviceContext->OMSetBlendState(blend->states[blender::BS_ALPHA].Get(), nullptr, 0xFFFFFFFF);

	if (wireframe)
	{
		pDeviceContext->RSSetState(rasterizer_states[1]);
	}
	else
	{
		pDeviceContext->RSSetState(rasterizer_states[0]);
	}

	//テクスチャー
	for (auto &material : Materials)
	{
		pDeviceContext->PSSetShaderResources(0, 1, &material.pTexture);
		pDeviceContext->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::POINT_BORDER));
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
	delete blend;
	shader->Deactivate();
}

void static_mesh::render(ID3D11DeviceContext *pDeviceContext, shader *shader, Texture *pTex, const DirectX::XMFLOAT4X4&world_view_projection, const DirectX::XMFLOAT4X4 &world, const DirectX::XMFLOAT4 &light_direction, /*const DirectX::XMFLOAT4 &material_color, */bool wireframe)
{
	//obj
	u_int stride = sizeof(vertex);
	u_int offset = 0;

	shader->Activate();

	blender* blend = new blender(FRAMEWORK.getInstance().getDevice());

	pDeviceContext->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetInputLayout(input_layout);
	pDeviceContext->OMSetDepthStencilState(depth_stencil_state, 1);
	pDeviceContext->OMSetBlendState(blend->states[blender::BS_ALPHA].Get(), nullptr, 0xFFFFFFFF);
	if (wireframe)
	{
		pDeviceContext->RSSetState(rasterizer_states[1]);
	}
	else
	{
		pDeviceContext->RSSetState(rasterizer_states[0]);
	}

	pDeviceContext->PSSetShaderResources(0, 1, pTex->GetShaderResourceViewPointer());
	pDeviceContext->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::POINT_BORDER));

	//テクスチャー
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
	delete blend;
	shader->Deactivate();
}

void static_mesh::create_buffer(ID3D11Device *pDevice, vertex *vertices, u_int num_vertices, u_int *indices, u_int num_indices)
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

void static_mesh::create_buffer(ID3D11Device *pDevice, vertex_tangent *vertices, u_int num_vertices, u_int *indices, u_int num_indices)
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
HRESULT static_mesh::LoadMaterialFromFile(ID3D11Device* pDevice, const wchar_t* FileName)
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
	int check_num = 0, check_num2 = 0;

	while (fin)
	{
		fin >> command;
		if (0 == wcscmp(command, L"newmtl"))
		{
			if(check_num!=check_num2)
			{
				//fin.ignore(1024, L'\n');
				load_dummy_texture(pDevice, temp.TextureName, &temp.pTexture, &temp.texture2d_desc);
				Materials.push_back(temp);
				check_num = check_num2 = 0;
			}

			check_num++;
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
			check_num2++;
		}
	}
	fin.close();
	return S_OK;
}

int static_mesh::RayPick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal, float* outLength)
{
	int ret = -1;
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&startPosition);//XMVECTOR型に変換
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);//終点引く始点でベクトル求める
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);//長さを求める(各成分の?)
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec);//正規化
	float nearest;//一番近い距離を保持
	DirectX::XMStoreFloat(&nearest, length);//floatで長さを持つ?

	DirectX::XMVECTOR position, normal;
	for (const auto& it : faces)
	{
		//コンストラクタで取得した面の3頂点の情報を代入
		DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&it.position[0]);
		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&it.position[1]);
		DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&it.position[2]);

		//3頂点からベクトルの辺を出す
		DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b, a);
		DirectX::XMVECTOR bc = DirectX::XMVectorSubtract(c, b);
		DirectX::XMVECTOR ca = DirectX::XMVectorSubtract(a, c);

		//面をなす2辺によって外積で面法線を求められる
		//面法線 n
		DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(ab, bc));

		//法線nとRay(dirはRayの正規化)の内積が正ならばnとRayは同じ方向に向いている(面は裏向き)
		//その場合それ以上の処理は無し
		float dot;
		DirectX::XMStoreFloat(&dot, DirectX::XMVector3Dot(n, dir));
		if (dot >= 0)
			continue;

		//交点算出
		//まずは面から交点までの距離を求める
		DirectX::XMVECTOR dist;//Ray始点から面までの距離
		dist = DirectX::XMVectorDivide(DirectX::XMVector3Dot(DirectX::XMVectorSubtract(a, start), n), DirectX::XMVector3Dot(dir, n));

		float ft;
		DirectX::XMStoreFloat(&ft, dist);
		if (ft<0.0f || ft>nearest)//距離が負か既にこれより小さい奴があるなら無視
			continue;
		DirectX::XMVECTOR RayColPos;//Rayが面とぶつかった座標
		//Rayの視点座標+(Rayの単位ベクトル*Rayの大きさ)=Rayがぶつかる面の座標
		RayColPos = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(dir, dist), start);

		//内点判定
		//RayColPosが三角形のそとにあれば外積で出るベクトルが逆になり、最後の内積の値が負になる
		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(a, RayColPos);//当たった座標から頂点へのベクトル
		DirectX::XMVECTOR cs1 = DirectX::XMVector3Cross(v1, ab);//外積
		DirectX::XMVECTOR dt1 = DirectX::XMVector3Dot(cs1, n);//内積
		float fdt1;
		DirectX::XMStoreFloat(&fdt1, dt1);
		DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(b, RayColPos);
		DirectX::XMVECTOR cs2 = DirectX::XMVector3Cross(v2, bc);
		DirectX::XMVECTOR dt2 = DirectX::XMVector3Dot(cs2, n);
		float fdt2;
		DirectX::XMStoreFloat(&fdt2, dt2);
		DirectX::XMVECTOR v3 = DirectX::XMVectorSubtract(c, RayColPos);
		DirectX::XMVECTOR cs3 = DirectX::XMVector3Cross(v3, ca);
		DirectX::XMVECTOR dt3 = DirectX::XMVector3Dot(cs3, n);
		float fdt3;
		DirectX::XMStoreFloat(&fdt3, dt3);
		if (fdt1 < 0.0f || fdt2 < 0.0f || fdt3 < 0.0f)
			continue;

		//情報保存
		position = RayColPos;
		normal = n;
		nearest = ft;
		ret = it.materialIndex;
	};

	if (ret != -1)
	{
		DirectX::XMStoreFloat3(outPosition, position);
		DirectX::XMStoreFloat3(outNormal, normal);
	}
	//最も近いヒット座標までの距離
	*outLength = nearest;
	return ret;
}
