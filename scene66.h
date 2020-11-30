#pragma once
#include "scene.h"
class frame_buffer;
class shader;
class main_camera;
class Texture;
class sprite;
class static_mesh;
class static_object;
class liner_depth;

class scene66 :public scene
{
private:
	int timer;
	//フレームバッファ
	std::unique_ptr<frame_buffer> f_buffer;
	//シェーダー
	std::unique_ptr<shader> volume_light_shader, volume_light_depth_shader, _2d_shader;
	std::unique_ptr<sprite> spr;
	std::unique_ptr<Texture> depth_map;
	void *depth_view;

	struct _status
	{
		float sphere_pos[3] = { -4.0f,0.0f,0.0f };
		int timer = 0;
		float cam_pos[3] = { -7.5f,-10.0f,6.2f };
		float obj_dist = 0.0f;
		float light_color[3] = { 1.0f, 0.9f, 0.8f };
		bool another_demo = false;
	};
	std::unique_ptr<_status> status;
public:
	scene66() {}
	virtual ~scene66() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;

};