#pragma once
#include "scene59.h"
#include "framework.h"
#include "scene_manager.h"
#include "sprite.h"
#include "shader.h"

//-------------------------------------------------------
//     レイマーチング(雲)
//-------------------------------------------------------

//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void scene59::Initialize()
{
	status = std::make_unique<_status>();
	//shader
	cloud_shader = std::make_unique<shader>();
	cloud_shader->Create(L"Shader/cloud_raymarching");
	cloud_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForCloudRaymarching));

	//sprite
	spr = std::make_unique<sprite>();

	timer = 0;
}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------
void scene59::Update(float elapsedTime)
{
#if USE_IMGUI
	ImGui::Begin("status");
	if (ImGui::TreeNode("sea status"))
	{
		ImGui::SliderFloat("camera distance", &status->camera_distance, 0.0f, 30.0f);
		ImGui::SliderFloat("target depth", &status->target_depth, 0.0f, 30.0f);
		ImGui::SliderFloat("transmittance", &status->transmittance, 0.0f, 1.0f);
		ImGui::TreePop();
	}
	ImGui::End();
#endif

	//更新
	//Light::SetDirLight(DirectX::XMFLOAT3(status->light[0], status->light[1], status->light[2]), DirectX::XMFLOAT3(1, 1, 1));

	timer++;

	scene_manager::getInstance().SceneJump();
}

//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void scene59::Render(float elapsed_time)
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

	ConstantBufferForCloudRaymarching cbcr;
	cbcr.timer = timer;
	cbcr.target_depth = status->target_depth;
	cbcr.camera_distance = status->camera_distance;
	cbcr.transmittance = status->transmittance;
	UpdateConstantBuffer(cloud_shader.get(), cbcr);

	spr->render(cloud_shader.get());
}

