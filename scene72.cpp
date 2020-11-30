#pragma once

#include "scene72.h"
#include "framework.h"
//#include "mesh.h"
//#include "deffered.h"
//#include "frame_buffer.h"
#include "scene_manager.h"
#include "sprite.h"

//#include "deffered.h"

//#include "texture.h"
//#include "sprite.h"
//#include "frame_buffer.h"
#include "deffered.h"
//#include "static_mesh.h"
//#include "static_object.h"
//#include "shader.h"
//#include "shader_ex.h"
//#include "camera.h"
#include "mesh.h"
#include "animation.h"

#include "blur.h"
#include "liner_depth.h"
#include "cube_mesh.h"
#include "environment_cubemap.h"
#include "gpu_particle.h"
#include "tessellation_object.h"




//-------------------------------------------------------
//     SSR(Screen Space Reflection)
//-------------------------------------------------------

//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void scene72::Initialize()
{
	//system
	f_buffer = std::make_unique<frame_buffer>();
	status = std::make_unique<_status>();
	m_defferd = std::make_unique<deffered>();
	m_blur = std::make_unique<blur>();

	for (auto &it : ssr_tex)
	{
		it = std::make_unique<Texture>();
	}

	result_tex = std::make_unique<Texture>();
	scene_tex = std::make_unique<Texture>();
	pbr_tex = std::make_unique<Texture>();
	depth_tex = std::make_unique<Texture_d>();
	specular_tex = std::make_unique<Texture>();
	specular_tex->Load(L"OBJ/Field_Plain/specular.png");
	specularmap_tex = std::make_unique<Texture>();

	//sprite
	spr = std::make_unique<sprite>();

	//shader
	_2d_shader = std::make_unique<shader>();
	_2d_shader->Create(L"Shader/2d.fx", "VSMain", "PSMain");

	m_shader = std::make_unique<shader>();
	m_shader->Create(L"Shader/phong.fx", "VSMain", "PSMain");

	fbx_m_shader = std::make_unique<shader_ex>();
	fbx_m_shader->create("Shader/skin_mesh_ex_vs.cso", "Shader/skin_mesh_ex_ps.cso");

	pbr_fbx_shader = std::make_unique<shader_ex>();
	pbr_fbx_shader->create("Shader/pbr_ex_vs.cso", "Shader/pbr_ex_ps.cso");
	pbr_fbx_shader->createConstantBufferOriginal(sizeof(ConstantBufferForPBREX));

	//fx//こっちの方が早い！！
	ssr_shader = std::make_unique<shader>();
	ssr_shader->Create(L"Shader/ssr.fx", "VSMain", "PSMain");
	ssr_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForSSR));

	ssr_composite_shader = std::make_unique<shader>();
	ssr_composite_shader->Create(L"Shader/ssr_composition.fx", "VSMain", "PSMain");

	//hlsl
	//ssr_shader = std::make_unique<shader>();
	//ssr_shader->Create(L"Shader/ssr");
	//ssr_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForSSR));

	//ssr_composite_shader = std::make_unique<shader>();
	//ssr_composite_shader->Create(L"Shader/ssr_composition");

	obj_mesh[0] = std::make_shared<static_mesh>(L"Slime");
	obj_mesh[1] = std::make_shared<static_mesh>(L"Mr.Incredible");
	obj[0] = std::make_unique<static_object>(obj_mesh[0]);
	obj[0]->setPosition(DirectX::XMFLOAT3(0, -5.0f, 120.0f));
	obj[0]->setScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	obj[1] = std::make_unique<static_object>(obj_mesh[1]);
	obj[1]->setPosition(DirectX::XMFLOAT3(0, -5.0f, 125.0f));
	obj[1]->setScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

	//stage
	stage = std::make_unique<my_mesh>();
	stage->initialize();
	stage->load("FBX/stagebone/stagebone.bin");
	stage->setMaterialTex(0,L"FBX/stagebone/floor1.bmp");
	stage->setMaterialTex(1,L"FBX/stagebone/floor2_high2.png");
	stage->setMaterialTex(2, L"FBX/stagebone/side.bmp");
	stage->setMaterialTex(3, L"FBX/stagebone/front1.bmp");
	stage->setMaterialTex(4, L"FBX/stagebone/side.bmp");
	stage->setMaterialTex(5, L"FBX/stagebone/floor2_high2.png");
	stage->setMaterialTex(6, L"FBX/stagebone/Door.bmp");
	stage->scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	stage->pos = DirectX::XMFLOAT3(0.0f, -5.0f, 0.0f);
	stage->angle = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	//camera
	m_camera = std::make_unique<main_camera>();
	m_camera->setPerspective(DirectX::XM_PI / 8.0f, FRAMEWORK.SCREEN_WIDTH / FRAMEWORK.SCREEN_HEIGHT, 0.1f, 1000.0f);
	m_camera->setFocusMoveEye(DirectX::XMFLOAT2(-2.5f, 0.5f),50.0f);
	m_camera->setMode(main_camera::MODE_FOCUS);
	m_camera->active();

	timer = 0;
}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------
void scene72::Update(float elapsedTime)
{
#if USE_IMGUI
	ImGui::Begin("view");

	if (ImGui::TreeNode("view status"))
	{
		const char *str[] = { "diffuse","normal","position","depth","specular_map" };
		ImGui::ListBox("diffuse brdf type", &status->view_type, str, IM_ARRAYSIZE(str));
		switch (status->view_type)
		{
		case 0:
			ImGui::Begin("diffuse");
			ImGui::Image(view[0], ImVec2(500, 500));
			ImGui::End();
			break;
		case 1:
			ImGui::Begin("normal");
			ImGui::Image(view[1], ImVec2(500, 500));
			ImGui::End();
			break;
		case 2:
			ImGui::Begin("position");
			ImGui::Image(view[2], ImVec2(500, 500));
			ImGui::End();
			break;
		case 3:
			ImGui::Begin("depth");
			ImGui::Image(view[3], ImVec2(500, 500));
			ImGui::End();
			break;
		case 4:
			ImGui::Begin("specular");
			ImGui::Image(view2, ImVec2(500, 500));
			ImGui::End();
			break;
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("ssr status"))
	{
		//ImGui::SliderInt("loop num", &status->step_num, 1, 200);
		ImGui::SliderFloat("ray length", &status->max_length, 1.0f, 30.0f);
		ImGui::SliderFloat("ray thickness", &status->max_thickness, 0.001f, 1.0f);
		ImGui::SliderFloat("eflection_enhancer", &status->reflection_enhancer, 1.0f, 30.0f);
		ImGui::TreePop();
	}

	ImGui::End();
#endif

#if !USE_IMGUI
	if (GetAsyncKeyState('1') < 0 && status->max_length > 0.0f)
		status->max_length -= 1.0f;
	if (GetAsyncKeyState('2') < 0 && status->max_length < 30.0f)
		status->max_length += 1.0f;
#endif
	//view設定
	int i = 0;
	for (auto &it : m_defferd->getGbuffer()->tex_buffer)
	{
		view[i] = it->GetShaderResourceView();
		i++;
	}
	view2 = specularmap_tex->GetShaderResourceView();

	//カメラ更新
	m_camera->setFocus(obj[0]->getPosition());
	m_camera->focusMove(2.0f);
	m_camera->update(1.0f);
	m_camera->active();

	obj[0]->focusMove(m_camera.get());
	obj[0]->update();
	obj[1]->update();

	timer++;
	timer %= INT_MAX;

	scene_manager::getInstance().SceneJump();
}

//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void scene72::Render(float elapsed_time)
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
	float ClearColor[4] = { 0.2f, 0.6f, 0.6f, 1.0f }; //red,green,blue,alpha
	FRAMEWORK.getImmediateContext()->ClearRenderTargetView(framework::getInstance().getRenderTargetView(), ClearColor);
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(framework::getInstance().getDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, &framework::getInstance().render_target_view, framework::getInstance().getDepthStencilView());
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0xFFFFFFFF);

	//pbr
	//ConstantBufferForPBREX cbpbrex;
	//cbpbrex.metalness = 0.3f;
	//cbpbrex.roughness = 0.6f;
	//UpdateConstantBuffer(pbr_fbx_shader.get(), cbpbrex);
	//f_buffer->swapRenderTargetViewBegin(pbr_tex.get());
	//obj[0]->render(FRAMEWORK.getImmediateContext(), m_shader.get(), m_camera.get());
	//obj[1]->render(FRAMEWORK.getImmediateContext(), m_shader.get(), m_camera.get());
	//stage->render(pbr_fbx_shader.get(), m_camera.get(), Light::LightDir);
	//f_buffer->swapRenderTargetViewEnd();

	//g_buffer書きだし
	m_defferd->begine(f_buffer.get(),true);
	obj[0]->render(FRAMEWORK.getImmediateContext(), m_defferd->getGBufferShader(), m_camera.get());
	obj[1]->render(FRAMEWORK.getImmediateContext(), m_defferd->getGBufferShader(), m_camera.get());
	stage->render(m_defferd->getGBufferShaderEX(), m_camera.get(), Light::LightDir);
	m_defferd->end(f_buffer.get());

	//スペキュラマップ
	f_buffer->swapRenderTargetViewBegin(specularmap_tex.get());
	stage->render(fbx_m_shader.get(), m_camera.get(), Light::LightDir);
	f_buffer->swapRenderTargetViewEnd();

	//非線形深度マップ
	f_buffer->createDepthMapBegin(depth_tex.get());
	obj[0]->render(FRAMEWORK.getImmediateContext(), m_shader.get(), m_camera.get());
	obj[1]->render(FRAMEWORK.getImmediateContext(), m_shader.get(), m_camera.get());
	stage->render(fbx_m_shader.get(), m_camera.get(), Light::LightDir);
	f_buffer->createDepthMapEnd();

	//SSR開始
	ConstantBufferForSSR cbssr;
	cbssr.raytrace_max_length = status->max_length;
	cbssr.raytrace_max_thickness = status->max_thickness;
	cbssr.reflection_enhancer = status->reflection_enhancer;
	cbssr.timer = timer;
	UpdateConstantBuffer(ssr_shader.get(), cbssr);

	//SSR結果描画
	f_buffer->swapRenderTargetViewBegin(result_tex.get());
	//swapRenderTargetViewBeginの中でSRVnullセットするからテクスチャのセットはその後で行う↓
	//m_defferd->getGbuffer()->tex_buffer[3]->Set(0);//深度マップ
	depth_tex->Set(0);
	m_defferd->getGbuffer()->tex_buffer[1]->Set(1);//法線マップ
	m_defferd->getGbuffer()->tex_buffer[0]->Set(2);//シーンカラー
	//pbr_tex->Set(3);
	specularmap_tex->Set(4);
	spr->render(ssr_shader.get());
	f_buffer->swapRenderTargetViewEnd();

	{//ブラー掛けるならコッチ
		//m_blur->setSceneTex(result_tex.get());
		//m_blur->render(f_buffer.get(), 2.0f, nullptr, false, true);
	}

	//composite//specular反映
	result_tex->Set(0);
	//m_blur->getResultTexture()->Set(0);//ブラー掛けるならコッチ
	specularmap_tex->Set(1);
	m_defferd->getGbuffer()->tex_buffer[0]->Set(2);
	spr->render(ssr_composite_shader.get());
	//

	//spr->render(_2d_shader.get(), result_tex.get());
}
