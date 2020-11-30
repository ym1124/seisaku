#pragma once
#include "scenes.h"
class scene56 :public scene
{
private:
	static const int MAX_LIGHT = 4;
	std::unique_ptr<my_mesh> player;
	std::unique_ptr<animation> anim;
	std::shared_ptr<static_mesh>  obj_mesh;
	std::unique_ptr<static_object>  obj[4];

	std::unique_ptr<Texture> tex_buffer;
	std::unique_ptr<sprite> spr;

	//frame_buffer
	std::unique_ptr<frame_buffer> f_buffer;

	struct _status
	{
		int view_type = 0;
		float scale = 0.5f;
		float attenuation = 0.01f;
		float offset = 0.1f;
		float height_weight_rate = 0.1f;
	};
	std::unique_ptr<_status> status;
	void *view[3];
public:
	scene56() {}
	virtual ~scene56() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;

	//ストリング
	char message[256] = { 0 };

	//シェーダー
	std::unique_ptr<shader_ex> fbx_m_shader;
	std::unique_ptr<shader> m_shader, const_fog_shader, height_fog_shader;
	//カメラ
	std::unique_ptr<main_camera> m_camera;
	//tex
	std::unique_ptr<Texture> scene_tex;
	//def
	std::unique_ptr<deffered> m_deff;
};
