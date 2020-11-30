#pragma once

namespace vf
{
	struct static_mesh
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 tangent;
		DirectX::XMFLOAT2 texcoord;
		static_mesh(const descartes::vertex& rhs)
		{
			position = rhs.position;
			normal = rhs.normal;
			tangent = rhs.tangent;
			texcoord = rhs.texcoord;
		}
		static_mesh() = default;
		~static_mesh() = default;
		static_mesh& operator =(static_mesh&) = delete;

		template<class T>
		void serialize(T& archive)
		{
			archive
				(
					position.x, position.y, position.z,
					normal.x, normal.y, normal.z,
					tangent.x, tangent.y, tangent.z, tangent.w,
					texcoord.x, texcoord.y
					);
		}
		/*static inline*/ D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	};
	struct skinned_mesh
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 tangent;
		DirectX::XMFLOAT2 texcoord;
		float bone_weights[MAX_BONE_INFLUENCES];
		std::uint32_t bone_indices[MAX_BONE_INFLUENCES];
		skinned_mesh(const descartes::vertex& rhs)
		{
			position = rhs.position;
			normal = rhs.normal;
			tangent = rhs.tangent;
			texcoord = rhs.texcoord;
			memcpy(bone_weights, rhs.bone_weights, sizeof(bone_weights));
			memcpy(bone_indices, rhs.bone_indices, sizeof(bone_indices));
		}
		skinned_mesh() = default;
		~skinned_mesh() = default;
		skinned_mesh& operator =(skinned_mesh&) = delete;

		template<class T>
		void serialize(T& archive)
		{
			archive
				(
					position.x, position.y, position.z,
					normal.x, normal.y, normal.z,
					tangent.x, tangent.y, tangent.z, tangent.w,
					texcoord.x, texcoord.y,
					bone_weights,
					bone_indices
					);
		}
		/*static inline*/ static D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	};
	struct collision_mesh
	{
		DirectX::XMFLOAT3 position;
		collision_mesh(const descartes::vertex& rhs)
		{
			position = rhs.position;
		}
		collision_mesh() = default;
		~collision_mesh() = default;
		collision_mesh& operator =(collision_mesh&) = delete;

		template<class T>
		void serialize(T& archive)
		{
			archive
				(
					position.x, position.y, position.z
					);
		}
		/*static inline*/ D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	};
}
