#pragma once
#include "framework.h"
#include "cube_mesh.h"

cube_mesh::cube_mesh(const wchar_t* filename)
{

	//頂点を定義
	VERTEX vertices[24] =
	{
		DirectX::XMFLOAT3(0.5f,	0.5f,  -0.5f),	DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1,0),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f),	DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0,0),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(0.5f,  -0.5f, -0.5f),	DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1,1),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f),	DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0,1),DirectX::XMFLOAT4(1,1,1,1),

		DirectX::XMFLOAT3(-0.5f,	0.5f, 0.5f),	DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),  DirectX::XMFLOAT2(1,0),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(0.5f,  0.5f, 0.5f),	DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),  DirectX::XMFLOAT2(0,0),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(-0.5f,-0.5f, 0.5f),	DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),  DirectX::XMFLOAT2(1,1),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f),	DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),  DirectX::XMFLOAT2(0,1),DirectX::XMFLOAT4(1,1,1,1),

		DirectX::XMFLOAT3(0.5f,	0.5f, 0.5f),	DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1,0),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(0.5f,  0.5f, -0.5f),	DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0,0),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f),	DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1,1),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f),	DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0,1),DirectX::XMFLOAT4(1,1,1,1),

		DirectX::XMFLOAT3(-0.5f,	0.5f, -0.5f),DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(1,0),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(-0.5f,  0.5f, 0.5f),	DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(0,0),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f),DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(1,1),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f),	DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(0,1),DirectX::XMFLOAT4(1,1,1,1),

		DirectX::XMFLOAT3(0.5f,	 0.5f,  0.5f),	DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(1,0),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f),	DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(0,0),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(0.5f,  0.5f, -0.5f),	DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(1,1),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f),	DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(0,1),DirectX::XMFLOAT4(1,1,1,1),

		DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f),	DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1,0),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(-0.5f,  -0.5f, -0.5f),	DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0,0),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(0.5f,  -0.5f, 0.5f),	DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1,1),DirectX::XMFLOAT4(1,1,1,1),
		DirectX::XMFLOAT3(-0.5f,  -0.5f, 0.5f),	DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0,1),DirectX::XMFLOAT4(1,1,1,1),

	};

	//上の頂点でバーテックスバッファ作成
	mesh.num_vertices = sizeof(vertices) / sizeof(vertices[0]);
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX)*mesh.num_vertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;
		FRAMEWORK.getDevice()->CreateBuffer(&bd, &InitData,mesh.vertex_buffer.GetAddressOf());
	}


	//インデックスを定義
	unsigned int indices[36];
	for (int face = 0; face < 6; face++) {
		indices[face * 6] = face * 4;
		indices[face * 6 + 1] = face * 4 + 1;
		indices[face * 6 + 2] = face * 4 + 2;
		indices[face * 6 + 3] = face * 4 + 1;
		indices[face * 6 + 4] = face * 4 + 3;
		indices[face * 6 + 5] = face * 4 + 2;
	}


	//上の頂点でインデックスバッファ作成
	mesh.num_indices = sizeof(indices) / sizeof(indices[0]);
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX)*mesh.num_indices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = indices;
		FRAMEWORK.getDevice()->CreateBuffer(&bd, &InitData,mesh.index_buffer.GetAddressOf());
	}

	HRESULT hr = S_OK;
	//テクスチャロード
	if (!shader_resource_view) 
	{
		DirectX::ScratchImage image;
		DirectX::TexMetadata metadata;
		//DDSファイルの読み込み
		hr = LoadFromDDSFile(filename, DirectX::DDS_FLAGS_NONE, &metadata, image);//metadataのarraysizeが6じゃないと環境マップになってないぽい
		//metadataが(任意画像サイズ,任意画像サイズ,深度値(1),配列の大きさ(6),ミップレベル(1))これなら映る
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = CreateShaderResourceViewEx(FRAMEWORK.getDevice(), image.GetImages(), image.GetImageCount(), metadata,
			//shader_resource_view.GetAddressOf()からshader_resource_view.ReleaseAndGetAddressOf())に変更
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, DirectX::TEX_MISC_TEXTURECUBE, false, shader_resource_view.GetAddressOf());//SRV作成
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//サンプラーステートに必要
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; //テクスチャサンプリングで使うフィルタ
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;//0~1の範囲外にあるUテクスチャ座標の描画方法
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;//0~1の範囲外にあるVテクスチャ座標の描画方法
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;//0~1の範囲外にあるWテクスチャ座標の描画方法
	samplerDesc.MipLODBias = 0;//LoDのバイアス。LoDにこれを足した値が最終的なLoDのレベル(Level of Detail)
	samplerDesc.MaxAnisotropy = 16;//サンプリングに異方性補完を採用している場合の最大値
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;//サンプルとの比較オプション
	memcpy(samplerDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	samplerDesc.MinLOD = 0;//LoDの下限。0が最大で最も精細
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;//LoDの上限。MaxLoD>=MinLoDでなければならない。
	hr = FRAMEWORK.getDevice()->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());
	if (FAILED(hr))
	{
		assert("Failed to CreateSamplerState");
	}


	// 定数バッファ生成(行列)
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof(ConstantBufferForPerMesh);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&bd, NULL, constant_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		assert(false && "ID3D11Device::CreateBuffer() Failed.");
		return;
	}

	world_matrix = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	env_shader = std::make_unique<shader>();
	env_shader->Create(L"Shader/sky_box");

	//描画順によってバグるのを防ぐ
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		char checkFile[255] = { 0 };
		if (!strcmp(checkFile, "Shader/static_mesh_vs.cso"))//読み込みが終わっていれば
			return;


		FILE* vsfp = NULL;
		fopen_s(&vsfp, "Shader/static_mesh_vs.cso", "rb");
		fseek(vsfp, 0, SEEK_END);
		long cso_sz = ftell(vsfp);
		fseek(vsfp, 0, SEEK_SET);
		unsigned char *cso_data = new unsigned char[cso_sz];
		fread(cso_data, cso_sz, 1, vsfp);
		fclose(vsfp);
		if (vsfp == NULL)
			assert(!"ロード失敗");

		strcpy_s(checkFile, "Shader/static_mesh_vs.cso");

		//インプットレイアウト//VSに送る頂点構造体の構成情報を規定するのに必要
		HRESULT hr = FRAMEWORK.getDevice()->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), cso_data, cso_sz, input_layout.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), "Failed to CreateInputLayout");

		delete cso_data;
	}
}

void cube_mesh::update()
{
	//ワールド変換行列作成
	//XMMATRIX mW = XMMatrixIdentity();
	//拡大
	DirectX::XMMATRIX s = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転
	DirectX::XMMATRIX r = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//移動
	DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	// 行列の合成
	DirectX::XMMATRIX mW = s*r*t;
	DirectX::XMStoreFloat4x4(&world_matrix, mW);

}

void cube_mesh::render(main_camera *m_camera)
{
	env_shader->Activate();

	ConstantBufferForPerMesh cb;
	cb.world = DirectX::XMLoadFloat4x4(&world_matrix);
	cb.matWVP = cb.world*DirectX::XMLoadFloat4x4(&m_camera->getView())*DirectX::XMLoadFloat4x4(&m_camera->getProjection());
	FRAMEWORK.getImmediateContext()->UpdateSubresource(constant_buffer.Get(), 0, NULL, &cb, 0, 0);
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	//変更　プリミティブ・トポロジーをセット
	FRAMEWORK.getImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//バーテックスバッファーをセット
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	FRAMEWORK.getImmediateContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
	//インデックスバッファ
	FRAMEWORK.getImmediateContext()->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	//これないと背景バグることがある
	{
		FRAMEWORK.getImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		FRAMEWORK.getImmediateContext()->IASetInputLayout(input_layout.Get());
	}

	//テクスチャ設定
	if (shader_resource_view)
	{
		FRAMEWORK.getImmediateContext()->PSSetShaderResources(6, 1, shader_resource_view.GetAddressOf());//load関数なければrtvの内容が反映される
		FRAMEWORK.getImmediateContext()->PSSetSamplers(0, 1, sampler.GetAddressOf());
	}

	//レンダリング(インデックス付き)
	FRAMEWORK.getImmediateContext()->DrawIndexed(mesh.num_indices, 0, 0);

	env_shader->Deactivate();
}

void cube_mesh::render(main_camera *m_camera,shader *shader)
{
	shader->Activate();

	ConstantBufferForPerMesh cb;
	cb.world = DirectX::XMLoadFloat4x4(&world_matrix);
	cb.matWVP = cb.world*DirectX::XMLoadFloat4x4(&m_camera->getView())*DirectX::XMLoadFloat4x4(&m_camera->getProjection());
	FRAMEWORK.getImmediateContext()->UpdateSubresource(constant_buffer.Get(), 0, NULL, &cb, 0, 0);
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	//変更　プリミティブ・トポロジーをセット
	FRAMEWORK.getImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//バーテックスバッファーをセット
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	FRAMEWORK.getImmediateContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
	//インデックスバッファ
	FRAMEWORK.getImmediateContext()->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	//テクスチャ設定
	if (shader_resource_view)
	{
		FRAMEWORK.getImmediateContext()->PSSetShaderResources(6, 1, shader_resource_view.GetAddressOf());//load関数なければrtvの内容が反映される
		FRAMEWORK.getImmediateContext()->PSSetSamplers(0, 1, sampler.GetAddressOf());
	}

	//レンダリング(インデックス付き)
	FRAMEWORK.getImmediateContext()->DrawIndexed(mesh.num_indices, 0, 0);
}