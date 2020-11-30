#pragma once

#include  <string>
#include "scenes.h"
#include "camera.h"
#include "framework.h"
#include "collision.h"
#include "input.h"
#include "multi_thread.h"

//テクスチャファー
//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void scene26::Initialize()
{
	f_buffer = std::make_unique<frame_buffer>();

	obj_mesh = std::make_unique<static_mesh>(L"korone");
	obj = std::make_unique<static_object>(obj_mesh);
	obj->setPosition(DirectX::XMFLOAT3(0, 0, 250.0f));
	obj->setScale(DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f));

	ground_mesh = std::make_unique<static_mesh>(L"Field_Plain");
	ground = std::make_unique<static_object>(ground_mesh);


	render_tex = std::make_unique<Texture>();
	render_tex->Create();
	draw_tex = std::make_unique<Texture>();
	draw_tex->Load(L"images/ColoredPencil2.png");

	render_spr = std::make_unique<sprite>();
	draw_spr = std::make_unique<sprite>();

	m_shader = std::make_unique<shader>();
	m_shader->Create(L"phong.fx", "VSMain", "PSMain");
	_2d_shader = std::make_unique<shader>();
	_2d_shader->Create(L"2d.fx", "VSMain", "PSMain");
	draw_shader = std::make_unique<shader>();
	draw_shader->Create(L"draw.fx", "VSMain", "PSMain");

	m_shader->CreateConstantBuffer(sizeof(ConstantBufferForPhong));
	draw_shader->CreateConstantBuffer(sizeof(ConstantBufferForDraw));

	//カメラの初期化
	m_camera = std::make_unique<MainCamera>();
	m_camera->SetPerspective(DirectX::XM_PI / 8.0f, FRAMEWORK.SCREEN_WIDTH / FRAMEWORK.SCREEN_HEIGHT, 0.1f, 1000.0f);
	m_camera->SetEye(DirectX::XMFLOAT3(0.0f, 100.0f, -10.0f));
	m_camera->Active();

	//collision::setTerrain(ground);//地形登録
}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------
void scene26::Update(float elapsedTime)
{
	//-----------------------------
	// シーン切り替え
	//-----------------------------
	if (KeyInput::KeyTrigger() & KEY_Z) {
		/*scene_manager::getInstance().ChangeScene(new scene_load(new scene_clear()));
		return;*/
	}
#if USE_IMGUI
	ImGui::Begin("texture fur");

	ImGui::End();
#endif

	obj->move();
	obj->update();

	ground->update();

	//collision::terrainsFaceUpdate();
	//collision::autoRayPick(obj.get(), 10.0f);

	m_camera->SetMode(MainCamera::MODE_SET);
	m_camera->SetFocus(DirectX::XMFLOAT3(obj->getPosition()));
	m_camera->Update(1.0f);
	m_camera->Active();
}

//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void scene26::Render(float elapsed_time)
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
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, framework::getInstance().render_target_view.GetAddressOf(), framework::getInstance().getDepthStencilView());//レンダーターゲットにもDepthStencilView教えないとダメ
																																									   //ブレンドステート
	static blender blender(FRAMEWORK.device.Get());
	FRAMEWORK.getImmediateContext()->OMSetBlendState(blender.states[blender::BS_ALPHA].Get(), nullptr, 0xFFFFFFFF);


	ConstantBufferForPhong cb;
	cb.AmbientColor = Light::Ambient;
	//cb.LightDir = Light::LightDir;
	cb.LightDir = Light::LightDir;
	cb.LightColor = Light::DirLightColor;
	cb.EyePos.x = m_camera->GetEye().x;
	cb.EyePos.y = m_camera->GetEye().y;
	cb.EyePos.z = m_camera->GetEye().z;
	cb.EyePos.w = 1.0f;
	UpdateConstantBuffer(m_shader.get(), cb);

	ConstantBufferForDraw cbd;
	cbd.AmbientColor = Light::Ambient;
	//cb.LightDir = Light::LightDir;
	cbd.LightDir = Light::LightDir;
	cbd.LightColor = Light::DirLightColor;
	cbd.EyePos.x = m_camera->GetEye().x;
	cbd.EyePos.y = m_camera->GetEye().y;
	cbd.EyePos.z = m_camera->GetEye().z;
	cbd.EyePos.w = 1.0f;
	UpdateConstantBuffer(draw_shader.get(), cbd);

	draw_tex->Set(1);
	obj->render(FRAMEWORK.getImmediateContext(), draw_shader.get(), m_camera.get(), Light::LightDir, false);
	//ground->render(FRAMEWORK.getImmediateContext(), draw_shader.get(), m_camera.get(), Light::LightDir, false);
}
