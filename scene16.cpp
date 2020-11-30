#pragma once

#include "scene16.h"
#include "scene_manager.h"
#include "framework.h"
#include "gpu_particle.h"
#include "shader_ex.h"
#include "blur.h"
#include "sprite.h"
#include "frame_buffer.h"
#include "mesh.h"
#include "animation.h"

//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

scene16::~scene16()
{
}

void scene16::Initialize()
{
	f_buffer = std::make_unique<frame_buffer>();
	status = std::make_unique<_status>();

	//モデル
	//cook = std::make_unique<my_mesh>();
	//cook->initialize();
	//cook->load("FBX/Player/Player_Res.bin");
	//cook->setAllMaterialTex(L"FBX/Texture/Player/Player_B.png");
	//cook->scale = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	//cook->angle = DirectX::XMFLOAT3(0.0f, 180.0f, 0.0f);
	//anim = std::make_unique<animation>("");
	//anim->add(cook->_model.get(), "FBX/Player/Player_Res.bin", "Result");
	//cook->pos = DirectX::XMFLOAT3(0.0f, -5.0f, 25.0f);

	s_mesh[0] = std::make_shared<static_mesh>(L"Mr.Incredible");
	s_obj[0] = std::make_unique<static_object>(s_mesh[0]);
	s_obj[0]->setPosition(DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f));
	s_mesh[1] = std::make_shared<static_mesh>(L"Slime");
	s_obj[1] = std::make_unique<static_object>(s_mesh[1]);
	s_obj[1]->setPosition(DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f));
	//s_obj->setScale(DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f));//


	//花火パーティクル宣言
	//fe = std::make_unique<firework_emitter>();
	//fe->init(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0.01, 0.01), 10);

	//パーティクル
	gp = std::make_unique<gpu_particle>();
	gp->create(L"particle_cs.hlsl", "CSFunc");
	//初期データ設定
	//gp->setInitData(gpu_particle::PATTERN::RADIAL, DirectX::XMFLOAT3(0.0f, 0.0f, 10.0f)/*fbx->getPosition()*//*参照渡し*/, DirectX::XMFLOAT2(0.01f, 0.01f), 5);
	gp->setInitData(s_obj[0].get(), DirectX::XMFLOAT2(0.01f, 0.01f), 5);//obj
	//gp->setInitData(cook.get(), DirectX::XMFLOAT2(0.01f, 0.01f), 5);

	//カメラの初期化
	m_camera = std::make_unique<main_camera>();
	m_camera->setPerspective(DirectX::XM_PI / 8.0f, FRAMEWORK.SCREEN_WIDTH / FRAMEWORK.SCREEN_HEIGHT, 0.1f, 3000.0f);
	/*m_camera->setEye(DirectX::XMFLOAT3(0.0f, 0.0f, -25.0f));*/
	m_camera->setFocusMoveEye(DirectX::XMFLOAT2(0.0f, 0.0f) ,25.0f);
	m_camera->setMode(main_camera::MODE_FOCUS);
	m_camera->active();

	//シェーダー
	fbx_m_shader = std::make_unique<shader_ex>();
	fbx_m_shader->create("Shader/skin_mesh_ex_vs.cso", "Shader/skin_mesh_ex_ps.cso");

	m_shader = std::make_unique<shader>();
	m_shader->Create(L"Shader/phong.fx", "VSMain", "PSMain");

	//ブラー
	m_blur = std::make_unique<blur>();
}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------
void scene16::Update(float elapsedTime)
{
	//-----------------------------
	// シーン切り替え
	//-----------------------------
	if (KeyInput::KeyTrigger() & KEY_Z) {
		/*scene_manager::getInstance().ChangeScene(new scene_load(new scene_clear()));
		return;*/
	}

#if USE_IMGUI
	ImGui::Begin("GPU PARTICLE");

	if (ImGui::TreeNode("particle status"))
	{
		ImGui::Checkbox("model draw on/off", &status->model_flg);
		ImGui::ColorEdit3("particle color", status->color);
		ImGui::TreePop();
	}
	ImGui::End();
#endif
	static float timer = 0;
	timer++;

	m_camera->setMode(main_camera::MODE_FOCUS);
	/*m_camera->setFocus(DirectX::XMFLOAT3(s_obj->getPosition().x, s_obj->getPosition().y+3.0f, s_obj->getPosition().z));*/
	m_camera->setFocus(DirectX::XMFLOAT3(s_obj[0]->getPosition().x, s_obj[0]->getPosition().y + 5.0f, s_obj[0]->getPosition().z));
	/*m_camera->move();*/
	m_camera->focusMove();
	m_camera->update(1.0f);
	m_camera->active();

	if (GetAsyncKeyState('1') & 1)
	{
		model_num++;
		model_num = model_num % 2;
		gp->setModel(s_obj[model_num].get());
	}
		
	s_obj[0]->update();
	s_obj[1]->update();
	gp->update();
	//花火っぽいの
	//fe->update();

	//モデルアニメーション更新
	//anim->update(cook->_model.get(), true);
	//anim->set("Result");


	scene_manager::getInstance().SceneJump();
}

//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void scene16::Render(float elapsed_time)
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

	m_blur->setSceneTexBegine(f_buffer.get());
	if(status->model_flg)
		//cook->render(fbx_m_shader.get(), m_camera.get(), Light::LightDir);
		s_obj[model_num]->render(FRAMEWORK.getImmediateContext(), m_shader.get(),m_camera.get(),Light::LightDir,false);
	gp->render(DirectX::XMFLOAT3(status->color[0], status->color[1], status->color[2]));
	m_blur->setSceneTexEnd(f_buffer.get());
	m_blur->render(f_buffer.get(), 0.1f, nullptr, true, false);

	//fe->render();
}
