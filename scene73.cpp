#pragma once
#include "scene.h"

#include "scene73.h"
#include <random>
#include "framework.h"
//#include "mesh.h"
//#include "deffered.h"
//#include "frame_buffer.h"
#include "scene_manager.h"
#include "sprite.h"
#include "frame_buffer.h"

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
//     ディファードレンダリングでまとめる
//-------------------------------------------------------

//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void scene73::Initialize()
{
	//system
	f_buffer = std::make_unique<frame_buffer>();
	status = std::make_unique<_status>();
	m_defferd = std::make_unique<deffered>();
	_blur = std::make_unique<blur>();

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

	//シェーダー
	ssao_shader = std::make_unique<shader>();
	ssao_shader->Create(L"Shader/ssao");
	ssao_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForSSAO));

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

	//camera
	m_camera = std::make_unique<main_camera>();
	m_camera->setPerspective(DirectX::XM_PI / 8.0f, FRAMEWORK.SCREEN_WIDTH / FRAMEWORK.SCREEN_HEIGHT, 0.1f, 1000.0f);
	m_camera->setFocusMoveEye(DirectX::XMFLOAT2(-2.5f, 0.5f), 50.0f);
	m_camera->setMode(main_camera::MODE_FOCUS);
	m_camera->active();

	timer = 0;
}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------
void scene73::Update(float elapsedTime)
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
		ImGui::Checkbox("ssr on/off", &status->ssr_flg);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("ssao"))
	{
		ImGui::SliderFloat("hemi_radius", &status->hemi_radius, 0, 10);
		ImGui::SliderFloat("bias", &status->bias, -1.0f, 1.0f);
		ImGui::SliderInt("kernel size", &status->kernel_size, 1, 64);
		ImGui::SliderFloat("variance", &status->variance, 0.0f, 5.0f);
		ImGui::Checkbox("ssao on/off", &status->ssao_flg);
		ImGui::NewLine();

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

void scene73::Render(float elapsed_time)
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
	m_defferd->begine(f_buffer.get(), true);
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
	if (status->ssr_flg)
	{
		ConstantBufferForSSR cbssr;
		cbssr.raytrace_max_length = status->max_length;
		cbssr.raytrace_max_thickness = status->max_thickness;
		cbssr.reflection_enhancer = status->reflection_enhancer;
		cbssr.timer = timer;
		UpdateConstantBuffer(ssr_shader.get(), cbssr);
		f_buffer->swapRenderTargetViewBegin(result_tex.get());
		//m_defferd->getGbuffer()->tex_buffer[3]->Set(0);//深度マップ
		depth_tex->Set(0);
		m_defferd->getGbuffer()->tex_buffer[1]->Set(1);//法線マップ
		m_defferd->getGbuffer()->tex_buffer[0]->Set(3);//シーンカラー
		//pbr_tex->Set(3);
		specularmap_tex->Set(4);
		spr->render(ssr_shader.get());
		f_buffer->swapRenderTargetViewEnd();

		//composite//specular反映
		result_tex->Set(0);
		specularmap_tex->Set(1);
		m_defferd->getGbuffer()->tex_buffer[0]->Set(2);
		spr->render(ssr_composite_shader.get());


		//合成無し
		//spr->render(_2d_shader.get(), result_tex.get());
	}

	if (status->ssao_flg)
	{
		cbssao.hemi_radius = status->hemi_radius;
		cbssao.kernel_size = status->kernel_size;
		cbssao.bias = status->bias;
		UpdateConstantBuffer(ssao_shader.get(), cbssao);

		f_buffer->swapRenderTargetViewBegin(ssao_tex.get());
		m_defferd->getGbuffer()->tex_buffer[2]->Set(1);//world_pos
		m_defferd->getGbuffer()->tex_buffer[1]->Set(2);//world_normal
		spr->render(ssao_shader.get(), m_defferd->getGbuffer()->tex_buffer[0].get());
		f_buffer->swapRenderTargetViewEnd();

		_blur->setSceneTex(ssao_tex.get());
		_blur->render(f_buffer.get(), status->variance, nullptr, false, false, 4, 5);
	}
}
