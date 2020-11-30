#pragma once
#include "scene60.h"
#include "framework.h"
#include "scene_manager.h"
#include "shader.h"
#include "sprite.h"

//-------------------------------------------------------
//     レイマーチング(雲+海)
//-------------------------------------------------------

//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void scene60::Initialize()
{
	status = std::make_unique<_status>();
	//shader//effectよりcso読み込みのが早い
	sky_shader = std::make_unique<shader>();
	sky_shader->Create(L"Shader/sky_raymarching");
	sky_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForCloudRaymarching));
	sea_shader = std::make_unique<shader>();
	sea_shader->Create(L"Shader/sea_raymarching");
	sea_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForSeaRaymarching));
	lens_shader = std::make_unique<shader>();
	lens_shader->Create(L"Shader/lens_ghost");
	lens_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForLensGhost));

	//sprite
	spr = std::make_unique<sprite>();

	timer = 0;
}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------
void scene60::Update(float elapsedTime)
{
#if USE_IMGUI
	ImGui::Begin("status");
	if (ImGui::TreeNode("sea status"))
	{
		//ImGui::SliderFloat2("sun pos", status->sun_pos, 0.0f, 0.45f);
		ImGui::ColorEdit3("base color", status->sea_base);
		ImGui::ColorEdit3("water color", status->sea_water_color);
		ImGui::SliderFloat3("ray_origin", status->ray_origin, 0.0f, 10.0f);
		ImGui::SliderFloat("sea_height", &status->sea_height, 0.0f, 1.0f);
		ImGui::SliderFloat("sea_freq", &status->sea_freq, 0.0f, 1.0f);
		ImGui::SliderFloat("choppy", &status->choppy, 0.0f, 10.0f);
		ImGui::SliderFloat("speed", &status->speed, 0.0f, 0.1f);
		ImGui::TreePop();
	}
	ImGui::End();
#endif

	//更新
	//Light::SetDirLight(DirectX::XMFLOAT3(status->light[0], status->light[1], status->light[2]), DirectX::XMFLOAT3(1, 1, 1));

	scene_manager::getInstance().SceneJump();
	timer ++;
}

//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void scene60::Render(float elapsed_time)
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

	ConstantBufferForSkyRaymarching cbsr;
	cbsr.timer = timer;
	cbsr.sun_pos.x = status->sun_pos[0];
	cbsr.sun_pos.y = status->sun_pos[1];
	UpdateConstantBuffer(sky_shader.get(), cbsr);

	spr->render(sky_shader.get());
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ADD), nullptr, 0xFFFFFFFF);

	{
		ConstantBufferForLensGhost cblg;
		cblg.timer = timer;
		cblg.sun_pos = DirectX::XMFLOAT2((timer % 2000) / 500.0f - 1.0f, 0.175f);
		UpdateConstantBuffer(lens_shader.get(), cblg);
	}
	spr->render(lens_shader.get());
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ADD), nullptr, 0xFFFFFFFF);

	{
		ConstantBufferForSeaRaymarching cbsr;
		cbsr.timer = timer;
		cbsr.sea_base = DirectX::XMFLOAT3(status->sea_base[0], status->sea_base[1], status->sea_base[2]);
		cbsr.sea_water_color = DirectX::XMFLOAT3(status->sea_water_color[0], status->sea_water_color[1], status->sea_water_color[2]);
		cbsr.ray_origin = DirectX::XMFLOAT3(status->ray_origin[0], status->ray_origin[1], status->ray_origin[2]);
		cbsr.sea_freq = status->sea_freq;
		cbsr.sea_height = status->sea_height;
		cbsr.choppy = status->choppy;
		cbsr.speed = status->speed;
		UpdateConstantBuffer(sea_shader.get(), cbsr);
	}

	spr->render(sea_shader.get());
}