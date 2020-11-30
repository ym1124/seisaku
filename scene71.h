#pragma once
#include "scene.h"
class frame_buffer;
class shader;
class main_camera;
class Texture;
class sprite;
class static_mesh;
class static_object;
class gpu_particle;
class shadow_map;
class geo_sphere;
class tessellation_mesh;
class tessellation_object;

class scene71 :public scene
{
private:
	//フレームバッファ
	std::unique_ptr<frame_buffer> f_buffer;
	//シェーダー
	std::unique_ptr<shader> m_shader, trail_shader, _2d_shader,snowman_shader;
	//カメラ
	std::unique_ptr<main_camera> m_camera,floor_camera,l_camera;
	//オブジェクト
	std::shared_ptr<tessellation_mesh> ts_mesh;
	std::unique_ptr<tessellation_object> ts_obj;
	std::shared_ptr<static_mesh> s_mesh;
	std::unique_ptr<static_object> s_obj;
	std::unique_ptr<geo_sphere> sph;
	//テクスチャ
	std::unique_ptr<Texture_d> depth_tex;
	std::unique_ptr<Texture> floor_tex;
	std::unique_ptr<Texture> pre_depth_tex;
	//スプライト
	std::unique_ptr <sprite > spr;
	//gpuパーティクル
	std::unique_ptr<gpu_particle> gp;
	//シャドウ
	std::unique_ptr<shadow_map> m_shadow;

	void* view;
	bool once;
	float s_scale;

	struct _status
	{
		int tess_factor = 64;
		int tess_inside_factor = 64;
		float depth = 3.0f;
		float f_camera[3] = { 0,-50,0 };
		float f_camera_orth[2] = { 60,60 };
		bool wire_frame = false;
	};
	std::unique_ptr<_status> status;
public:
	scene71() {}
	virtual ~scene71() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;
};
