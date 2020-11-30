#include "shader_ex.h"
#include "resource_manager.h"
#include "framework.h"
#include "math.h"

void shader_ex::init()
{
	input_layout = nullptr;
	vertex_shader = nullptr;
	pixel_shader = nullptr;
}

bool shader_ex::create2D(const char* vs_filename, const char* ps_filename)
{
	//入力レイアウトオブジェクトの生成
	D3D11_INPUT_ELEMENT_DESC VertexDesc[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//頂点シェーダーオブジェクトの生成 
	if (!resource_manager::loadVertexShaders(FRAMEWORK.getDevice(), vs_filename, VertexDesc, ARRAYSIZE(VertexDesc), &vertex_shader, &input_layout))return false;

	//ピクセルシェーダーオブジェクトの生成 
	if (!resource_manager::loadPixelShaders(FRAMEWORK.getDevice(), ps_filename, &pixel_shader)) return false;

	//pSystem->GetContext()->IASetInputLayout(input_layout);

	return true;
}


bool shader_ex::create(const char* vs_filename, const char* ps_filename)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//頂点シェーダーオブジェクトの生成 
	if (!resource_manager::loadVertexShaders(FRAMEWORK.getDevice(), vs_filename, input_element_desc, ARRAYSIZE(input_element_desc), &vertex_shader, &input_layout))return false;

	//ピクセルシェーダーオブジェクトの生成 
	if (!resource_manager::loadPixelShaders(FRAMEWORK.getDevice(), ps_filename, &pixel_shader)) return false;

	//pSystem->GetContext()->IASetInputLayout(input_layout);

	return true;
}

bool shader_ex::create(D3D11_INPUT_ELEMENT_DESC* inputElementDescs, int numElement, const char* vs_filename, const char* ps_filename)
{
	//頂点シェーダーオブジェクトの生成 
	if (!resource_manager::loadVertexShaders(FRAMEWORK.getDevice(), vs_filename, inputElementDescs, numElement, &vertex_shader, &input_layout))return false;

	//ピクセルシェーダーオブジェクトの生成 
	if (!resource_manager::loadPixelShaders(FRAMEWORK.getDevice(), ps_filename, &pixel_shader)) return false;

	//pSystem->GetContext()->IASetInputLayout(input_layout);

	return true;
}

//独自パラメーター用
bool shader_ex::createConstantBufferOriginal(u_int size)
{
	// 定数バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = size;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = FRAMEWORK.getDevice()->CreateBuffer(&bd, NULL, ConstantBufferOriginal.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;
}

bool shader_ex::setGS(const char* gs_filename)
{
	//ジオメトリーシェーダーオブジェクトの生成 
	if (!resource_manager::loadGeometryShaders(FRAMEWORK.getDevice(), gs_filename, &geometry_shader)) return false;

	return true;
}

bool shader_ex::setHS(const char* hs_filename)
{
	//ハルシェーダーオブジェクトの生成 
	if (!resource_manager::loadHullShaders(FRAMEWORK.getDevice(), hs_filename, &hull_shader)) return false;

	return true;
}

bool shader_ex::setDS(const char* ds_filename)
{
	//ドメインシェーダーオブジェクトの生成 
	if (!resource_manager::loadDomainShaders(FRAMEWORK.getDevice(), ds_filename, &domain_shader)) return false;

	return true;
}

bool shader_ex::setCS(const char* cs_filename)
{
	//コンピュートシェーダーオブジェクトの生成 
	if (!resource_manager::loadComputeShaders(FRAMEWORK.getDevice(), cs_filename, &compute_shader)) return false;

	return true;
}

void shader_ex::activate()
{
	FRAMEWORK.getImmediateContext()->IASetInputLayout(input_layout);

	FRAMEWORK.getImmediateContext()->DSSetShader(domain_shader, nullptr, 0);
	FRAMEWORK.getImmediateContext()->HSSetShader(hull_shader, nullptr, 0);
	FRAMEWORK.getImmediateContext()->VSSetShader(vertex_shader, nullptr, 0);
	FRAMEWORK.getImmediateContext()->GSSetShader(geometry_shader, nullptr, 0);
	FRAMEWORK.getImmediateContext()->PSSetShader(pixel_shader, nullptr, 0);
}

void shader_ex::inactivate()
{
	//FRAMEWORK.getImmediateContext()->IASetInputLayout(nullptr);//これあるとshader_textureバグる//背景バグるのもこいつのせい?

	FRAMEWORK.getImmediateContext()->DSSetShader(nullptr, nullptr, 0);
	FRAMEWORK.getImmediateContext()->HSSetShader(nullptr, nullptr, 0);
	FRAMEWORK.getImmediateContext()->VSSetShader(nullptr, nullptr, 0);
	FRAMEWORK.getImmediateContext()->GSSetShader(nullptr, nullptr, 0);
	FRAMEWORK.getImmediateContext()->PSSetShader(nullptr, nullptr, 0);
}

void shader_ex::release()
{
	resource_manager::releaseGeometryShaders(geometry_shader);
	resource_manager::releaseHullShaders(hull_shader);
	resource_manager::releaseDomainShaders(domain_shader);
	resource_manager::releaseComputeShaders(compute_shader);
	resource_manager::releasePixelShaders(pixel_shader);
	resource_manager::releaseVertexShaders(vertex_shader, input_layout);
}

void UpdateConstantBuffer(shader_ex* shader, ConstantBufferForPBREX cb, UINT slot_num)
{
	//コンスタントバッファ更新
	FRAMEWORK.getImmediateContext()->UpdateSubresource(shader->ConstantBufferOriginal.Get(), 0, NULL, &cb, 0, 0);
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(slot_num, 1, shader->ConstantBufferOriginal.GetAddressOf());
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(slot_num, 1, shader->ConstantBufferOriginal.GetAddressOf());
}

void UpdateConstantBuffer(shader_ex* shader, ConstantBufferForDissolve cb, UINT slot_num)
{
	//コンスタントバッファ更新
	FRAMEWORK.getImmediateContext()->UpdateSubresource(shader->ConstantBufferOriginal.Get(), 0, NULL, &cb, 0, 0);
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(slot_num, 1, shader->ConstantBufferOriginal.GetAddressOf());
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(slot_num, 1, shader->ConstantBufferOriginal.GetAddressOf());
}

void UpdateConstantBuffer(shader_ex* shader, ConstantBufferForFur cb, UINT slot_num)
{
	//コンスタントバッファ更新
	FRAMEWORK.getImmediateContext()->UpdateSubresource(shader->ConstantBufferOriginal.Get(), 0, NULL, &cb, 0, 0);
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(slot_num, 1, shader->ConstantBufferOriginal.GetAddressOf());
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(slot_num, 1, shader->ConstantBufferOriginal.GetAddressOf());
	FRAMEWORK.getImmediateContext()->GSSetConstantBuffers(slot_num, 1, shader->ConstantBufferOriginal.GetAddressOf());
}