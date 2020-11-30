#pragma once
#include "scene.h"

class frame_buffer;
class Texture;
class sprite;
class main_camera;
class my_mesh;
class animation;
class static_mesh;
class static_object;
class shader;
class shader_ex;
class deffered;
class blur;

class scene45 :public scene
{
private:
	u_int timer = 0;

	//std::shared_ptr<static_mesh>  ground_mesh;
	//std::unique_ptr<static_object> ground[3];
	std::unique_ptr<my_mesh> player,stage;
	std::unique_ptr<animation> anim;

	std::unique_ptr<Texture> ssao_tex;
	std::unique_ptr<sprite> spr;
	std::unique_ptr<blur> _blur;

	//frame_buffer
	std::unique_ptr<frame_buffer> f_buffer;
	std::unique_ptr<deffered> m_deffered;

	ConstantBufferForSSAO cbssao;

	struct _status
	{
		float wall_pos[3] = { -5.0f,0.0f,-15.0f };
		/*float player_pos[3] = { 0.0f,-5.0f,-0.5f };*/
		float player_pos_z = 64.0f;

		//float hemi_radius = 0.68f;
		float hemi_radius = 0.472f;
		float bias = -0.085f;
		float variance = 3.5f;
		int kernel_size = 38;
		bool flg = true;
	};
	void *tex1, *tex2, *tex3, *tex4;
	std::unique_ptr<_status> status;
public:
	scene45() {}
	virtual ~scene45() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;

	//ストリング
	char message[256] = { 0 };

	//シェーダー
	std::unique_ptr <shader_ex> fbx_m_shader;
	std::unique_ptr<shader> m_shader, _2d_shader, ssao_shader;
	//カメラ
	std::unique_ptr<main_camera> m_camera;
};