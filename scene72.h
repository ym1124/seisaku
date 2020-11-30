#pragma once
#include "scene.h"

class Texture;
class Texture_d;
class sprite;
class frame_buffer;
class shader;
class shader_ex;
class deffered;
class static_mesh;
class static_object;
class my_mesh;
class main_camera;
#include "shadow_map.h"
#include "tessellation_object.h"
#include "liner_depth.h"
#include "cube_mesh.h"
#include "environment_cubemap.h"
#include "gpu_particle.h"
#include "shader_ex.h"
#include "deffered.h"
#include "blur.h"
#include "animation.h"
#include "mesh.h"



class scene72 : public scene
{
private:
	u_int timer = 0;

	std::unique_ptr<Texture> ssr_tex[4], ref_tex, scene_tex, result_tex,specular_tex, specularmap_tex,pbr_tex;
	std::unique_ptr<Texture_d> depth_tex;
	std::unique_ptr<sprite> spr;
	std::unique_ptr<frame_buffer> f_buffer;
	std::unique_ptr<blur> m_blur;

	//シェーダー
	std::unique_ptr<shader> _2d_shader, ssr_shader, ssr_composite_shader, m_shader;
	std::unique_ptr<shader_ex> fbx_m_shader, pbr_fbx_shader;
	//
	std::unique_ptr<deffered> m_defferd;
	//
	std::shared_ptr<static_mesh> obj_mesh[2];
	std::unique_ptr<static_object> obj[2];
	std::unique_ptr<my_mesh> stage;
	//カメラ
	std::unique_ptr<main_camera> m_camera;

	struct _status
	{
		int view_type;
		int step_num = 100;
		float max_length = 20.0f;
		float max_thickness = 0.001f;
		float reflection_enhancer = 1.0f;
	};
	void *view[4], *view2;
	std::unique_ptr<_status> status;
public:
	scene72() {}
	virtual ~scene72() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;
};