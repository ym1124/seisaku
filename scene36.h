#pragma once
#include "scene.h"

class environment_cubemap;
class cube_mesh;
class shader;
class main_camera;
class f_buffer;

#include "geometric.h"//継承

class scene36 :public scene
{
private:
	static const int MAX_LIGHT = 4;
	int focus_flg = 0;
	float angle_x;
	float angle_y;
	std::unique_ptr<my_mesh> fbx;
	std::unique_ptr<animation> anim;
	std::unique_ptr<environment_cubemap> env_cubemap;
	std::unique_ptr<cube_mesh> skybox;
	std::unique_ptr<geo_sphere> spheres[16];
	//シェーダー
	std::unique_ptr<shader_ex>  pbr_fbx_shader, m_shader;
	//カメラ
	std::unique_ptr<main_camera> m_camera;
	//画面管理
	std::unique_ptr<frame_buffer> f_buffer;

	struct dLights
	{
		float dir[3] = { 0.0f,0.0f,-1.0f };
		float color[3] = { 1.0f,1.0f,1.0f };
		bool is_visible = false;
	};
	struct pLights
	{
		float pos[3] = { 0,0,0 };
		float color[3] = { 1,1,1 };
		float distance = 10000.0f;
		float decay = 0.0f;
		bool is_visible = false;
	};
	struct _status
	{
		float obj_angle[3] = { 0.0f,0.0f,0.0f };
		float metallic = 0.5f;
		float roughness = 0.5f;
		float albedo[3] = { 1.0f,1.0f,1.0f };
		float diffuse[3] = { 0.0f,0.0f,0.0f };
		float specular[3] = { 0.0f,0.0f,0.0f };
		dLights d_lights[MAX_LIGHT];
		pLights p_lights[MAX_LIGHT];
		bool auto_move = true;
		bool rotation_flg = false;
		bool tex_flg = true;
		bool focus_flg = false;
	};
	std::unique_ptr<_status> status;
public:
	scene36() {}
	virtual ~scene36() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;
};