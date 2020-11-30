
//#include "logger.h"
#include "model.h"
#include "misc.h"

static const int				BLEND_TIME = 3;

model::model(std::shared_ptr<model_resource>& resource)
{
	m_model_resource = resource;

	// ノード
	const std::vector<ModelData::Node>& res_nodes = resource->getNodes();

	m_nodes.resize(res_nodes.size());
	for (size_t node_index = 0; node_index < m_nodes.size(); ++node_index)
	{
		auto&& src = res_nodes.at(node_index);
		auto&& dst = m_nodes.at(node_index);

		dst.name = src.name.c_str();
		dst.parent = src.parent_index >= 0 ? &m_nodes.at(src.parent_index) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;
	}
}

//// アニメーション再生
//void Model::PlayAnimation(int animation_index, bool loop)
//{
//	m_prev_current_animation = m_current_animation;
//
//	//if (isPlayingPlay&&playIndex == animation_index) { return; }
//
//	//playIndex = animation_index;
//	m_current_animation = animation_index;
//	m_loop_animation = loop;
//	m_end_animation = false;
//	m_current_seconds = 0.0f;
//}
//
//// アニメーション計算
//bool Model::UpdateAnimation(float elapsed_time)
//{
//	if (m_current_animation < 0)
//	{
//		return false;
//	}
//
//	if (m_model_resource->GetAnimations().empty())
//	{
//		return false;
//	}
//
//	if (m_current_animation != m_prev_current_animation) {
//		m_blend_mode = true;
//		m_prev_current_animation = m_current_animation;
//		m_blend_seconds = 0;
//	}
//
//	if (m_blend_mode) {
//		const ModelData::Animation& animation = m_model_resource->GetAnimations().at(m_current_animation);
//
//		const std::vector<ModelData::Keyframe>& keyframes = animation.keyframes;
//
//		const ModelData::Keyframe& keyframe = keyframes.at(1);
//
//		int node_count = static_cast<int>(m_nodes.size());
//		for (int node_index = 0; node_index < node_count; ++node_index)
//		{
//			// ２つのキーフレーム間の補完計算
//			const ModelData::NodeKeyData& key = keyframe.node_keys.at(node_index);
//
//			Node& node = m_nodes[node_index];
//
//			XMVECTOR s0 = XMLoadFloat3(&node.scale);
//			XMVECTOR s1 = XMLoadFloat3(&key.scale);
//			XMVECTOR r0 = XMLoadFloat4(&node.rotate);
//			XMVECTOR r1 = XMLoadFloat4(&key.rotate);
//			XMVECTOR t0 = XMLoadFloat3(&node.translate);
//			XMVECTOR t1 = XMLoadFloat3(&key.translate);
//
//			XMVECTOR s = XMVectorLerp(s0, s1, 1.0f / (float)BLEND_TIME);
//			XMVECTOR r = XMQuaternionSlerp(r0, r1, 1.0f / (float)BLEND_TIME);
//			XMVECTOR t = XMVectorLerp(t0, t1, 1.0f / (float)BLEND_TIME);
//
//			XMStoreFloat3(&node.scale, s);
//			XMStoreFloat4(&node.rotate, r);
//			XMStoreFloat3(&node.translate, t);
//		}
//
//		m_blend_seconds++;
//
//		if (m_blend_seconds > BLEND_TIME + 1) {
//			m_blend_seconds -= BLEND_TIME + 1;
//			m_blend_mode = false;
//
//			m_current_seconds = keyframe.seconds;
//		}
//
//		return false;
//	}
//
//
//	const ModelData::Animation& animation = m_model_resource->GetAnimations().at(m_current_animation);
//
//	const std::vector<ModelData::Keyframe>& keyframes = animation.keyframes;
//	int key_count = static_cast<int>(keyframes.size());
//	for (int key_index = 0; key_index < key_count - 1; ++key_index)
//	{
//		// 現在の時間がどのキーフレームの間にいるか判定する
//		const ModelData::Keyframe& keyframe0 = keyframes.at(key_index);
//		const ModelData::Keyframe& keyframe1 = keyframes.at(key_index + 1);
//		if (m_current_seconds >= keyframe0.seconds && m_current_seconds < keyframe1.seconds)
//		{
//			float rate = (m_current_seconds - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);
//
//			//assert(m_nodes.size() == keyframe0.node_keys.size());
//			//assert(m_nodes.size() == keyframe1.node_keys.size());
//			int node_count = static_cast<int>(m_nodes.size());
//			for (int node_index = 0; node_index < node_count; ++node_index)
//			{
//				// ２つのキーフレーム間の補完計算
//				const ModelData::NodeKeyData& key0 = keyframe0.node_keys.at(node_index);
//				const ModelData::NodeKeyData& key1 = keyframe1.node_keys.at(node_index);
//
//				Node& node = m_nodes[node_index];
//				
//				XMVECTOR s0 = XMLoadFloat3(&key0.scale);
//				XMVECTOR s1 = XMLoadFloat3(&key1.scale);
//				XMVECTOR r0 = XMLoadFloat4(&key0.rotate);
//				XMVECTOR r1 = XMLoadFloat4(&key1.rotate);
//				XMVECTOR t0 = XMLoadFloat3(&key0.translate);
//				XMVECTOR t1 = XMLoadFloat3(&key1.translate);
//
//				XMVECTOR s = XMVectorLerp(s0, s1, rate);
//				XMVECTOR r = XMQuaternionSlerp(r0, r1, rate);
//				XMVECTOR t = XMVectorLerp(t0, t1, rate);
//
//				XMStoreFloat3(&node.scale, s);
//				XMStoreFloat4(&node.rotate, r);
//				XMStoreFloat3(&node.translate, t);
//			}
//			break;
//		}
//	}
//
//	// 最終フレーム処理
//	if (m_end_animation)
//	{
//		m_end_animation = false;
//		m_current_animation = -1;
//		return true;
//	}
//
//	// 時間経過
//	m_current_seconds += elapsed_time;
//	if (m_current_seconds >= animation.seconds_length)
//	{
//		if (m_loop_animation)
//		{
//			m_current_seconds -= animation.seconds_length;
//		}
//		else
//		{
//			m_current_seconds = animation.seconds_length;
//			m_end_animation = true;
//		}
//	}
//	return false;
//}



// ローカル変換行列計算
void model::calculateLocalTransform()
{
	for (node& node : m_nodes)
	{
		XMMATRIX scale, rotate, translate, local_matrix;
		scale = XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		rotate = XMMatrixRotationQuaternion(XMVectorSet(node.rotate.x, node.rotate.y, node.rotate.z, node.rotate.w));
		translate = XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);

		local_matrix = scale * rotate * translate;
		DirectX::XMStoreFloat4x4(&node.local_transform, local_matrix);
	}
}

// ワールド変換行列計算
void model::calculateWorldTransform(const XMFLOAT4X4& world_transform)
{
	for (node& node : m_nodes)
	{
		XMMATRIX local_matrix, node_world_trans;
		local_matrix = DirectX::XMLoadFloat4x4(&node.local_transform);

		if (node.parent != nullptr)
		{
			XMMATRIX world_trans;
			world_trans = DirectX::XMLoadFloat4x4(&node.parent->world_transform);

			node_world_trans = local_matrix * world_trans;
		}
		else
		{
			node_world_trans = local_matrix * DirectX::XMLoadFloat4x4(&world_transform);
		}

		DirectX::XMStoreFloat4x4(&node.world_transform, node_world_trans);
	}
}

void model::setMaterialTex(ID3D11Device* device, const int index, const wchar_t* filename)
{
	m_model_resource->setMaterialTex(device, index, filename);
}

XMFLOAT4X4 model::getBoneLocalMatrix(const char* name)
{
	//const ModelResource* model_resource = model.GetModelResource();
	//const std::vector<Model::Node>& nodes = model.GetNodes();

	//for (const ModelResource::Mesh& mesh : model_resource->GetMeshes())
	//{
	//	// メッシュ用定数バッファ更新
	//	CbMesh cb_mesh;
	//	::memset(&cb_mesh, 0, sizeof(cb_mesh));
	//	if (mesh.node_indices.size() > 0)
	//	{
	//		for (size_t i = 0; i < mesh.node_indices.size(); ++i)
	//		{
	//			//DirectX::XMMATRIX inverse_transform = DirectX::XMLoadFloat4x4(mesh.inverse_transforms.at(i));
	//			//DirectX::XMMATRIX world_transform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_indices.at(i)).world_transform);
	//			XMMATRIX bone_transform = mesh.inverse_transforms.at(i) * DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_indices.at(i)).world_transform);
	//			XMStoreFloat4x4(&cb_mesh.bone_transforms[i], bone_transform);
	//		}
	//	}
	//	else
	//	{
	//		XMStoreFloat4x4(&cb_mesh.bone_transforms[0], DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_index).world_transform));
	//	}
	//}

	for (size_t node_index = 0; node_index < m_nodes.size(); ++node_index)
	{
		auto&& dst = m_nodes.at(node_index);

		if (std::string(dst.name) == std::string(name)) {
			return dst.world_transform;
		}
	}
}
