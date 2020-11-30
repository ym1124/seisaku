#pragma once
#include "scene61.h"
#include "framework.h"
#include "frame_buffer.h"
#include "deffered.h"
#include "scene_manager.h"
#include "mesh.h"
#include "animation.h"
#include "liner_depth.h"
#include "sprite.h"
#include "blur.h"


//-------------------------------------------------------
//     被写界深度
//-------------------------------------------------------

//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void scene61::Initialize()
{
	f_buffer = std::make_unique<frame_buffer>();
	status = std::make_unique<_status>();

	Light::SetDirLight(DirectX::XMFLOAT3(0.6f, -0.6f, 0.6f), DirectX::XMFLOAT3(1, 1, 1));

	conv_tex = std::make_unique<Texture>();

	//stage
	stage = std::make_unique<my_mesh>();
	stage->initialize();
	stage->load("FBX/stagebone/stagebone.bin");
	stage->setMaterialTex(0, L"FBX/stagebone/floor1.bmp");
	stage->setMaterialTex(1, L"FBX/stagebone/floor2_high2.png");
	stage->setMaterialTex(2, L"FBX/stagebone/side.bmp");
	stage->setMaterialTex(3, L"FBX/stagebone/front1.bmp");
	stage->setMaterialTex(4, L"FBX/stagebone/side.bmp");
	stage->setMaterialTex(5, L"FBX/stagebone/floor2_high2.png");
	stage->setMaterialTex(6, L"FBX/stagebone/Door.bmp");
	stage->scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	stage->pos = DirectX::XMFLOAT3(0.0f, -5.0f, 0.0f);
	stage->angle = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	//player
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		player[i] = std::make_unique<my_mesh>();
		player[i]->initialize();
		player[i]->load("FBX/Player/Player_Res.bin");
		player[i]->setAllMaterialTex(L"FBX/Texture/Player/Player_B.png");
		player[i]->scale = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
		player[i]->angle = DirectX::XMFLOAT3(0.0f, 180.0f, 0.0f);
		anim[i] = std::make_unique<animation>("");
		anim[i]->add(player[i]->_model.get(), "FBX/Player/Player_Res.bin", "Idle");
	}
	player[0]->pos = DirectX::XMFLOAT3(0.0f, -5.0f, 25.0f);
	player[1]->pos = DirectX::XMFLOAT3(0.0f, -5.0f, 10.0f);
	player[2]->pos = DirectX::XMFLOAT3(0.0f, -5.0f, -20.0f);

	//shader
	fbx_m_shader = std::make_unique<shader_ex>();
	fbx_m_shader->create("Shader/skin_mesh_ex_vs.cso", "Shader/skin_mesh_ex_ps.cso");
	m_shader = std::make_unique<shader>();
	m_shader->Create(L"Shader/phong.fx", "VSMain", "PSMain");
	_2d_shader = std::make_unique<shader>();
	_2d_shader->Create(L"Shader/2d.fx", "VSMain", "PSMain");
	conv_depth_shader = std::make_unique<shader>();
	conv_depth_shader->Create(L"Shader/convert_dof_depth");
	conv_depth_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForDoF));
	composite_shader = std::make_unique<shader>();
	composite_shader->Create(L"Shader/composite_dof");
	//m_deffered = std::make_unique<deffered>();
	l_depth = std::make_unique<liner_depth>();
	for (auto &it : m_blur)
	{
		it = std::make_unique<blur>();
	}

	//sprite
	spr = std::make_unique<sprite>();

	//camera
	m_camera = std::make_unique<main_camera>();
	m_camera->setPerspective(DirectX::XM_PI / 8.0f, FRAMEWORK.SCREEN_WIDTH / FRAMEWORK.SCREEN_HEIGHT, 0.1f, 3000.0f);
	//m_camera->setEye(DirectX::XMFLOAT3(4.0f, 1.0f, -6.0f));
	m_camera->setFocusMoveEye(DirectX::XMFLOAT2(3.11f, 0.03f), 50.0f);
	m_camera->setMode(main_camera::MODE_NORMAL);
	m_camera->active();

	timer = 0;
	focus_num = 1;
}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------
void scene61::Update(float elapsedTime)
{
#if USE_IMGUI
	ImGui::Begin("status");
	if (ImGui::TreeNode("DoF status"))
	{
		ImGui::SliderFloat("camera distance", &status->focus, 0.0f, 1.0f);
		//ImGui::Image(view[0], ImVec2(500, 500));
		ImGui::Image(view[1], ImVec2(500, 500));
		ImGui::TreePop();
	}
	ImGui::End();
#endif
	if (GetAsyncKeyState('1') & 1)
		focus_num++;
	focus_num %= 3;

	//view[0] = l_depth->getTexture()->GetShaderResourceView();
	view[1] = conv_tex->GetShaderResourceView();
	//カメラ更新
	m_camera->setMode(main_camera::MODE_FOCUS);
	m_camera->setFocus(DirectX::XMFLOAT3(player[focus_num]->pos.x, player[focus_num]->pos.y + 5.0f, player[focus_num]->pos.z));
	m_camera->focusMove();
	m_camera->update(1.0f);
	m_camera->active();

	//更新
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		anim[i]->update(player[i]->_model.get(), true);
		anim[i]->set("Result");
	}

	player[focus_num]->focusMove(m_camera.get());

	//更新
	//Light::SetDirLight(DirectX::XMFLOAT3(status->light[0], status->light[1], status->light[2]), DirectX::XMFLOAT3(1, 1, 1));

	timer += 0.05f;
	scene_manager::getInstance().SceneJump();
}

//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void scene61::Render(float elapsed_time)
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
	float ClearColor[4] = { 0.2f, 0.6f, 0.2f, 1.0f }; //red,green,blue,alpha
	FRAMEWORK.getImmediateContext()->ClearRenderTargetView(framework::getInstance().getRenderTargetView(), ClearColor);
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(framework::getInstance().getDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, &framework::getInstance().render_target_view, framework::getInstance().getDepthStencilView());
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0xFFFFFFFF);

	//ブラー
	m_blur[0]->setSceneTexBegine(f_buffer.get());
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		player[i]->render(fbx_m_shader.get(), m_camera.get(), Light::LightDir);
	}
	stage->render(fbx_m_shader.get(), m_camera.get(), Light::LightDir);
	m_blur[0]->setSceneTexEnd(f_buffer.get());
	m_blur[1]->setSceneTex(m_blur[0]->getSceneTexture());

	m_blur[0]->render(f_buffer.get(), 2.5f, nullptr, false, true);
	m_blur[1]->render(f_buffer.get(), 1.5f, nullptr, false, true);

	//深度
	l_depth->begine(f_buffer.get(), m_camera.get());
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		player[i]->render(l_depth->getShaderEX(), m_camera.get(), Light::LightDir);
	}
	stage->render(l_depth->getShaderEX(), m_camera.get(), Light::LightDir);
	l_depth->end(f_buffer.get());

	//深度変換
	ConstantBufferForDoF cbdof;
	cbdof.focus = status->focus;
	UpdateConstantBuffer(conv_depth_shader.get(), cbdof);
	f_buffer->swapRenderTargetViewBegin(conv_tex.get());
	spr->render(conv_depth_shader.get(), l_depth->getTexture());
	f_buffer->swapRenderTargetViewEnd();

	//描画
	m_blur[0]->getResultTexture()->Set(1);
	m_blur[1]->getResultTexture()->Set(2);
	conv_tex->Set(3);
	spr->render(composite_shader.get(), m_blur[0]->getSceneTexture());
}

