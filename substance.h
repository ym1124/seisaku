#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <shlwapi.h>
#include <directxmath.h>
using namespace DirectX;

#include <fbxsdk.h>
using namespace fbxsdk;

#include <set>
#include <vector>
#include <algorithm>

#include <iostream>
#include <fstream>
#undef max
#undef min
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>

#include <DirectXTex.h>

#include "texture_ex.h"
#include "utilities.h"

#define MAX_BONE_INFLUENCES 4
#define MAX_BONES 180
namespace descartes
{
	struct vertex
	{
		XMFLOAT3 position = {};
		XMFLOAT3 normal = {};
		XMFLOAT4 tangent = {};
		XMFLOAT3 binormal = {};
		XMFLOAT2 texcoord = {};
		XMFLOAT4 colour = { 1, 1, 1, 1 };
		float bone_weights[MAX_BONE_INFLUENCES] = { 1, 0, 0, 0 };
		std::uint32_t bone_indices[MAX_BONE_INFLUENCES] = {};
	};

	struct hierarchy_node
	{
		FbxNodeAttribute::EType attribute = FbxNodeAttribute::EType::eUnknown;
		std::string name;
		std::vector<hierarchy_node> chirdlen;

		template<class T>
		void serialize(T& archive)
		{
			archive(attribute, name, chirdlen);
		}
	};

	struct animation_take
	{
		std::string name;
		std::int32_t sampling_rate = 0;
		std::int32_t number_of_keyframes = 0; // '0' means not yet loaded

		template<class T>
		void serialize(T& archive)
		{
			archive(name, sampling_rate, number_of_keyframes);
		}
	};
	void fetch_animation_takes(FbxScene* scene, std::vector<animation_take>& animation_takes, std::int32_t sampling_rate/*0:default value*/);

	struct bind_bone
	{
		std::string name;
		XMFLOAT4X4 offset_matrix = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }; // transforms position from mesh space to bone space
		template<class T>
		void serialize(T& archive)
		{
			archive
				(
					name,
					offset_matrix._11, offset_matrix._12, offset_matrix._13, offset_matrix._14,
					offset_matrix._21, offset_matrix._22, offset_matrix._23, offset_matrix._24,
					offset_matrix._31, offset_matrix._32, offset_matrix._33, offset_matrix._34,
					offset_matrix._41, offset_matrix._42, offset_matrix._43, offset_matrix._44
					);
		}
	};
	void fetch_bind_bones(FbxMesh* fbx_mesh, std::vector<bind_bone>& bind_bones);

	struct bone
	{
		std::string name;

		XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }; // transforms position from bone space to global space
		XMFLOAT4X4 transform_to_parent = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }; // transforms position from this bone space to parent bone space

		template<class T>
		void serialize(T& archive)
		{
			archive
				(
					name,
					transform._11, transform._12, transform._13, transform._14,
					transform._21, transform._22, transform._23, transform._24,
					transform._31, transform._32, transform._33, transform._34,
					transform._41, transform._42, transform._43, transform._44,
					transform_to_parent._11, transform_to_parent._12, transform_to_parent._13, transform_to_parent._14,
					transform_to_parent._21, transform_to_parent._22, transform_to_parent._23, transform_to_parent._24,
					transform_to_parent._31, transform_to_parent._32, transform_to_parent._33, transform_to_parent._34,
					transform_to_parent._41, transform_to_parent._42, transform_to_parent._43, transform_to_parent._44
					);
		}
	};
	struct skeletal
	{
		std::vector<bone> bones;
		const bone* find(const char* bone_name) const
		{
			for (const bone& bone : bones)
			{
				if (bone.name == bone_name)
				{
					return &bone;
				}
			}
			return 0;
		}

		template<class T>
		void serialize(T& archive)
		{
			archive(bones);
		}
	};
#if 0
	struct global_transform
	{
		std::string mesh_name;
		XMFLOAT4X4 transform; // transforms position from model space to global space
		template<class T>
		void serialize(T& archive)
		{
			archive
				(
					mesh_name,
					transform._11, transform._12, transform._13, transform._14,
					transform._21, transform._22, transform._23, transform._24,
					transform._31, transform._32, transform._33, transform._34,
					transform._41, transform._42, transform._43, transform._44
					);
		}
	};
#endif
	struct animation
	{
		std::string name;
		std::vector<skeletal> skeletal_animation;
#if 0
		std::vector<global_transform> global_transforms;
#endif

		template<class T>
		void serialize(T& archive)
		{
			archive(name, skeletal_animation/*, global_transforms*/);
		}
	};
	void fetch_animations(FbxScene* scene, std::vector<animation_take>& animation_takes, std::vector<animation>& animations, std::int32_t sampling_rate /*0:use default value, less than 0:do not fetch*/);

	void blend_skeletals
		(
			const hierarchy_node* parent,
			const DirectX::XMFLOAT4X4& parent_transform,
			const std::vector<bone>* skeletals[2],
			float ratio/*0 < ratio < 1*/,
			std::vector<bone>* blended_skeletal
			);

	struct texture
	{
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	};

	enum class blend_mode
	{
		opaque,
		masked,
		translucent,
		additive,
		modulate,
		unknown,
	};
	enum class material_domain
	{
		surface,
		deferred_decals,
		light_function,
		post_process,
		user_interface,
		unknown,
	};
	enum class shading_model
	{
		unlit,
		default_lit,
		subsurface,
		preintegrated_skin,
		clear_coat,
		subsurface_profile,
		two_sided_foliage,
		hair,
		cloth,
		eye,
		single_layer_water,
		unknown,
	};
	struct material
	{
		std::string name;
		//std::string shading_model;
		shading_model shading_model = shading_model::default_lit;
		material_domain domain = material_domain::surface;
		blend_mode blend_mode = blend_mode::opaque;
		bool two_sided = false;
		bool multi_layer = false;

		struct property
		{
			std::string name;

			XMFLOAT4 colour = { 0.0f, 0.0f, 0.0f, 0.0f }; // w channel is used as shininess by only specular.
			float factor = 1.0f;

			std::string texture_keyname;
			float scale_u = 1.0;
			float scale_v = 1.0;

			bool force_srgb = false; // This parameter is used on load.
			bool generate_mips = false;
			std::uint32_t levels = 1; // Number of mip map levels
			bool alpha_channel_to_opaque_flag = true; // This parameter is used on load.

			template<class T>
			void serialize(T& archive)
			{
				archive
					(
						name,
						colour.x, colour.y, colour.z, colour.w,
						factor,
						texture_keyname,
						scale_u, scale_v, force_srgb,
						generate_mips, levels,
						alpha_channel_to_opaque_flag
						);
			}
		};
		std::vector<property> properties;

		template<class T>
		void serialize(T& archive)
		{
			archive(name, multi_layer, shading_model, domain, blend_mode, two_sided, properties);
		}
	};
	void fetch_materials(FbxNode* fbx_node, std::unordered_map<std::string, material>& materials);

	struct subset
	{
		std::string material_name; // same as material name
		std::int32_t index_start = 0;	// start number of index buffer
		std::int32_t index_count = 0;	// number of vertices (indices)

		std::int32_t draworder = 0; // the subset is drawn in ascending order(0..n).
		bool invisible = false;

		template<class T>
		void serialize(T& archive)
		{
			archive(material_name, index_start, index_count);
		}
		bool operator<(const subset& rhs)
		{
			return draworder < rhs.draworder;
		}
	};

	struct bounding_box
	{
		XMFLOAT3 min = { +FLT_MAX, +FLT_MAX, +FLT_MAX };
		XMFLOAT3 max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		template<class T>
		void serialize(T& archive)
		{
			archive(min.x, min.y, min.z, max.x, max.y, max.z);
		}
	};

	void fetch_vertices_and_indices(FbxMesh* fbx_mesh, std::vector<vertex>& vertices, std::vector<std::uint32_t>& indices, std::vector<subset>& subsets);

	template <class T>
	struct substance
	{
		struct mesh
		{
			std::string name;

			std::vector<T> vertices;
			std::vector<std::uint32_t> indices;

			Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

			std::vector<subset> subsets;
			subset* find_subset(std::string material_name)
			{
				for (subset& subset : subsets)
				{
					if (subset.material_name == material_name)
					{
						return &subset;
					}
				}
				return nullptr;
			}
			void sort_subset()
			{
				std::sort(subsets.begin(), subsets.end());
			}

			std::vector<bind_bone> bind_bones; // transforms possition from mesh space to bone spaces

			const animation* find_animation(const char* animation_name) const
			{
				for (const animation& animation : animations)
				{
					if (animation.name == animation_name)
					{
						return &animation;
					}
				}
				return nullptr;
			}

			XMFLOAT4X4 global_transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			bounding_box bounding_box; // model space

			template<class T>
			void serialize(T& archive)
			{
				archive
					(
						name, vertices, indices, subsets,
						global_transform._11, global_transform._12, global_transform._13, global_transform._14,
						global_transform._21, global_transform._22, global_transform._23, global_transform._24,
						global_transform._31, global_transform._32, global_transform._33, global_transform._34,
						global_transform._41, global_transform._42, global_transform._43, global_transform._44,
						bind_bones, bounding_box
						);
			}
		};
		std::vector<mesh> meshes;
		const mesh* find_mesh(const char* mesh_name) const
		{
			for (const mesh& mesh : meshes)
			{
				if (mesh.name == mesh_name)
				{
					return &mesh;
				}
			}
			return nullptr;
		}
		void override_subsets(const std::vector<descartes::subset>& custom_subsets)
		{
			for (descartes::substance<T>::mesh& mesh : meshes)
			{
				for (const descartes::subset& custom_subset : custom_subsets)
				{
					descartes::subset* subset = mesh.find_subset(custom_subset.material_name);
					if (subset)
					{
						subset->draworder = custom_subset.draworder;
						subset->invisible = custom_subset.invisible;
					}
				}
				mesh.sort_subset();
			}
		}

		std::unordered_map<std::string, material> materials;
		material* find_material(const char* material_name)
		{
			std::unordered_map<std::string, material>::iterator iterator = materials.find(material_name);
			return iterator == materials.end() ? nullptr : &(iterator->second);
		}

		std::unordered_map<std::string, texture> textures;
		const texture* find_texture(const char* texture_name) const
		{
			std::unordered_map<std::string, texture>::const_iterator iterator = textures.find(texture_name);
			return iterator == textures.end() ? nullptr : &(iterator->second);
		}

		std::vector<animation_take> animation_takes;
		std::vector<animation> animations;

		struct null_object
		{
			std::string name;
			XMFLOAT4X4 global_transform;
			template<class T>
			void serialize(T& archive)
			{
				archive
					(
						name,
						global_transform._11, global_transform._12, global_transform._13, global_transform._14,
						global_transform._21, global_transform._22, global_transform._23, global_transform._24,
						global_transform._31, global_transform._32, global_transform._33, global_transform._34,
						global_transform._41, global_transform._42, global_transform._43, global_transform._44
						);
			}
		};
		std::vector<null_object> null_objects;
		const null_object* find_null_object(const char* name) const
		{
			for (const null_object& null_object : null_objects)
			{
				if (null_object.name == name)
				{
					return &null_object;
				}
			}
			return nullptr;
		}

		struct light
		{
			std::string name;
			XMFLOAT4X4 global_transform;
			template<class T>
			void serialize(T& archive)
			{
				archive
					(
						name,
						global_transform._11, global_transform._12, global_transform._13, global_transform._14,
						global_transform._21, global_transform._22, global_transform._23, global_transform._24,
						global_transform._31, global_transform._32, global_transform._33, global_transform._34,
						global_transform._41, global_transform._42, global_transform._43, global_transform._44
						);
			}
		};
		std::vector<light> lights;
		const light* find_light(const char* name) const
		{
			for (const light& light : lights)
			{
				if (light.name == name)
				{
					return &light;
				}
			}
			return nullptr;
		}

		// Returns first node with 'attribute' found
		//const hierarchy_node* find_skeleton_root_node()
		//{
		//	std::function<const hierarchy_node * (const hierarchy_node*)> traverse_node =
		//		[&traverse_node](const hierarchy_node* node) -> const hierarchy_node*
		//	{
		//		const hierarchy_node* root_node = nullptr;
		//		if (node->attribute == FbxNodeAttribute::eSkeleton)
		//		{
		//			return node;
		//		}
		//		for (const hierarchy_node& child : node->chirdlen)
		//		{
		//			const hierarchy_node* node = traverse_node(&child);
		//			if (node) return node;
		//		}
		//		return nullptr;
		//	};
		//	return traverse_node(&scene_graph);
		//}
		hierarchy_node scene_graph;

		substance() = default;
		virtual ~substance() = default;
		substance(substance&) = delete;
		substance& operator=(substance&) = delete;
		substance(const char* fbx_filename, bool triangulate, std::int32_t animation_sampling_rate = 0 /*0:default value, -1:ignore animations*/, const char* extension = ".bin")
		{
			if (PathFileExistsA((std::string(fbx_filename) + extension).c_str()))
			{
				// Deserialize from 'fbx_filename + extension' file.
				std::ifstream ifs;
				ifs.open((std::string(fbx_filename) + extension).c_str(), std::ios::binary);
				cereal::BinaryInputArchive i_archive(ifs);
				i_archive(*this);
			}
			else
			{
				// Create from 'fbx_filename' file.
				create_from_fbx(fbx_filename, triangulate, animation_sampling_rate);

				// Serialize to 'fbx_filename + extension' file.
				std::ofstream ofs;
				ofs.open((std::string(fbx_filename) + extension).c_str(), std::ios::binary);
				cereal::BinaryOutputArchive o_archive(ofs);
				o_archive(*this);
			}
		}
		substance(const char* fbx_filename, bool triangulate, const std::vector<std::string>& animation_filenames, int animation_sampling_rate = 0 /*0:default value*/, const char* extension = ".bin")
		{
			if (PathFileExistsA((std::string(fbx_filename) + extension).c_str()))
			{
				// Deserialize from 'fbx_filename + extension' file.
				std::ifstream ifs;
				ifs.open((std::string(fbx_filename) + extension).c_str(), std::ios::binary);
				cereal::BinaryInputArchive i_archive(ifs);
				i_archive(*this);
			}
			else
			{
				// Create from 'fbx_filename' file.
				create_from_fbx(fbx_filename, triangulate, animation_sampling_rate);

				// Add animations from 'animation_filenames' files.
				for (std::string animation_filename : animation_filenames)
				{
					// Create the FBX SDK manager
					FbxManager* manager = FbxManager::Create();

					// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h.
					manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

					// Create an importer.
					FbxImporter* importer = FbxImporter::Create(manager, "");

					// Initialize the importer.
					bool import_status = false;
					import_status = importer->Initialize(animation_filename.c_str(), -1, manager->GetIOSettings());
					_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

					// Create a new scene so it can be populated by the imported file.
					FbxScene* scene = FbxScene::Create(manager, "");

					// Import the contents of the file into the scene.
					import_status = importer->Import(scene);
					_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

					_ASSERT_EXPR_A(animation_sampling_rate >= 0, "'animation_sampling_rate' must be 0 or more");
					fetch_animations(scene, animation_takes, animations, animation_sampling_rate);

					manager->Destroy();
				}

				// Serialize to 'fbx_filename + extension' file.
				std::ofstream ofs;
				ofs.open((std::string(fbx_filename) + extension).c_str(), std::ios::binary);
				cereal::BinaryOutputArchive o_archive(ofs);
				o_archive(*this);
			}
		}
	private:
		void create_from_fbx(const char* fbx_filename, bool triangulate, int animation_sampling_rate)
		{
			// Create the FBX SDK manager
			FbxManager* manager = FbxManager::Create();

			// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h.
			manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

			// Create an importer.
			FbxImporter* importer = FbxImporter::Create(manager, "");

			// Initialize the importer.
			bool import_status = false;
			import_status = importer->Initialize(fbx_filename, -1, manager->GetIOSettings());
			_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

			// Create a new scene so it can be populated by the imported file.
			FbxScene* scene = FbxScene::Create(manager, "");

			// Import the contents of the file into the scene.
			import_status = importer->Import(scene);
			_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

#if 0
			FbxAxisSystem our_axis_system = FbxAxisSystem::DirectX;
			FbxAxisSystem scene_axis_system = scene->GetGlobalSettings().GetAxisSystem();
			if (scene_axis_system != our_axis_system)
			{
				FbxAxisSystem::DirectX.ConvertScene(scene);
			}
#endif
			// Convert axis system (to Y-UP Z-FRONT LHS)
			FbxAxisSystem axis_sytem_reference = scene->GetGlobalSettings().GetAxisSystem();
			FbxAxisSystem::ECoordSystem coord_system = axis_sytem_reference.GetCoorSystem();
			//int front_vector_sign = 1;
			//FbxAxisSystem::EFrontVector front_vector = axis_sytem_reference.GetFrontVector(front_vector_sign);
			int up_vector_sign = 1;
			FbxAxisSystem::EUpVector up_vector = axis_sytem_reference.GetUpVector(up_vector_sign);
			_ASSERT_EXPR(up_vector == FbxAxisSystem::eYAxis || up_vector == FbxAxisSystem::eZAxis, L"Not support X-Up axis system");
			if (up_vector == FbxAxisSystem::eYAxis)
			{
				axis_system_transform = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
				if (coord_system == FbxAxisSystem::eRightHanded)
				{
					axis_system_transform._11 = -1;
				}
				else if (coord_system == FbxAxisSystem::eLeftHanded)
				{
				}
			}
			else if (up_vector == FbxAxisSystem::eZAxis)
			{
				axis_system_transform = XMFLOAT4X4(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1);
				if (coord_system == FbxAxisSystem::eRightHanded)
				{
					axis_system_transform._11 = -1;
					axis_system_transform._23 = -1;
				}
				else if (coord_system == FbxAxisSystem::eLeftHanded)
				{
					axis_system_transform._11 = -1;
				}
			}

			// Convert unit system (to meter)
			if (scene->GetGlobalSettings().GetSystemUnit() == FbxSystemUnit::cm)
			{
				system_unit_transform._11 = system_unit_transform._22 = system_unit_transform._33 = 0.01f;
			}

			// Convert mesh, NURBS and patch into triangle mesh
			if (triangulate)
			{
				fbxsdk::FbxGeometryConverter geometry_converter(manager);
				geometry_converter.Triangulate(scene, /*replace*/true);
			}

			std::vector<FbxNode*> fetched_meshes;
			std::vector<FbxNode*> fetched_lights;
			std::vector<FbxNode*> fetched_null_objects;
			// Fetch node attributes and materials under this node recursively.
			std::function<void(FbxNode*)> traverse = [&](FbxNode* node)
			{
				fetch_materials(node, materials);

				FbxNodeAttribute* fbx_node_attribute = node->GetNodeAttribute();
				if (fbx_node_attribute)
				{
					//enum EType
					//{
					//	eUnknown,
					//	eNull,
					//	eMarker,
					//	eSkeleton,
					//	eMesh,
					//	eNurbs,
					//	ePatch,
					//	eCamera,
					//	eCameraStereo,
					//	eCameraSwitcher,
					//	eLight,
					//	eOpticalReference,
					//	eOpticalMarker,
					//	eNurbsCurve,
					//	eTrimNurbsSurface,
					//	eBoundary,
					//	eNurbsSurface,
					//	eShape,
					//	eLODGroup,
					//	eSubDiv,
					//	eCachedEffect,
					//	eLine
					//};
					switch (fbx_node_attribute->GetAttributeType())
					{
					case FbxNodeAttribute::eMesh:
						fetched_meshes.push_back(node);
						break;
					case FbxNodeAttribute::eNull:
						fetched_null_objects.push_back(node);
						break;
					case FbxNodeAttribute::eLight:
						fetched_lights.push_back(node);
						break;
					default:
						break;
					}
				}
				for (int index_of_child = 0; index_of_child < node->GetChildCount(); ++index_of_child)
				{
					traverse(node->GetChild(index_of_child));
				}
			};
			traverse(scene->GetRootNode());
			for (FbxNode* node : fetched_null_objects)
			{
				null_object null_object;
				//FbxNull* fbx_null = node->GetNull();
				null_object.name = node->GetName();
				fbxamatrix_to_xmfloat4x4(node->EvaluateGlobalTransform(0), null_object.global_transform);
				null_objects.push_back(null_object);
			}
			for (FbxNode* node : fetched_lights)
			{
				light light;
				//FbxLight* fbx_light = node->GetLight();
				light.name = node->GetName();
				fbxamatrix_to_xmfloat4x4(node->EvaluateGlobalTransform(0), light.global_transform);
				lights.push_back(light);
			}
			meshes.resize(fetched_meshes.size());
			for (size_t index_of_fetched_meshe = 0; index_of_fetched_meshe < fetched_meshes.size(); ++index_of_fetched_meshe)
			{
				FbxMesh* fbx_mesh = fetched_meshes.at(index_of_fetched_meshe)->GetMesh();

				mesh& mesh = meshes.at(index_of_fetched_meshe);
				mesh.name = fbx_mesh->GetNode()->GetName();

				FbxAMatrix global_transform = fbx_mesh->GetNode()->EvaluateGlobalTransform(0);
				fbxamatrix_to_xmfloat4x4(global_transform, mesh.global_transform);

				std::vector<vertex> vertices;
				fetch_vertices_and_indices(fbx_mesh, vertices, mesh.indices, mesh.subsets);

				for (const vertex& v : vertices)
				{
					mesh.vertices.push_back(v);
					mesh.bounding_box.max.x = std::max<float>(v.position.x, mesh.bounding_box.max.x);
					mesh.bounding_box.max.y = std::max<float>(v.position.y, mesh.bounding_box.max.y);
					mesh.bounding_box.max.z = std::max<float>(v.position.z, mesh.bounding_box.max.z);
					mesh.bounding_box.min.x = std::min<float>(v.position.x, mesh.bounding_box.min.x);
					mesh.bounding_box.min.y = std::min<float>(v.position.y, mesh.bounding_box.min.y);
					mesh.bounding_box.min.z = std::min<float>(v.position.z, mesh.bounding_box.min.z);
				}

				fetch_bind_bones(fbx_mesh, mesh.bind_bones);
			}

			fetch_animations(scene, animation_takes, animations, animation_sampling_rate);

			std::function<void(FbxNode*, hierarchy_node&)> traverse_hierarchy = [&traverse_hierarchy](FbxNode* fbx_node, hierarchy_node& node)
			{
				if (fbx_node)
				{
					FbxNodeAttribute* fbx_node_attribute = fbx_node->GetNodeAttribute();

					node.attribute = fbx_node_attribute ? fbx_node_attribute->GetAttributeType() : FbxNodeAttribute::eUnknown;

					node.name = fbx_node->GetName();
#if 0
					// Remove namespace
					std::string namespace_token = ":";
					size_t pos = node.name.find(namespace_token);
					if (pos != std::string::npos)
					{
						node.name = node.name.substr(pos + namespace_token.length());
					}
#endif
					int number_of_children = fbx_node->GetChildCount();
					node.chirdlen.resize(number_of_children);
					for (int index_of_child = 0; index_of_child < number_of_children; ++index_of_child)
					{
						traverse_hierarchy(fbx_node->GetChild(static_cast<int>(index_of_child)), node.chirdlen.at(index_of_child));
					}
				}
			};
			traverse_hierarchy(scene->GetRootNode(), scene_graph);

			manager->Destroy();
		}
	public:
		void create_render_objects(ID3D11Device* device, bool clear_vertex_container = true)
		{
			for (mesh& mesh : meshes)
			{
				HRESULT hr;
				{
					D3D11_BUFFER_DESC buffer_desc = {};
					D3D11_SUBRESOURCE_DATA subresource_data = {};

					buffer_desc.ByteWidth = static_cast<UINT>(sizeof(T) * mesh.vertices.size());
					buffer_desc.Usage = D3D11_USAGE_DEFAULT;
					buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					buffer_desc.CPUAccessFlags = 0;
					buffer_desc.MiscFlags = 0;
					buffer_desc.StructureByteStride = 0;
					subresource_data.pSysMem = mesh.vertices.data();
					subresource_data.SysMemPitch = 0;
					subresource_data.SysMemSlicePitch = 0;

					hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.vertex_buffer.ReleaseAndGetAddressOf());
					_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
				}
				{
					D3D11_BUFFER_DESC buffer_desc = {};
					D3D11_SUBRESOURCE_DATA subresource_data = {};

					buffer_desc.ByteWidth = static_cast<UINT>(sizeof(std::uint32_t) * mesh.indices.size());
					buffer_desc.Usage = D3D11_USAGE_DEFAULT;
					buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
					buffer_desc.CPUAccessFlags = 0;
					buffer_desc.MiscFlags = 0;
					buffer_desc.StructureByteStride = 0;
					subresource_data.pSysMem = mesh.indices.data();
					subresource_data.SysMemPitch = 0;
					subresource_data.SysMemSlicePitch = 0;
					hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.index_buffer.ReleaseAndGetAddressOf());
					_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
				}

				if (clear_vertex_container)
				{
					mesh.vertices.clear();
					mesh.indices.clear();
				}
			}
		}
		void create_shader_resource_views(ID3D11Device* device, const char* media_directory)
		{
			for (std::unordered_map<std::string, material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator)
			{
				material& material = iterator->second;
				for (material::property& property : material.properties)
				{
					std::string keyname;
					texture texture;

					char concatenated_resource_path[256];
					strcpy_s(concatenated_resource_path, media_directory);
					strcat_s(concatenated_resource_path, property.texture_keyname.c_str());
					if (PathFileExistsA(concatenated_resource_path) && !PathIsDirectoryA(concatenated_resource_path))
					{
						load_texture_from_file(device, concatenated_resource_path, texture.shader_resource_view.ReleaseAndGetAddressOf(), property.force_srgb, property.generate_mips, property.levels, property.alpha_channel_to_opaque_flag, true/*enable_caching*/);
						keyname = property.texture_keyname;
					}
					else
					{
						std::uint32_t R = static_cast<std::uint8_t>(property.colour.x * 255);
						std::uint32_t G = static_cast<std::uint8_t>(property.colour.y * 255);
						std::uint32_t B = static_cast<std::uint8_t>(property.colour.z * 255);
						std::uint32_t A = static_cast<std::uint8_t>(property.colour.w * 255);
						std::uint32_t RGBA = R | (G << 8) | (B << 16) | (A << 24);
						keyname = make_dummy_texture(device, texture.shader_resource_view.ReleaseAndGetAddressOf(), RGBA, 16u, property.force_srgb, true/*enable_caching*/);
						property.texture_keyname = keyname;
					}
					texture2d_description(texture.shader_resource_view.Get(), texture.texture2d_desc);
					textures.insert(std::make_pair(keyname, texture));
				}
			}
		}

		XMFLOAT4X4 axis_system_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		XMFLOAT4X4 system_unit_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		void make_world_transform(XMFLOAT4X4* world_transform, const XMFLOAT4X4* global_transform, float x = 0, float y = 0, float z = 0, float sx = 1, float sy = 1, float sz = 1, float pitch = 0, float yaw = 0, float roll = 0)
		{
			XMStoreFloat4x4(world_transform,
				XMLoadFloat4x4(global_transform) *
				XMLoadFloat4x4(&system_unit_transform) *
				XMLoadFloat4x4(&axis_system_transform) *
				XMMatrixScaling(sx, sy, sz) *
				XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
				XMMatrixTranslation(x, y, z));
		}

		template<class T>
		void serialize(T& archive)
		{
			archive(meshes, animation_takes, animations, materials, null_objects, lights, scene_graph,
				axis_system_transform._11, axis_system_transform._12, axis_system_transform._13, axis_system_transform._14,
				axis_system_transform._21, axis_system_transform._22, axis_system_transform._23, axis_system_transform._24,
				axis_system_transform._31, axis_system_transform._32, axis_system_transform._33, axis_system_transform._34,
				axis_system_transform._41, axis_system_transform._42, axis_system_transform._43, axis_system_transform._44,
				system_unit_transform._11, system_unit_transform._12, system_unit_transform._13, system_unit_transform._14,
				system_unit_transform._21, system_unit_transform._22, system_unit_transform._23, system_unit_transform._24,
				system_unit_transform._31, system_unit_transform._32, system_unit_transform._33, system_unit_transform._34,
				system_unit_transform._41, system_unit_transform._42, system_unit_transform._43, system_unit_transform._44
				);
		}
	};
}

inline void fbxamatrix_to_xmfloat4x4(const FbxAMatrix& fbxamatrix, XMFLOAT4X4& xmfloat4x4)
{
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
}
