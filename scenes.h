#pragma once

#include "scene16.h"
#include "scene36.h"
#include "scene45.h"
#include "scene59.h"
#include "scene60.h"
#include "scene61.h"
#include "scene62.h"
#include "scene66.h"
#include "scene71.h"
#include "scene72.h"
#include "scene73.h"
#include "scene74.h"
#include "scene75.h"


//#include <memory>
//#include  <string>
//#include "sprite.h"
//#include "camera.h"
//#include "static_mesh.h"
//#include "static_object.h"
//#include "collision.h"
//#include "texture.h"
//#include "bloom.h"
//#include "light_beam.h"
//#include "billboard.h"
//#include "hit_fx.h"
//#include "frame_buffer.h"
//#include "environment_cubemap.h"
//#include "ir_map.h"
//#include "cube_mesh.h"
//#include "g_buffer.h"
//#include "gpu_particle.h"
//#include "liner_depth.h"
//#include "deffered.h"
//#include "ssao.h"
//#include "shadow_map.h"
//#include "dissolve.h"
//#include "mesh.h"
//#include "animation.h"
//#include "ray.h"
//#include "sphere.h"
//#include "hit.h"
//#include "ray_material.h"
//#include "ray_light.h"
//
//#define SHADOW_SIZE_W 1280
//#define SHADOW_SIZE_H 720
//
//class environment_cubemap;
//class irmap;
//class deffered;
//class g_buffer;
//class ssao;

//class scene
//{
//public:
//	scene() {}
//	virtual ~scene() {}
//	virtual void Initialize() = 0;
//	virtual void Update(float elapsedTime) = 0;
//	virtual void Render(float elapsedTime) = 0;
//	DirectX::XMFLOAT4X4 LightViewProjection;
//
//private:
//	friend class scene_load;
//	friend class scene_manager;
//	bool initialized = false;//初期化完了フラグ
//};


//class scene_main :public scene
//{
//private:
//	std::unique_ptr<frame_buffer> f_buffer;
//	std::unique_ptr<main_camera> m_camera,l_camera;
//
//	//オブジェクト
//	std::shared_ptr<static_mesh> obj_mesh;
//	std::unique_ptr<static_object> obj,obj2;
//	std::shared_ptr<skin_mesh> s_mesh;
//	std::unique_ptr<skin_object> obj_s;
//	std::shared_ptr<static_mesh> ground_mesh;
//	std::shared_ptr<static_object> ground;
//
//	//シェーダー
//	std::unique_ptr<shader> m_shader;
//	std::unique_ptr<shader> s_shader;
//	std::unique_ptr<shader> fog_shader;
//	std::unique_ptr<shader> fog_shader_s;
//	std::unique_ptr<shader> shadow_shader;
//	std::unique_ptr<shader> shadow_shader_s;
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> brightness_shader;
//	std::unique_ptr<shader> blur_shader;
//	std::unique_ptr<shader> custom_fog_shadow_shader;
//	std::unique_ptr<shader> custom_fog_shadow_shader_s;
//	std::unique_ptr<shader> waver_shader;
//
//
//	std::unique_ptr<Texture> tex;
//	std::unique_ptr<Texture> waver_tex;
//	std::unique_ptr<Texture> tex_render;
//	std::unique_ptr<Texture> tex_brightness;
//	std::unique_ptr<Texture> tex_blur;
//	std::unique_ptr<Texture_d> tex_d;
//
//
//	float blendalpha = 0.5f;
//
//	//スプライト
//	std::unique_ptr<sprite> font;
//	std::unique_ptr<sprite> spr;
//	std::unique_ptr<sprite> l_cam_view;
//	std::unique_ptr<sprite> l_cam_view_depth;
//	std::unique_ptr<sprite> brightness_spr,blur_spr;
//	std::unique_ptr<sprite> back_spr;
//	//ストリング
//	char message[256] = { 0 };
//
//public:
//	scene_main() {}
//	virtual ~scene_main() {};
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//};
//
//
//
//class scene_title :public scene
//{
//private:
//	std::unique_ptr<sprite> img;
//public:
//	scene_title() {}
//	virtual ~scene_title() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//オブジェクト
//	std::shared_ptr<static_mesh> obj_mesh;
//	std::unique_ptr<static_object> obj;
//
//	std::shared_ptr<static_mesh> field_mesh;
//	std::shared_ptr<static_object> field;
//
//	//フォント
//	std::unique_ptr<sprite> font;
//
//	//シェーダー
//	std::unique_ptr<shader> toon_shader = nullptr;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//
//	//ストリング
//	char message[256] = { 0 };
//};
//
//class scene_clear :public scene
//{
//private:
//	std::unique_ptr<frame_buffer> f_buffer;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//
//	//オブジェクト
//	std::shared_ptr<static_mesh> stage_mesh;
//	std::unique_ptr<static_object> stage;
//	std::shared_ptr<static_mesh> obj_mesh;
//	std::unique_ptr<static_object> obj;
//
//	std::unique_ptr<sprite> spr = nullptr;
//	std::unique_ptr<sprite> normal_spr = nullptr;
//	std::unique_ptr<sprite> depth_spr = nullptr;
//	std::unique_ptr<sprite> edge_spr = nullptr;
//
//	//シェーダー
//	std::unique_ptr<shader> m_shader;
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> normal_shader;
//	std::unique_ptr<shader> edge_shader;
//
//	//テクスチャ
//	std::unique_ptr<Texture> normal_tex = nullptr;
//	std::unique_ptr<Texture> tex = nullptr;
//	std::unique_ptr<Texture_d> depth_tex = nullptr;
//	std::unique_ptr<Texture> edge_tex = nullptr;
//
//	float blendalpha = 0.5f;
//public:
//	scene_clear() {}
//	virtual ~scene_clear() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//};
//
//class scene_over :public scene
//{
//private:
//	std::unique_ptr<frame_buffer> f_buffer;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//
//	//オブジェクト
//	std::shared_ptr<static_mesh> obj_mesh;
//	std::unique_ptr<static_object> obj;
//
//	std::unique_ptr<sprite> depth_sprite;
//
//	//テクスチャ
//	std::unique_ptr<Texture> waver_tex;
//	
//	//spr
//	std::unique_ptr<sprite> spr;
//
//	//シェーダー
//	std::unique_ptr<shader> m_shader;
//	std::unique_ptr<shader> s_shader;
//	std::unique_ptr<shader> shadow_shader;
//	std::unique_ptr<shader> waver_shader;
//
//	float blendalpha = 0.5f;
//
//public:
//	scene_over() {}
//	virtual ~scene_over() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//};
//
//class scene1 :public scene
//{
//private:
//	//オブジェクト
//	std::shared_ptr<static_mesh> obj_mesh[3];
//	std::unique_ptr<static_object> obj[3];
//
//	std::shared_ptr<skin_mesh> fbx_mesh;
//	std::unique_ptr<skin_object> fbx;
//
//	std::shared_ptr<skin_mesh> fbx_mesh2;
//	std::shared_ptr<skin_object> fbx2;
//
//	std::shared_ptr<skin_mesh> ground_mesh;
//	std::shared_ptr<skin_object>ground[2];
//	//シェーダー
//	std::unique_ptr<shader> m_shader = nullptr;
//	std::unique_ptr<shader> s_shader = nullptr;
//
//	std::unique_ptr<Texture> normalMap = nullptr;
//
//	//スプライト
//	std::unique_ptr<sprite> font;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	float blendalpha = 0.5f;
//
//	struct status
//	{
//		float eye_pos[3] = { 0.0f,100.0f,-100.0f };
//		int timer_update = 60;
//		float range = 100.0f;
//		float outer_corn = 0.99f;
//		float inner_corn = 0.9f;
//		bool reset = false;
//	};
//
//	std::unique_ptr<status> _status;
//
//public:
//	scene1() {}
//	virtual ~scene1() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//};
//
//class scene2 :public scene
//{
//private:
//	//カメラ
//	std::unique_ptr<main_camera> m_camera,l_camera;
//
//	//オブジェクト
//	std::shared_ptr<static_mesh> obj_mesh;
//	std::shared_ptr<static_object> obj;
//	std::shared_ptr<static_mesh> ground_mesh;
//	std::shared_ptr<static_object> ground,back;
//
//	std::unique_ptr<sprite> spr,depth_sprite;
//
//	//テクスチャ
//	std::unique_ptr<Texture> tex;
//	std::unique_ptr<Texture_d> depth_tex;
//
//	//シェーダー
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> phong_shader;
//	std::unique_ptr<shader> shadow_shader;
//
//	//frame_buffer
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	float blendalpha = 0.5f;
//
//	struct status
//	{
//		float obj_pos[3] = { 0.0f, 0.0f, 10.0f };
//		float dirlight_pos[3] = { 0.0f, 0.6f, -0.6f };
//		float m_cam_pos[3] = { 0.0f,10.0f,-1.0f };
//		float shadow_color[3] = { 0.5f,0.5f,0.5f };
//	};
//
//	std::unique_ptr<status> _status;
//
//public:
//	scene2() {}
//	virtual ~scene2() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//};
//
//class scene3 :public scene
//{
//private:
//	std::unique_ptr<sprite> img;
//public:
//	scene3() {}
//	virtual ~scene3() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//オブジェクト
//	std::shared_ptr<static_mesh> obj_mesh;
//	std::unique_ptr<static_object> obj;
//
//	//スプライト
//	std::unique_ptr<sprite> font;
//
//	//エフェクトマネージャー
//	std::unique_ptr<hit_fx_manager> hfm;
//
//	//ビルボード
//	std::shared_ptr<billboard> fx;
//	std::unique_ptr<billboard_obj> fx_obj;
//
//	//エフェクト
//	std::shared_ptr<hit_fx> hit_fx_inst;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//
//	//ストリング
//	char message[256] = { 0 };
//
//};
//
//class scene4 :public scene
//{
//private:
//	std::unique_ptr<sprite> img;
//public:
//	scene4() {}
//	virtual ~scene4() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//オブジェクト
//	std::shared_ptr<static_mesh> obj_mesh,plane_mesh;
//	std::unique_ptr<static_object> obj, obj2,plane;
//
//	std::unique_ptr<Texture> tex,tex2, tex3;
//	std::unique_ptr<Texture_d> tex_d;
//	std::unique_ptr<shader> model_shader;
//	std::unique_ptr<shader> contour_shader;//輪郭
//	std::unique_ptr<shader> normal_shader;
//	std::unique_ptr<shader> depth_shader;
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> edge_shader;
//	std::unique_ptr<shader> gray_shader;
//	std::unique_ptr<sprite> sp1, sp2, sp3;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera,l_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene5 :public scene
//{
//
//	struct _status
//	{
//		float variance = 1.0f;
//		bool bloom_flg = true;
//		float threshold = 0.1f;
//	};
//
//	std::unique_ptr<_status> status;
//
//public:
//	scene5() {}
//	virtual ~scene5() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	float weight_array[8];
//
//	//spr
//	std::unique_ptr<sprite> font;
//	//ストリング
//	char message[256] = { 0 };
//
//	//オブジェクト
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::shared_ptr<static_object>  obj;
//	std::unique_ptr<shader> m_shader, _2d_shader,bright_shader, compute_shader, blur_shader_vertical, blur_shader_horizontal;
//	std::unique_ptr<gpu_particle> gp;
//	std::unique_ptr<Texture> render_tex,bright_tex,blur_tex;
//	std::unique_ptr<sprite> spr;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene6 :public scene
//{
//public:
//	scene6() {}
//	virtual ~scene6() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//apr
//	std::unique_ptr<sprite> font;
//	//ストリング
//	char message[256] = { 0 };
//
//	//オブジェクト
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::shared_ptr<static_object>  obj;
//	std::unique_ptr<shader> m_shader, compute_shader;
//	std::unique_ptr<gpu_particle> gp;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene7 :public scene
//{
//private:
//	std::shared_ptr<static_mesh> ground_mesh,obj_mesh;
//	std::shared_ptr<static_object> ground,obj;
//public:
//	scene7() {}
//	virtual ~scene7() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//オブジェクト
//	std::unique_ptr<Texture> hight_tex, normal_tex,normal_write_tex,sea_tex,sea_gray_tex;
//	std::unique_ptr<Texture_d> depth_tex;
//	std::unique_ptr<sprite> hight_spr,normal_spr;
//	std::unique_ptr<shader> m_shader;
//	std::unique_ptr<shader> normal_shader;
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> normal_map_shader;
//	std::unique_ptr<shader> gray_scale_shader;
//	std::unique_ptr<shader> wave_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene8 :public scene
//{
//private:
//	std::shared_ptr<static_mesh> ground_mesh;
//	std::shared_ptr<static_object> ground;
//public:
//	scene8() {}
//	virtual ~scene8() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//オブジェクト
//	std::unique_ptr<Texture> hight_tex, normal_tex, wave_tex;
//	std::unique_ptr<sprite> hight_spr, normal_spr,wave_spr;
//	std::unique_ptr<shader> m_shader;
//	std::unique_ptr<shader> normal_shader;
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> normal_map_shader;
//	std::unique_ptr<shader> wave_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene9 :public scene
//{
//private:
//	std::shared_ptr<static_mesh> ground_mesh, obj_mesh;
//	std::shared_ptr<static_object> ground, obj;
//public:
//	scene9() {}
//	virtual ~scene9() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//オブジェクト
//	std::unique_ptr<Texture> hight_tex, normal_tex, normal_write_tex, sea_tex, sea_gray_tex,water_camera_tex;
//	std::unique_ptr<Texture_d> depth_tex;
//	std::unique_ptr<sprite> hight_spr, normal_spr,water_camera_spr;
//	std::unique_ptr<shader> m_shader;
//	std::unique_ptr<shader> normal_shader;
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> normal_map_shader;
//	std::unique_ptr<shader> gray_scale_shader;
//	std::unique_ptr<shader> wave_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera,water_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene10 :public scene
//{
//private:
//	std::shared_ptr<static_mesh> ground_mesh, obj_mesh;
//	std::shared_ptr<static_object> ground, obj;
//public:
//	scene10() {}
//	virtual ~scene10() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//オブジェクト
//	std::unique_ptr<Texture> hight_tex, normal_tex, normal_write_tex, sea_tex, sea_gray_tex, water_camera_tex;
//	std::unique_ptr<Texture_d> depth_tex;
//	std::unique_ptr<sprite> hight_spr, normal_spr, water_camera_spr;
//	std::unique_ptr<shader> m_shader;
//	std::unique_ptr<shader> normal_shader;
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> normal_map_shader;
//	std::unique_ptr<shader> gray_scale_shader;
//	std::unique_ptr<shader> wave_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera, water_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	struct wave_status
//	{
//		float timer;
//		float wave_power = 15.0f;
//	};
//	std::unique_ptr<wave_status> w_status;
//};
//
//class scene11 :public scene
//{
//private:
//	std::shared_ptr<static_mesh> ground_mesh, obj_mesh;
//	std::shared_ptr<static_object> ground, obj;
//public:
//	scene11() {}
//	virtual ~scene11() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//apr
//	std::unique_ptr<sprite> font;
//	//ストリング
//	char message[256] = { 0 };
//
//	//オブジェクト
//	std::unique_ptr<Texture> hight_tex, normal_tex, normal_write_tex, sea_tex, sea_gray_tex, water_camera_tex;
//	std::unique_ptr<Texture_d> depth_tex;
//	std::unique_ptr<sprite> hight_spr, normal_spr, water_camera_spr;
//	std::unique_ptr<shader> m_shader;
//	std::unique_ptr<shader> normal_shader;
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> normal_map_shader;
//	std::unique_ptr<shader> gray_scale_shader;
//	std::unique_ptr<shader> wave_shader;
//	std::unique_ptr<shader> fog_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera, water_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene12 :public scene
//{
//private:
//	std::shared_ptr<static_mesh> ground_mesh, obj_mesh;
//	std::shared_ptr<static_object> ground, obj;
//public:
//	scene12() {}
//	virtual ~scene12() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//apr
//	std::unique_ptr<sprite> font;
//	//ストリング
//	char message[256] = { 0 };
//
//	//オブジェクト
//	std::unique_ptr<Texture> diffuse_tex, normal_tex, normal_write_tex, alpha_tex, render_tex;
//	std::unique_ptr<sprite> hight_spr, normal_spr, alpha_spr;
//	std::unique_ptr<shader> m_shader, normal_shader, _2d_shader, alpha_shader, alpha_2d_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene13 :public scene
//{
//private:
//	std::unique_ptr<sprite> img;
//public:
//	scene13() {}
//	virtual ~scene13() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//オブジェクト
//	std::unique_ptr<Texture> tex, tex2,bright_tex;
//
//	std::unique_ptr<shader> bloom_shader;
//	std::unique_ptr<shader> _2d_shader,blur_shader,bright_shader;
//
//	std::unique_ptr<sprite> tmp_spr;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene14 :public scene
//{
//private:
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::shared_ptr<static_object>  obj;
//public:
//	scene14() {}
//	virtual ~scene14() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//apr
//	std::unique_ptr<sprite> font;
//	//ストリング
//	char message[256] = { 0 };
//
//	//オブジェクト
//	std::unique_ptr<Texture> diffuse2_tex;
//	std::unique_ptr<shader> m_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene15 :public scene
//{
//
//	static const UINT NUM_ELEMENTS = 1024;
//public:
//	struct BufType
//	{
//		int     s32;
//		float   f32;
//	};
//
//	BufType g_Buf0[NUM_ELEMENTS];
//	BufType g_Buf1[NUM_ELEMENTS];
//
//	BufType* pBufType=nullptr;
//	
//	ID3D11Buffer*           g_pBuf0 = nullptr;//後でdeleteする
//	ID3D11Buffer*           g_pBuf1 = nullptr;
//	ID3D11Buffer*           g_pBufResult = nullptr;
//
//	ID3D11ShaderResourceView*   g_pBufSRV0 = nullptr;
//	ID3D11ShaderResourceView*   g_pBufSRV1 = nullptr;
//	ID3D11UnorderedAccessView*  g_pBufUAV = nullptr;
//
//public:
//	scene15() {}
//	virtual ~scene15() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//apr
//	std::unique_ptr<sprite> font;
//	//ストリング
//	char message[256] = { 0 };
//
//	//オブジェクト
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::shared_ptr<static_object>  obj;
//	std::unique_ptr<shader> m_shader,compute_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene16 :public scene
//{
//
//	static const UINT NUM_ELEMENTS = 4096;
//	bool type = false;
//public:
//	struct ParticleData
//	{
//		DirectX::XMFLOAT3 pos;
//		DirectX::XMFLOAT3 velocity;
//	};
//
//	ParticleData Buf[NUM_ELEMENTS];
//
//	ParticleData* pData = nullptr;
//
//	ID3D11Buffer*           g_pBuf = nullptr;//後でdeleteする
//	ID3D11Buffer*           g_pBufResult = nullptr;
//
//	ID3D11ShaderResourceView*   g_pBufSRV = nullptr;
//	ID3D11UnorderedAccessView*  g_pBufUAV = nullptr;
//
//public:
//	scene16() {}
//	virtual ~scene16() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//シェーダー
//	std::unique_ptr<shader> m_shader, compute_shader;
//
//	//spr
//	std::unique_ptr<sprite> font;
//	//ストリング
//	char message[256] = { 0 };
//
//	//オブジェクト
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::shared_ptr<static_object>  obj[NUM_ELEMENTS];
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene17 : public scene
//{
//bool type = false;
//public:
//	struct ParticleData
//	{
//		DirectX::XMFLOAT3 pos;
//		DirectX::XMFLOAT3 velocity;
//	};
//
//	ID3D11Buffer*           g_pBuf = nullptr;//後でdeleteする
//	ID3D11Buffer*           g_pBufResult = nullptr;
//
//	ID3D11ShaderResourceView*   g_pBufSRV = nullptr;
//	ID3D11UnorderedAccessView*  g_pBufUAV = nullptr;
//
//public:
//	scene17() {}
//	virtual ~scene17() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//シェーダー
//	std::unique_ptr<shader> compute_shader,geo_shader,particle_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//};
//
//class scene18 : public scene
//{
//	bool type = false;
//public:
//	struct ParticleData
//	{
//		DirectX::XMFLOAT3 pos;
//		DirectX::XMFLOAT3 velocity;
//	};
//
//	ID3D11Buffer*           g_pBuf = nullptr;//後でdeleteする
//	ID3D11Buffer*           g_pBufResult = nullptr;
//
//	ID3D11ShaderResourceView*   g_pBufSRV = nullptr;
//	ID3D11UnorderedAccessView*  g_pBufUAV = nullptr;
//
//public:
//	scene18() {}
//	virtual ~scene18() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//シェーダー
//	std::unique_ptr<shader> m_shader, compute_shader, geo_shader, particle_shader;
//
//	//apr
//	std::unique_ptr<sprite> font;
//	//ストリング
//	char message[256] = { 0 };
//
//	//オブジェクト
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::shared_ptr<static_object>  obj;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene19 : public scene
//{
//	bool type = false;
//public:
//	struct ParticleData
//	{
//		DirectX::XMFLOAT3 pos;
//		DirectX::XMFLOAT3 velocity;
//	};
//
//	ID3D11Buffer*           g_pBuf = nullptr;//後でdeleteする
//	ID3D11Buffer*           g_pBufResult = nullptr;
//
//	ID3D11ShaderResourceView*   g_pBufSRV = nullptr;
//	ID3D11UnorderedAccessView*  g_pBufUAV = nullptr;
//
//public:
//	scene19() {}
//	virtual ~scene19() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//シェーダー
//	std::unique_ptr<shader> compute_shader, geo_shader, particle_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//};
//
//class scene20 : public scene
//{
//	static const UINT NUM_ELEMENTS = 4096;
//	bool type = false;
//public:
//	struct ParticleData
//	{
//		DirectX::XMFLOAT3 pos;
//		DirectX::XMFLOAT3 velocity;
//	};
//
//	struct Vertex1 {
//		DirectX::XMFLOAT3 pos;
//	};
//
//	Vertex1 g_VertexList[NUM_ELEMENTS];
//
//	ParticleData Buf[NUM_ELEMENTS];
//
//	ParticleData* pData = nullptr;
//
//	ID3D11Buffer*           g_pBuf = nullptr;//後でdeleteする
//	ID3D11Buffer*           g_pBufResult = nullptr;
//
//	ID3D11ShaderResourceView*   g_pBufSRV = nullptr;
//	ID3D11UnorderedAccessView*  g_pBufUAV = nullptr;
//
//public:
//	scene20() {}
//	virtual ~scene20() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//シェーダー
//	std::unique_ptr<shader> compute_shader, geo_shader, particle_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//};
//
//class scene21 :public scene
//{
//private:
//	std::shared_ptr<static_mesh> ground_mesh, obj_mesh;
//	std::unique_ptr<static_object> ground, obj;
//	std::shared_ptr<skin_mesh> fbx_mesh;
//	std::unique_ptr<skin_object> fbx;
//
//public:
//	scene21() {}
//	virtual ~scene21() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//モーションブラー用構造体
//	MotionBlurMatrix mbm,mbm_s;
//
//	//spr
//	std::unique_ptr<sprite> font;
//	//ストリング
//	char message[256] = { 0 };
//
//	//オブジェクト
//	std::unique_ptr<Texture> velocity_tex,render_tex,result_tex;
//	std::unique_ptr<sprite> velocity_spr,render_spr,result_spr;
//	std::unique_ptr<shader> m_shader, velocity_shader,_2d_shader,motion_blur_shader, velocity_s_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	struct _status
//	{
//		float power = 50.0f;
//		float color[3] = { 1.0f,1.0f,1.0f };
//		float eye[3] = { 0.0f, 24.0f, 30.0f };
//		bool moveAutoFlg = true;
//	};
//
//	std::unique_ptr<_status> status;
//};
//
//class scene22 :public scene
//{
//private:
//	std::shared_ptr<static_mesh> ground_mesh, obj_mesh;
//	std::shared_ptr<static_object> ground, obj;
//public:
//	scene22() {}
//	virtual ~scene22() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//モーションブラー用構造体
//	MotionBlurMatrix mbm;
//
//	//spr
//	std::unique_ptr<sprite> font;
//	//ストリング
//	char message[256] = { 0 };
//
//	//オブジェクト
//	std::unique_ptr<Texture> render_tex, monochorome_tex,result_tex;
//	std::unique_ptr<sprite> render_spr, result_spr,spr;
//	std::unique_ptr<shader> m_shader,  _2d_shader,monochorome_shader,laplacian_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene23 :public scene
//{
//private:
//	std::shared_ptr<static_mesh> ground_mesh, obj_mesh;
//	std::shared_ptr<static_object> ground, obj;
//public:
//	scene23() {}
//	virtual ~scene23() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//モーションブラー用構造体
//	MotionBlurMatrix mbm;
//
//	//spr
//	std::unique_ptr<sprite> font;
//	//ストリング
//	char message[256] = { 0 };
//
//	//オブジェクト
//	std::unique_ptr<Texture> render_tex, monochorome_tex, result_tex;
//	std::unique_ptr<sprite> render_spr, result_spr, spr;
//	std::unique_ptr<shader> m_shader, _2d_shader, monochorome_shader, laplacian_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene24 :public scene
//{
//private:
//	std::shared_ptr<static_mesh> ground_mesh, obj_mesh;
//	std::shared_ptr<static_object> ground[2], obj;
//	struct status
//	{
//		float obj_pos[3] = { 0,0,0 };
//	};
//
//	std::unique_ptr<status> _status;
//public:
//	scene24() {}
//	virtual ~scene24() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//spr
//	std::unique_ptr<sprite> font;
//	//ストリング
//	char message[256] = { 0 };
//
//	//オブジェクト
//	std::unique_ptr<Texture> render_tex;
//	std::unique_ptr<sprite> render_spr;
//	std::unique_ptr<shader> m_shader, _2d_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene25 :public scene
//{
//	static const int FUR_MAX = 800;
//private:
//	std::shared_ptr<static_mesh> ground_mesh, obj_mesh;
//	std::shared_ptr<static_object> ground[FUR_MAX], obj;
//public:
//	scene25() {}
//	virtual ~scene25() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//spr
//	std::unique_ptr<sprite> fur_map_spr;
//	//ストリング
//	char message[256] = { 0 };
//
//	//オブジェクト
//	std::unique_ptr<Texture> render_tex,fur_tex,fur_map_tex;
//	std::unique_ptr<sprite> render_spr,fur_spr;
//	std::unique_ptr<shader> m_shader, _2d_shader,fur_shader, fur_map_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	struct _status
//	{
//		int fur_num = 100.0f;
//		float fur_dist = 0.1f;
//	};
//	std::unique_ptr<_status> status;
//};
//
//class scene26 :public scene
//{
//private:
//	std::shared_ptr<static_mesh> ground_mesh, obj_mesh;
//	std::shared_ptr<static_object> ground, obj;
//public:
//	scene26() {}
//	virtual ~scene26() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//オブジェクト
//	std::unique_ptr<Texture> render_tex,draw_tex;
//	std::unique_ptr<sprite> render_spr, draw_spr;
//	std::unique_ptr<shader> m_shader, _2d_shader, draw_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	struct _status
//	{
//		float obj_pos[3] = { 0.0f, 0.0f, 250.0f };
//	};
//
//	std::unique_ptr<_status> status;
//};
//
//class scene27 :public scene
//{
//private:
//	std::shared_ptr<static_mesh> ground_mesh, obj_mesh;
//	std::shared_ptr<static_object> ground, obj;
//	struct _status
//	{
//		int mode = 0;
//		char str[4][13] = { "lambert","half lambert","phong","no lighting" };
//		float dir_light_vec[3] = { 0.6f,0.6f,0.6f };
//		float mouse_pos[3] = { 0,0,0 };
//		bool model_rotate = false;
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene27() {}
//	virtual ~scene27() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene28 :public scene
//{
//private:
//	std::shared_ptr<static_mesh> obj2_mesh[2], obj_mesh[2];
//	std::shared_ptr<static_object> obj2[2], obj[2];
//	struct _status
//	{
//		float dir_light_vec[3] = { 0.1f,0.0f,0.6f };
//		float obj_angle[3] = { 0.0f,0.0f,0.0f };
//		int obj_type = 0;
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene28() {}
//	virtual ~scene28() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader>  toon_shader,m_shader;
//	//テクスチャ
//	std::shared_ptr<Texture> toon_map;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene29 :public scene
//{
//private:
//	//カメラ
//	std::unique_ptr<main_camera> m_camera, l_camera;
//
//	//シャドウマップ
//	std::unique_ptr<shadow_map> m_shadow_map;
//
//	//オブジェクト
//	std::shared_ptr<static_mesh> obj_mesh;
//	std::shared_ptr<static_object> obj;
//	std::shared_ptr<static_mesh> ground_mesh;
//	std::shared_ptr<static_object> ground, back;
//
//	//スプライト
//	std::unique_ptr<sprite> spr;
//
//	////テクスチャ
//	//std::unique_ptr<Texture> tex,depth_blur_tex;
//	//std::unique_ptr<Texture_d> depth_tex;
//
//	//シェーダー
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> phong_shader;
//	//std::unique_ptr<shader> shadow_shader;
//	//std::unique_ptr<shader> depth_map_shader;
//	//frame_buffer
//	std::unique_ptr<frame_buffer> f_buffer;
//	//blur
//	//std::unique_ptr<blur> m_blur;
//
//	float blendalpha = 0.5f;
//
//	struct status
//	{
//		float obj_pos[3] = { 0.0f, 0.0f, 30.0f };
//		float dirlight_pos[3] = { 0.0f, -0.6f, -0.6f };
//		float m_cam_pos[3] = { 0.0f,10.0f,-1.0f };
//		float shadow_color[4] = { 0.5f,0.5f,0.5f,1.0f };
//		float variance_pow = 4.4f;
//
//		float mouse_pos[3] = { 0,0,0 };
//		bool raypick_flg = true;
//		bool rotation_flg = false;
//	};
//
//	std::unique_ptr<status> _status;
//
//public:
//	scene29() {}
//	virtual ~scene29() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//};
//
//class scene30 :public scene
//{
//private:
//	std::unique_ptr<sprite> img;
//
//	struct _status
//	{
//		int down_sampling_num = 1;
//		int tex_num = 0;
//		float variance = 10.0f;
//	};
//	std::unique_ptr<_status> status;
//
//	const static int TEXES_NUM = 10;
//	float weight_array[8];
//public:
//	scene30() {}
//	virtual ~scene30() { }
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//オブジェクト
//	std::shared_ptr<static_mesh> obj_mesh;
//	std::unique_ptr<static_object> obj, obj2;
//	std::shared_ptr<static_mesh> plane_mesh;
//	std::unique_ptr<static_object> plane;
//
//	std::unique_ptr<Texture> texes[TEXES_NUM];
//	std::unique_ptr<Texture> temp[TEXES_NUM];
//	std::unique_ptr<Texture> tex;
//	std::unique_ptr<Texture> tex2;
//	std::unique_ptr<Texture> test_tex[3];
//	std::unique_ptr<shader> model_shader;
//	std::unique_ptr<shader> blur_shader[2];
//	std::unique_ptr<shader> bloom_shader;
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> _2dba0_shader;
//	std::unique_ptr<sprite> sp1, sp2, sp3;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene31 :public scene
//{
//private:
//	std::unique_ptr<sprite> img;
//	float weight_array[8];
//
//	struct _status
//	{
//		float variance = 10.0f;
//		float dir_light_vec[3] = { 0,0,0 };
//		bool bloom_flg = true;
//		float threshold = 0.6f;
//
//		float model_angle[3] = { 0,0,0 };
//		bool rotation_flg = false;
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene31() {}
//	virtual ~scene31() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//オブジェクト
//	std::shared_ptr<static_mesh> obj_mesh;
//	std::unique_ptr<static_object> obj, obj2;
//
//	std::unique_ptr<Texture> tex;
//	std::unique_ptr<Texture> tex2;
//	std::unique_ptr<Texture> bright_tex;
//	std::unique_ptr<shader> model_shader;
//	std::unique_ptr<shader> blur_shader_vertical;
//	std::unique_ptr<shader> blur_shader_horizontal;
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> bright_shader;
//	std::unique_ptr<sprite> sp1, sp2, sp3;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	//bloom
//	std::unique_ptr<bloom> _bloom;
//};
//
//class scene32 :public scene
//{
//private:
//	std::unique_ptr<sprite> img;
//	float weight_array[8];
//
//	struct _status
//	{
//		bool fx_flg = true;
//		float threshold = 0.6f;
//		float model_angle[3] = { 0,0,0 };
//		bool rotation_flg = false;
//	};
//
//	std::unique_ptr<_status> status;
//public:
//	scene32() {}
//	virtual ~scene32() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//オブジェクト
//	std::shared_ptr<static_mesh> obj_mesh;
//	std::unique_ptr<static_object> obj, obj2;
//	std::shared_ptr<static_mesh> plane_mesh;
//	std::unique_ptr<static_object> plane;
//
//	std::unique_ptr<Texture> tex;
//	std::unique_ptr<Texture> tex2;
//	std::unique_ptr<Texture> irradiation_tex[4];
//	std::unique_ptr<Texture> tex_buffer[3];
//	std::unique_ptr<Texture> bright_tex;
//	std::unique_ptr<shader> model_shader;
//	std::unique_ptr<shader> irradiation_shader;
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> copy_shader;
//	std::unique_ptr<shader> bright_shader;
//	std::unique_ptr<sprite> spr;
//
//	//光芒
//	std::unique_ptr<light_beam> m_light_beam;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//
//class scene33 :public scene
//{
//private:
//	std::unique_ptr<sprite> img;
//	float weight_array[8];
//
//	struct _status
//	{
//		float variance = 10.0f;
//		bool bloom_flg = true;
//		float threshold = 0.01f;
//	};
//
//	std::unique_ptr<_status> status;
//public:
//	scene33() {}
//	virtual ~scene33() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//オブジェクト
//	std::shared_ptr<static_mesh> obj_mesh;
//	std::unique_ptr<static_object> obj, obj2;
//	std::shared_ptr<static_mesh> plane_mesh;
//	std::unique_ptr<static_object> plane;
//
//	std::unique_ptr<Texture> tex;
//	std::unique_ptr<Texture> tex2;
//	std::unique_ptr<Texture> bright_tex;
//	std::unique_ptr<shader> model_shader;
//	std::unique_ptr<shader> blur_shader_vertical;
//	std::unique_ptr<shader> blur_shader_horizontal;
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> bright_shader;
//	std::unique_ptr<sprite> sp1, sp2, sp3;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene34 :public scene
//{
//private:
//	std::unique_ptr<sprite> img;
//	float weight_array[8];
//
//	struct _status
//	{
//		float variance = 10.0f;
//		bool bloom_flg = true;
//		float threshold = 0.01f;
//
//		int tone_type = 0;
//		float exposure = 0;
//		float shoulder_strength = 0.15f;
//		float linear_strength = 0.5f;
//		float linear_angle = 0.1f;
//		float toe_strength = 0.2f;
//		float toe_numerator = 0.02f;
//		float toe_denominator = 0.3f;
//		float linear_white = 11.2f;
//		bool isGamma = false;
//
//		int tex_type = 0;
//		float mouse_pos[3];
//		bool sRGB_flg = false;
//	};
//
//	std::unique_ptr<_status> status;
//public:
//	scene34() {}
//	virtual ~scene34() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//オブジェクト
//	std::shared_ptr<static_mesh> obj_mesh;
//	std::unique_ptr<static_object> obj, obj2;
//	std::shared_ptr<static_mesh> plane_mesh;
//	std::unique_ptr<static_object> plane;
//
//	std::unique_ptr<Texture> tex[5];
//	std::unique_ptr<Texture> original_tex[5];
//	std::unique_ptr<Texture> tex2;
//	std::unique_ptr<Texture> bright_tex;
//	std::unique_ptr<Texture> result_tex;
//	std::unique_ptr<shader> model_shader;
//	std::unique_ptr<shader> blur_shader_vertical;
//	std::unique_ptr<shader> blur_shader_horizontal;
//	std::unique_ptr<shader> _2d_shader;
//	std::unique_ptr<shader> tone_map_shader;
//	std::unique_ptr<shader> linear_to_srgb_shader;
//	std::unique_ptr<shader> bright_shader;
//	std::unique_ptr<sprite> sp1, sp2, sp3;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene35 :public scene
//{
//private:
//	static const int MAX_LIGHT = 4;
//	std::shared_ptr<static_mesh> obj2_mesh, obj_mesh;
//	std::shared_ptr<static_object> obj2, obj;
//	struct dLights
//	{
//		float dir[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		bool is_visible = false;
//	};
//	struct pLights
//	{
//		float pos[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		float distance = 10000.0f;
//		float decay = 0.0f;
//		bool is_visible = false;
//	};
//	struct _status
//	{
//		float obj_angle[3] = { 0.0f,0.0f,0.0f };
//		float metallic=0.5f;
//		float roughness=0.5f;
//		float albedo[3] = { 1.0f,1.0f,1.0f };
//		float diffuse[3] = { 0.0f,0.0f,0.0f };
//		float specular[3] = { 0.0f,0.0f,0.0f };
//		dLights d_lights[MAX_LIGHT];
//		pLights p_lights[MAX_LIGHT];
//		bool auto_move = true;
//		bool rotation_flg = false;
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene35() {}
//	virtual ~scene35() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader>  pbr_shader, m_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene36 :public scene
//{
//private:
//	static const int MAX_LIGHT = 4;
//	std::shared_ptr<skin_mesh> fbx_mesh;
//	std::unique_ptr<skin_object> fbx;
//	std::unique_ptr<environment_cubemap> env_cubemap;
//	std::unique_ptr<cube_mesh> skybox;
//
//	struct dLights
//	{
//		float dir[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		bool is_visible = false;
//	};
//	struct pLights
//	{
//		float pos[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		float distance = 10000.0f;
//		float decay = 0.0f;
//		bool is_visible = false;
//	};
//	struct _status
//	{
//		float obj_angle[3] = { 90.0f,0.0f,0.0f };
//		float metallic = 0.5f;
//		float roughness = 0.5f;
//		float albedo[3] = { 1.0f,1.0f,1.0f };
//		float diffuse[3] = { 0.0f,0.0f,0.0f };
//		float specular[3] = { 0.0f,0.0f,0.0f };
//		dLights d_lights[MAX_LIGHT];
//		pLights p_lights[MAX_LIGHT];
//		bool auto_move = true;
//		bool rotation_flg = false;
//		bool tex_flg = true;
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene36() {}
//	virtual ~scene36() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader>  pbr_fbx_shader, m_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene37:public scene
//{
//private:
//	static const int MAX_LIGHT = 4;
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::shared_ptr<static_object>  obj;
//	std::shared_ptr<skin_mesh> fbx_mesh;
//	std::unique_ptr<skin_object> fbx;
//	std::unique_ptr<environment_cubemap> env_cubemap;
//	std::unique_ptr<cube_mesh> skybox;
//
//	struct dLights
//	{
//		float dir[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		bool is_visible = false;
//	};
//	struct pLights
//	{
//		float pos[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		float distance = 10000.0f;
//		float decay = 0.0f;
//		bool is_visible = false;
//	};
//	struct _status
//	{
//		float obj_angle[3] = { 0.0f,0.0f,0.0f };
//		float metallic = 0.5f;
//		float roughness = 0.5f;
//		float albedo[3] = { 1.0f,1.0f,1.0f };
//		float diffuse[3] = { 0.0f,0.0f,0.0f };
//		float specular[3] = { 0.0f,0.0f,0.0f };
//		dLights d_lights[MAX_LIGHT];
//		pLights p_lights[MAX_LIGHT];
//		bool auto_move = true;
//		bool rotation_flg = false;
//		bool tex_flg = true;
//		int diffuse_brdf_type;
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene37() {}
//	virtual ~scene37() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//
//	//シェーダー
//	std::unique_ptr<shader>  pbr_fbx_shader, pbr_shader, m_shader,fbx_m_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene38 :public scene
//{
//private:
//	static const int MAX_LIGHT = 4;
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::shared_ptr<static_object>  obj;
//	std::shared_ptr<skin_mesh> fbx_mesh;
//	std::unique_ptr<skin_object> fbx;
//	std::unique_ptr<environment_cubemap> env_cubemap;
//
//	struct dLights
//	{
//		float dir[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		bool is_visible = false;
//	};
//	struct pLights
//	{
//		float pos[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		float distance = 10000.0f;
//		float decay = 0.0f;
//		bool is_visible = false;
//	};
//	struct _status
//	{
//		float obj_angle[3] = { 0.0f,0.0f,0.0f };
//		float metallic = 0.5f;
//		float roughness = 0.5f;
//		float albedo[3] = { 1.0f,1.0f,1.0f };
//		float diffuse[3] = { 0.0f,0.0f,0.0f };
//		float specular[3] = { 0.0f,0.0f,0.0f };
//		dLights d_lights[MAX_LIGHT];
//		pLights p_lights[MAX_LIGHT];
//		bool auto_move = true;
//		bool rotation_flg = false;
//		bool tex_flg = false;
//		int diffuse_brdf_type = 0;
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene38() {}
//	virtual ~scene38() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//
//	//シェーダー
//	std::unique_ptr<shader>  pbr_fbx_shader, pbr_shader, m_shader, fbx_m_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//};
//
//class scene39 :public scene
//{
//private:
//	u_int timer=0;
//	static const int MAX_LIGHT = 4;
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::unique_ptr<static_object>  obj,test_objs[6];
//	std::shared_ptr<skin_mesh> fbx_mesh;
//	std::unique_ptr<skin_object> fbx;
//	std::unique_ptr<environment_cubemap> env_cubemap;
//	std::unique_ptr<cube_mesh> skybox;
//
//	struct dLights
//	{
//		float dir[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		bool is_visible = false;
//	};
//	struct pLights
//	{
//		float pos[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		float distance = 10000.0f;
//		float decay = 0.0f;
//		bool is_visible = false;
//	};
//	struct _status
//	{
//		float obj_angle[3] = { 0.0f,0.0f,0.0f };
//		float metallic = 0.5f;
//		float roughness = 0.5f;
//		float albedo[3] = { 1.0f,1.0f,1.0f };
//		float diffuse[3] = { 0.0f,0.0f,0.0f };
//		float specular[3] = { 0.0f,0.0f,0.0f };
//		dLights d_lights[MAX_LIGHT];
//		pLights p_lights[MAX_LIGHT];
//		bool auto_move = true;
//		bool tex_flg = false;
//		int diffuse_brdf_type = 0;
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene39() {}
//	virtual ~scene39() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader>  pbr_fbx_shader, pbr_shader, cube_map_shader,m_shader, fbx_m_shader,_2d_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//};
//
//class scene40 :public scene
//{
//private:
//	float weight_array[8];
//
//	std::unique_ptr<frame_buffer> f_buffer;
//	u_int timer = 0;
//	static const int MAX_LIGHT = 4;
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::unique_ptr<static_object>  obj, test_objs[6];
//	std::shared_ptr<skin_mesh> fbx_mesh;
//	std::unique_ptr<skin_object> fbx;
//	std::unique_ptr<environment_cubemap> env_cubemap;
//
//	std::unique_ptr<Texture> render_tex, blur_tex, result_tex;
//	std::unique_ptr<Texture_d> depth_tex;
//
//	std::unique_ptr<sprite> spr;
//
//	struct dLights
//	{
//		float dir[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		bool is_visible = false;
//	};
//	struct pLights
//	{
//		float pos[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		float distance = 10000.0f;
//		float decay = 0.0f;
//		bool is_visible = false;
//	};
//	struct _status
//	{
//		float obj_angle[3] = { 0.0f,0.0f,0.0f };
//		float metallic = 0.5f;
//		float roughness = 0.5f;
//		float albedo[3] = { 1.0f,1.0f,1.0f };
//		float diffuse[3] = { 0.0f,0.0f,0.0f };
//		float specular[3] = { 0.0f,0.0f,0.0f };
//		dLights d_lights[MAX_LIGHT];
//		pLights p_lights[MAX_LIGHT];
//		bool auto_move = true;
//		bool rotation_flg = false;
//		bool tex_flg = false;
//		int diffuse_brdf_type = 0;
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene40() {}
//	virtual ~scene40() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader>  pbr_fbx_shader, pbr_shader, cube_map_shader, m_shader, fbx_m_shader, _2d_shader, blur_shader_vertical, blur_shader_horizontal, depth_of_field_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//};
//
//
//class scene41 :public scene
//{
//private:
//	std::unique_ptr<frame_buffer> f_buffer;
//	u_int timer = 0;
//	static const int MAX_LIGHT = 4;
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::unique_ptr<static_object>  obj;
//	std::unique_ptr<sprite> spr;
//	std::unique_ptr<Texture>  normal_map_tex;
//	std::unique_ptr<g_buffer> g_buf;
//	//シェーダー
//	std::unique_ptr<shader>  _2d_shader,deferred_shader, g_normal_shader,diffuse_specular_shader,lighting_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//
//public:
//	scene41() {}
//	virtual ~scene41() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	struct _status
//	{
//		int view_type=0;
//		float obj_angle[3] = { 0,0,0 };
//		float dir_light[3] = { 0,0,1 };
//		float ambient[3] = { 1,1,1 };
//	};
//	std::unique_ptr<_status> status;
//	//確認用ビュー
//	void *view[6];
//};
//
//class scene42 :public scene
//{
//private:
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	ray* r;
//	sphere *s,*s2, *s3, *s4, *s5, *s6, *s7;
//	hit *h;
//
//	//shader
//	std::unique_ptr<shader>  ray_shader;
//	std::unique_ptr<sprite> spr;
//
//	struct _status
//	{
//		float dir_light_direction[3] = { 1.0f,1.0f,1.0f };
//		float dir_light_color[3] = { 0.0f,0.0f,0.0f };
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene42() {}
//	virtual ~scene42() { delete r; delete s; delete h; delete s2; delete s3;delete s4;delete s5;delete s6;delete s7;}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//};
//
//class scene43 :public scene
//{
//private:
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	ray* r;
//	sphere* s;
//	hit *h;
//	std::shared_ptr<ray_material> mat,mat2,mat3;
//	std::shared_ptr<ray_light> light, light2;
//public:
//	scene43() {}
//	virtual ~scene43() { delete r; delete s; delete h;}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//};
//
//
//
//class scene44 :public scene
//{
//private:
//	u_int timer = 0;
//
//	static const int MAX_LIGHT = 4;
//	float weight_array[8];
//
//	//オブジェクト
//	std::unique_ptr<my_mesh> player;
//	std::unique_ptr<animation> anim;
//	std::shared_ptr<static_mesh>  obj_mesh,ground_mesh;
//	std::unique_ptr<static_object>  obj, ground;
//	//std::shared_ptr<static_object> ground;
//	std::shared_ptr<skin_mesh> fbx_mesh;
//	std::unique_ptr<skin_object> fbx;
//	std::unique_ptr<environment_cubemap> env_cubemap,ir_cubemap;
//	std::unique_ptr<cube_mesh> skybox;
//
//	//テクスチャ
//	std::unique_ptr<Texture> scene_tex,voro_noise_tex,dissolve_tex,dissolve_edge_tex;
//	std::unique_ptr<Texture_d> depth_tex;
//	std::unique_ptr<sprite> spr;
//
//	//エフェクト
//	std::unique_ptr<bloom> _bloom;
//	std::unique_ptr<kawase_bloom> k_bloom;
//	std::unique_ptr<light_beam> l_beam;
//	std::unique_ptr<shadow_map> m_shadow_map;
//	std::unique_ptr<ssao> m_ssao;
//	std::unique_ptr<irmap> ir_map;
//	std::unique_ptr<dissolve> m_dissolve;
//
//	//システム
//	std::unique_ptr<deffered> m_deffered;
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	struct dLights
//	{
//		float dir[3] = { 0.2f,-0.1f,1.0f };
//		float color[3] = { 1,1,1 };
//		bool is_visible = false;
//	};
//	struct pLights
//	{
//		float pos[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		float distance = 10000.0f;
//		float decay = 0.0f;
//		bool is_visible = false;
//	};
//
//	struct _Pbr
//	{
//		float metalness = 0.5f;
//		float roughness = 0.5f;
//		float albedo[3] = { 1.0f,1.0f,1.0f };
//		float diffuse[3] = { 0.0f,0.0f,0.0f };
//		float specular[3] = { 0.0f,0.0f,0.0f };
//		bool flg = true;
//	};
//
//	struct _Bloom
//	{
//		int bloom_sampling_num = 0;
//		float bloom_variance = 10.0f;
//		bool bloom_flg = true;
//		int bloom_type = 0;
//		float bloom_threshold = 1.0f;
//	};
//
//	struct _Shadow
//	{
//		float shadow_variance = 2.56f;
//		bool shadow_flg = true;
//		bool soft_shadow_flg = true;
//	};
//
//	struct _Ssao
//	{
//		float hemi_radius = 0.5f;
//		float bias = -0.3f;
//		float variance = 1.0f;
//		bool ssao_flg = false;
//	};
//
//	struct _Dissolve
//	{
//		float dissolve_emissive_width = 0.0f;
//		float dissolve_threshold = 0.0f;
//		bool flg = false;
//	};
//
//	struct _status
//	{
//		dLights d_lights;
//		pLights p_lights[MAX_LIGHT];
//		_Pbr s_pbr;
//		_Bloom s_bloom;
//		_Shadow s_shadow;
//		_Ssao s_ssao;
//		_Dissolve s_dissolve;
//
//		float obj_angle[3] = { 0.0f,0.0f,0.0f };
//
//		bool deffered_flg = false;
//
//		bool tex_flg = false;
//		int diffuse_brdf_type = 0;
//
//		int animation_type = 0;
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene44() {}
//	virtual ~scene44() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader> m_shader,no_sv_shader,  _2d_shader, shadow_shader;
//	std::unique_ptr<shader> pbr_shader, cube_map_shader, pbr_v2_shader;
//	std::unique_ptr<shader> irmap_shader;
//	std::unique_ptr<shader> voro_noise_shader;
//	std::unique_ptr<shader_ex> fbx_m_shader, fbx_shadow_to_tex_shader, pbr_ex_shader,dissolve_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera,l_camera;
//	//確認用ビュー
//	void *scene_view, *normal_view, *position_view, *ssao_view;
//};
//
//class scene45 :public scene
//{
//private:
//	u_int timer = 0;
//
//	std::shared_ptr<static_mesh>  ground_mesh;
//	std::unique_ptr<static_object> ground[3];
//	std::unique_ptr<my_mesh> player;
//	std::unique_ptr<animation> anim;
//
//	std::unique_ptr<Texture> ssao_tex;
//	std::unique_ptr<sprite> spr;
//	std::unique_ptr<blur> _blur;
//
//	//frame_buffer
//	std::unique_ptr<frame_buffer> f_buffer;
//	std::unique_ptr<deffered> m_deffered;
//
//	ConstantBufferForSSAO cbssao;
//
//	struct _status
//	{
//		float wall_pos[3] = { -5.0f,0.0f,0.0f };
//		float player_pos[3] = { 0.0f,-5.0f,29.5f };
//
//		float hemi_radius = 0.6f;
//		float bias = -0.25f;
//		float variance = 3.5f;
//		int kernel_size = 64;
//		bool flg = true;
//	};
//	void *tex1, *tex2, *tex3, *tex4;
//	std::unique_ptr<_status> status;
//public:
//	scene45() {}
//	virtual ~scene45() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr <shader_ex> fbx_m_shader;
//	std::unique_ptr<shader> m_shader, ndc_pos_shader, _2d_shader, shadow_shader;
//	std::unique_ptr<shader> ssao_shader,cube_map_shader,normal_map_shader, depth_map_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera, sub_camera;
//};
//
//class scene46 :public scene
//{
//private:
//	u_int timer = 0;
//
//	static const int MAX_LIGHT = 4;
//	float weight_array[8];
//
//	std::shared_ptr<static_mesh>  obj_mesh, ground_mesh;
//	std::unique_ptr<static_object>  obj, ground[2];
//	std::unique_ptr<environment_cubemap> env_cubemap, ir_cubemap;
//	std::unique_ptr<cube_mesh> skybox;
//
//	std::unique_ptr<sprite> spr;
//	std::unique_ptr<ssao> m_ssao;
//
//	//frame_buffer
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	std::unique_ptr<irmap> ir_map;
//
//	std::unique_ptr<deffered> m_deffered;
//
//	struct dLights
//	{
//		float dir[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		bool is_visible = false;
//	};
//	struct pLights
//	{
//		float pos[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		float distance = 10000.0f;
//		float decay = 0.0f;
//		bool is_visible = false;
//	};
//	struct _status
//	{
//		float obj_angle[3] = { 0.0f,0.0f,0.0f };
//		float metallic = 0.5f;
//		float roughness = 0.5f;
//		float albedo[3] = { 1.0f,1.0f,1.0f };
//		float diffuse[3] = { 0.0f,0.0f,0.0f };
//		float specular[3] = { 0.0f,0.0f,0.0f };
//		dLights d_lights;
//		pLights p_lights[MAX_LIGHT];
//
//		int bloom_sampling_num = 0;
//		float bloom_variance = 10.0f;
//		bool tex_flg = false;
//		int diffuse_brdf_type = 0;
//
//		float hemi_radius = 5.0f;
//		float bias = -0.3f;
//		float variance = 1.0f;
//		bool ssao_flg = true;
//	};
//	void *tex1, *tex2, *tex3;
//	std::unique_ptr<_status> status;
//public:
//	scene46() {}
//	virtual ~scene46() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader> m_shader, no_shader, _2d_shader;
//	std::unique_ptr<shader> ssao_shader, cube_map_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera,sub_camera;
//};
//
//class scene47 :public scene
//{
//private:
//	u_int timer = 0;
//
//	static const int MAX_LIGHT = 4;
//	float weight_array[8];
//
//	std::shared_ptr<static_mesh>  obj_mesh, ground_mesh;
//	std::unique_ptr<static_object>  obj, ground;
//	//std::shared_ptr<static_object> ground;
//	std::shared_ptr<skin_mesh> fbx_mesh;
//	std::unique_ptr<skin_object> fbx;
//	std::unique_ptr<environment_cubemap> env_cubemap, ir_cubemap;
//	std::unique_ptr<cube_mesh> skybox;
//
//	std::unique_ptr<Texture> metalness_tex;
//	std::unique_ptr<Texture> scene_tex;
//	std::unique_ptr<Texture> normal_tex;
//	std::unique_ptr<Texture_d> depth_tex;
//	std::unique_ptr<Texture> depth_tex2;
//	std::unique_ptr<Texture> ssao_tex;
//
//	std::unique_ptr<sprite> spr;
//
//	std::unique_ptr<blur> _blur;
//	//線形深度マップ
//	std::unique_ptr<liner_depth> l_depth;
//
//
//	//frame_buffer
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	std::unique_ptr<irmap> ir_map;
//
//	struct dLights
//	{
//		float dir[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		bool is_visible = false;
//	};
//	struct pLights
//	{
//		float pos[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		float distance = 10000.0f;
//		float decay = 0.0f;
//		bool is_visible = false;
//	};
//	struct _status
//	{
//		float obj_angle[3] = { 0.0f,0.0f,0.0f };
//		float metallic = 0.5f;
//		float roughness = 0.5f;
//		float albedo[3] = { 1.0f,1.0f,1.0f };
//		float diffuse[3] = { 0.0f,0.0f,0.0f };
//		float specular[3] = { 0.0f,0.0f,0.0f };
//		dLights d_lights;
//		pLights p_lights[MAX_LIGHT];
//
//		int bloom_sampling_num = 0;
//		float bloom_variance = 10.0f;
//		bool tex_flg = false;
//		int diffuse_brdf_type = 0;
//
//		float hemi_radius = 10;
//		float zfar = 10000;
//		float ao_power = 500;
//		float variance = 20;
//	};
//	void *tex1, *tex2, *tex3;
//	std::unique_ptr<_status> status;
//public:
//	scene47() {}
//	virtual ~scene47() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader> m_shader, ndc_pos_shader, _2d_shader, shadow_shader;
//	std::unique_ptr<shader> ssao_shader, ssao_mul_shader, cube_map_shader, normal_map_shader, depth_map_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//};
//
//
//class scene48 :public scene
//{
//public:
//	scene48() {}
//	virtual ~scene48() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//};
//
//class scene49 :public scene
//{
//private:
//	static const int MAX_LIGHT = 4;
//	std::shared_ptr<static_mesh>  obj_mesh, ground_mesh;
//	std::unique_ptr<static_object>  obj, ground;
//	std::shared_ptr<skin_mesh> fbx_mesh;
//	std::unique_ptr<skin_object> fbx;
//
//	std::unique_ptr<Texture> tex_buffer[3];
//	std::unique_ptr<Texture> ssr_tex;
//
//	std::unique_ptr<sprite> spr;
//	//線形深度マップ
//	std::unique_ptr<liner_depth> l_depth;
//
//	//frame_buffer
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	//def
//	std::unique_ptr<deffered> m_deff;
//
//	struct _status
//	{
//		int view_type = 0;
//	};
//	std::unique_ptr<_status> status;
//	void *view[3];
//public:
//	scene49() {}
//	virtual ~scene49() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader> ssr_shader,view_shader, _2d_shader, result_shader;
//	std::unique_ptr<shader> normal_map_shader, depth_map_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//};
//
//class scene50 :public scene
//{
//private:
//	std::unique_ptr<frame_buffer> f_buffer;
//	u_int timer = 0;
//	static const int MAX_LIGHT = 4;
//	std::shared_ptr<static_mesh>  obj_mesh, obj_mesh2;
//	std::unique_ptr<static_object>  obj,obj2;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//
//	std::unique_ptr<deffered> m_deffered;
//	//
//	std::unique_ptr<sprite> spr;
//	//
//	std::unique_ptr<shader> _2d_shader;
//
//public:
//	scene50() {}
//	virtual ~scene50() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	struct _status
//	{
//		int view_type = 0;
//		float obj_angle[3] = { 0,0,0 };
//		float dir_light[3] = { 0,0,1 };
//		float ambient[3] = { 0.2f,0.2f,0.2f };
//	};
//	std::unique_ptr<_status> status;
//	//確認用ビュー
//	void *view[6];
//};
//
//class scene51 :public scene
//{
//private:
//	static const int MAX_LIGHT = 4;
//	std::shared_ptr<static_mesh>  obj_mesh, ground_mesh;
//	std::unique_ptr<static_object>  obj, ground;
//	std::shared_ptr<skin_mesh> fbx_mesh;
//	std::unique_ptr<skin_object> fbx;
//
//	std::unique_ptr<Texture> tex_buffer[3];
//	std::unique_ptr<Texture_d> depth_tex;
//	std::unique_ptr<Texture> ssr_tex;
//
//	std::unique_ptr<sprite> spr;
//	//線形深度マップ
//	std::unique_ptr<liner_depth> l_depth;
//
//	//frame_buffer
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	struct _status
//	{
//		int view_type = 0;
//	};
//	std::unique_ptr<_status> status;
//	void *view[3];
//public:
//	scene51() {}
//	virtual ~scene51() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader> m_shader,ssr_shader, view_shader, _2d_shader, result_shader;
//	std::unique_ptr<shader> normal_map_shader, depth_map_shader;
//
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//};
//
//class scene52 :public scene
//{
//private:
//	std::unique_ptr<frame_buffer> f_buffer;
//	u_int timer = 0;
//	static const int MAX_LIGHT = 4;
//
//	std::unique_ptr<my_mesh> player;
//	std::unique_ptr<animation> anim;
//	std::shared_ptr<static_mesh>  obj_mesh, obj_mesh2;
//	std::unique_ptr<static_object>  obj, obj2;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//
//	std::unique_ptr<deffered> m_deffered;
//	//
//	std::unique_ptr<sprite> spr;
//	//
//	std::unique_ptr<environment_cubemap> env_cubemap;
//	//
//	std::unique_ptr<shader_ex> gbuffer_deffered_shader;
//
//public:
//	scene52() {}
//	virtual ~scene52() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	struct _status
//	{
//		int view_type = 0;
//		float obj_angle[3] = { 0,0,0 };
//		float dir_light[3] = { 0,0,1 };
//		float ambient[3] = { 0.2f,0.2f,0.2f };
//
//		float metalness = 0.5f;
//		float roughness = 0.5f;
//	};
//	std::unique_ptr<_status> status;
//	//確認用ビュー
//	void *view[6];
//};
//
//class scene53 :public scene
//{
//private:
//	u_int timer = 0;
//
//	static const int MAX_LIGHT = 4;
//	float weight_array[8];
//
//	//テクスチャ
//	std::unique_ptr<Texture> tex_buffer[4], mask_tex, ghost_tex;
//	std::unique_ptr<sprite> spr;
//
//	std::unique_ptr<frame_buffer> f_buffer;
//
//public:
//	scene53() {}
//	virtual ~scene53() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader> ghost_shader, _2d_shader, blur_shader_horizontal, blur_shader_vertical;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//};
//
//class scene54 :public scene
//{
//private:
//	static const int MAX_LIGHT = 4;
//	std::unique_ptr<my_mesh> player;
//	std::unique_ptr<animation> anim;
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::unique_ptr<static_object>  obj;
//
//	std::unique_ptr<Texture> tex_buffer;
//	std::unique_ptr<sprite> spr;
//
//	//frame_buffer
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	struct _status
//	{
//		int view_type = 0;
//	};
//	std::unique_ptr<_status> status;
//	void *view[3];
//public:
//	scene54() {}
//	virtual ~scene54() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader_ex> fbx_m_shader;
//	std::unique_ptr<shader> m_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//};
//
//class scene55 :public scene
//{
//private:
//	static const int MAX_LIGHT = 4;
//	std::unique_ptr<my_mesh> player;
//	std::unique_ptr<animation> anim;
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::unique_ptr<static_object>  obj;
//
//	std::unique_ptr<Texture> tex_buffer;
//	std::unique_ptr<sprite> spr;
//
//	//frame_buffer
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	struct _status
//	{
//		int view_type = 0;
//		float fur_density = 0.5f;
//		float fur_distance = 0.005f;
//	};
//	std::unique_ptr<_status> status;
//	void *view[3];
//public:
//	scene55() {}
//	virtual ~scene55() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader_ex> fbx_m_shader;
//	std::unique_ptr<shader> m_shader,fur_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//tex
//	std::unique_ptr<Texture> fur_tex;
//};
//
//class scene56 :public scene
//{
//private:
//	static const int MAX_LIGHT = 4;
//	std::unique_ptr<my_mesh> player;
//	std::unique_ptr<animation> anim;
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::unique_ptr<static_object>  obj[4];
//
//	std::unique_ptr<Texture> tex_buffer;
//	std::unique_ptr<sprite> spr;
//
//	//frame_buffer
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	struct _status
//	{
//		int view_type = 0;
//		float scale = 0.5f;
//		float attenuation = 0.01f;
//		float offset = 0.1f;
//		float height_weight_rate = 0.1f;
//	};
//	std::unique_ptr<_status> status;
//	void *view[3];
//public:
//	scene56() {}
//	virtual ~scene56() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader_ex> fbx_m_shader;
//	std::unique_ptr<shader> m_shader, const_fog_shader,height_fog_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//tex
//	std::unique_ptr<Texture> scene_tex;
//	//def
//	std::unique_ptr<deffered> m_deff;
//};
//
//class scene57 :public scene
//{
//private:
//	static const int MAX_LIGHT = 4;
//	std::unique_ptr<my_mesh> player;
//	std::unique_ptr<animation> anim;
//	std::shared_ptr<static_mesh>  obj_mesh;
//	std::unique_ptr<static_object>  obj[4];
//
//	std::unique_ptr<Texture> tex_buffer;
//	std::unique_ptr<sprite> spr;
//
//	//frame_buffer
//	std::unique_ptr<frame_buffer> f_buffer;
//
//	struct _status
//	{
//		int view_type = 0;
//		float plane[4] = { 0.0f, 1.0f, 0.0f, 2.0f };
//		float camera[3] = { 2.0f, 2.0f, 1.5f};
//		float sky[3] = { 0.0f, 0.745f, 0.9f };
//		float light[3] = { -0.6f,-0.6f,0.6f };
//		float height_factor = 3.0f;
//		int sampling = 128;
//	};
//	std::unique_ptr<_status> status;
//	void *view[3];
//public:
//	scene57() {}
//	virtual ~scene57() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader> field_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//tex
//	std::unique_ptr<Texture> height_tex;
//	//def
//	std::unique_ptr<deffered> m_deff;
//};
//
//
//class scene58 :public scene
//{
//private:
//	int timer;
//	std::unique_ptr<sprite> spr;
//
//	struct _status
//	{
//		float sea_base[3] = { 0.1f, 0.19f, 0.22f };
//		float ray_origin[3] = { 0.0f, 3.5f, 5.0f };
//		float sea_height = 0.6f;
//		float sea_freq = 0.16f;
//		float choppy = 4.0f;//三角波
//		float speed = 0.04f;
//		float sea_water_color[3] = { 0.8f, 0.9f, 0.6f };
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene58() {}
//	virtual ~scene58() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader> sea_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//};
//
//class scene59 :public scene
//{
//private:
//	float timer;
//	std::unique_ptr<sprite> spr;
//
//	struct _status
//	{
//		float camera_distance = 20.0f;
//		float target_depth = 1.3f;
//		float transmittance = 1.0f;
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene59() {}
//	virtual ~scene59() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader> cloud_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//};
//
//class scene60 :public scene
//{
//private:
//	int timer;
//	std::unique_ptr<sprite> spr;
//
//	struct _status
//	{
//		float sun_pos[2] = { 0.5f,0.3f };
//		float sea_base[3] = { 0.1f, 0.19f, 0.22f };
//		float ray_origin[3] = { 0.0f, 3.5f, 5.0f };
//		float sea_height = 0.6f;
//		float sea_freq = 0.16f;
//		float choppy = 4.0f;//三角波
//		float speed = 0.04f;
//		float sea_water_color[3] = { 0.8f, 0.9f, 0.6f };
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene60() {}
//	virtual ~scene60() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader> sky_shader,sea_shader, lens_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//};

//class scene61 :public scene
//{
//private:
//	float timer;
//	std::unique_ptr<sprite> spr;
//
//	struct _status
//	{
//		float camera_distance = 2.0f;
//		float target_depth = 4.5f;
//		float transmittance = 1.0f;
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene61() {}
//	virtual ~scene61() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//	//tex
//	std::unique_ptr<Texture> conv_tex;
//	//フレームバッファ
//	std::unique_ptr<frame_buffer> f_buffer;
//	//シェーダー
//	std::unique_ptr<shader> sky_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//テクスチャ
//	std::unique_ptr<Texture> sky_noise_tex;
//	//シェーダー
//	std::unique_ptr<shader> m_shader,_2d_shader, conv_depth_shader,conposite_shader;
//	std::unique_ptr<shader_ex> fbx_m_shader;
//	//
//	std::unique_ptr<deffered> m_deffered;
//	//
//	std::unique_ptr<liner_depth> l_depth;
//	//
//	std::unique_ptr<blur> m_blur;
//	//
//	static const int PLAYER_MAX = 3;
//	std::unique_ptr<my_mesh> player[PLAYER_MAX];
//	std::unique_ptr<my_mesh> stage;
//	std::unique_ptr<animation> anim[PLAYER_MAX];
//	std::shared_ptr<static_mesh>  ground_mesh;
//	std::unique_ptr<static_object>  ground[2];
//	void *view1;
//};

//class scene62 :public scene
//{
//private:
//	float timer;
//	int update_flg;
//	std::unique_ptr<sprite> spr;
//	std::unique_ptr<Texture> prev_tex,current_tex;
//
//	struct _status
//	{
//		float cam_pos[3] = { 0.0f,3.0f,-7.5f };
//		float cam_look[3] = { 0.0f,0.0f,0.5f };
//		float color[3] = { 1,1,1 };
//		float emissive[3] = { 0,0,0 };
//		float ior = 0.17f;
//		float gloss = 0.1f;
//		int surf_type = 0;
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene62() {}
//	virtual ~scene62() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//フレームバッファ
//	std::unique_ptr<frame_buffer> f_buffer;
//	//シェーダー
//	std::unique_ptr<shader> sky_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//テクスチャ
//	std::unique_ptr<Texture> sky_noise_tex;
//	//シェーダー
//	std::unique_ptr<shader> pt_shader,screen_shader, _2d_shader;
//};

//class scene63 :public scene
//{
//private:
//	int timer;
//	std::unique_ptr<sprite> spr;
//
//	struct _status
//	{
//		float cam_pos[2] = { 0.0f,0.0f};
//	};
//	std::unique_ptr<_status> status;
//public:
//	scene63() {}
//	virtual ~scene63() {}
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//フレームバッファ
//	std::unique_ptr<frame_buffer> f_buffer;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//シェーダー
//	std::unique_ptr<shader> _2d_shader;
//	//テクスチャ
//	std::unique_ptr<Texture> input_tex,texbuffer[10];
//};












//class sea :public scene
//{
//private:
//	static const int MAX_LIGHT = 4;
//	std::shared_ptr<skin_mesh> fbx_mesh;
//	std::unique_ptr<skin_object> fbx;
//	std::unique_ptr<environment_cubemap> env_cubemap;
//	std::unique_ptr<cube_mesh> skybox;
//
//	struct dLights
//	{
//		float dir[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		bool is_visible = false;
//	};
//	struct pLights
//	{
//		float pos[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		float distance = 10000.0f;
//		float decay = 0.0f;
//		bool is_visible = false;
//	};
//	struct _status
//	{
//		float obj_angle[3] = { 90.0f,0.0f,0.0f };
//		float metallic = 0.5f;
//		float roughness = 0.5f;
//		float albedo[3] = { 1.0f,1.0f,1.0f };
//		float diffuse[3] = { 0.0f,0.0f,0.0f };
//		float specular[3] = { 0.0f,0.0f,0.0f };
//		dLights d_lights[MAX_LIGHT];
//		pLights p_lights[MAX_LIGHT];
//		bool auto_move = true;
//		bool rotation_flg = false;
//		bool tex_flg = true;
//	};
//	std::unique_ptr<_status> status;
//public:
//	sea() {}
//	virtual ~sea();
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader>  pbr_fbx_shader, m_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//	//GPUパーティクル
//	std::unique_ptr<gpu_particle> gp;
//};
//
//class fire_work :public scene
//{
//private:
//	static const int MAX_LIGHT = 4;
//	std::unique_ptr<environment_cubemap> env_cubemap;
//	std::unique_ptr<cube_mesh> skybox;
//	std::shared_ptr<skin_mesh> fbx_mesh;
//	std::unique_ptr<skin_object> fbx;
//	//シェーダー
//	std::unique_ptr<shader>  pbr_fbx_shader;
//
//	struct dLights
//	{
//		float dir[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		bool is_visible = false;
//	};
//	struct pLights
//	{
//		float pos[3] = { 0,0,0 };
//		float color[3] = { 1,1,1 };
//		float distance = 10000.0f;
//		float decay = 0.0f;
//		bool is_visible = false;
//	};
//	struct _status
//	{
//		float obj_angle[3] = { 90.0f,0.0f,0.0f };
//		float metallic = 0.5f;
//		float roughness = 0.5f;
//		float albedo[3] = { 1.0f,1.0f,1.0f };
//		float diffuse[3] = { 0.0f,0.0f,0.0f };
//		float specular[3] = { 0.0f,0.0f,0.0f };
//		dLights d_lights[MAX_LIGHT];
//		pLights p_lights[MAX_LIGHT];
//		bool auto_move = true;
//		bool rotation_flg = false;
//		bool tex_flg = true;
//	};
//	std::unique_ptr<_status> status;
//public:
//	fire_work() {}
//	virtual ~fire_work();
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader>  m_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//	//GPUパーティクル
//	//std::unique_ptr<gpu_particle> gp;
//	std::unique_ptr<firework_emitter> fe;
//};
//
//class test :public scene
//{
//private:
//	static const int MAX_LIGHT = 4;
//	std::unique_ptr<my_mesh> player;
//	std::shared_ptr<skin_mesh> fbx_mesh;
//	std::unique_ptr<skin_object> fbx;
//	std::unique_ptr<environment_cubemap> env_cubemap;
//	std::unique_ptr<cube_mesh> skybox;
//	std::unique_ptr<animation> anim;
//public:
//	test() { }
//	virtual ~test();
//	void Initialize() override;
//	void Update(float elapsedTime) override;
//	void Render(float elapsedTime) override;
//
//	//ストリング
//	char message[256] = { 0 };
//
//	//シェーダー
//	std::unique_ptr<shader>  pbr_fbx_shader;
//	std::unique_ptr<shader_ex> m_shader;
//	//カメラ
//	std::unique_ptr<main_camera> m_camera;
//	//画面管理
//	std::unique_ptr<frame_buffer> f_buffer;
//	//GPUパーティクル
//	std::unique_ptr<gpu_particle> gp;
//};

////----------------------------------------------------
////		SceneManager(管理)クラス
////----------------------------------------------------
//class scene_manager
//{
//public:
//	//std::unique_ptr<scene> currentScene;//現在のシーン
//	//									//std::unique_ptr<Scene> temp;
//	//									//Scene *currentScene;
//	//									//Scene *temp;
//
//	scene* currentScene;
//	enum SELECT_SCENE { SEA,FIREWORK,TEST,TITLE, MAIN, CLEAR, OVER, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16, S17, S18, S19, S20, S21, S22, S23, S24, S25, S26,S27,S28,S29,S30,S31, S32, S33, S34, S35,S36,S37,S38,S39,S40,S41,S42,S43,S44,S45,S46,S47,S48,S49,S50,S51,S52,S53,S54,S55,S56,S57,S58,S59,S60,S61,S62,S63,END };
//	struct _status
//	{
//		int select_scene = SELECT_SCENE::END;
//		bool go = false;
//		char str[SELECT_SCENE::END][16] = { "toon geo","main","clear","over","spot light","depth shadow","3","4","bloom","6","hight to normal","wave map","9","wave","11","alpha map","down sample","multi tex","compute","compute","17","18","19","20","motion blur","laplacian","23","camouflage","tex far","hatching","DSL","toon tex","variance shadow","kawase bloom","gaussian bloom2","irradiation","hdr","hdr tonemap","pbr","pbr fbx" };
//	};
//	std::unique_ptr<_status> status;
//	
//private:
//	scene_manager() { status = std::make_unique<_status>(); };
//	~scene_manager() { if(currentScene) delete currentScene; };
//
//public:
//	static scene_manager& getInstance()
//	{
//		static scene_manager inst;
//		return inst;
//	}
//
//	void Update(float elapsedTime);
//	void Render(float elapsedTime);
//	//マルチスレッド用
//	//void ChangeScene(scene_load* scene);
//	//通常
//	void ChangeScene(int scene_num);
//	void SceneJump();
//};

