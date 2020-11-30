#pragma once

#include "skin_mesh.h"
#include "load_file.h"
#include "misc.h"
#include "load_file.h"
//#include <fbxsdk.h>
//#include <functional>
#include "shader.h"
#include "framework.h"

//姿勢行列作成
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

//ボーン構造体
struct bone_influence
{
	int index; //影響されるボーン
	float weight; // 重さ
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

//アニメーション取得関数
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
		//sampling_rate:一秒間で何度アニメーションをサンプリングするか
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

	//FBX Manager作成//FBX読み込み関係のマネージャー?
	FbxManager* manager = FbxManager::Create();

	//
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

	//インポーター作成//インポートするために必要
	FbxImporter* importer = FbxImporter::Create(manager, "");

	//インポーター初期化
	bool import_status = false;
	import_status = importer->Initialize(fbx_filename, -1, manager->GetIOSettings());
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	//シーン作成
	FbxScene* scene = FbxScene::Create(manager, "");

	//シーンにファイルの中身をインポートする
	import_status = importer->Import(scene);
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	// Convert mesh, NURBS and patch into triangle mesh
	fbxsdk::FbxGeometryConverter geometry_converter(manager);
	geometry_converter.Triangulate(scene, /*replace*/true);

	//fech:読み込む//attribute:属性
	//attributeがeMesh(メッシュ)属性であるノードをpush_backして配列を作成。頂点情報だけ抜き取る?
	std::vector<FbxNode*> fetched_meshes;
	std::function<void(FbxNode*)> traverse = [&](FbxNode* node)//ラムダ式
	{
		if (node)
		{
			//ノードの属性を取得
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
			//ルートノード→子ノードの順
			//子ノードの数だけだけ回す
			for (int i = 0; i < node->GetChildCount(); i++)
			{
				//子ノード取得?
				traverse(node->GetChild(i));
			}
		}
	};
	traverse(scene->GetRootNode());

	//vector::at(x):x番目の要素にアクセスする//頂点配列のポインタ設定?
	//FbxMesh *fbx_mesh = fetched_meshes.at(0)->GetMesh();

	// マテリアル情報取得
	//const int number_of_materials = fbx_mesh->GetNode()->GetMaterialCount();

	//メッシュ用意
	meshes.resize(fetched_meshes.size());
	//メッシュの数だけループ
	for (size_t i = 0; i < fetched_meshes.size(); i++)
	{
		//push_back用のメッシュ
		FbxMesh *fbx_mesh = fetched_meshes.at(i)->GetMesh();
		mesh &mesh = meshes.at(i);

		//姿勢
		FbxAMatrix global_transform = fbx_mesh->GetNode()->EvaluateGlobalTransform(0);
		fbxamatrix_to_xmfloat4x4(global_transform, mesh.global_transform);

		//ノードのマテリアルの数を取得
		const int number_of_materials = fbx_mesh->GetNode()->GetMaterialCount();
		//resize:要素数を変更する//配列拡張
		mesh.subsets.resize(number_of_materials > 0 ? number_of_materials : 1);

		//マテリアルの数(種類)だけループ//全てのマテリアル情報を得てsubset毎に分ける
		for (int index_of_material = 0; index_of_material < number_of_materials; ++index_of_material)
		{
			subset &subset = mesh.subsets.at(index_of_material);

			const FbxSurfaceMaterial *surface_material = fbx_mesh->GetNode()->GetMaterial(index_of_material);//マテリアル情報を格納
			const FbxProperty property = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			const FbxProperty factor = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
			//色情報の設定
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

						//hr = load_texture_from_file(device, filename_fbx, subset.diffuse.shader_resource_view.GetAddressOf(), &subset.diffuse.tex2d_desc);//ここは1度しか通らない//shader_resource_viewが設定されるのは1個目のsubsetの分だけ
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

		// 各マテリアルをポリゴンで分ける(?)
		if (number_of_materials > 0)
		{
			// 各マテリアルごとのポリゴン数を数える
			const int number_of_polygons = fbx_mesh->GetPolygonCount();
			//meshのsubset(material毎に分けて描画する)のindex_count(頂点数)を数える
			u_int material_index;
			for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; ++index_of_polygon)
			{
				material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
				mesh.subsets.at(material_index).index_count += 3;
			}

			// どこからカウントを開始するか(オフセット)
			int offset = 0;
			for (subset &subset : mesh.subsets)
			{
				subset.index_start = offset;//各subsetのスタート地点
				offset += subset.index_count;
				subset.index_count = 0;
			}
		}

		//メッシュ頂点データ
		std::vector<vertex> vertices; // Vertex buffer 
		std::vector<u_int> indices;  // Index buffer 
		u_int vertex_count = 0;

		//UV取得する
		FbxStringList uv_names;//名前を格納するリスト?
		fbx_mesh->GetUVSetNames(uv_names);//全UVの名前を取得?

		//頂点座標をバッファに格納する　　　　　　　　　　　　　　　　　　　　　　　　//PolygonVertex:頂点インデックス
		const FbxVector4 *array_of_control_points = fbx_mesh->GetControlPoints();//頂点情報をとりあえず取る
		const int number_of_polygons = fbx_mesh->GetPolygonCount();//(頂点3の)ポリゴンの総数
		indices.resize(number_of_polygons * 3);

		//ボーン影響度をFBXデータから取得する
		std::vector<bone_influences_per_control_point> bone_influences;
		fetch_bone_influences(fbx_mesh, bone_influences);

		//面情報格納用
		Face f;

		for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++)//ポリゴンの総数までループ処理
		{
			int index_of_material = 0;
			if (number_of_materials > 0)
			{
				index_of_material = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
			}

			f.materialIndex = index_of_material;

			subset &subset = mesh.subsets.at(index_of_material);
			const int index_offset = subset.index_start + subset.index_count;

			for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++) {//各ポリゴンの頂点3つ分をループ処理//GetPolygonVertex(a,b):a:ポリゴンの番号,b:そのポリゴンの何個目の頂点か,戻り値:全体でのインデックス番号
				const int index_of_control_point = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);//現在探査中の[index_of_polygon]個目のポリゴンの[index_of_vertex]個めの頂点の【全体での番号】(インデックス)を取得
				//インデックス番号通りに頂点の情報が格納される
				vertex vertex;
				vertex.position.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);//各成分ごとにやる
				vertex.position.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
				vertex.position.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);

				f.position[index_of_vertex] = vertex.position;//面ごとの３頂点を格納

				FbxVector4 normal;
				fbx_mesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
				vertex.normal.x = static_cast<float>(normal[0]);
				vertex.normal.y = static_cast<float>(normal[1]);
				vertex.normal.z = static_cast<float>(normal[2]);

				if (fbx_mesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmapped_uv;
					//指定した頂点のuv座標を二行前に宣言したuvに格納する                      //↓0番目に格納されてる名前のuvのuv座標???
					fbx_mesh->GetPolygonVertexUV(index_of_polygon, index_of_vertex, uv_names[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}

				//ボーンのインデックスと重さを格納
				bone_influences_per_control_point influences_per_control_point = bone_influences.at(index_of_control_point);//現在の頂点が影響されるボーン取得
				for (size_t index_of_influence = 0; index_of_influence < influences_per_control_point.size(); index_of_influence++)//影響されるボーンの数だけループ
				{
					if (index_of_influence < MAX_BONE_INFLUENCES)//最大4ボーンまでしか影響を受けない
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

				vertices.push_back(vertex);//設定したvertexをverticiesにpush_backで格納
				//indices.push_back(vertex_count);//今設定した頂点のインデックス番号を格納
				indices.at(index_offset + index_of_vertex) = static_cast<u_int>(vertex_count);
				vertex_count += 1;//次の頂点用にカウントを増やしてインデックス番号にする
			}
			subset.index_count += 3;

			faces.push_back(f);//面情報
		}
		//ボーン行列取得
		//FbxTime::EMode time_mode = fbx_mesh->GetScene()->GetGlobalSettings().GetTimeMode();
		//FbxTime frame_time;
		//frame_time.SetTime(0, 0, 0, 1, 0, time_mode);
		//fetch_bone_matrices(fbx_mesh, mesh.skeletal, frame_time * 20); // pose at frame 20

		//ボーンアニメーションの行列取得
		fetch_animations(fbx_mesh, mesh.skeletal_animation);

		mesh.create_buffer(FRAMEWORK.getInstance().getDevice(), vertices.data(), vertices.size(), indices.data(), indices.size());

		//各サブセット毎にshader_resource_viewを設定する//現時点では1個目しか設定されていない//疑似的なテクスチャを作って映るようにだけする
		for (subset &subset : mesh.subsets)
		{
			if (!subset.diffuse.shader_resource_view)
			{
				make_dummy_texture(FRAMEWORK.getInstance().getDevice(), &subset.diffuse.shader_resource_view);
			}
		}
	}
	manager->Destroy();
	//頂点情報抜出終わり

	// Initialize Direct3D COM objects 

	//入力レイアウトオブジェクトの生成 
	D3D11_INPUT_ELEMENT_DESC VertexDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//頂点シェーダーオブジェクトの生成 
	vs_Load(FRAMEWORK.getInstance().getDevice(), "Shader/skin_mesh_vs.cso", vs.GetAddressOf(), input_layout.GetAddressOf(), VertexDesc, ARRAYSIZE(VertexDesc));

	//ピクセルシェーダーオブジェクトの生成 
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
	//定数バッファ作成
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

//頂点バッファ作成
void skin_mesh::mesh::create_buffer(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices)
{
	HRESULT hr;
	//頂点バッファ
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
	//インデックスバッファ
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

void skin_mesh::render(ID3D11DeviceContext *device_context,				//デバイスコンテキスト 
	const DirectX::XMFLOAT4X4 & world_view_projection,	//ワールド・ビュー・プロジェクション合成行列 
	const DirectX::XMFLOAT4X4 & world,					//ワールド変換行列 
	const DirectX::XMFLOAT4 & light_direction,			//ライト進行方向 
	const DirectX::XMFLOAT4 & material_color,			//材質色 
	bool mode,								//線・塗りつぶし描画フラグ 
	float elapsed_time //経過時間
	)
{
	for (mesh &mesh : meshes)
	{
		// 頂点バッファのバインド
		UINT stride = sizeof(vertex);
		UINT offset = 0;
		device_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);

		//	インデックスバッファのバインド
		device_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//	定数バッファの作成
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

		//	プリミティブモードの設定
		device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//	入力レイアウトのバインド
		device_context->IASetInputLayout(input_layout.Get());

		//	ラスタライザーの設定
		if (!mode)	device_context->RSSetState(rasterizer_states[0].Get());
		else		device_context->RSSetState(rasterizer_states[1].Get());

		//	シェーダー(2種)の設定
		device_context->VSSetShader(vs.Get(), nullptr, 0);
		device_context->PSSetShader(ps.Get(), nullptr, 0);

		//device_context->PSSetShaderResources(0, 1, subsets.at(0).diffuse.shader_resource_view.GetAddressOf());
		//device_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());

		//	深度テストの設定
		device_context->OMSetDepthStencilState(depth_stencil_state.Get(), 0);

		//	プリミティブの描画(index付き)
		//device_context->DrawIndexed(36, 0, 0);

		/*std::vector<bone> &skeletal = mesh.skeletal;
		for (size_t i = 0; i < skeletal.size(); i++)
		{
			DirectX::XMStoreFloat4x4(&cb.bone_transforms[i], DirectX::XMLoadFloat4x4(&skeletal.at(i).transform));
		}*/

		//ボーン行列をコンスタントバッファにセット//アニメーションさせるためのポーズ情報
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

		//ダミー行列
		/*DirectX::XMStoreFloat4x4(&cb.bone_transforms[0], DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&cb.bone_transforms[1], DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&cb.bone_transforms[2], DirectX::XMMatrixIdentity());
		static float angle = 0;
		DirectX::XMStoreFloat4x4(&cb.bone_transforms[0], DirectX::XMMatrixRotationRollPitchYaw(0 * 0.01745f, 0 * 0.01745f, 0 * 0.01745f));
		DirectX::XMStoreFloat4x4(&cb.bone_transforms[1], DirectX::XMMatrixRotationRollPitchYaw(0 * 0.01745f, 0 * 0.01745f, angle * 0.01745f));
		DirectX::XMStoreFloat4x4(&cb.bone_transforms[2], DirectX::XMMatrixRotationRollPitchYaw(0 * 0.01745f, 0 * 0.01745f, 0 * 0.01745f));
		angle += 0.1f;*/

		//サブセット単位で描画
		for (subset &subset : mesh.subsets)
		{
			cb.material_color = subset.diffuse.color;
			//cb.material_color = DirectX::XMFLOAT4(1, 0, 0, 1);
			device_context->UpdateSubresource(cs_buffer.Get(), 0, nullptr, &cb, 0, 0);
			device_context->VSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());
			device_context->PSSetConstantBuffers(0, 1, cs_buffer.GetAddressOf());

			device_context->PSSetShaderResources(0, 1, subset.diffuse.shader_resource_view.GetAddressOf());//各サブセットごとにshader_resourceがあって、それぞれを0番目に登録されてる
			device_context->PSSetSamplers(0, 1, FRAMEWORK.getSamplerStateAdress(FRAMEWORK.SM::POINT_BORDER));
			device_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
		//DirectX::XMStoreFloat4(&cb.focusVec ,focusVec);
	}
}

void skin_mesh::render(ID3D11DeviceContext *device_context,				//デバイスコンテキスト 
	shader *shader,                                                        //シェーダー
	const DirectX::XMFLOAT4X4 & world_view_projection,	//ワールド・ビュー・プロジェクション合成行列 
	const DirectX::XMFLOAT4X4 & world,					//ワールド変換行列 
	const DirectX::XMFLOAT4 & light_direction,			//ライト進行方向 
	const DirectX::XMFLOAT4 & material_color,			//材質色 
	bool mode,								//線・塗りつぶし描画フラグ 
	float elapsed_time //経過時間
	)
{

	for (mesh &mesh : meshes)
	{
		// 頂点バッファのバインド
		UINT stride = sizeof(vertex);
		UINT offset = 0;
		device_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);

		//	インデックスバッファのバインド
		device_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//	定数バッファの作成
		ConstantBufferWorld_s cb;
		cb.matWVP = DirectX::XMLoadFloat4x4(&world_view_projection);
		cb.world = DirectX::XMLoadFloat4x4(&world);
		

		//cb.light_direction = light_direction;


		//	プリミティブモードの設定
		device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//	入力レイアウトのバインド
		device_context->IASetInputLayout(input_layout.Get());

		//	ラスタライザーの設定
		if (!mode)	device_context->RSSetState(rasterizer_states[0].Get());
		else		device_context->RSSetState(rasterizer_states[1].Get());

		//	シェーダー(2種)の設定
		shader->Activate();
		/*device_context->VSSetShader(vs.Get(), nullptr, 0);
		device_context->PSSetShader(ps.Get(), nullptr, 0);*/

		//	深度テストの設定
		device_context->OMSetDepthStencilState(depth_stencil_state.Get(), 0);

		//ボーン行列をコンスタントバッファにセット//アニメーションさせるためのポーズ情報
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
				DirectX::XMStoreFloat4x4(&BoneTransForms[i], DirectX::XMLoadFloat4x4(&skeletal.at(i).transform));//getter用の方にもセット
			}
			mesh.skeletal_animation.animation_tick += elapsed_time;
		}

		//サブセット単位で描画
		for (subset &subset : mesh.subsets)
		{
			//cb.material_color = subset.diffuse.color;
			//cb.material_color = DirectX::XMFLOAT4(1, 0, 0, 1);
			device_context->UpdateSubresource(shader->ConstantBuffer_w.Get(), 0, nullptr, &cb, 0, 0);
			device_context->VSSetConstantBuffers(3, 1, shader->ConstantBuffer_w.GetAddressOf());
			device_context->PSSetConstantBuffers(3, 1, shader->ConstantBuffer_w.GetAddressOf());
			device_context->GSSetConstantBuffers(3, 1, shader->ConstantBuffer_w.GetAddressOf());

			device_context->PSSetShaderResources(0, 1, subset.diffuse.shader_resource_view.GetAddressOf());//各サブセットごとにshader_resourceがあって、それぞれを0番目に登録されてる
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
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&startPosition);//XMVECTOR型に変換
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);//終点引く始点でベクトル求める
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);//長さを求める(各成分の?)
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec);//正規化
	float nearest;//一番近い距離を保持
	DirectX::XMStoreFloat(&nearest, length);//floatで長さを持つ?

	DirectX::XMVECTOR position, normal;
	for (const auto& it : faces)
	{
		//コンストラクタで取得した面の3頂点の情報を代入
		DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&it.position[0]);
		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&it.position[1]);
		DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&it.position[2]);

		//3頂点からベクトルの辺を出す
		DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b, a);
		DirectX::XMVECTOR bc = DirectX::XMVectorSubtract(c, b);
		DirectX::XMVECTOR ca = DirectX::XMVectorSubtract(a, c);

		//面をなす2辺によって外積で面法線を求められる
		//面法線 n
		DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(ab, bc));

		//法線nとRay(dirはRayの正規化)の内積が正ならばnとRayは同じ方向に向いている(面は裏向き)
		//その場合それ以上の処理は無し
		float dot;
		DirectX::XMStoreFloat(&dot, DirectX::XMVector3Dot(n, dir));
		if (dot >= 0)
			continue;

		//交点算出
		//まずは面から交点までの距離を求める

		//ray始点と面の最短距離
		DirectX::XMVECTOR dist;//Ray始点から面までの距離
		//面法線と面上のある点から始点へのベクトルの内積値が最短距離
		dist = DirectX::XMVectorDivide(DirectX::XMVector3Dot(DirectX::XMVectorSubtract(a, start), n), DirectX::XMVector3Dot(dir, n));

		float ft;
		DirectX::XMStoreFloat(&ft, dist);
		if (ft<0.0f || ft>nearest)//距離が負か既にこれより小さい奴があるなら無視
			continue;
		DirectX::XMVECTOR RayColPos;//Rayが面とぶつかった座標
		//Rayの始点座標+(Rayの単位ベクトル*Rayの大きさ)=Rayがぶつかる面の座標
		RayColPos = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(dir, dist), start);

		//内点判定
		//RayColPosが三角形のそとにあれば外積で出るベクトルが逆になり、最後の内積の値が負になる
		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(a, RayColPos);//当たった座標から頂点へのベクトル
		DirectX::XMVECTOR cs1 = DirectX::XMVector3Cross(v1, ab);//外積
		DirectX::XMVECTOR dt1 = DirectX::XMVector3Dot(cs1, n);//内積
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

		//情報保存
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
	//最も近いヒット座標までの距離
	*outLength = nearest;
	return ret;
}
