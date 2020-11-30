#pragma once
#include <random>
#include "scene66.h"
#include "framework.h"
#include "scene_manager.h"
#include "static_mesh.h"
#include "static_object.h"
#include "frame_buffer.h"
#include "sprite.h"
#include "shader.h"

//-------------------------------------------------------
//     �{�����[�����C�g�@���p
//-------------------------------------------------------

//--------------------------------------------------------
//		������
//--------------------------------------------------------

void scene66::Initialize()
{
	f_buffer = std::make_unique<frame_buffer>();
	status = std::make_unique<_status>();

	//
	//volume_light_shader = std::make_unique<shader>();
	//volume_light_shader->Create(L"volume_light_spheres.fx","VSMain","PSMain");
	//volume_light_depth_shader = std::make_unique<shader>();
	//volume_light_depth_shader->Create(L"volume_light_spheres_depth.fx", "VSMain", "PSMain");
	//volume_light_depth_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForBoxVolumeLightSphere));

	//shader//�������̂��͂₢
	volume_light_shader = std::make_unique<shader>();
	volume_light_shader->Create(L"Shader/volume_light_spheres");
	volume_light_depth_shader = std::make_unique<shader>();
	volume_light_depth_shader->Create(L"Shader/volume_light_spheres_depth");
	volume_light_depth_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForBoxVolumeLightSphere));
	_2d_shader = std::make_unique<shader>();
	_2d_shader->Create(L"Shader/2d.fx", "VSMain", "PSMain");
	//sprite
	spr = std::make_unique<sprite>();
	//texture 
	depth_map = std::make_unique<Texture>();

	timer = 0;
}

//--------------------------------------------------------
//		�X�V
//--------------------------------------------------------
void scene66::Update(float elapsedTime)
{
#if USE_IMGUI
	ImGui::Begin("volume light");
	if (ImGui::TreeNode("volume light status"))
	{
		ImGui::SliderFloat3("sphere pos##", status->sphere_pos, -5.0f, 5.0f);
		ImGui::ColorEdit3("light_color##", status->light_color);
		ImGui::SliderFloat3("cam_pos##", status->cam_pos, -10.0f, 10.0f);
		ImGui::SliderFloat("dist##", &status->obj_dist, 0.0f, 10.0f);
		ImGui::Checkbox("another demo##", &status->another_demo);
		ImGui::Image(depth_view, ImVec2(300, 300));
		ImGui::TreePop();
	}
	ImGui::End();
#endif

#if !USE_IMGUI
	if (GetAsyncKeyState('W') < 0)
		status->sphere_pos[0] += 0.02f;
	if (GetAsyncKeyState('S') < 0)
		status->sphere_pos[0] -= 0.02f;
	if (GetAsyncKeyState('A') < 0)
		status->sphere_pos[2] -= 0.02f;
	if (GetAsyncKeyState('D') < 0)
		status->sphere_pos[2] += 0.02f;
	if (GetAsyncKeyState('2') & 1)
	{
		status->light_color[0] = 1.0f;
		status->light_color[1] = 0.1f;
		status->light_color[2] = 0.1f;
	}
	if (GetAsyncKeyState('3') & 1)
	{
		status->light_color[0] = 0.1f;
		status->light_color[1] = 1.0f;
		status->light_color[2] = 0.1f;
	}
	if (GetAsyncKeyState('4') & 1)
	{
		status->light_color[0] = 0.1f;
		status->light_color[1] = 0.1f;
		status->light_color[2] = 1.0f;
	}
#endif

	depth_view = depth_map->GetShaderResourceView();

	timer++;

	scene_manager::getInstance().SceneJump();
	//if (status->another_demo)
	//{
	//	scene_manager::getInstance().ChangeScene(scene_manager::SELECT_SCENE::S65);
	//}
}

//--------------------------------------------------------
//		�`��
//--------------------------------------------------------

void scene66::Render(float elapsed_time)
{
	// �r���[�|�[�g�ݒ�
	D3D11_VIEWPORT vp;
	vp.Width = FRAMEWORK.SCREEN_WIDTH;//���X�^���C�Y�����ʂ̕�
	vp.Height = FRAMEWORK.SCREEN_HEIGHT;//����
	vp.MinDepth = 0.0f;//�Œ�[�x
	vp.MaxDepth = 1.0f;//�ō��[�x
	vp.TopLeftX = 0;//������W?
	vp.TopLeftY = 0;//

	FRAMEWORK.getImmediateContext()->RSSetViewports(1, &vp);
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red,green,blue,alpha
	FRAMEWORK.getImmediateContext()->ClearRenderTargetView(framework::getInstance().getRenderTargetView(), ClearColor);
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(framework::getInstance().getDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, &framework::getInstance().render_target_view, framework::getInstance().getDepthStencilView());
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0xFFFFFFFF);

	//����[�x�}�b�v
	ConstantBufferForBoxVolumeLightSphere bvl;
	bvl.sphere_pos = DirectX::XMFLOAT3(status->sphere_pos[0], status->sphere_pos[1], status->sphere_pos[2]);
	bvl.timer = timer;
	bvl.cam_pos = DirectX::XMFLOAT3(status->cam_pos[0], status->cam_pos[1], status->cam_pos[2]);
	bvl.obj_dist = status->obj_dist;
	bvl.light_color = DirectX::XMFLOAT3(status->light_color[0], status->light_color[1], status->light_color[2]);
	UpdateConstantBuffer(volume_light_depth_shader.get(), bvl);
	f_buffer->swapRenderTargetViewBegin(depth_map.get());
	spr->render(volume_light_depth_shader.get());
	f_buffer->swapRenderTargetViewEnd();

	if (GetAsyncKeyState('1') < 0)//��o�p
		spr->render(_2d_shader.get(), depth_map.get());
	else
		spr->render(volume_light_shader.get(), depth_map.get());
}