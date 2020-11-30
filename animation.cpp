#include "animation.h"
#include "model.h"
#include "input_data.h"

#include <DirectXMath.h>
#include <vector>

static const int				BLEND_TIME = 3;

//void Animation::Play(int animation_index, bool loop)
//{
//	m_prev_current_animation = m_current_animation;
//
//	m_current_animation = animation_index;
//	m_loop_animation = loop;
//	m_end_animation = false;
//	m_current_seconds = 0.0f;
//}

animation::animation(const char* animationName)
{
	m_animID = 0;
	//�A�j���[�V�����̖��O�Ɣԍ��̊֘A�t��
	m_index.emplace_hint(m_index.begin(), std::string(animationName), m_animID);

	m_current_animation = m_animID;
	m_prev_current_animation = m_current_animation;

	m_end_animation = false;
	m_current_seconds = 0.0f;
}

bool animation::set(const char* animationName)
{
	m_prev_current_animation = m_current_animation;

	//�A�j���[�V�����̌���
	auto it = m_index.find(animationName);

	if (it != m_index.end()) {
		m_current_animation = it->second;
	}
	else {
		//�w�肳�ꂽ�A�j���[�V�������Ȃ�
		return false;
	}

	return true;
}

bool animation::update(model* _model, bool loop)
{
	//���f���ɃA�j���[�V�����f�[�^���Ȃ��I
	if (_model->getModelResource()->getAnimations().empty())
	{
		return false;
	}

	//�A�j���[�V�������؂�ւ������
	if (m_current_animation != m_prev_current_animation) {
		m_blend_mode = true;
		m_prev_current_animation = m_current_animation;
		m_blend_seconds = 0;
		m_end_animation = false;
		//m_current_seconds = 0.0f;
	}

	if (m_blend_mode) {
		const ModelData::Animation& animation = _model->getModelResource()->getAnimations().at(m_current_animation);

		const std::vector<ModelData::Keyframe>& keyframes = animation.keyframes;

		const ModelData::Keyframe& keyframe = keyframes.at(1);

		int node_count = static_cast<int>(_model->m_nodes.size());
		for (int node_index = 0; node_index < node_count; ++node_index)
		{
			// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
			const ModelData::NodeKeyData& key = keyframe.node_keys.at(node_index);

			model::node& _node = _model->m_nodes[node_index];

			DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&_node.scale);
			DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key.scale);
			DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&_node.rotate);
			DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key.rotate);
			DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&_node.translate);
			DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key.translate);

			DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, 1.0f / (float)BLEND_TIME);
			DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, 1.0f / (float)BLEND_TIME);
			DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, 1.0f / (float)BLEND_TIME);

			DirectX::XMStoreFloat3(&_node.scale, s);
			DirectX::XMStoreFloat4(&_node.rotate, r);
			DirectX::XMStoreFloat3(&_node.translate, t);
		}

		//�A�j���[�V�����ꎞ��~
		if (m_pause) {
			return true;
		}

		m_blend_seconds++;

		if (m_blend_seconds > BLEND_TIME + 1) {
			m_blend_seconds -= BLEND_TIME + 1;
			m_blend_mode = false;

			m_current_seconds = keyframe.seconds;
		}
		else {
			return true;
		}
	}


	const ModelData::Animation& animation = _model->getModelResource()->getAnimations().at(m_current_animation);

	const std::vector<ModelData::Keyframe>& keyframes = animation.keyframes;
	int key_count = static_cast<int>(keyframes.size());
	for (int key_index = 0; key_index < key_count - 1; ++key_index)
	{
		// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
		const ModelData::Keyframe& keyframe0 = keyframes.at(key_index);
		const ModelData::Keyframe& keyframe1 = keyframes.at(key_index + 1);
		if (m_current_seconds >= keyframe0.seconds && m_current_seconds < keyframe1.seconds)
		{
			float rate = (m_current_seconds - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);

			//assert(m_nodes.size() == keyframe0.node_keys.size());
			//assert(m_nodes.size() == keyframe1.node_keys.size());
			int node_count = static_cast<int>(_model->m_nodes.size());
			for (int node_index = 0; node_index < node_count; ++node_index)
			{
				// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
				const ModelData::NodeKeyData& key0 = keyframe0.node_keys.at(node_index);
				const ModelData::NodeKeyData& key1 = keyframe1.node_keys.at(node_index);

				model::node& _node = _model->m_nodes[node_index];

				DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
				DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
				DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&key0.rotate);
				DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
				DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&key0.translate);
				DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

				DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, rate);
				DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, rate);
				DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, rate);

				DirectX::XMStoreFloat3(&_node.scale, s);
				DirectX::XMStoreFloat4(&_node.rotate, r);
				DirectX::XMStoreFloat3(&_node.translate, t);
			}
			break;
		}
	}

	// �ŏI�t���[������
	if (m_end_animation)
	{
		m_end_animation = false;
		m_current_animation = -1;
		return true;
	}

	//�A�j���[�V�����ꎞ��~
	if (m_pause) {
		return true;
	}

	// ���Ԍo��
	m_current_seconds += m_playSpeed;

	if (m_current_seconds >= animation.seconds_length)
	{
		//�A�j���[�V���������[�v���邩���Ȃ���
		if (loop)
		{
			m_current_seconds -= animation.seconds_length;
		}
		else
		{
			m_current_seconds = animation.seconds_length;
			m_end_animation = true;
		}
	}
	return false;
}

bool animation::add(model* _model, const char* fbxFileName, const char* animationName)
{
	//�A�j���[�V�����̌���
	auto it = m_index.find(animationName);

	if (it != m_index.end()) {
		return false;
	}

	std::unique_ptr<ModelData> anim_data = std::make_unique<ModelData>();

	//�ǂݍ���
	if (!BinaryInput(fbxFileName, *anim_data)) {
		return false;
	}

	_model->getModelResource()->getAnimations().push_back(anim_data->animations.at(0));

	m_animID++;

	//�A�j���[�V�����̖��O�Ɣԍ��̊֘A�t��
	m_index.emplace_hint(m_index.end(), std::string(animationName), m_animID);

	return true;
}

bool animation::add(const char* animationName)
{
	//�A�j���[�V�����̌���
	auto it = m_index.find(animationName);

	if (it != m_index.end()) {
		return false;
	}

	m_animID++;

	//�A�j���[�V�����̖��O�Ɣԍ��̊֘A�t��
	m_index.emplace_hint(m_index.end(), std::string(animationName), m_animID);

	return true;
}

bool animation::isCurrentAnimation(const char* animationName)
{
	int checkAnimation = -1;

	//�A�j���[�V�����̌���
	auto it = m_index.find(animationName);

	if (it != m_index.end()) {
		checkAnimation = it->second;
	}
	else {
		//�w�肳�ꂽ�A�j���[�V�������Ȃ�
		return false;
	}

	if (checkAnimation != m_current_animation) return false;

	return true;
}

std::string animation::getCurrentAnimation()
{
	std::string animationName = nullptr;

	//�A�j���[�V�����̌���
	//auto it = m_index.find(m_current_animation);

	//if (it != m_index.end()) {
	//	animationName = it->first;
	//}

	return animationName;
}
