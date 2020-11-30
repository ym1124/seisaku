
#include <WICTextureLoader.h>

//#include "logger.h"
#include "model_resource.h"
#include "misc.h"
//#include "directxmath.h"

model_resource::model_resource(ID3D11Device *device, std::unique_ptr<ModelData> data)
{
	m_data = std::move(data);

	// マテリアル
	m_materials.resize(m_data->materials.size());
	for (size_t material_index = 0; material_index < m_materials.size(); ++material_index)
	{
		auto &&src = m_data->materials.at(material_index);
		auto &&dst = m_materials.at(material_index);

		dst.color = src.color;
		dst.texCount = src.texCount;

		// テクスチャ読み込み
		//if (!src.texture_filename.empty())
		//{
		//	size_t length;
		//	wchar_t filename[256];
		//	::mbstowcs_s(&length, filename, 256, src.texture_filename.c_str(), _TRUNCATE);

		//	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		//	HRESULT hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), dst.shader_resource_view.GetAddressOf());
		//	HRESULT HR = device->GetDeviceRemovedReason();
		//	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//}
	}

	// メッシュ
	m_meshes.resize(m_data->meshes.size());
	for (size_t mesh_index = 0; mesh_index < m_meshes.size(); ++mesh_index)
	{
		auto &&src = m_data->meshes.at(mesh_index);
		auto &&dst = m_meshes.at(mesh_index);


		for (unsigned int i = 0; i < m_data->meshes.size(); i++)
		{

			int count = 0;
			for (unsigned int j = 0; j < m_data->meshes[i].vertices.size() / 3; j++)
			{
				ModelData::Face face;

				for (int k = 0; k < 3; k++)
				{
					face.vertex[k] = m_data->meshes[i].vertices[(j * 3) + k].position;
				}
				m_data->faces.push_back(face);
			}


		}

		// 頂点バッファ
		{
			D3D11_BUFFER_DESC buffer_desc = {};
			D3D11_SUBRESOURCE_DATA subresource_data = {};

			buffer_desc.ByteWidth = static_cast< UINT >(sizeof(ModelData::Vertex) * src.vertices.size());
			//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			subresource_data.pSysMem = src.vertices.data();
			subresource_data.SysMemPitch = 0;
			subresource_data.SysMemSlicePitch = 0;

			HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, dst.vertex_buffer.GetAddressOf());
			//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		// インデックスバッファ
		{
			D3D11_BUFFER_DESC buffer_desc = {};
			D3D11_SUBRESOURCE_DATA subresource_data = {};

			buffer_desc.ByteWidth = sizeof(u_int) * src.indices.size();
			//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			subresource_data.pSysMem = src.indices.data();
			subresource_data.SysMemPitch = 0; //Not use for index buffers.
			subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
			HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, dst.index_buffer.GetAddressOf());
			//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		dst.node_index = src.node_index;

		// サブセット
		dst.subsets.resize(src.subsets.size());
		for (size_t subset_index = 0; subset_index < src.subsets.size(); ++subset_index)
		{
			auto &&src_subset = src.subsets.at(subset_index);
			auto &&dst_subset = dst.subsets.at(subset_index);

			dst_subset.start_index = src_subset.start_index;
			dst_subset.index_count = src_subset.index_count;
			dst_subset._material = &m_materials.at(src_subset.material_index);
		}

		// ボーン変換行列用
		dst.node_indices.resize(src.node_indices.size());
		::memcpy(dst.node_indices.data(), src.node_indices.data(), sizeof(int) * dst.node_indices.size());

		dst.inverse_transforms.resize(src.inverse_transforms.size());
		for (size_t index = 0; index < dst.inverse_transforms.size(); ++index)
		{
			dst.inverse_transforms.at(index) = XMLoadFloat4x4(&src.inverse_transforms.at(index));
		}
	}
}

void model_resource::setMaterialTex(ID3D11Device* device, const int index, const wchar_t* filename)
{
	if (index >= (int)m_materials.size()) return;


	// マテリアル
	for (size_t material_index = 0; material_index < m_materials.size(); ++material_index)
	{
		auto&& dst = m_materials.at(material_index);

		if (dst.texCount != index)continue;

		auto&& src = m_data->materials.at(material_index);

		//テクスチャ読み込み//同じモデルをロードしたらその度におなじテクスチャをクリエイトして重くなる
		if (!src.texture_filename.empty())
		{
			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			HRESULT hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), dst.shader_resource_view.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		if (!dst.shader_resource_view)
		{
			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			HRESULT hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), dst.shader_resource_view.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}
}
