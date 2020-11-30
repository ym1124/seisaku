#pragma once
#include <random>
#include "scene71.h"
#include "framework.h"
#include "scene_manager.h"
#include "geometric.h"
#include "tessellation_mesh.h"
#include "tessellation_object.h"
#include "gpu_particle.h"
#include "frame_buffer.h"
#include "sprite.h"
#include "shadow_map.h"
#include "blur.h"
#include "liner_depth.h"
#include "cube_mesh.h"
#include "environment_cubemap.h"
#include "gpu_particle.h"
#include "tessellation_object.h"
#include "shader_ex.h"


//-------------------------------------------------------
//     テッセレーション
//-------------------------------------------------------

//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void scene71::Initialize()
{
	f_buffer = std::make_unique<frame_buffer>();
	status = std::make_unique<_status>();
	Light::SetDirLight(DirectX::XMFLOAT3(0.6f, -0.6f, 0.6f), DirectX::XMFLOAT3(1, 1, 1));

	//object
	ts_mesh = std::make_shared<tessellation_mesh>(L"plane",L"Shader/tessellation_snow");
	ts_obj = std::make_unique<tessellation_object>(ts_mesh);
	ts_obj->setPosition(DirectX::XMFLOAT3(0, 0, 0));
	ts_obj->setAngle(DirectX::XMFLOAT3(0, 0, 0));
	ts_obj->setScale(DirectX::XMFLOAT3(30, 1, 30));

	s_mesh = std::make_shared<static_mesh>(L"snowman");
	s_obj = std::make_unique<static_object>(s_mesh);
	s_obj->setPosition(DirectX::XMFLOAT3(0.0f, -2.0f, 0.0f));
	s_obj->setAngle(DirectX::XMFLOAT3(0, 0, 0));
	s_obj->setScale(DirectX::XMFLOAT3(1, 1, 1));

	//sph = std::make_unique<geo_sphere>(FRAMEWORK.getDevice(), 32, 32, true);

	//gpu particle
	gp = std::make_unique<gpu_particle>();
	gp->create(L"Shader/particle_cs.hlsl", "CSFunc");
	//初期データ設定
	gp->setInitData(gpu_particle::PATTERN::SNOW, DirectX::XMFLOAT3(0.0f, 30.0f, 0.0f), DirectX::XMFLOAT2(0.05f, 0.05f),50,400);

	//shader
	m_shader = std::make_unique<shader>();
	m_shader->Create(L"Shader/phong.fx", "VSMain", "PSMain");

	trail_shader = std::make_unique<shader>();
	trail_shader->Create(L"Shader/snow_trail");

	_2d_shader = std::make_unique<shader>();
	_2d_shader->Create(L"Shader/2d.fx", "VSMain", "PSMain");

	snowman_shader = std::make_unique<shader>();
	snowman_shader->Create(L"Shader/snowman");

	//camera
	m_camera = std::make_unique<main_camera>();
	m_camera->setPerspective(DirectX::XM_PI / 8.0f, FRAMEWORK.SCREEN_WIDTH / FRAMEWORK.SCREEN_HEIGHT, 0.1f, 3000.0f);
	m_camera->setMode(main_camera::MODE_NORMAL);
	/*m_camera->setEye(DirectX::XMFLOAT3(-40.01f, 40.01f, -100.0f));*/
	m_camera->setFocusMoveEye(DirectX::XMFLOAT2(-2.5f, 2.0f), 80.0f);
	m_camera->active();

	floor_camera = std::make_unique<main_camera>();
	floor_camera->setOrthographic(100 , 100, 0.1f, 1000.0f);
	//floor_camera->setPerspective(DirectX::XM_PI / 8.0f, FRAMEWORK.SCREEN_WIDTH / FRAMEWORK.SCREEN_HEIGHT, 0.1f, 3000.0f);
	floor_camera->setMode(main_camera::MODE_FOCUS);
	floor_camera->active(/*false*/);

	//深度記憶用テクスチャ
	floor_tex = std::make_unique<Texture>();
	floor_tex->Create(1280, 720);
	pre_depth_tex = std::make_unique<Texture>();
	pre_depth_tex->Create(1280,720);
	//深度テクスチャ
	depth_tex = std::make_unique<Texture_d>();
	depth_tex->Create(1280,720);
	//スプライト
	spr = std::make_unique<sprite>();

	once = false;
	s_scale = 1.0f;
}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------
void scene71::Update(float elapsedTime)
{
#if USE_IMGUI
	ImGui::Begin("tesselation");
	if (ImGui::TreeNode("tesselation status"))
	{
		ImGui::SliderInt("tess_factor##", &status->tess_factor, 0, 64);
		ImGui::SliderInt("tess_inside_factor##", &status->tess_inside_factor, 0, 64);
		ImGui::SliderFloat("depth##", &status->depth, 0.0f, 10.0f);
		ImGui::SliderFloat3("floor camera##", status->f_camera, -1000.0f, 1000.0f);
		ImGui::SliderFloat2("floor camera orth##", status->f_camera_orth, 1.0f, 2000.0f);
		ImGui::Checkbox("wire frame##", &status->wire_frame);
		ImGui::Image(view, ImVec2(250, 250));
		ImGui::TreePop();
	}
	ImGui::End();
#endif

	view = pre_depth_tex->GetShaderResourceView();
	//view = floor_tex->GetShaderResourceView();

	//カメラ更新
	//floor_camera->setOrthographic(status->f_camera_orth[0], status->f_camera_orth[1], 0.1f, 1000.0f);
	floor_camera->setOrthographic(ts_obj->getScale().x * 2, ts_obj->getScale().z * 2, 0.1f, 1000.0f);//width,heightは地形のサイズと一致させること
	floor_camera->setEye(DirectX::XMFLOAT3(status->f_camera[0], status->f_camera[1], status->f_camera[2]));
	floor_camera->setMode(main_camera::MODE_FOCUS);
	floor_camera->setFocus(DirectX::XMFLOAT3(0.0f, 0.01f, 0.0f));
	//floor_camera->setFocus(s_obj->getPosition());
	floor_camera->setUp(DirectX::XMFLOAT3(0, 0, -1));
	floor_camera->update(1.0f);
	floor_camera->active(false);

	m_camera->setMode(main_camera::MODE_FOCUS);
	m_camera->setFocus(DirectX::XMFLOAT3(s_obj->getPosition().x, s_obj->getPosition().y, s_obj->getPosition().z));
	m_camera->update(1.0f);
	m_camera->focusMove(2.0f);
	m_camera->active();

	
	//更新
	ts_obj->update();
	s_obj->update();
	//x,z平面において計算する
	//前後
	DirectX::XMFLOAT2 cam_to_obj = DirectX::XMFLOAT2(s_obj->getPosition().x - m_camera->getEye().x, s_obj->getPosition().z - m_camera->getEye().z);
	DirectX::XMVECTOR vec = DirectX::XMLoadFloat2(&cam_to_obj);
	vec = DirectX::XMVector2Normalize(vec);
	DirectX::XMStoreFloat2(&cam_to_obj, vec);
	//左右
	DirectX::XMFLOAT3 cam_to_obj3 = DirectX::XMFLOAT3(s_obj->getPosition().x - m_camera->getEye().x, s_obj->getPosition().y - m_camera->getEye().y, s_obj->getPosition().z - m_camera->getEye().z);
	DirectX::XMVECTOR vec3 = DirectX::XMLoadFloat3(&cam_to_obj3);
	vec3 = DirectX::XMVector3Normalize(vec3);
	DirectX::XMVECTOR vec_up = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 1, 0));
	vec_up = DirectX::XMVector3Normalize(vec_up);
	DirectX::XMVECTOR vec_cross = DirectX::XMVector3Cross(vec_up, vec3);
	DirectX::XMStoreFloat3(&cam_to_obj3, vec_cross);

	if (GetAsyncKeyState('W') < 0)
		s_obj->setPosition(DirectX::XMFLOAT3(s_obj->getPosition().x + cam_to_obj.x / 5.0f, s_obj->getPosition().y, s_obj->getPosition().z + cam_to_obj.y / 5.0f));
	if (GetAsyncKeyState('S') < 0)
		s_obj->setPosition(DirectX::XMFLOAT3(s_obj->getPosition().x - cam_to_obj.x / 5.0f, s_obj->getPosition().y, s_obj->getPosition().z - cam_to_obj.y / 5.0f));
	if (GetAsyncKeyState('A') < 0)
		s_obj->setPosition(DirectX::XMFLOAT3(s_obj->getPosition().x - cam_to_obj3.x / 5.0f, s_obj->getPosition().y, s_obj->getPosition().z - cam_to_obj3.z / 5.0f));
	if (GetAsyncKeyState('D') < 0)
		s_obj->setPosition(DirectX::XMFLOAT3(s_obj->getPosition().x + cam_to_obj3.x / 5.0f, s_obj->getPosition().y, s_obj->getPosition().z + cam_to_obj3.z / 5.0f));

	bool flg = false;
	s_obj->setScale(DirectX::XMFLOAT3(s_scale, s_scale, s_scale));
	float pos_y = 0;
	if (GetAsyncKeyState('W') < 0)
	{
		s_scale += 0.001f;
		if (s_scale > 4.0f)
		{
			s_scale = 4.0f;
			pos_y = s_obj->getPosition().y;
		}
		else
			pos_y = s_obj->getPosition().y + s_scale / 2500.0f;
		flg = true;
	}
	if (GetAsyncKeyState('S') < 0)
	{
		s_scale += 0.001f;
		if (s_scale > 4.0f)
		{
			s_scale = 4.0f;
			pos_y = s_obj->getPosition().y;
		}
		else
			pos_y = s_obj->getPosition().y + s_scale / 2500.0f;
		flg = true;
	}
	if (GetAsyncKeyState('A') < 0)
	{
		s_scale += 0.001f;
		if (s_scale > 4.0f)
		{
			s_scale = 4.0f;
			pos_y = s_obj->getPosition().y;
		}
		else
			pos_y = s_obj->getPosition().y + s_scale / 2500.0f;
		flg = true;
	}
	if (GetAsyncKeyState('D') < 0)
	{
		s_scale += 0.001f;
		if (s_scale > 4.0f)
		{
			s_scale = 4.0f;
			pos_y = s_obj->getPosition().y;
		}
		else
			pos_y = s_obj->getPosition().y + s_scale / 2500.0f;
		flg = true;
	}
	if (!flg)
		pos_y = s_obj->getPosition().y;

	s_obj->setPosition(DirectX::XMFLOAT3(s_obj->getPosition().x, pos_y, s_obj->getPosition().z));

	//static float _x = 0,_z = 0;
	//if (GetAsyncKeyState('W') < 0)
	//	_z += 0.1f;
	//if (GetAsyncKeyState('S') < 0)
	//	_z -= 0.1f;
	//if (GetAsyncKeyState('A') < 0)
	//	_x -= 0.1f;
	//if (GetAsyncKeyState('D') < 0)
	//	_x += 0.1f;

	//sph->update(DirectX::XMFLOAT3(_x, 0, _z), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(3, 3, 3));
	//
	gp->update();

	scene_manager::getInstance().SceneJump();
}

//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void scene71::Render(float elapsed_time)
{
	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = FRAMEWORK.SCREEN_WIDTH;//ラスタライズする画面の幅
	vp.Height = FRAMEWORK.SCREEN_HEIGHT;//高さ
	vp.MinDepth = 0.0f;//最低深度
	vp.MaxDepth = 1.0f;//最高深度
	vp.TopLeftX = 0;//左上座標?
	vp.TopLeftY = 0;//

	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);
	float ClearColor[4] = { 0.0f, 0.0f, 0.6f, 1.0f }; //red,green,blue,alpha
	FRAMEWORK.getImmediateContext()->ClearRenderTargetView(framework::getInstance().getRenderTargetView(), ClearColor);
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(framework::getInstance().getDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, &framework::getInstance().render_target_view, framework::getInstance().getDepthStencilView());
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0xFFFFFFFF);

	//深度書き込み
	f_buffer->createDepthMapBegin(depth_tex.get(), Light::LightDir, DirectX::XMFLOAT2(1280, 720));
	//sph->render(FRAMEWORK.getImmediateContext(), DirectX::XMFLOAT4(1, 1, 1, 1), false);
	s_obj->render(FRAMEWORK.getImmediateContext(), m_shader.get(), floor_camera.get());
	//カメラ確認用//ts_obj->render(FRAMEWORK.getImmediateContext(), floor_camera.get(), Light::LightDir, DirectX::XMFLOAT4(1, 1, 1, 1), status->wire_frame, status->tess_factor, status->tess_inside_factor, status->depth);
	f_buffer->createDepthMapEnd();

	//1度だけfloor_depthに深度書き込み
	if (!once)
	{
		f_buffer->swapRenderTargetViewBegin(floor_tex.get(), Light::LightDir, 1, DirectX::XMFLOAT2(1280, 720));
		spr->render(_2d_shader.get(), depth_tex.get(), 0, 0, 1280, 720, 0, 0, 1280, 720);
		f_buffer->swapRenderTargetViewEnd();
		once = true;
	}

	//深度軌跡書き込み
	f_buffer->swapRenderTargetViewBegin(pre_depth_tex.get(), Light::LightDir, 1, DirectX::XMFLOAT2(1280, 720));
	//swapRenderTargetViewBeginの中でSRVnullセットするからテクスチャのセットはその後で行う↓
	floor_tex->Set(1);
	spr->render(trail_shader.get(), depth_tex.get(), 0, 0, 1280, 720, 0, 0, 1280, 720);
	f_buffer->swapRenderTargetViewEnd();

	f_buffer->swapRenderTargetViewBegin(floor_tex.get(), Light::LightDir, 1, DirectX::XMFLOAT2(1280, 720));
	spr->render(_2d_shader.get(), pre_depth_tex.get(), 0, 0, 1280, 720, 0, 0, 1280, 720);
	f_buffer->swapRenderTargetViewEnd();

	//f_buffer->swapRenderTargetViewBegin(floor_tex.get(), Light::LightDir, 1, DirectX::XMFLOAT2(1280, 720), DirectX::XMFLOAT4(0, 0, 0, 1), false);
	//s_obj->render(FRAMEWORK.getImmediateContext(), m_shader.get(), floor_camera.get());
	////ts_obj->render(FRAMEWORK.getImmediateContext(), floor_camera.get(), Light::LightDir, DirectX::XMFLOAT4(1, 1, 1, 1), status->wire_frame, status->tess_factor, status->tess_inside_factor, status->depth);
	//f_buffer->swapRenderTargetViewEnd();

	gp->render(DirectX::XMFLOAT3(1, 1, 1));

	if (GetAsyncKeyState('1') & 1)
		status->wire_frame = !status->wire_frame;
	if (GetAsyncKeyState('2') & 1)
		status->tess_factor++;
	if (GetAsyncKeyState('3') & 1)
		status->tess_factor--;

	if (status->tess_factor > 64)
		status->tess_factor = 64;
	else if (status->tess_factor < 0)
		status->tess_factor = 0;
	floor_tex->Set(1);
	ts_obj->render(FRAMEWORK.getImmediateContext(), m_camera.get(), Light::LightDir, DirectX::XMFLOAT4(1, 1, 1, 1), status->wire_frame, status->tess_factor, status->tess_inside_factor, status->depth);
	s_obj->render(FRAMEWORK.getImmediateContext(), snowman_shader.get(), m_camera.get(), Light::LightDir, false, false);
	//sph->render(FRAMEWORK.getImmediateContext(), DirectX::XMFLOAT4(1, 1, 1, 1), false);
}
