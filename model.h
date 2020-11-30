#pragma once

#include <memory>
#include <d3d11.h>
#include "model_resource.h"

class model
{
public:

	float m_motion_speed;
	int playIndex = -1;

	model(std::shared_ptr<model_resource>& resource);
	~model() {}


	struct node
	{
		const char* name;
		node* parent;
		XMFLOAT3	scale;
		XMFLOAT4	rotate;
		XMFLOAT3	translate;
		XMFLOAT4X4	local_transform;
		XMFLOAT4X4	world_transform;
	};

	//// アニメーション
	//bool IsPlayAnimation() const { return m_current_animation >= 0; }
	//void PlayAnimation(int animation_index, bool loop = false);
	//bool UpdateAnimation(float elapsed_time);

	// 行列計算
	void calculateLocalTransform();
	void calculateWorldTransform(const XMFLOAT4X4& world_transform);

	//テクスチャの設定
	void setMaterialTex(ID3D11Device* device, const int index, const wchar_t* filename);
	int getMaterialNum() { return m_model_resource->getMaterialNum(); }

	const std::vector<node>& getNodes() const { return m_nodes; }
	const model_resource* getModelResource() const { return m_model_resource.get(); }

	XMFLOAT4X4 getBoneLocalMatrix(const char* name);

	////ボーンブレンドしてる最中かどうか
	//bool GetBlendMode() { return m_blend_mode; }

	std::vector<node>	 m_nodes;
private:
	std::shared_ptr<model_resource>	m_model_resource;

	//int								m_current_animation = -1;
	//int								m_prev_current_animation = -1;

	//int								m_blend_seconds = 0;

	//float							m_current_seconds = 0.0f;
	//bool							m_loop_animation = false;
	//bool							m_end_animation = false;

	//bool							m_blend_mode = false;	//ボーンブレンドしてる最中ならtrue
};
