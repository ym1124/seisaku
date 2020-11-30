#pragma once
#include "scene.h"

class my_mesh;
class animation;
class gpu_particle;
class firework_emitter;
class shader;
class shader_ex;
class frame_buffer;
class main_camera;
class cube_mesh;
class environment_cubemap;
class blur;


class scene16 :public scene
{
private:
	static const int MAX_LIGHT = 4;
	int model_num = 0;

	struct _status
	{
		float color[4] = { 1.0f,0.058f,0.0f,1.0f };
		bool model_flg = false;
	};
	std::unique_ptr<_status> status;
public:
	scene16() {}
	virtual ~scene16();
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;

	//ストリング
	char message[256] = { 0 };
	//モデル
	std::unique_ptr<my_mesh>  cook;
	std::unique_ptr<animation> anim;
	std::shared_ptr<static_mesh> s_mesh[2];
	std::unique_ptr<static_object> s_obj[2];

	//ブルーム用
	std::unique_ptr<blur>  m_blur;
	//シェーダー
	std::unique_ptr<shader_ex>  fbx_m_shader;
	std::unique_ptr<shader>  m_shader;
	//カメラ
	std::unique_ptr<main_camera> m_camera;
	//画面管理
	std::unique_ptr<frame_buffer> f_buffer;
	//GPUパーティクル
	std::unique_ptr<gpu_particle> gp;
	std::unique_ptr<firework_emitter> fe;
};