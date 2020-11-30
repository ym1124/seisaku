#pragma once
#pragma once

#include <string>
#include <vector>
#include <directxmath.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#undef max

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

using namespace DirectX;

struct tMatrix
{
	float	m[4][4];

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(m[0][0], m[0][1], m[0][2], m[0][3],
			m[1][0], m[1][1], m[1][2], m[1][3],
			m[2][0], m[2][1], m[2][2], m[2][3],
			m[3][0], m[3][1], m[3][2], m[3][3]);
	}
};


struct ModelData
{
public:
	struct Node
	{
		std::string name;
		int	 parent_index;
		XMFLOAT3	scale;
		XMFLOAT4	rotate;
		XMFLOAT3	translate;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(name, parent_index,
				scale.x, scale.y, scale.z,
				rotate.x, rotate.y, rotate.z, rotate.w,
				translate.x, translate.y, translate.z);
		}
	};

	struct Vertex
	{
		XMFLOAT3	position;
		XMFLOAT3	normal;
		XMFLOAT2	texcoord;
		XMFLOAT4	bone_weight;
		XMUINT4	bone_index;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(
				position.x, position.y, position.z,
				normal.x, normal.y, normal.z,
				texcoord.x, texcoord.y,
				bone_weight.x, bone_weight.y, bone_weight.z, bone_weight.w,
				bone_index.x, bone_index.y, bone_index.z, bone_index.w
				);
		}
	};

	struct Subset
	{
		int	 material_index;
		int	 start_index;
		int	 index_count;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(material_index, start_index, index_count);
		}
	};

	struct Mesh
	{
		std::vector<Vertex>	vertices;
		std::vector<int> indices;
		std::vector<Subset>	subsets;

		int	 node_index;

		std::vector<int> node_indices;
		std::vector<XMFLOAT4X4>	inverse_transforms;
		std::vector<tMatrix>	tinvers;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(vertices, indices, subsets, node_index, node_indices, tinvers);
		}
	};

	struct Face
	{
		XMFLOAT3 vertex[3];
		int matIndex;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(vertex[0].x, vertex[0].y, vertex[0].z
				, vertex[1].x, vertex[1].y, vertex[1].z
				, vertex[2].x, vertex[2].y, vertex[2].z,
				matIndex);
		}
	};

	struct Material
	{
		XMFLOAT4	color;
		std::string			texture_filename;
		int			texCount;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(color.x, color.y, color.z, color.w,
				texture_filename, texCount);
		}
	};

	struct NodeKeyData
	{
		XMFLOAT3	scale;
		XMFLOAT4	rotate;
		XMFLOAT3	translate;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(scale.x, scale.y, scale.z,
				rotate.x, rotate.y, rotate.z, rotate.w,
				translate.x, translate.y, translate.z);
		}

	};

	struct Keyframe
	{
		float						seconds;
		std::vector<NodeKeyData>	node_keys;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(seconds, node_keys);
		}
	};
	struct Animation
	{
		float						seconds_length;
		std::vector<Keyframe>		keyframes;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(seconds_length, keyframes);
		}
	};

	std::vector<Node>		nodes;
	std::vector<Face>		faces;
	std::vector<Mesh>		meshes;
	std::vector<Material>	materials;

	std::vector<Animation>	animations;

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(nodes, faces ,meshes, materials, animations);
	}
};