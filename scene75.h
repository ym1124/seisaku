#pragma once
#include "scene.h"
#include "postfx_cs.h"


class scene75 :public scene
{

	static const UINT NUM_ELEMENTS = 1024;
public:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> g_pSRV = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> g_pRTV = nullptr;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> g_pBufUAV = nullptr;
	//texture2d
	//Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	//Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mpScreenUAV;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cssrv = nullptr;
	//Microsoft::WRL::ComPtr<ID3D11RenderTargetView> csrtv = nullptr;

public:
	scene75() {}
	virtual ~scene75() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;

private:
	u_int timer = 0;

	//std::shared_ptr<static_mesh>  ground_mesh;
	//std::unique_ptr<static_object> ground[3];
	std::unique_ptr<my_mesh> player, stage;
	std::unique_ptr<animation> anim;

	std::unique_ptr<postfx_cs> m_ssao_cs,m_ssr_cs;

	std::unique_ptr<Texture> ssao_tex,ssr_tex[4], ref_tex, scene_tex, result_tex, specular_tex, specularmap_tex, pbr_tex;
	std::unique_ptr<Texture_d> depth_tex;
	std::unique_ptr<blur> m_blur;

	std::unique_ptr<deffered> m_deffered;

	ConstantBufferForSSAO cbssao;

	//シェーダー
	std::unique_ptr <shader_ex> fbx_m_shader;
	std::unique_ptr<shader> m_shader, _2d_shader, ssao_shader, compute_shader, ssr_shader, ssr_composite_shader;
	//テクスチャ
	std::unique_ptr<Texture> texture[2];
	//スプライト
	std::unique_ptr<sprite> spr;
	//カメラ
	std::unique_ptr<main_camera> m_camera;
	//画面管理
	std::unique_ptr<frame_buffer> f_buffer;
	void* view;

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
		bool ssao_flg = true;

		int view_type;
		int step_num = 100;
		float max_length = 20.0f;
		float max_thickness = 0.001f;
		float reflection_enhancer = 1.0f;
		bool ssr_flg = true;
	};
	void *tex1, *tex2, *tex3, *tex4;
	std::unique_ptr<_status> status;

	//ストリング
	char message[256] = { 0 };
};
