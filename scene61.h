#pragma once
#include "scene.h"

class frame_buffer;
class main_camera;
class sprite;
class Texture;
class shader;
class shader_ex;
class static_mesh;
class static_object;
class my_mesh;
class animation;
class deffered;
class liner_depth;
class blur;
class animation;
class liner_depth;
class sprite;
class blur;

class scene61 :public scene
{
private:
	float timer;
	std::unique_ptr<sprite> spr;

	struct _status
	{
		float focus = 0.5f;
	};
	std::unique_ptr<_status> status;
public:
	scene61() {}
	virtual ~scene61() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;

	//ストリング
	char message[256] = { 0 };
	//tex
	std::unique_ptr<Texture> conv_tex;
	//フレームバッファ
	std::unique_ptr<frame_buffer> f_buffer;
	//シェーダー
	std::unique_ptr<shader> sky_shader;
	//カメラ
	std::unique_ptr<main_camera> m_camera;
	//テクスチャ
	std::unique_ptr<Texture> sky_noise_tex;
	//シェーダー
	std::unique_ptr<shader> m_shader, _2d_shader, conv_depth_shader, composite_shader;
	std::unique_ptr<shader_ex> fbx_m_shader;
	//
	std::unique_ptr<deffered> m_deffered;
	//
	std::unique_ptr<liner_depth> l_depth;
	//
	std::unique_ptr<blur> m_blur[2];
	//
	static const int PLAYER_MAX = 3;
	std::unique_ptr<my_mesh> player[PLAYER_MAX];
	std::unique_ptr<my_mesh> stage;
	std::unique_ptr<animation> anim[PLAYER_MAX];
	void *view[2];
	int focus_num;
};