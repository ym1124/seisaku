#pragma once
#include <DirectXMath.h>
#include "shader.h"

//--------------------------------------------------------------------
//	cube_meshクラス
//--------------------------------------------------------------------
class cube_mesh
{
private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
protected:

	struct MESH
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		int num_vertices;	// 頂点数　
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		int num_indices;	// インデックス数　
	};

	MESH mesh;
	//テクスチャ利用
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view = NULL;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler = NULL;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	std::unique_ptr<shader> env_shader;
	struct ConstantBufferForPerMesh
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX matWVP;
	};


	// 情報
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 angle = { 0.0f, 0.0f, 90.0f };
	DirectX::XMFLOAT4X4 world_matrix;


public:
	cube_mesh(const wchar_t* filename = nullptr);
	~cube_mesh(){}

	void update();
	void render(main_camera* m_camera);
	void render(main_camera* m_camera,shader *shader);

	//getter
	ID3D11ShaderResourceView* getShaderResourceView() { return shader_resource_view.Get(); }
	ID3D11ShaderResourceView** getShaderResourceViewAddress() { return shader_resource_view.GetAddressOf(); }
	//setter
	void setPos(const DirectX::XMFLOAT3& pos) { this->pos = pos; }
	void setScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	void setAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
	void setShaderResourceView(ID3D11ShaderResourceView *srv) { shader_resource_view = srv; }
	//void setShader(shader* shader) { env_shader.reset(shader); };
};
