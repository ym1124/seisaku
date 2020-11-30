#pragma once

#include "scene62.h"
#include "framework.h"
#include "frame_buffer.h"
#include "scene_manager.h"
#include "sprite.h"
#include "shader.h"
//#include "texture.h"

//-------------------------------------------------------
//     パストレーシング
//-------------------------------------------------------

//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void scene62::Initialize()
{
	f_buffer = std::make_unique<frame_buffer>();
	status = std::make_unique<_status>();

	Light::SetDirLight(DirectX::XMFLOAT3(0.6f, -0.6f, 0.6f), DirectX::XMFLOAT3(1, 1, 1));

	//shader
	//pt_shader = std::make_unique<shader>();
	//pt_shader->Create(L"path_trace_cornell.fx", "VSMain", "PSMain");
	//pt_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForPathTracing1));
	//screen_shader = std::make_unique<shader>();
	//screen_shader->Create(L"path_trace.fx", "VSMain", "PSMain2");
	//_2d_shader = std::make_unique<shader>();
	//_2d_shader->Create(L"2d.fx", "VSMain", "PSMain");

	//こっちのが早い
	pt_shader = std::make_unique<shader>();
	pt_shader->Create(L"Shader/path_trace_cornell");
	pt_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForPathTracing1));
	screen_shader = std::make_unique<shader>();
	screen_shader->Create(L"Shader/path_trace_cornell_screen");
	_2d_shader = std::make_unique<shader>();
	_2d_shader->Create(L"Shader/2d.fx", "VSMain", "PSMain");

	//sprite
	spr = std::make_unique<sprite>();
	prev_tex = std::make_unique<Texture>();
	current_tex = std::make_unique<Texture>();

	float angle_x = 0.0f;
	float angle_y = 0.0f;
	timer = 0;
}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------
void scene62::Update(float elapsedTime)
{
	update_flg = 0;
#if USE_IMGUI
	//視点移動
	if (GetAsyncKeyState('W') < 0)
	{
		status->cam_pos[0] += status->cam_look[0] / 30.0f;
		status->cam_pos[1] += status->cam_look[1] / 30.0f;
		status->cam_pos[2] += status->cam_look[2] / 30.0f;
		update_flg = true;
	}
	if (GetAsyncKeyState('S') < 0)
	{
		status->cam_pos[0] -= status->cam_look[0] / 30.0f;
		status->cam_pos[1] -= status->cam_look[1] / 30.0f;
		status->cam_pos[2] -= status->cam_look[2] / 30.0f;
		update_flg = true;
	}
	if (GetAsyncKeyState('D') < 0)
	{
		DirectX::XMFLOAT3 a = DirectX::XMFLOAT3(status->cam_look[0], status->cam_look[1], status->cam_look[2]);
		DirectX::XMFLOAT3 b = DirectX::XMFLOAT3(0, 1, 0);
		DirectX::XMVECTOR cross = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&b), DirectX::XMLoadFloat3(&a));
		DirectX::XMFLOAT3 d;
		DirectX::XMStoreFloat3(&d, cross);
		status->cam_pos[0] += d.x / 30.0f;
		status->cam_pos[1] += d.y / 30.0f;
		status->cam_pos[2] += d.z / 30.0f;
		update_flg = true;
	}
	if (GetAsyncKeyState('A') < 0)
	{
		DirectX::XMFLOAT3 a = DirectX::XMFLOAT3(status->cam_look[0], status->cam_look[1], status->cam_look[2]);
		DirectX::XMFLOAT3 b = DirectX::XMFLOAT3(0, 1, 0);
		DirectX::XMVECTOR cross = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&a), DirectX::XMLoadFloat3(&b));
		DirectX::XMFLOAT3 d;
		DirectX::XMStoreFloat3(&d, cross);
		status->cam_pos[0] += d.x / 30.0f;
		status->cam_pos[1] += d.y / 30.0f;
		status->cam_pos[2] += d.z / 30.0f;
		update_flg = true;
	}

	//注視点移動

	if (GetAsyncKeyState(VK_UP) < 0)
	{
		angle_y += 0.01f;
		if (angle_y > DirectX::XM_PI / 2)
			angle_y = DirectX::XM_PI / 2;
		status->cam_look[1] = sin(angle_y);
		update_flg = true;
	}
	if (GetAsyncKeyState(VK_DOWN) < 0)
	{
		angle_y -= 0.01f;
		if (angle_y < -DirectX::XM_PI / 2)
			angle_y = -DirectX::XM_PI / 2;
		status->cam_look[1] = sin(angle_y);
		update_flg = true;
	}
	if (GetAsyncKeyState(VK_RIGHT) < 0)
	{
		angle_x += 0.01f;
		status->cam_look[0] = sin(angle_x);
		status->cam_look[2] = cos(angle_x);
		update_flg = true;
	}
	if (GetAsyncKeyState(VK_LEFT) < 0)
	{
		angle_x -= 0.01f;
		status->cam_look[0] = sin(angle_x);
		status->cam_look[2] = cos(angle_x);
		update_flg = true;
	}

	ImGui::Begin("path tracing");
	if (ImGui::TreeNode("path tracing status"))
	{
		//update_flg += ImGui::SliderFloat3("eye##", status->cam_pos, -10.0f, 10.0f);
		//update_flg += ImGui::SliderFloat3("look", status->cam_look, -10.0f, 10.0f);
		update_flg += ImGui::SliderFloat3("color", status->color, 0.0f, 1.0f);
		//update_flg += ImGui::SliderFloat3("emissive", status->emissive, 0.0f, 1.0f);
		update_flg += ImGui::SliderFloat("ior", &status->ior, 0.0f, 1.0f);
		update_flg += ImGui::SliderFloat("gloss", &status->gloss, 0.0f, 10.0f);
		const char *str[] = { "dissuse","mirror","metall","emissive" };
		update_flg += ImGui::ListBox("floor", &status->surf_type, str, IM_ARRAYSIZE(str));
		ImGui::TreePop();
	}
	ImGui::End();
#endif
	//更新
	//Light::SetDirLight(DirectX::XMFLOAT3(status->light[0], status->light[1], status->light[2]), DirectX::XMFLOAT3(1, 1, 1));

#if !USE_IMGUI
	//視点移動
	if (GetAsyncKeyState('W') < 0)
	{
		status->cam_pos[0] += status->cam_look[0] / 30.0f;
		status->cam_pos[1] += status->cam_look[1] / 30.0f;
		status->cam_pos[2] += status->cam_look[2] / 30.0f;
		update_flg = true;
	}
	if (GetAsyncKeyState('S') < 0)
	{
		status->cam_pos[0] -= status->cam_look[0] / 30.0f;
		status->cam_pos[1] -= status->cam_look[1] / 30.0f;
		status->cam_pos[2] -= status->cam_look[2] / 30.0f;
		update_flg = true;
	}
	if (GetAsyncKeyState('D') < 0)
	{
		DirectX::XMFLOAT3 a = DirectX::XMFLOAT3(status->cam_look[0], status->cam_look[1], status->cam_look[2]);
		DirectX::XMFLOAT3 b = DirectX::XMFLOAT3(0, 1, 0);
		DirectX::XMVECTOR cross = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&b), DirectX::XMLoadFloat3(&a));
		DirectX::XMFLOAT3 d;
		DirectX::XMStoreFloat3(&d, cross);
		status->cam_pos[0] += d.x / 30.0f;
		status->cam_pos[1] += d.y / 30.0f;
		status->cam_pos[2] += d.z / 30.0f;
		update_flg = true;
	}
	if (GetAsyncKeyState('A') < 0)
	{
		DirectX::XMFLOAT3 a = DirectX::XMFLOAT3(status->cam_look[0], status->cam_look[1], status->cam_look[2]);
		DirectX::XMFLOAT3 b = DirectX::XMFLOAT3(0, 1, 0);
		DirectX::XMVECTOR cross = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&a), DirectX::XMLoadFloat3(&b));
		DirectX::XMFLOAT3 d;
		DirectX::XMStoreFloat3(&d, cross);
		status->cam_pos[0] += d.x / 30.0f;
		status->cam_pos[1] += d.y / 30.0f;
		status->cam_pos[2] += d.z / 30.0f;
		update_flg = true;
	}

	//注視点移動

	if (GetAsyncKeyState(VK_UP) < 0)
	{
		angle_y += 0.01f;
		if (angle_y > DirectX::XM_PI / 2)
			angle_y = DirectX::XM_PI / 2;
		status->cam_look[1] = sin(angle_y);
		update_flg = true;
	}
	if (GetAsyncKeyState(VK_DOWN) < 0)
	{
		angle_y -= 0.01f;
		if (angle_y < -DirectX::XM_PI / 2)
			angle_y = -DirectX::XM_PI / 2;
		status->cam_look[1] = sin(angle_y);
		update_flg = true;
	}
	if (GetAsyncKeyState(VK_RIGHT) < 0)
	{
		angle_x +=0.01f;
		status->cam_look[0] = sin(angle_x);
		status->cam_look[2] = cos(angle_x);
		update_flg = true;
	}
	if (GetAsyncKeyState(VK_LEFT) < 0)
	{
		angle_x -= 0.01f;
		status->cam_look[0] = sin(angle_x);
		status->cam_look[2] = cos(angle_x);
		update_flg = true;
	}

	//床材質変更
	if (GetAsyncKeyState('1') & 1)
	{
		status->surf_type++;
		status->surf_type %= 4;
		update_flg = true;
	}

	if (GetAsyncKeyState('2') & 1)
	{
		status->color[0] = 0.1f;
		status->color[1] = 0.1f;
		status->color[2] = 1.0f;
		update_flg = true;
	}

	if (GetAsyncKeyState('3') & 1)
	{
		status->color[0] = 0.1f;
		status->color[1] = 1.0f;
		status->color[2] = 0.1f;
		update_flg = true;
	}

	if (GetAsyncKeyState('4') & 1)
	{
		status->color[0] = 1.0f;
		status->color[1] = 0.1f;
		status->color[2] = 0.1f;
		update_flg = true;
	}

#endif

	timer += 0.05f;
	scene_manager::getInstance().SceneJump();
}

//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void scene62::Render(float elapsed_time)
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
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red,green,blue,alpha
	FRAMEWORK.getImmediateContext()->ClearRenderTargetView(framework::getInstance().getRenderTargetView(), ClearColor);
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(framework::getInstance().getDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, &framework::getInstance().render_target_view, framework::getInstance().getDepthStencilView());
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0xFFFFFFFF);

	ConstantBufferForPathTracing1 cbpt;
	cbpt.cam_pos = DirectX::XMFLOAT3(status->cam_pos[0], status->cam_pos[1], status->cam_pos[2]);
	cbpt.cam_look = DirectX::XMFLOAT3(status->cam_look[0], status->cam_look[1], status->cam_look[2]);
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
	cbpt.random_modulator = randomFloats(generator);
	static int ite = 0;
	cbpt.iteration = ite++;
	if (update_flg)
	{
		ite = 0;
		FRAMEWORK.getImmediateContext()->ClearRenderTargetView(prev_tex->GetRenderTarget(), ClearColor);
	}
	cbpt.width = 1280.0f;
	cbpt.height = 720.0f;
	cbpt.surf_type = status->surf_type + 1;
	cbpt.color = DirectX::XMFLOAT3(status->color[0], status->color[1], status->color[2]);
	cbpt.emit_color = DirectX::XMFLOAT3(status->color[0], status->color[1], status->color[2]);
	cbpt.ior = status->ior;
	cbpt.gloss = status->gloss;
	UpdateConstantBuffer(pt_shader.get(), cbpt);
	//パストレーシング
	f_buffer->swapRenderTargetViewBegin(current_tex.get());
	spr->render(pt_shader.get(), prev_tex.get());
	f_buffer->swapRenderTargetViewEnd();
	//描画
	current_tex->Set(1);
	spr->render(screen_shader.get());
	//結果を更新
	f_buffer->swapRenderTargetViewBegin(prev_tex.get());
	spr->render(_2d_shader.get(), current_tex.get());
	f_buffer->swapRenderTargetViewEnd();
}