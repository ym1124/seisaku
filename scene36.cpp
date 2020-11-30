#pragma once

#include "scenes.h"
#include "scene_manager.h"
#include "framework.h"

//-------------------------------------------------------
//     PBR(物理ベースレンダリング)
//-------------------------------------------------------

//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void scene36::Initialize()
{
	f_buffer = std::make_unique<frame_buffer>();
	status = std::make_unique<_status>();

	fbx = std::make_unique<my_mesh>();
	fbx->initialize();
	fbx->load("FBX/kuzira_v1/kuzira_v1.bin");
	fbx->setAllMaterialTex(L"FBX/kuzira_v1/10054_Whale_Diffuse_v2.jpg");
	fbx->scale = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	anim = std::make_unique<animation>("");
	anim->add(fbx->_model.get(), "FBX/kuzira_v1/kuzira_v1.bin", "Idle");

	for (auto &it : spheres)
	{
		it = std::make_unique<geo_sphere>(FRAMEWORK.getDevice(), 32, 32, true);
	}
	skybox = std::make_unique<cube_mesh>(L"images/OceanCubeMap.dds");
	skybox->setScale(DirectX::XMFLOAT3(10000.0f, 10000.0f, 10000.0f));
	env_cubemap = std::make_unique<environment_cubemap>(FRAMEWORK.getDevice(), 1024, DXGI_FORMAT_R16G16B16A16_FLOAT, false);
	env_cubemap->load_dds(FRAMEWORK.getImmediateContext(), L"images/OceanCubeMap.dds");

	//カメラの初期化
	m_camera = std::make_unique<main_camera>();
	m_camera->setPerspective(DirectX::XM_PI / 8.0f, FRAMEWORK.SCREEN_WIDTH / FRAMEWORK.SCREEN_HEIGHT, 0.1f, 10000.0f);
	//m_camera->setEye(DirectX::XMFLOAT3(0.01f, 0.01f, 80.0f));
	m_camera->setFocusMoveEye(DirectX::XMFLOAT2(0.0f, 0.0f), 80.0f);
	m_camera->setMode(main_camera::MODE_FOCUS);
	m_camera->setFocus(DirectX::XMFLOAT3(0, 0.1f, 0));
	m_camera->active();

	//シェーダー
	pbr_fbx_shader = std::make_unique<shader_ex>();
	pbr_fbx_shader->create("Shader/pbr_ex_vs.cso", "Shader/pbr_ex_ps.cso");
	pbr_fbx_shader->createConstantBufferOriginal(sizeof(ConstantBufferForPBREX));

	//１つだけ有効化
	status->p_lights[0].is_visible = true;
	float angle_x = 0;
	float angle_y = 0;
}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------
void scene36::Update(float elapsedTime)
{
#if USE_IMGUI
	ImGui::Begin("PBR");

	if (ImGui::TreeNode("pbr status"))
	{
		ImGui::ColorEdit3("albedo", status->albedo);
		ImGui::SliderFloat("metallic", &status->metallic, 0.0f, 1.0f);
		ImGui::SliderFloat("roughness", &status->roughness, 0.0f, 1.0f);
		ImGui::SliderFloat3("reflected diffuse", status->diffuse, 0.0f, 1.0f);
		ImGui::SliderFloat3("reflected specular", status->specular, 0.0f, 1.0f);
		ImGui::NewLine();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Light status"))
	{
		if (ImGui::TreeNode("directional light"))
		{
			ImGui::SliderFloat3("light direction", status->d_lights[0].dir, -1.0f, 1.0f);
			ImGui::ColorEdit3("light color", status->d_lights[0].color);
			ImGui::Checkbox("visible", &status->d_lights[0].is_visible);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("point light"))
		{
			ImGui::SliderFloat3("light pos", status->p_lights[0].pos, -100.0f, 100.0f);
			ImGui::ColorEdit3("light color", status->p_lights[0].color);
			ImGui::SliderFloat("distance", &status->p_lights[0].distance, 0.0f, 10000.0f);
			ImGui::SliderFloat("decay", &status->p_lights[0].decay, 0.0f, 100.0f);
			ImGui::Checkbox("visible", &status->p_lights[0].is_visible);
			ImGui::Checkbox("move auto", &status->auto_move);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("obj status"))
	{
		ImGui::SliderFloat3("obj angle", status->obj_angle, 0.0f, 360.0f);
		if (ImGui::Checkbox("focus change", &status->focus_flg))
			focus_flg++;
		ImGui::TreePop();
	}

	ImGui::End();
	if (focus_flg % 2)
	{
		m_camera->setMode(main_camera::MODE_FOCUS);
		m_camera->setFocus(DirectX::XMFLOAT3(0, 0, 0));
	}
	else
	{
		m_camera->setMode(main_camera::MODE_FOCUS);
		m_camera->setFocus(fbx->pos);
	}
	m_camera->move();
#endif

#if !USE_IMGUI
	//視点移動		
	//if (GetAsyncKeyState('W') < 0)
	//	m_camera->setEye(DirectX::XMFLOAT3(m_camera->getEye().x, m_camera->getEye().y, m_camera->getEye().z - 1));
	//if (GetAsyncKeyState('S') < 0)
	//	m_camera->setEye(DirectX::XMFLOAT3(m_camera->getEye().x, m_camera->getEye().y, m_camera->getEye().z + 1));
	//if (GetAsyncKeyState('D') < 0)
	//	m_camera->setEye(DirectX::XMFLOAT3(m_camera->getEye().x - 1.0f, m_camera->getEye().y, m_camera->getEye().z));
	//if (GetAsyncKeyState('A') < 0)
	//	m_camera->setEye(DirectX::XMFLOAT3(m_camera->getEye().x + 1.0f, m_camera->getEye().y, m_camera->getEye().z));
	//if (GetAsyncKeyState('Q') < 0)
	//	m_camera->setEye(DirectX::XMFLOAT3(m_camera->getEye().x, m_camera->getEye().y + 1, m_camera->getEye().z));
	//if (GetAsyncKeyState('E') < 0)
	//	m_camera->setEye(DirectX::XMFLOAT3(m_camera->getEye().x, m_camera->getEye().y - 1, m_camera->getEye().z));

	if (GetAsyncKeyState('S') < 0 && status->d_lights[0].dir[1]<1)
		status->d_lights[0].dir[1] += 0.01f;
	if (GetAsyncKeyState('W') < 0 && status->d_lights[0].dir[1] >0)
		status->d_lights[0].dir[1] -= 0.01f;
	if (GetAsyncKeyState('D') < 0 && status->d_lights[0].dir[0] < 1)
		status->d_lights[0].dir[0] += 0.01f;
	if (GetAsyncKeyState('A') < 0 && status->d_lights[0].dir[0] > 0)
		status->d_lights[0].dir[0] -= 0.01f;

	if (status->d_lights[0].dir[0] > 1)
		status->d_lights[0].dir[0] = 1.0f;
	else if (status->d_lights[0].dir[0] < 0)
		status->d_lights[0].dir[0] = 0.0f;

	if (GetAsyncKeyState('1') & 1)
		focus_flg++;

	if (focus_flg % 2)
	{
		m_camera->setMode(main_camera::MODE_FOCUS);
		m_camera->setFocus(DirectX::XMFLOAT3(0, 0, 0));
	}
	else
	{
		m_camera->setMode(main_camera::MODE_FOCUS);
		m_camera->setFocus(DirectX::XMFLOAT3(fbx->pos.x + 10.0f, fbx->pos.y, fbx->pos.z));
	}

	if (GetAsyncKeyState('2') & 1)
	{
		status->metallic = 0.9f;
		status->roughness = 0.1f;
	}

	if (GetAsyncKeyState('3') & 1)
	{
		status->metallic = 0.1f;
		status->roughness = 0.9f;
	}



	////注視点移動
	//if (GetAsyncKeyState(VK_UP) < 0)
	//{
	//	angle_y += 0.1f;
	//	if (angle_y > DirectX::XM_PI / 2)
	//		angle_y = DirectX::XM_PI / 2;
	//	m_camera->setFocus(DirectX::XMFLOAT3(m_camera->getEye().x + sinf(angle_x), m_camera->getEye().y + sinf(angle_y), m_camera->getEye().z + cosf(angle_x)));
	//}
	//if (GetAsyncKeyState(VK_DOWN) < 0)
	//{
	//	angle_y -= 0.1f;
	//	if (angle_y < -DirectX::XM_PI / 2)
	//		angle_y = -DirectX::XM_PI / 2;
	//	m_camera->setFocus(DirectX::XMFLOAT3(m_camera->getEye().x + sinf(angle_x), m_camera->getEye().y + sinf(angle_y), m_camera->getEye().z + cosf(angle_x)));
	//}
	//if (GetAsyncKeyState(VK_RIGHT) < 0)
	//{
	//	angle_x += 0.1f;
	//	m_camera->setFocus(DirectX::XMFLOAT3(m_camera->getEye().x + sinf(angle_x), m_camera->getEye().y + sinf(angle_y), m_camera->getEye().z + cosf(angle_x)));
	//}
	//if (GetAsyncKeyState(VK_LEFT) < 0)
	//{
	//	angle_x -= 0.1f;
	//	m_camera->setFocus(DirectX::XMFLOAT3(m_camera->getEye().x + sinf(angle_x), m_camera->getEye().y + sinf(angle_y), m_camera->getEye().z + cosf(angle_x)));
	//}
	////Light::SetDirLight(DirectX::XMFLOAT3(0.6f, 0.6f, 0.6f), DirectX::XMFLOAT3(1, 1, 1));
#endif
	Light::SetDirLight(DirectX::XMFLOAT3(status->d_lights[0].dir[0], status->d_lights[0].dir[1], status->d_lights[0].dir[2]), DirectX::XMFLOAT3(status->d_lights[0].color[0], status->d_lights[0].color[1], status->d_lights[0].color[2]));

	static float timer = 0;
	timer++;
	if (status->auto_move)
	{
		status->p_lights[0].pos[2] = 100.0f*(sinf(timer*(3.141592f / 180.0f)));
		status->p_lights[0].pos[0] = 100.0f*(cosf(timer*(3.141592f / 180.0f)));
	}

	for (int i = 0; i < 3; i++)
	{
		status->obj_angle[i] = (int)status->obj_angle[i] % 360;
	}

	fbx->pos = DirectX::XMFLOAT3(20.0f, 0.0f, 0.0f);
	fbx->angle = DirectX::XMFLOAT3(status->obj_angle[0] * (XM_PI / 180.0f), status->obj_angle[1] * (XM_PI / 180.0f), status->obj_angle[2] * (XM_PI / 180.0f));
	anim->update(fbx->_model.get(), true);

	static int i = 0;
	for (auto &it : spheres)
	{
		it->update(DirectX::XMFLOAT3(-12 + (i % 4) * 8, 12 - (i / 4) * 8, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(6, 6, 6));
		i++;
	}
	i = 0;

	m_camera->focusMove();
	m_camera->update(1.0f);
	m_camera->active();
	skybox->update();

	scene_manager::getInstance().SceneJump();
}

//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void scene36::Render(float elapsed_time)
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
	// 指定色で画面クリア
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
	FRAMEWORK.getImmediateContext()->ClearRenderTargetView(framework::getInstance().getRenderTargetView(), ClearColor);//何色でクリアするか
	//深度ステンシルビュークリア
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(framework::getInstance().getDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);//クリアする値
	// レンダーターゲットビューを登録
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, &framework::getInstance().render_target_view, framework::getInstance().getDepthStencilView());//レンダーターゲットにもDepthStencilView教えないとダメ
	//ブレンドステート
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0xFFFFFFFF);

	FRAMEWORK.getImmediateContext()->PSSetShaderResources(4, 1, env_cubemap->getShaderResourceViewAddress());
	//FRAMEWORK.getImmediateContext()->PSSetShaderResources(5, 1, ir_map->getICTShaderResourceViewAddress());
	FRAMEWORK.getImmediateContext()->PSSetShaderResources(5, 1, env_cubemap->getShaderResourceViewAddress());
	FRAMEWORK.getImmediateContext()->PSSetShaderResources(6, 1, env_cubemap->getShaderResourceViewAddress());//shader側の受け取りと一致していないエラー

	ConstantBufferForPBREX cbpbrex;
	cbpbrex.metalness = status->metallic;
	cbpbrex.roughness = status->roughness;
	UpdateConstantBuffer(pbr_fbx_shader.get(), cbpbrex);
	fbx->render(pbr_fbx_shader.get(), m_camera.get(), Light::LightDir);

	int i = 0;
	for (auto &it : spheres)
	{
		i++;
		cbpbrex.metalness = (16 - i) / 16.0f;//メタルネスは右下に向かって下がっていく
		cbpbrex.roughness = ((float)(i / 4) / 4.0f) / 16.0f;//行ごとにラフネスは一定
		UpdateConstantBuffer(pbr_fbx_shader.get(), cbpbrex);
		it->render(FRAMEWORK.getImmediateContext(), DirectX::XMFLOAT4(1, 1, 1, 1), false);
	}
	skybox->render(m_camera.get());
}
