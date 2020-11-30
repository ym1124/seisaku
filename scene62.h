#pragma once
#include "scene.h"
#include <random>

class frame_buffer;
class sprite;
class Texture;
class shader;

class scene62 :public scene
{
private:
	float timer;
	int update_flg;
	float angle_x;
	float angle_y;
	
	std::default_random_engine generator;
	std::unique_ptr<sprite> spr;
	std::unique_ptr<Texture> prev_tex, current_tex;
	//フレームバッファ
	std::unique_ptr<frame_buffer> f_buffer;
	//シェーダー
	std::unique_ptr<shader> sky_shader;
	//テクスチャ
	std::unique_ptr<Texture> sky_noise_tex;
	//シェーダー
	std::unique_ptr<shader> pt_shader, screen_shader, _2d_shader;

	struct _status
	{
		float cam_pos[3] = { 0.0f,3.0f,-7.5f };
		float cam_look[3] = { 0.0f,0.0f,0.5f };
		float color[3] = { 1,1,1 };
		float emissive[3] = { 0,0,0 };
		float ior = 0.17f;
		float gloss = 0.1f;
		int surf_type = 0;
	};
	std::unique_ptr<_status> status;
public:
	scene62() {}
	virtual ~scene62() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;
};
