#pragma once

#pragma once
//#include <DirectXMath.h>
//#include "shader.h"

#include <memory>
#include "ray.h"
#include "hit.h"
#include "ray_material.h"
#include "ray_light.h"


//--------------------------------------------------------------------
//	sphere_meshクラス
//--------------------------------------------------------------------

//class sphere_mesh
//{
//private:
//protected:
//
//	struct MESH
//	{
//		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
//		int num_vertices;	// 頂点数　
//		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
//		int num_indices;	// インデックス数　
//	};
//
//	MESH mesh;
//	//テクスチャ利用
//	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view = NULL;
//	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler = NULL;
//
//	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
//	std::unique_ptr<shader> env_shader;
//	struct ConstantBufferForPerMesh
//	{
//		DirectX::XMMATRIX world;
//		DirectX::XMMATRIX matWVP;
//	};
//
//
//	// 情報
//	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
//	DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
//	DirectX::XMFLOAT3 angle = { 0.0f, 0.0f, 0.0f };
//	DirectX::XMFLOAT4X4 world_matrix;
//
//
//public:
//	sphere_mesh(const wchar_t* filename = nullptr);
//
//	~sphere_mesh()
//	{
//	}
//
//
//	void update();
//	void render(MainCamera* m_camera);
//	void setPos(const DirectX::XMFLOAT3& pos) { this->pos = pos; }
//	void setScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
//	void setAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
//};

//--------------------------------------------------------------------
//	ray tracing用 sphereクラス
//--------------------------------------------------------------------

//class sphere
//{
//private:
//	DirectX::XMFLOAT3 center;
//	float radius;
//public:
//	sphere(DirectX::XMFLOAT3 c, float r) :center(c), radius(r) {};
//	bool intersect(ray* _ray, hit* _hit);
//
//	//getter
//	DirectX::XMFLOAT3 getCenter() { return center; }
//	float getRadius() { return radius; }
//};

class hit;

class sphere
{
private:
	DirectX::XMFLOAT3 center;
	float radius;
	std::shared_ptr<ray_material>_material;
	std::shared_ptr<ray_light>_light;
public:
	sphere(DirectX::XMFLOAT3 c, float r) :center(c), radius(r) {};
	sphere(DirectX::XMFLOAT3 c, float r,std::shared_ptr<ray_material>& mtl, std::shared_ptr<ray_light>& light) :center(c), radius(r) ,_material(mtl),_light(light){};
	bool intersect(ray* _ray, hit* _hit);


	//getter
	DirectX::XMFLOAT3 getCenter() { return center; }
	float getRadius() { return radius; }
	ray_material* getMaterial() { return _material.get(); }
	ray_light* getLight() { return _light.get(); }
};
