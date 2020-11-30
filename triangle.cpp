//#include <direct.h>
//#include <DirectXMath.h>
//#include <iostream>
//#include <string>
//#include <stdio.h>
#include "triangle.h"
#include "texture.h"
#include "load_file.h"
#include "framework.h"

triangle::triangle()
{
	HRESULT hr = S_OK;

	//三角形
	TRI_VERTEX v[] = {
		DirectX::XMFLOAT3(0.0f, 0.5f,0),
		DirectX::XMFLOAT3(-0.5f,-0.5f, 0.0f),
		DirectX::XMFLOAT3(0.5f,-0.5f, 0.0f), 
	};

	//	頂点バッファ作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(v);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA res;
	ZeroMemory(&res, sizeof(res));
	res.pSysMem = v;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&bd, &res, vertex_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		assert("Failed to CreateVertexBuffer");
	}

	//デプスステンシルステート
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	//Zバッファのオンオフ(TRUE : on,FALSE : off)
	depth_stencil_desc.DepthEnable = FALSE;
	//Zバッファへの書き込み(ZERO : off,ALL : on)
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	//Zテストの条件
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
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
	hr = FRAMEWORK.getDevice()->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());
	if (FAILED(hr))
	{
		assert("Failed to CreateDepthStencilState");
	}
}


void triangle::render(shader* shader, bool wire_frame, bool tess_flg ,float dx, float dy, float size, float alpha)
{
	shader->Activate();
	//頂点データ設定
	TRI_VERTEX data[3];

	data[0].position.x = dx;
	data[0].position.y = dy - size / 2.0f;
	data[0].position.z = 0.0f;

	data[1].position.x = dx + size / 2.0f;
	data[1].position.y = dy + size / 2.0f;
	data[1].position.z = 0.0f;

	data[2].position.x = dx - size / 2.0f;
	data[2].position.y = dy + size / 2.0f;
	data[2].position.z = 0.0f;

	//正規化デバイス座標系
	for (int i = 0; i < 3; i++) {
		data[i].position.x = 2.0f * data[i].position.x / FRAMEWORK.SCREEN_WIDTH - 1.0f;
		data[i].position.y = 1.0f - 2.0f * data[i].position.y / FRAMEWORK.SCREEN_HEIGHT;
		data[i].position.z = 0.0f;
	}

	//頂点データ更新
	FRAMEWORK.getImmediateContext()->UpdateSubresource(vertex_buffer.Get(), 0, NULL, data, 0, 0);

	//頂点バッファの指定
	UINT stride = sizeof(TRI_VERTEX);
	UINT offset = 0;
	FRAMEWORK.getImmediateContext()->IASetVertexBuffers(
		0, 1, vertex_buffer.GetAddressOf(), // スロット, 数, バッファ
		&stride,		// １頂点のサイズ
		&offset			// 開始位置
		);
	if (!tess_flg)
		FRAMEWORK.getImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	if(tess_flg)
		FRAMEWORK.getImmediateContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(depth_stencil_state.Get(), 1);
	if(wire_frame)
		FRAMEWORK.getImmediateContext()->RSSetState(FRAMEWORK.getRasterizerState(FRAMEWORK.RS::RS_WIRE));

	//定数バッファ
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(0, 1, &shader->ConstantBuffer);
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(0, 1, &shader->ConstantBuffer);
	FRAMEWORK.getImmediateContext()->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::POINT_BORDER));

	FRAMEWORK.getImmediateContext()->Draw(4, 0);
}

ts_triangle::ts_triangle()
{
	HRESULT hr = S_OK;

	//三角形
	TRI_VERTEX v[] = {
		DirectX::XMFLOAT3(0.0f, 0.5f,0),
		DirectX::XMFLOAT3(-0.5f,-0.5f, 0.0f),
		DirectX::XMFLOAT3(0.5f,-0.5f, 0.0f),
	};

	//	頂点バッファ作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(v);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA res;
	ZeroMemory(&res, sizeof(res));
	res.pSysMem = v;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&bd, &res, vertex_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		assert("Failed to CreateVertexBuffer");
	}

	//デプスステンシルステート
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
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
	hr = FRAMEWORK.getDevice()->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());
	if (FAILED(hr))
	{
		assert("Failed to CreateDepthStencilState");
	}

	//インプットレイアウト//頂点の構成を規定する//各セマンティクスの規定
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	vs_Load(FRAMEWORK.getInstance().getDevice(), "triangle_vs.cso", vertexShader.GetAddressOf(), input_layout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	ps_Load(FRAMEWORK.getInstance().getDevice(), "triangle_ps.cso", pixelShader.GetAddressOf());
	hs_Load(FRAMEWORK.getInstance().getDevice(), "triangle_hs.cso", hullShader.GetAddressOf());
	ds_Load(FRAMEWORK.getInstance().getDevice(), "triangle_ds.cso", domainShader.GetAddressOf());

	D3D11_BUFFER_DESC bd2;
	ZeroMemory(&bd2, sizeof(D3D11_BUFFER_DESC));
	bd2.ByteWidth = sizeof(ConstantBufferForTessellation);
	bd2.Usage = D3D11_USAGE_DEFAULT;
	bd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd2.CPUAccessFlags = 0;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&bd2, NULL, constant_buffer.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void ts_triangle::render(bool wire_frame, float dx, float dy, float size, float alpha, bool tess_flg, int tess_factor, int inside_tess_factor)
{
	//頂点データ設定
	TRI_VERTEX data[3];

	data[0].position.x = dx;
	data[0].position.y = dy - size / 2.0f;
	data[0].position.z = 0.0f;

	data[1].position.x = dx - size / 2.0f;
	data[1].position.y = dy + size / 2.0f;
	data[1].position.z = 0.0f;

	data[2].position.x = dx + size / 2.0f;
	data[2].position.y = dy + size / 2.0f;
	data[2].position.z = 0.0f;

	//正規化デバイス座標系
	for (int i = 0; i < 3; i++) {
		data[i].position.x = 2.0f * data[i].position.x / FRAMEWORK.SCREEN_WIDTH - 1.0f;
		data[i].position.y = 1.0f - 2.0f * data[i].position.y / FRAMEWORK.SCREEN_HEIGHT;
		data[i].position.z = 0.0f;
	}

	//頂点データ更新
	FRAMEWORK.getImmediateContext()->UpdateSubresource(vertex_buffer.Get(), 0, NULL, data, 0, 0);

	//頂点バッファの指定
	UINT stride = sizeof(TRI_VERTEX);
	UINT offset = 0;
	FRAMEWORK.getImmediateContext()->IASetVertexBuffers(
		0, 1, vertex_buffer.GetAddressOf(), // スロット, 数, バッファ
		&stride,		// １頂点のサイズ
		&offset			// 開始位置
		);
	if (!tess_flg)
		FRAMEWORK.getImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	if (tess_flg)
		FRAMEWORK.getImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	if (wire_frame)
		FRAMEWORK.getImmediateContext()->RSSetState(FRAMEWORK.getRasterizerState(FRAMEWORK.RS::RS_WIRE));
	FRAMEWORK.getImmediateContext()->OMSetDepthStencilState(depth_stencil_state.Get(), 1);

	FRAMEWORK.getImmediateContext()->VSSetShader(vertexShader.Get(), NULL, 0);
	FRAMEWORK.getImmediateContext()->HSSetShader(hullShader.Get(), NULL, 0);
	FRAMEWORK.getImmediateContext()->DSSetShader(domainShader.Get(), NULL, 0);
	FRAMEWORK.getImmediateContext()->PSSetShader(pixelShader.Get(), NULL, 0);

	//定数バッファ
	ConstantBufferForTessellation cb;
	cb.tessFactor = tess_factor;
	cb.insideTessFactor = inside_tess_factor;
	FRAMEWORK.getImmediateContext()->UpdateSubresource(constant_buffer.Get(), 0, NULL, &cb, 0, 0);

	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(2, 1, constant_buffer.GetAddressOf());
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(2, 1, constant_buffer.GetAddressOf());
	FRAMEWORK.getImmediateContext()->HSSetConstantBuffers(2, 1, constant_buffer.GetAddressOf());
	//FRAMEWORK.getImmediateContext()->DSSetConstantBuffers(2, 1, constant_buffer.GetAddressOf());
	FRAMEWORK.getImmediateContext()->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::POINT_BORDER));

	FRAMEWORK.getImmediateContext()->Draw(4, 0);
}