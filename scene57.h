#pragma once
#include "scenes.h"

class scene57 :public scene
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
		float plane[4] = { 0.0f, 1.0f, 0.0f, 2.0f };
		float camera[3] = { 2.0f, 2.0f, 1.5f };
		float sky[3] = { 0.0f, 0.745f, 0.9f };
		float light[3] = { -0.6f,-0.6f,0.6f };
		float height_factor = 3.0f;
		int sampling = 128;
	};
	std::unique_ptr<_status> status;
	void *view[3];
public:
	scene57() {}
	virtual ~scene57() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;

	//ストリング
	char message[256] = { 0 };

	//シェーダー
	std::unique_ptr<shader> field_shader;
	//カメラ
	std::unique_ptr<main_camera> m_camera;
	//tex
	std::unique_ptr<Texture> height_tex;
	//def
	std::unique_ptr<deffered> m_deff;
};