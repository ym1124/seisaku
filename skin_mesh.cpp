#pragma once

#include "skin_mesh.h"
#include "load_file.h"
#include "misc.h"
#include "load_file.h"
//#include <fbxsdk.h>
//#include <functional>
#include "shader.h"
#include "framework.h"

//�p���s��쐬
void fbxamatrix_to_xmfloat4x4(const FbxAMatrix &fbxamatrix, DirectX::XMFLOAT4X4 &xmfloat4x4)
{
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
}

//�{�[���\����
struct bone_influence
{
	int index; //�e�������{�[��
	float weight; // �d��
};
typedef std::vector<bone_influence> bone_influences_per_control_point;

void fetch_bone_influences(const FbxMesh *fbx_mesh, std::vector<bone_influences_per_control_point> &influences)
{
	const int number_of_control_points = fbx_mesh->GetControlPointsCount();
	influences.resize(number_of_control_points);
	const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer) {
		FbxSkin *skin = static_cast<FbxSkin *>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));
		const int number_of_clusters = skin->GetClusterCount();
		for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster) {
			FbxCluster* cluster = skin->GetCluster(index_of_cluster);
			const int number_of_control_point_indices = cluster->GetControlPointIndicesCount();
			const int *array_of_control_point_indices = cluster->GetControlPointIndices();
			const double *array_of_control_point_weights = cluster->GetControlPointWeights();
			for (int i = 0; i < number_of_control_point_indices; ++i) {
				bone_influences_per_control_point &influences_per_control_point= influences.at(array_of_control_point_indices[i]);
				bone_influence influence;
				influence.index = index_of_cluster;
				influence.weight = static_cast<float>(array_of_control_point_weights[i]);
				influences_per_control_point.push_back(influence);
			}
		}
	}
}

void fetch_bone_matrices(FbxMesh *fbx_mesh, std::vector<skin_mesh::bone> &skeletal, FbxTime time)
{
	const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer)
	{
		FbxSkin *skin = static_cast<FbxSkin *>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));

		FbxSkin::EType skinning_type = skin->GetSkinningType();
		_ASSERT_EXPR(skinning_type == FbxSkin::eLinear || skinning_type == FbxSkin::eRigid, L"only support FbxSkin::eLinear or FbxSkin::eRigid");

		FbxCluster::ELinkMode link_mode = skin->GetCluster(0)->GetLinkMode();
		_ASSERT_EXPR(link_mode != FbxCluster::eAdditive, L"only support FbxCluster::eNormalize or FbxCluster::eTotalOne");

		const int number_of_clusters = skin->GetClusterCount();
		skeletal.resize(number_of_clusters);
		for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster)
		{
			skin_mesh::bone &bone = skeletal.at(index_of_cluster);

			FbxCluster *cluster = skin->GetCluster(index_of_cluster);

			// this matrix trnasforms coordinates of the initial pose from mesh space to global space
			FbxAMatrix reference_global_init_position;
			cluster->GetTransformMatrix(reference_global_init_position);

			//const FbxVector4 T = fbx_mesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot);
			//const FbxVector4 R = fbx_mesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot);
			//const FbxVector4 S = fbx_mesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot);
			//FbxAMatrix reference_geometry(T, R, S);
			//reference_global_init_position *= reference_geometry;

			// this matrix trnasforms coordinates of the initial pose from bone space to global space
			FbxAMatrix cluster_global_init_position;
			cluster->GetTransformLinkMatrix(cluster_global_init_position);

			// this matrix trnasforms coordinates of the current pose from bone space to global space
			FbxAMatrix cluster_global_current_position;
			cluster_global_current_position = cluster->GetLink()->EvaluateGlobalTransform(time);

			// this matrix trnasforms coordinates of the current pose from mesh space to global space
			FbxAMatrix reference_global_current_position;
			reference_global_current_position = fbx_mesh->GetNode()->EvaluateGlobalTransform(time);

			// Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation (translation, rotation and scale), the last row of the matrix represents the translation part of the transformation.
			FbxAMatrix transform = reference_global_current_position.Inverse() * cluster_global_current_position * cluster_global_init_position.Inverse() * reference_global_init_position;

			fbxamatrix_to_xmfloat4x4(transform, bone.transform);
		}
	}
}

//�A�j���[�V�����擾�֐�
void fetch_animations(FbxMesh *fbx_mesh, skin_mesh::skeletal_animation &skeletal_animation,
	u_int sampling_rate = 0)
{
	// Get the list of all the animation stack.
	FbxArray<FbxString *> array_of_animation_stack_names;
	fbx_mesh->GetScene()->FillAnimStackNameArray(array_of_animation_stack_names);
	// Get the number of animations.
	int number_of_animations = array_of_animation_stack_names.Size();
	if (number_of_animations > 0)
	{
		// Get the FbxTime per animation's frame.
		FbxTime::EMode time_mode = fbx_mesh->GetScene()->GetGlobalSettings().GetTimeMode();
		FbxTime frame_time;
		frame_time.SetTime(0, 0, 0, 1, 0, time_mode);
		//sampling_rate:��b�Ԃŉ��x�A�j���[�V�������T���v�����O���邩
		sampling_rate = sampling_rate > 0 ? sampling_rate : frame_time.GetFrameRate(time_mode);
		float sampling_time = 1.0f / sampling_rate;
		skeletal_animation.sampling_time = sampling_time;
		skeletal_animation.animation_tick = 0.0f;
		FbxString *animation_stack_name = array_of_animation_stack_names.GetAt(0);
		FbxAnimStack * current_animation_stack
			= fbx_mesh->GetScene()->FindMember<FbxAnimStack>(animation_stack_name->Buffer());
		fbx_mesh->GetScene()->SetCurrentAnimationStack(current_animation_stack);
		FbxTakeInfo *take_info = fbx_mesh->GetScene()->GetTakeInfo(animation_stack_name->Buffer());
		FbxTime start_time = take_info->mLocalTimeSpan.GetStart();
		FbxTime end_time = take_info->mLocalTimeSpan.GetStop();
		FbxTime sampling_step;
		sampling_step.SetTime(0, 0, 1, 0, 0, time_mode);
		sampling_step = static_cast<FbxLongLong>(sampling_step.Get() * sampling_time);
		for (FbxTime current_time = start_time; current_time < end_time; current_time += sampling_step)
		{
			skin_mesh::skeletal skeletal;
			fetch_bone_matrices(fbx_mesh, skeletal, current_time);
			skeletal_animation.push_back(skeletal);
		}
	}
	for (int i = 0; i < number_of_animations; i++)
	{
		delete array_of_animation_stack_names[i];
	}
}

skin_mesh::skin_mesh(const char *fbx_filename, const bool space_type)
{
	HRESULT hr = S_OK;

	//FBX Manager�쐬//FBX�ǂݍ��݊֌W�̃}�l�[�W���[?
	FbxManager* manager = FbxManager::Create();

	//
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

	//�C���|�[�^�[�쐬//�C���|�[�g���邽�߂ɕK�v
	FbxImporter* importer = FbxImporter::Create(manager, "");

	//�C���|�[�^�[������
	bool import_status = false;
	import_status = importer->Initialize(fbx_filename, -1, manager->GetIOSettings());
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	//�V�[���쐬
	FbxScene* scene = FbxScene::Create(manager, "");

	//�V�[���Ƀt�@�C���̒��g���C���|�[�g����
	import_status = importer->Import(scene);
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	// Convert mesh, NURBS and patch into triangle mesh
	fbxsdk::FbxGeometryConverter geometry_converter(manager);
	geometry_converter.Triangulate(scene, /*replace*/true);

	//fech:�ǂݍ���//attribute:����
	//attribute��eMesh(���b�V��)�����ł���m�[�h��push_back���Ĕz����쐬�B���_��񂾂��������?
	std::vector<FbxNode*> fetched_meshes;
	std::function<void(FbxNode*)> traverse = [&](FbxNode* node)//�����_��
	{
		if (node)
		{
			//�m�[�h�̑������擾
			FbxNodeAttribute *fbx_node_attribute = node->GetNodeAttribute();
			if (fbx_node_attribute)
			{
				switch (fbx_node_attribute->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
					fetched_meshes.push_back(node);
					break;
				}
			}
			//���[�g�m�[�h���q�m�[�h�̏�
			//�q�m�[�h�̐�����������
			for (int i = 0; i < node->GetChildCount(); i++)
			{
				//�q�m�[�h�擾?
				traverse(node->GetChild(i));
			}
		}
	};
	traverse(scene->GetRootNode());

	//vector::at(x):x�Ԗڂ̗v�f�ɃA�N�Z�X����//���_�z��̃|�C���^�ݒ�?
	//FbxMesh *fbx_mesh = fetched_meshes.at(0)->GetMesh();

	// �}�e���A�����擾
	//const int number_of_materials = fbx_mesh->GetNode()->GetMaterialCount();

	//���b�V���p��
	meshes.resize(fetched_meshes.size());
	//���b�V���̐��������[�v
	for (size_t i = 0; i < fetched_meshes.size(); i++)
	{
		//push_back�p�̃��b�V��
		FbxMesh *fbx_mesh = fetched_meshes.at(i)->GetMesh();
		mesh &mesh = meshes.at(i);

		//�p��
		FbxAMatrix global_transform = fbx_mesh->GetNode()->EvaluateGlobalTransform(0);
		fbxamatrix_to_xmfloat4x4(global_transform, mesh.global_transform);

		//�m�[�h�̃}�e���A���̐����擾
		const int number_of_materials = fbx_mesh->GetNode()->GetMaterialCount();
		//resize:�v�f����ύX����//�z��g��
		mesh.subsets.resize(number_of_materials > 0 ? number_of_materials : 1);

		//�}�e���A���̐�(���)�������[�v//�S�Ẵ}�e���A�����𓾂�subset���ɕ�����
		for (int index_of_material = 0; index_of_material < number_of_materials; ++index_of_material)
		{
			subset &subset = mesh.subsets.at(index_of_material);

			const FbxSurfaceMaterial *surface_material = fbx_mesh->GetNode()->GetMaterial(index_of_material);//�}�e���A�������i�[
			const FbxProperty property = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			const FbxProperty factor = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
			//�F���̐ݒ�
			if (property.IsValid() && factor.IsValid())
			{
				FbxDouble3 color = property.Get<FbxDouble3>();
				double f = factor.Get<FbxDouble>();
				subset.diffuse.color.x = static_cast<float>(color[0] * f);
				subset.diffuse.color.y = static_cast<float>(color[1] * f);
				subset.diffuse.color.z = static_cast<float>(color[2] * f);
				subset.diffuse.color.w = 1.0f;
			}
			if (property.IsValid())
			{
				const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();
				if (number_of_textures)
				{
					const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
					if (file_texture)
					{
						const char *filename = file_texture->GetRelativeFileName();

						wchar_t fbxUnicode[256];
						MultiByteToWideChar(CP_ACP, 0, fbx_filename, static_cast<int>(strlen(fbx_filename) + 1), fbxUnicode, 1024);
						wchar_t textureUnicode[256];
						MultiByteToWideChar(CP_ACP, 0, file_texture->GetFileName(), static_cast<int>(strlen(file_texture->GetFileName()) + 1), textureUnicode, 1024);
						combine_resource_path(textureUnicode, fbxUnicode, textureUnicode);

						load_texture_from_file(FRAMEWORK.getInstance().getDevice(), textureUnicode, &subset.diffuse.shader_resource_view, &subset.diffuse.tex2d_desc);

						//wchar_t filename_fbx[256] = { L"FBX/akaoni/run_ichi_sonomama.fbm/test00.png" };

						//hr = load_texture_from_file(device, filename_fbx, subset.diffuse.shader_resource_view.GetAddressOf(), &subset.diffuse.tex2d_desc);//������1�x�����ʂ�Ȃ�//shader_resource_view���ݒ肳���̂�1�ڂ�subset�̕�����
						//if (FAILED(hr))
						//{
						//	assert("Failed to CreateTextureFromFile");
						//}

						//D3D11_TEXTURE2D_DESC texture2d_desc;
						//load_texture_from_file(device, filename_fbx, subset.diffuse.shader_resource_view.GetAddressOf(), &texture2d_desc);
						// Create "diffuse.shader_resource_view" from "filename".
					}
				}
			}
		}

		// �e�}�e���A�����|���S���ŕ�����(?)
		if (number_of_materials > 0)
		{
			// �e�}�e���A�����Ƃ̃|���S�����𐔂���
			const int number_of_polygons = fbx_mesh->GetPolygonCount();
			//mesh��subset(material���ɕ����ĕ`�悷��)��index_count(���_��)�𐔂���
			u_int material_index;
			for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; ++index_of_polygon)
			{
				material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
				mesh.subsets.at(material_index).index_count += 3;
			}

			// �ǂ�����J�E���g���J�n���邩(�I�t�Z�b�g)
			int offset = 0;
			for (subset &subset : mesh.subsets)
			{
				subset.index_start = offset;//�esubset�̃X�^�[�g�n�_
				offset += subset.index_count;
				subset.index_count = 0;
			}
		}

		//���b�V�����_�f�[�^
		std::vector<vertex> vertices; // Vertex buffer 
		std::vector<u_int> indices;  // Index buffer 
		u_int vertex_count = 0;

		//UV�擾����
		FbxStringList uv_names;//���O���i�[���郊�X�g?
		fbx_mesh->GetUVSetNames(uv_names);//�SUV�̖��O���擾?

		//���_���W���o�b�t�@�Ɋi�[����@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@//PolygonVertex:���_�C���f�b�N�X
		const FbxVector4 *array_of_control_points = fbx_mesh->GetControlPoints();//���_�����Ƃ肠�������
		const int number_of_polygons = fbx_mesh->GetPolygonCount();//(���_3��)�|���S���̑���
		indices.resize(number_of_polygons * 3);

		//�{�[���e���x��FBX�f�[�^����擾����
		std::vector<bone_influences_per_control_point> bone_influences;
		fetch_bone_influences(fbx_mesh, bone_influences);

		//�ʏ��i�[�p
		Face f;

		for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++)//�|���S���̑����܂Ń��[�v����
		{
			int index_of_material = 0;
			if (number_of_materials > 0)
			{
				index_of_material = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
			}

			f.materialIndex = index_of_material;

			subset &subset = mesh.subsets.at(index_of_material);
			const int index_offset = subset.index_start + subset.index_count;

			for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++) {//�e�|���S���̒��_3�������[�v����//GetPolygonVertex(a,b):a:�|���S���̔ԍ�,b:���̃|���S���̉��ڂ̒��_��,�߂�l:�S�̂ł̃C���f�b�N�X�ԍ�
				const int index_of_control_point = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);//���ݒT������[index_of_polygon]�ڂ̃|���S����[index_of_vertex]�߂̒��_�́y�S�̂ł̔ԍ��z(�C���f�b�N�X)���擾
				//�C���f�b�N�X�ԍ��ʂ�ɒ��_�̏�񂪊i�[�����
				vertex vertex;
				vertex.position.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);//�e�������Ƃɂ��
				vertex.position.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
				vertex.position.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);

				f.position[index_of_vertex] = vertex.position;//�ʂ��Ƃ̂R���_���i�[

				FbxVector4 normal;
				fbx_mesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
				vertex.normal.x = static_cast<float>(normal[0]);
				vertex.normal.y = static_cast<float>(normal[1]);
				vertex.normal.z = static_cast<float>(normal[2]);

				if (fbx_mesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmapped_uv;
					//�w�肵�����_��uv���W���s�O�ɐ錾����uv�Ɋi�[����                      //��0�ԖڂɊi�[����Ă閼�O��uv��uv���W???
					fbx_mesh->GetPolygonVertexUV(index_of_polygon, index_of_vertex, uv_names[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}

				//�{�[���̃C���f�b�N�X�Əd�����i�[
				bone_influences_per_control_point influences_per_control_point = bone_influences.at(index_of_control_point);//���݂̒��_���e�������{�[���擾
				for (size_t index_of_influence = 0; index_of_influence < influences_per_control_point.size(); index_of_influence++)//�e�������{�[���̐��������[�v
				{
					if (index_of_influence < MAX_BONE_INFLUENCES)//�ő�4�{�[���܂ł����e�����󂯂Ȃ�
					{
						vertex.bone_weights[index_of_influence] = influences_per_control_point.at(index_of_influence).weight;
						vertex.bone_indices[index_of_influence] = influences_per_control_point.at(index_of_influence).index;
					}
					//else
					//{
					//	for (int i = 0; i < MAX_BONE_INFLUENCES; i++)
					//	{
					//		if (vertex.bone_weights[i] < influences_per_control_point.at(index_of_influence).weight)
					//		{
					//			//vertex.bone_weights[i] = influences_per_control_point.at(index_of_influence).weight;
					//			vertex.bone_weights[i] += influences_per_control_point.at(index_of_influence).weight;
					//			vertex.bone_indices[i] = influences_per_control_point.at(index_of_influence).index;
					//			break;
					//		}
					//	}
					//}
				}

				vertices.push_back(vertex);//�ݒ肵��vertex��verticies��push_back�Ŋi�[
				//indices.push_back(vertex_count);//���ݒ肵�����_�̃C���f�b�N�X�ԍ����i�[
				indices.at(index_offset + index_of_vertex) = static_cast<u_int>(vertex_count);
				vertex_count += 1;//���̒��_�p�ɃJ�E���g�𑝂₵�ăC���f�b�N�X�ԍ��ɂ���
			}
			subset.index_count += 3;

			faces.push_back(f);//�ʏ��
		}
		//�{�[���s��擾
		//FbxTime::EMode time_mode = fbx_mesh->GetScene()->GetGlobalSettings().GetTimeMode();
		//FbxTime frame_time;
		//frame_time.SetTime(0, 0, 0, 1, 0, time_mode);
		//fetch_bone_matrices(fbx_mesh, mesh.skeletal, frame_time * 20); // pose at frame 20

		//�{�[���A�j���[�V�����̍s��擾
		fetch_animations(fbx_mesh, mesh.skeletal_animation);

		mesh.create_buffer(FRAMEWORK.getInstance().getDevice(), vertices.data(), vertices.size(), indices.data(), indices.size());

		//�e�T�u�Z�b�g����shader_resource_view��ݒ肷��//�����_�ł�1�ڂ����ݒ肳��Ă��Ȃ�//�^���I�ȃe�N�X�`��������ĉf��悤�ɂ�������
		for (subset &subset : mesh.subsets)
		{
			if (!subset.diffuse.shader_resource_view)
			{
				make_dummy_texture(FRAMEWORK.getInstance().getDevice(), &subset.diffuse.shader_resource_view);
			}
		}
	}
	manager->Destroy();
	//���_��񔲏o�I���

	// Initialize Direct3D COM objects 

	//���̓��C�A�E�g�I�u�W�F�N�g�̐��� 
	D3D11_INPUT_ELEMENT_DESC VertexDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//���_�V�F�[�_�[�I�u�W�F�N�g�̐��� 
	vs_Load(FRAMEWORK.getInstance().getDevice(), "Shader/skin_mesh_vs.cso", vs.GetAddressOf(), input_layout.GetAddressOf(), VertexDesc, ARRAYSIZE(VertexDesc));

	//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐��� 
	ps_Load(FRAMEWORK.getInstance().getDevice(), "Shader/skin_mesh_ps.cso", ps.GetAddressOf());

	// create rasterizer state : solid mode
	{
		D3D11_RASTERIZER_DESC rasterizer_desc = {};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		if (space_type == Z_FRONT)
			rasterizer_desc.FrontCounterClockwise = FALSE;
		if (space_type == Z_UP)
			rasterizer_desc.FrontCounterClockwise = TRUE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = FRAMEWORK.getInstance().getDevice()->CreateRasterizerState(&rasterizer_desc, rasterizer_states[0].GetAddressOf());
		if (FAILED(hr)) return;
	}
	// create rasterizer state : wireframe mode
	{
		D3D11_RASTERIZER_DESC rasterizer_desc = {};
		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		if (space_type == Z_FRONT)
			rasterizer_desc.FrontCounterClockwise = FALSE;
		if (space_type == Z_UP)
			rasterizer_desc.FrontCounterClockwise = TRUE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = FRAMEWORK.getInstance().getDevice()->CreateRasterizerState(&rasterizer_desc, rasterizer_states[1].GetAddressOf());
		if (FAILED(hr)) return;
	}

	//D3D11_SAMPLER_DESC smpDesc = {};
	//smpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //D3D11_FILTER_ANISOTROPIC
	//smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//smpDesc.MipLODBias = 0;
	//smpDesc.MaxAnisotropy = 16;
	//smpDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//memcpy(smpDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	//smpDesc.MinLOD = 0;
	//smpDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//hr = FRAMEWORK.getInstance().getDevice()->CreateSamplerState(&smpDesc, sampler_state.GetAddressOf());
	//if (FAILED(hr)) return;

	// create depth stencil state
	{
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.StencilEnable = FALSE;
		depth_stencil_desc.StencilReadMask = 0xFF;
		depth_stencil_desc.StencilWriteMask = 0xFF;
		depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = FRAMEWORK.getInstance().getDevice()->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());
		if (FAILED(hr)) return;
	}
	//�萔�o�b�t�@�쐬
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(cbuffer);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = FRAMEWORK.getInstance().getDevice()->CreateBuffer(&buffer_desc, nullptr, cs_buffer.GetAddressOf());
		if (FAILED(hr)) return;
	}
}

skin_mesh::~skin_mesh()
{
}

//���_�o�b�t�@�쐬
void skin_mesh::mesh::create_buffer(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices)
{
	HRESULT hr;
	//���_�o�b�t�@
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(vertex)*num_vertices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.

		hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());
		if (FAILED(hr)) return;
	}
	//�C���f�b�N�X�o�b�t�@
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(u_int)*num_indices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = indices;
		subresource_data.SysMemPitch = 0; //Not use for index buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.GetAddressOf());
		if (FAILED(hr)) return;
	}
}

void skin_mesh::render(ID3D11DeviceContext *device_context,				//�f�o�C�X�R���e�L�X�g 
	const DirectX::XMFLOAT4X4 & world_view_projection,	//���[���h�E�r���[�E�v���W�F�N�V���������s�� 
	const DirectX::XMFLOAT4X4 & world,					//���[���h�ϊ��s�� 
	const DirectX::XMFLOAT4 & light_direction,			//���C�g�i�s���� 
	const DirectX::XMFLOAT4 & material_color,			//�ގ��F 
	bool mode,								//���E�h��Ԃ��`��t���O 
	float elapsed_time //�o�ߎ���
	)
{
	for (mesh &mesh : meshes)
	{
		// ���_�o�b�t�@�̃o�C���h
		UINT stride = sizeof(vertex);
		UINT offset = 0;
		device_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);

		//	�C���f�b�N�X�o�b�t�@�̃o�C���h
		device_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//	�萔�o�b�t�@�̍쐬
		cbuffer cb;
		if (space_type == Z_UP)
		{
			DirectX::XMStoreFloat4x4(&cb.world_view_projection, DirectX::XMLoadFloat4x4(&mesh.global_transform) *DirectX::XMLoadFloat4x4(&coordinate_conversion)*DirectX::XMLoadFloat4x4(&world_view_projection));
			DirectX::XMStoreFloat4x4(&cb.world, DirectX::XMLoadFloat4x4(&mesh.global_transform)*DirectX::XMLoadFloat4x4(&coordinate_conversion) *DirectX::XMLoadFloat4x4(&world));
		}
		else if (space_type == Z_FRONT)
		{
			DirectX::XMStoreFloat4x4(&cb.world_view_projection, DirectX::XMLoadFloat4x4(&mesh.global_transform)*DirectX::XMLoadFloat4x4(&world_view_projection));
			DirectX::XMStoreFloat4x4(&cb.world, DirectX::XMLoadFloat4x4(&mesh.global_transform)*DirectX::XMLoadFloat4x4(&world));
		}
		else
		{
			DirectX::XMStoreFloat4x4(&cb.world_view_projection,/* DirectX::XMLoadFloat4x4(&mesh.global_transform) *DirectX::XMLoadFloat4x4(&coordinate_conversion)**/DirectX::XMLoadFloat4x4(&world_view_projection));
			DirectX::XMStoreFloat4x4(&cb.world, /*DirectX::XMLoadFloat4x4(&mesh.global_transform)*DirectX::XMLoadFloat4x4(&coordinate_conversion) **/DirectX::XMLoadFloat4x4(&world));
		}

		//cb.world_view_projection = world_view_projection;
		//cb.world = world;
		cb.light_direction = light_direction;
		//cb.material_color = subsets.at(0).diffuse.color;
		//cb.material_color = material_color;
		//device_context->UpdateSubresource(cs_buffer.Get(), 0, nullptr, &cb, 0, 0);
		//device_context->VSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());
		//device_context->PSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());

		//	�v���~�e�B�u���[�h�̐ݒ�
		device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//	���̓��C�A�E�g�̃o�C���h
		device_context->IASetInputLayout(input_layout.Get());

		//	���X�^���C�U�[�̐ݒ�
		if (!mode)	device_context->RSSetState(rasterizer_states[0].Get());
		else		device_context->RSSetState(rasterizer_states[1].Get());

		//	�V�F�[�_�[(2��)�̐ݒ�
		device_context->VSSetShader(vs.Get(), nullptr, 0);
		device_context->PSSetShader(ps.Get(), nullptr, 0);

		//device_context->PSSetShaderResources(0, 1, subsets.at(0).diffuse.shader_resource_view.GetAddressOf());
		//device_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());

		//	�[�x�e�X�g�̐ݒ�
		device_context->OMSetDepthStencilState(depth_stencil_state.Get(), 0);

		//	�v���~�e�B�u�̕`��(index�t��)
		//device_context->DrawIndexed(36, 0, 0);

		/*std::vector<bone> &skeletal = mesh.skeletal;
		for (size_t i = 0; i < skeletal.size(); i++)
		{
			DirectX::XMStoreFloat4x4(&cb.bone_transforms[i], DirectX::XMLoadFloat4x4(&skeletal.at(i).transform));
		}*/

		//�{�[���s����R���X�^���g�o�b�t�@�ɃZ�b�g//�A�j���[�V���������邽�߂̃|�[�Y���
		if (mesh.skeletal_animation.size() > 0)
		{
			size_t frame = static_cast<size_t>(mesh.skeletal_animation.animation_tick / mesh.skeletal_animation.sampling_time);
			if (frame > mesh.skeletal_animation.size() - 1)
			{
				frame = 0;
				mesh.skeletal_animation.animation_tick = 0;
			}
			std::vector<bone> &skeletal = mesh.skeletal_animation.at(frame);
			size_t number_of_bones = skeletal.size();
			_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");
			for (size_t i = 0; i < number_of_bones; i++)
			{
				DirectX::XMStoreFloat4x4(&cb.bone_transforms[i], DirectX::XMLoadFloat4x4(&skeletal.at(i).transform));
			}
			mesh.skeletal_animation.animation_tick += elapsed_time;
		}

		//�_�~�[�s��
		/*DirectX::XMStoreFloat4x4(&cb.bone_transforms[0], DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&cb.bone_transforms[1], DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&cb.bone_transforms[2], DirectX::XMMatrixIdentity());
		static float angle = 0;
		DirectX::XMStoreFloat4x4(&cb.bone_transforms[0], DirectX::XMMatrixRotationRollPitchYaw(0 * 0.01745f, 0 * 0.01745f, 0 * 0.01745f));
		DirectX::XMStoreFloat4x4(&cb.bone_transforms[1], DirectX::XMMatrixRotationRollPitchYaw(0 * 0.01745f, 0 * 0.01745f, angle * 0.01745f));
		DirectX::XMStoreFloat4x4(&cb.bone_transforms[2], DirectX::XMMatrixRotationRollPitchYaw(0 * 0.01745f, 0 * 0.01745f, 0 * 0.01745f));
		angle += 0.1f;*/

		//�T�u�Z�b�g�P�ʂŕ`��
		for (subset &subset : mesh.subsets)
		{
			cb.material_color = subset.diffuse.color;
			//cb.material_color = DirectX::XMFLOAT4(1, 0, 0, 1);
			device_context->UpdateSubresource(cs_buffer.Get(), 0, nullptr, &cb, 0, 0);
			device_context->VSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());
			device_context->PSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());

			device_context->PSSetShaderResources(0, 1, subset.diffuse.shader_resource_view.GetAddressOf());//�e�T�u�Z�b�g���Ƃ�shader_resource�������āA���ꂼ���0�Ԗڂɓo�^����Ă�
			device_context->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::POINT_BORDER));
			device_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
		//DirectX::XMStoreFloat4(&cb.focusVec ,focusVec);
	}
}

void skin_mesh::render(ID3D11DeviceContext *device_context,				//�f�o�C�X�R���e�L�X�g 
	shader *shader,                                                        //�V�F�[�_�[
	const DirectX::XMFLOAT4X4 & world_view_projection,	//���[���h�E�r���[�E�v���W�F�N�V���������s�� 
	const DirectX::XMFLOAT4X4 & world,					//���[���h�ϊ��s�� 
	const DirectX::XMFLOAT4 & light_direction,			//���C�g�i�s���� 
	const DirectX::XMFLOAT4 & material_color,			//�ގ��F 
	bool mode,								//���E�h��Ԃ��`��t���O 
	float elapsed_time //�o�ߎ���
	)
{

	for (mesh &mesh : meshes)
	{
		// ���_�o�b�t�@�̃o�C���h
		UINT stride = sizeof(vertex);
		UINT offset = 0;
		device_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);

		//	�C���f�b�N�X�o�b�t�@�̃o�C���h
		device_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//	�萔�o�b�t�@�̍쐬
		ConstantBufferWorld_s cb;
		cb.matWVP = DirectX::XMLoadFloat4x4(&world_view_projection);
		cb.world = DirectX::XMLoadFloat4x4(&world);
		

		//cb.light_direction = light_direction;


		//	�v���~�e�B�u���[�h�̐ݒ�
		device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//	���̓��C�A�E�g�̃o�C���h
		device_context->IASetInputLayout(input_layout.Get());

		//	���X�^���C�U�[�̐ݒ�
		if (!mode)	device_context->RSSetState(rasterizer_states[0].Get());
		else		device_context->RSSetState(rasterizer_states[1].Get());

		//	�V�F�[�_�[(2��)�̐ݒ�
		shader->Activate();
		/*device_context->VSSetShader(vs.Get(), nullptr, 0);
		device_context->PSSetShader(ps.Get(), nullptr, 0);*/

		//	�[�x�e�X�g�̐ݒ�
		device_context->OMSetDepthStencilState(depth_stencil_state.Get(), 0);

		//�{�[���s����R���X�^���g�o�b�t�@�ɃZ�b�g//�A�j���[�V���������邽�߂̃|�[�Y���
		if (mesh.skeletal_animation.size() > 0)
		{
			size_t frame = static_cast<size_t>(mesh.skeletal_animation.animation_tick / mesh.skeletal_animation.sampling_time);
			if (frame > mesh.skeletal_animation.size() - 1)
			{
				frame = 0;
				mesh.skeletal_animation.animation_tick = 0;
			}
			std::vector<bone> &skeletal = mesh.skeletal_animation.at(frame);
			size_t number_of_bones = skeletal.size();
			_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");
			for (size_t i = 0; i < number_of_bones; i++)
			{
				DirectX::XMStoreFloat4x4(&cb.bone_transforms[i], DirectX::XMLoadFloat4x4(&skeletal.at(i).transform));
				DirectX::XMStoreFloat4x4(&BoneTransForms[i], DirectX::XMLoadFloat4x4(&skeletal.at(i).transform));//getter�p�̕��ɂ��Z�b�g
			}
			mesh.skeletal_animation.animation_tick += elapsed_time;
		}

		//�T�u�Z�b�g�P�ʂŕ`��
		for (subset &subset : mesh.subsets)
		{
			//cb.material_color = subset.diffuse.color;
			//cb.material_color = DirectX::XMFLOAT4(1, 0, 0, 1);
			device_context->UpdateSubresource(shader->ConstantBuffer_w.Get(), 0, nullptr, &cb, 0, 0);
			device_context->VSSetConstantBuffers(3, 1, shader->ConstantBuffer_w.GetAddressOf());
			device_context->PSSetConstantBuffers(3, 1, shader->ConstantBuffer_w.GetAddressOf());
			device_context->GSSetConstantBuffers(3, 1, shader->ConstantBuffer_w.GetAddressOf());

			device_context->PSSetShaderResources(0, 1, subset.diffuse.shader_resource_view.GetAddressOf());//�e�T�u�Z�b�g���Ƃ�shader_resource�������āA���ꂼ���0�Ԗڂɓo�^����Ă�
			device_context->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::POINT_BORDER));
			device_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
		//DirectX::XMStoreFloat4(&cb.focusVec ,focusVec);
		shader->Deactivate();
	}
}

HRESULT skin_mesh::LoadMaterialFromFile(ID3D11Device * pDevice, const wchar_t * filename)
{
	return E_NOTIMPL;
}

int skin_mesh::RayPick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal, float* outLength)
{
	int ret = -1;
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&startPosition);//XMVECTOR�^�ɕϊ�
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);//�I�_�����n�_�Ńx�N�g�����߂�
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);//���������߂�(�e������?)
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec);//���K��
	float nearest;//��ԋ߂�������ێ�
	DirectX::XMStoreFloat(&nearest, length);//float�Œ���������?

	DirectX::XMVECTOR position, normal;
	for (const auto& it : faces)
	{
		//�R���X�g���N�^�Ŏ擾�����ʂ�3���_�̏�����
		DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&it.position[0]);
		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&it.position[1]);
		DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&it.position[2]);

		//3���_����x�N�g���̕ӂ��o��
		DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b, a);
		DirectX::XMVECTOR bc = DirectX::XMVectorSubtract(c, b);
		DirectX::XMVECTOR ca = DirectX::XMVectorSubtract(a, c);

		//�ʂ��Ȃ�2�ӂɂ���ĊO�ςŖʖ@�������߂���
		//�ʖ@�� n
		DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(ab, bc));

		//�@��n��Ray(dir��Ray�̐��K��)�̓��ς����Ȃ��n��Ray�͓��������Ɍ����Ă���(�ʂ͗�����)
		//���̏ꍇ����ȏ�̏����͖���
		float dot;
		DirectX::XMStoreFloat(&dot, DirectX::XMVector3Dot(n, dir));
		if (dot >= 0)
			continue;

		//��_�Z�o
		//�܂��͖ʂ����_�܂ł̋��������߂�

		//ray�n�_�Ɩʂ̍ŒZ����
		DirectX::XMVECTOR dist;//Ray�n�_����ʂ܂ł̋���
		//�ʖ@���Ɩʏ�̂���_����n�_�ւ̃x�N�g���̓��ϒl���ŒZ����
		dist = DirectX::XMVectorDivide(DirectX::XMVector3Dot(DirectX::XMVectorSubtract(a, start), n), DirectX::XMVector3Dot(dir, n));

		float ft;
		DirectX::XMStoreFloat(&ft, dist);
		if (ft<0.0f || ft>nearest)//�������������ɂ����菬�����z������Ȃ疳��
			continue;
		DirectX::XMVECTOR RayColPos;//Ray���ʂƂԂ��������W
		//Ray�̎n�_���W+(Ray�̒P�ʃx�N�g��*Ray�̑傫��)=Ray���Ԃ���ʂ̍��W
		RayColPos = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(dir, dist), start);

		//���_����
		//RayColPos���O�p�`�̂��Ƃɂ���ΊO�ςŏo��x�N�g�����t�ɂȂ�A�Ō�̓��ς̒l�����ɂȂ�
		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(a, RayColPos);//�����������W���璸�_�ւ̃x�N�g��
		DirectX::XMVECTOR cs1 = DirectX::XMVector3Cross(v1, ab);//�O��
		DirectX::XMVECTOR dt1 = DirectX::XMVector3Dot(cs1, n);//����
		float fdt1;
		DirectX::XMStoreFloat(&fdt1, dt1);
		DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(b, RayColPos);
		DirectX::XMVECTOR cs2 = DirectX::XMVector3Cross(v2, bc);
		DirectX::XMVECTOR dt2 = DirectX::XMVector3Dot(cs2, n);
		float fdt2;
		DirectX::XMStoreFloat(&fdt2, dt2);
		DirectX::XMVECTOR v3 = DirectX::XMVectorSubtract(c, RayColPos);
		DirectX::XMVECTOR cs3 = DirectX::XMVector3Cross(v3, ca);
		DirectX::XMVECTOR dt3 = DirectX::XMVector3Dot(cs3, n);
		float fdt3;
		DirectX::XMStoreFloat(&fdt3, dt3);
		if (fdt1 < 0.0f || fdt2 < 0.0f || fdt3 < 0.0f)
			continue;

		//���ۑ�
		position = RayColPos;
		normal = n;
		nearest = ft;
		ret = it.materialIndex;
	};

	if (ret != -1)
	{
		DirectX::XMStoreFloat3(outPosition, position);
		DirectX::XMStoreFloat3(outNormal, normal);
	}
	//�ł��߂��q�b�g���W�܂ł̋���
	*outLength = nearest;
	return ret;
}
