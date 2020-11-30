#pragma once

#include <random>
#include "scene45.h"
#include "framework.h"
#include "scene_manager.h"
#include "mesh.h"
#include "animation.h"
#include "sprite.h"
#include "deffered.h"
#include "blur.h"

//-------------------------------------------------------
//     SSAO
//-------------------------------------------------------

//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void scene45::Initialize()
{
	f_buffer = std::make_unique<frame_buffer>();
	status = std::make_unique<_status>();
	_blur = std::make_unique<blur>();
	m_deffered = std::make_unique<deffered>();

	Light::SetDirLight(DirectX::XMFLOAT3(0.6f, -0.6f, 0.6f), DirectX::XMFLOAT3(1, 1, 1));

	//player
	player = std::make_unique<my_mesh>();
	player->initialize();
	player->load("FBX/Player/Player_Res.bin");
	player->setAllMaterialTex(L"FBX/Texture/Player/Player_B.png");
	player->scale = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	anim = std::make_unique<animation>("");
	anim->add(player->_model.get(),"FBX/Player/player_idle2.bin","Idle");
	player->pos = DirectX::XMFLOAT3(0.0f, -5.0f, 29.5f);
	player->angle = DirectX::XMFLOAT3(0.0f, 180.0f, 0.0f);

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
	stage->scale = DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f);
	stage->pos = DirectX::XMFLOAT3(0.0f, -5.0f, 0.0f);
	stage->angle = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	ssao_tex = std::make_unique<Texture>();
	ssao_tex->Create();
	spr = std::make_unique<sprite>();

	//カメラの初期化
	m_camera = std::make_unique<main_camera>();
	m_camera->setPerspective(DirectX::XM_PI / 8.0f, FRAMEWORK.SCREEN_WIDTH / FRAMEWORK.SCREEN_HEIGHT, 0.1f, 1000.0f);
	//m_camera->setEye(DirectX::XMFLOAT3(10.0f, 1.0f, -20.0f));
	m_camera->setMode(main_camera::MODE_NORMAL);
	m_camera->setFocusMoveEye(DirectX::XMFLOAT2(3.14f, 0.0f), 30.0f);
	m_camera->active();

	//シェーダー
	ssao_shader = std::make_unique<shader>();
	ssao_shader->Create(L"Shader/ssao");
	ssao_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForSSAO));

	m_shader = std::make_unique<shader>();
	m_shader->Create(L"Shader/phong.fx", "VSMain", "PSMain");

	fbx_m_shader = std::make_unique<shader_ex>();
	fbx_m_shader->create("Shader/skin_mesh_ex_vs.cso", "Shader/skin_mesh_ex_ps.cso");

	_2d_shader = std::make_unique<shader>();
	_2d_shader->Create(L"Shader/2d.fx", "VSMain", "PSMain");

	std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
	std::default_random_engine generator;
	//半球カーネル//使うのはxyz部分だけ
	const int kernelSize = 64;
	DirectX::XMFLOAT4 kernel[kernelSize];
	for (int i = 0; i <kernelSize; ++i)
	{
		kernel[i] = DirectX::XMFLOAT4(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator),
			0.0f
			);

		DirectX::XMVECTOR kv = DirectX::XMVector3Normalize(DirectX::XMLoadFloat4(&kernel[i]));
		DirectX::XMStoreFloat4(&kernel[i], kv);
		kernel[i].x *= randomFloats(generator);
		kernel[i].y *= randomFloats(generator);
		kernel[i].z *= randomFloats(generator);
		kernel[i].w = 0.0f;

		float scale = (float)i / 64.0;
		scale = 0.1f + scale * scale * (1.0f - 0.1f);
		kernel[i].x *= scale;
		kernel[i].y *= scale;
		kernel[i].z *= scale;
		kernel[i].w = 0.0f;
	}

	const int kernel_noise_size = 16;
	DirectX::XMFLOAT4 kernel_noise[kernel_noise_size];
	for (unsigned int i = 0; i < kernel_noise_size; i++)
	{
		DirectX::XMFLOAT4 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f,
			0.0f);
		kernel_noise[i] = noise;
	}

	for (int i = 0; i < 64; i++)
	{
		cbssao.kernel[i] = kernel[i];
	}
	for (int i = 0; i < 16; i++)
	{
		cbssao.kernel_noise[i] = kernel_noise[i];
	}

}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------
void scene45::Update(float elapsedTime)
{
	//-----------------------------
	// シーン切り替え
	//-----------------------------
	if (KeyInput::KeyTrigger() & KEY_Z) {
		/*scene_manager::getInstance().ChangeScene(new scene_load(new scene_clear()));
		return;*/
	}

#if USE_IMGUI
	ImGui::Begin("SSAO");

	if (ImGui::TreeNode("object status"))
	{
		//ImGui::SliderFloat3("wall pos", status->wall_pos, -5.0f, 5.0f);
		ImGui::SliderFloat("player pos", &status->player_pos_z, 50.0f, 65.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("ssao"))
	{
		ImGui::SliderFloat("hemi_radius", &status->hemi_radius, 0, 10);
		ImGui::SliderFloat("bias", &status->bias, -1.0f, 1.0f);
		ImGui::SliderInt("kernel size", &status->kernel_size, 1, 64);
		ImGui::SliderFloat("variance", &status->variance, 0.0f, 5.0f);
		ImGui::Checkbox("ssao on/off", &status->flg);
		ImGui::Image(tex1, ImVec2(250, 250));
		ImGui::Image(tex2, ImVec2(250, 250));
		ImGui::Image(tex3, ImVec2(250, 250));
		ImGui::Image(tex4, ImVec2(250, 250));
		ImGui::NewLine();

		ImGui::TreePop();
	}
	ImGui::End();
#endif

#if !USE_IMGUI
	////視点移動
	//if (GetAsyncKeyState('W') < 0)
	//{
	//	m_camera->setEye(DirectX::XMFLOAT3(m_camera->getFocus().x + m_camera->getEye().x, m_camera->getFocus().y + m_camera->getEye().y, m_camera->getFocus().z + m_camera->getEye().z));
	//}
	//if (GetAsyncKeyState('S') < 0)
	//{
	//	m_camera->setEye(DirectX::XMFLOAT3(-m_camera->getFocus().x + m_camera->getEye().x, -m_camera->getFocus().y + m_camera->getEye().y, -m_camera->getFocus().z + m_camera->getEye().z));
	//}
	//if (GetAsyncKeyState('D') < 0)
	//{
	//	DirectX::XMFLOAT3 a = m_camera->getFocus();
	//	DirectX::XMFLOAT3 b = DirectX::XMFLOAT3(0, 1, 0);
	//	DirectX::XMVECTOR cross = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&b), DirectX::XMLoadFloat3(&a));
	//	DirectX::XMFLOAT3 d;
	//	DirectX::XMStoreFloat3(&d, cross);
	//	m_camera->setEye(DirectX::XMFLOAT3(d.x, d.y, d.z));
	//}
	//if (GetAsyncKeyState('A') < 0)
	//{
	//	DirectX::XMFLOAT3 a = m_camera->getFocus();
	//	DirectX::XMFLOAT3 b = DirectX::XMFLOAT3(0, 1, 0);
	//	DirectX::XMVECTOR cross = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&a), DirectX::XMLoadFloat3(&b));
	//	DirectX::XMFLOAT3 d;
	//	DirectX::XMStoreFloat3(&d, cross);
	//	m_camera->setEye(DirectX::XMFLOAT3(d.x, d.y, d.z));
	//}

	////注視点移動
	//static float angle_x = 0.0f;
	//static float angle_y = 0.0f;
	//DirectX::XMFLOAT3 lookat;
	//if (GetAsyncKeyState(VK_UP) < 0)
	//{
	//	angle_y -= 0.01f;
	//	lookat.y = sin(angle_y);
	//}
	//if (GetAsyncKeyState(VK_DOWN) < 0)
	//{
	//	angle_y += 0.01f;
	//	lookat.y = sin(angle_y);
	//}
	//if (GetAsyncKeyState(VK_RIGHT) < 0)
	//{
	//	angle_x += 0.01f;
	//	lookat.x = sin(angle_x);
	//	lookat.z = cos(angle_x);
	//}
	//if (GetAsyncKeyState(VK_LEFT) < 0)
	//{
	//	angle_x -= 0.01f;
	//	lookat.x = sin(angle_x);
	//	lookat.z = cos(angle_x);
	//}

	//m_camera->setMode(main_camera::MODE_FOCUS);
	//m_camera->setFocus(lookat);
	//m_camera->update(1.0f);
	//m_camera->active();
	//if (GetAsyncKeyState('D') < 0)
	//	status->wall_pos[0] += 0.1f;
	//if (GetAsyncKeyState('A') < 0)
	//	status->wall_pos[0] -= 0.1f;

	//if (GetAsyncKeyState('W') < 0)
	//	status->player_pos[2] += 0.1f;
	//if (GetAsyncKeyState('S') < 0)
	//	status->player_pos[2] -= 0.1f;

	if (GetAsyncKeyState('W') < 0)
		status->player_pos_z += 0.1f;
	if (GetAsyncKeyState('S') < 0)
		status->player_pos_z -= 0.1f;
	if (GetAsyncKeyState('2') & 1)
		status->flg = !status->flg;
#endif

	m_camera->setMode(main_camera::MODE_FOCUS);
	m_camera->setFocus(DirectX::XMFLOAT3(player->pos.x, player->pos.y + 5.0f, player->pos.z));
	//m_camera->move();
	m_camera->focusMove();
	m_camera->update(1.0f);
	m_camera->active();

	//テクスチャセット
	tex1 = m_deffered->getGbuffer()->tex_buffer[0]->GetShaderResourceView();
	tex2 = m_deffered->getGbuffer()->tex_buffer[2]->GetShaderResourceView();
	tex3 = m_deffered->getGbuffer()->tex_buffer[1]->GetShaderResourceView();
	tex4 = _blur->getSceneTexture()->GetShaderResourceView();

	timer++;
	if (timer > UINT_MAX)
		timer = 0;

	//更新
	player->pos = DirectX::XMFLOAT3(0.0f, -5.0f, status->player_pos_z);
	anim->update(player->_model.get(), true);
	anim->set("Idle");

	//ground[2]->setPosition(DirectX::XMFLOAT3(status->wall_pos[0], status->wall_pos[1], status->wall_pos[2]));
	//ground[0]->update();
	//ground[1]->update();
	//ground[2]->update();
	//skybox->update();

	scene_manager::getInstance().SceneJump();
}

//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void scene45::Render(float elapsed_time)
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
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red,green,blue,alpha
	FRAMEWORK.getImmediateContext()->ClearRenderTargetView(framework::getInstance().getRenderTargetView(), ClearColor);//何色でクリアするか
	//深度ステンシルビュークリア
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(framework::getInstance().getDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);//クリアする値
	// レンダーターゲットビューを登録
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, &framework::getInstance().render_target_view, framework::getInstance().getDepthStencilView());//レンダーターゲットにもDepthStencilView教えないとダメ
	//ブレンドステート
	/*static blender blender(FRAMEWORK.device);*/
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0xFFFFFFFF);

	player->render(fbx_m_shader.get(), m_camera.get(), Light::LightDir);
	stage->render(fbx_m_shader.get(), m_camera.get(), Light::LightDir);
	//ground[0]->render(FRAMEWORK.getImmediateContext(), m_shader.get(), m_camera.get());
	//ground[1]->render(FRAMEWORK.getImmediateContext(), m_shader.get(), m_camera.get());
	//ground[2]->render(FRAMEWORK.getImmediateContext(), m_shader.get(), m_camera.get());

	if (GetAsyncKeyState('1') < 0)//作品提出用のモード
	{
		m_deffered->begine(f_buffer.get());
		player->render(m_deffered->getGBufferShaderEX(), m_camera.get(), Light::LightDir);
		stage->render(m_deffered->getGBufferShaderEX(), m_camera.get(), Light::LightDir);
		//ground[0]->render(FRAMEWORK.getImmediateContext(), m_deffered->getGBufferShader(), m_camera.get());
		//ground[1]->render(FRAMEWORK.getImmediateContext(), m_deffered->getGBufferShader(), m_camera.get());
		//ground[2]->render(FRAMEWORK.getImmediateContext(), m_deffered->getGBufferShader(), m_camera.get());
		m_deffered->end(f_buffer.get());

		cbssao.hemi_radius = status->hemi_radius;
		cbssao.kernel_size = status->kernel_size;
		cbssao.bias = status->bias;
		UpdateConstantBuffer(ssao_shader.get(), cbssao);

		m_deffered->getGbuffer()->tex_buffer[2]->Set(1);//world_pos
		m_deffered->getGbuffer()->tex_buffer[1]->Set(2);//world_normal

		//f_buffer->swapRenderTargetViewBegin(ssao_tex.get());
		spr->render(ssao_shader.get(), m_deffered->getGbuffer()->tex_buffer[0].get());
		//	f_buffer->swapRenderTargetViewEnd();
	}

	else if (status->flg)
	{
		//g_bufferへの書きだし
		m_deffered->begine(f_buffer.get());
		player->render(m_deffered->getGBufferShaderEX(), m_camera.get(), Light::LightDir);
		stage->render(m_deffered->getGBufferShaderEX(), m_camera.get(), Light::LightDir);
		//ground[0]->render(FRAMEWORK.getImmediateContext(), m_deffered->getGBufferShader(), m_camera.get());
		//ground[1]->render(FRAMEWORK.getImmediateContext(), m_deffered->getGBufferShader(), m_camera.get());
		//ground[2]->render(FRAMEWORK.getImmediateContext(), m_deffered->getGBufferShader(), m_camera.get());
		m_deffered->end(f_buffer.get());

		cbssao.hemi_radius = status->hemi_radius;
		cbssao.kernel_size = status->kernel_size;
		cbssao.bias = status->bias;
		UpdateConstantBuffer(ssao_shader.get(), cbssao);


		f_buffer->swapRenderTargetViewBegin(ssao_tex.get());
		//swapRenderTargetViewBeginの中でSRVnullセットするからテクスチャのセットはその後で行う
		m_deffered->getGbuffer()->tex_buffer[2]->Set(1);//world_pos
		m_deffered->getGbuffer()->tex_buffer[1]->Set(2);//world_normal
		spr->render(ssao_shader.get(), m_deffered->getGbuffer()->tex_buffer[0].get());
		f_buffer->swapRenderTargetViewEnd();

		_blur->setSceneTex(ssao_tex.get());
		_blur->render(f_buffer.get(), status->variance, nullptr, false, false, 4, 5);
	}
}
