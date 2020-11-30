#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include "model_data.h"

class model_resource
{
public:
	model_resource(ID3D11Device* device, std::unique_ptr<ModelData> data);
	~model_resource() {}

	struct material
	{
		XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f };
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;

		int texCount = -1;
	};

	struct subset
	{
		u_int		start_index = 0;
		u_int		index_count = 0;
		material*	_material;
	};

	struct mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>	vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>	index_buffer;
		std::vector<subset>						subsets;

		int										node_index;
		std::vector<int>						node_indices;
		std::vector<XMMATRIX>			inverse_transforms;
	};

	const std::vector<mesh>& getMeshes() const { return m_meshes; }
	const std::vector<ModelData::Node>& getNodes() const { return m_data->nodes; }
	const std::unique_ptr<ModelData> &getModelData()const { return m_data; };
	std::vector<ModelData::Animation>& getAnimations() const { return m_data->animations; }

	void setMaterialTex(ID3D11Device* device, const int index, const wchar_t* filename);
	int getMaterialNum() { return (int)m_materials.size(); }

private:
	std::unique_ptr<ModelData>	m_data;
	std::vector<material>	m_materials;
	std::vector<mesh>		m_meshes;
};
