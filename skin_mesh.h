#pragma once
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>
#include <fbxsdk.h>
#include "shader.h"

#define MAX_BONE_INFLUENCES 4
#define MAX_BONES 128

class skin_mesh
{
public:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cs_buffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[2];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;
	//Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;

	bool space_type;

	//DirectX::XMVECTOR focusVec;
	//DirectX::XMFLOAT3 pos;
	//DirectX::XMFLOAT3 scale;
	//DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT4X4 BoneTransForms[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };

	enum SPACE_TYPE {
		Z_FRONT = 0,
		Z_UP=1,
	};

	//
	DirectX::XMFLOAT4X4 coordinate_conversion = {
		1,0,0,0,
		0,0,1,0,
		0,1,0,0,
		0,0,0,1,
	};

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;//uv
		FLOAT bone_weights[MAX_BONE_INFLUENCES] = { 1, 0, 0, 0 };
		INT bone_indices[MAX_BONE_INFLUENCES] = {};
	};

	struct cbuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
		DirectX::XMFLOAT4 light_direction;
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
	};

	struct material
	{
		DirectX::XMFLOAT4 color = { 0.0f, 0.0f, 0.8f, 1.0f };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
		D3D11_TEXTURE2D_DESC	tex2d_desc;
	};
	//material diffuse;

	struct subset
	{
		u_int index_start = 0; // start number of index buffer
		u_int index_count = 0; // number of vertices (indices)
		material diffuse;
	};
	//std::vector<subset> subsets;

	struct bone
	{
		DirectX::XMFLOAT4X4 transform;
	};

	//ボーンの集合体//ボーンはその時間(フレーム)のポーズを表す
	typedef std::vector<bone> skeletal;
	struct skeletal_animation : public std::vector<skeletal>
	{
		float sampling_time = 1 / 24.0f;//サンプリングの間隔
		float animation_tick = 0.0f;//実行中の経過時間
	};

	struct mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		std::vector<subset> subsets;
		DirectX::XMFLOAT4X4 global_transform = { 1, 0, 0, 0,/**/ 0, 1, 0, 0, /**/0, 0, 1, 0, /**/0, 0, 0, 1 };//姿勢

		void create_buffer(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices);
		//std::vector<skin_mesh::bone> skeletal;
		skeletal_animation skeletal_animation;
	};
	std::vector<mesh> meshes;

	struct Face//RayPick用の面を取るための構造体
	{
		DirectX::XMFLOAT3 position[3];
		int materialIndex;
	};
	std::vector<Face> faces;

	int RayPick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal, float* outLength);

	skin_mesh(const char *fbx_filename, const bool space_type = 0);
	~skin_mesh();
	void render(ID3D11DeviceContext*, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4&, const DirectX::XMFLOAT4&, bool, float elapsed_time);
	void render(ID3D11DeviceContext *, shader *, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4&, const DirectX::XMFLOAT4&, bool, float elapsed_time);
	HRESULT skin_mesh::LoadMaterialFromFile(ID3D11Device *pDevice, const wchar_t* filename);

	//getter
	DirectX::XMFLOAT4X4* GetBoneTransForms(){ return BoneTransForms; };
};

//ボーン行列取得関数//void fetch_bone_matrices(FbxMesh *fbx_mesh, std::vector<skin_mesh::bone> &skeletal, FbxTime time);