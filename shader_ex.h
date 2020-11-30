#pragma once

#include <d3d11.h>
#include "math.h"
#include "shader.h"

class shader_ex
{
public:
	ID3D11VertexShader* vertex_shader;
	ID3D11GeometryShader* geometry_shader;
	ID3D11ComputeShader* compute_shader;
	ID3D11HullShader* hull_shader;
	ID3D11DomainShader* domain_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11InputLayout* input_layout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBufferOriginal;//各々が使うやつ

	shader_ex() :
		vertex_shader(nullptr),
		geometry_shader(nullptr),
		compute_shader(nullptr),
		hull_shader(nullptr),
		domain_shader(nullptr),
		pixel_shader(nullptr),
		input_layout(nullptr)
	{

	}

	~shader_ex() { release(); }

	void init();
	void release();

	bool create2D(const char* vs_filename, const char* ps_filename);
	bool create(const char* vs_filename, const char* ps_filename);
	bool create(D3D11_INPUT_ELEMENT_DESC* inputElementDescs, int numElement, const char* vs_filename, const char* ps_filename);

	bool createConstantBufferOriginal(u_int size);

	bool setGS(const char* gs_filename);
	bool setHS(const char* hs_filename);
	bool setDS(const char* ds_filename);
	bool setCS(const char* cs_filename);

	void activate();
	void inactivate();
};

void UpdateConstantBuffer(shader_ex* shader, ConstantBufferForPBREX cb, UINT slot_num = 2);
void UpdateConstantBuffer(shader_ex* shader, ConstantBufferForDissolve cb, UINT slot_num = 2);
void UpdateConstantBuffer(shader_ex* shader, ConstantBufferForFur cb, UINT slot_num = 2);

//----------------------------------------------
//
//	頂点データ構造体定義
//
//----------------------------------------------

struct Vertex
{
	DirectX::XMFLOAT3 Pos;	//位置
	DirectX::XMFLOAT3 Normal;//法線
	DirectX::XMFLOAT2 Tex;	//UV座標
	DirectX::XMFLOAT4 Color;	//頂点色
};
