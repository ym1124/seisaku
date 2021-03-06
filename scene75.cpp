#pragma once

//#include  <string>
#include "scenes.h"
#include "scene_manager.h"
#include "framework.h"
#include <array>

#define BLUR 0
#define PSSSR 0
#define PSSSAO 0

//#include "collision.h"
//#include "input.h"
//#include "multi_thread.h"

//コンピュートシェーダーでSSAO書き換え

//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void scene75::Initialize()
{
	f_buffer = std::make_unique<frame_buffer>();
	status = std::make_unique<_status>();
	m_blur = std::make_unique<blur>();
	m_deffered = std::make_unique<deffered>();

	m_ssr_cs = std::make_unique<postfx_cs>(L"Shader/ssr_cs.hlsl", sizeof(ConstantBufferForSSR));
	m_ssao_cs = std::make_unique<postfx_cs>(L"Shader/ssao_cs.hlsl", sizeof(ConstantBufferForSSAO));


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

	//fx//こっちの方が早い！！
	ssr_shader = std::make_unique<shader>();
	ssr_shader->Create(L"Shader/ssr.fx", "VSMain", "PSMain");
	ssr_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForSSR));

	ssr_composite_shader = std::make_unique<shader>();
	ssr_composite_shader->Create(L"Shader/ssr_composition.fx", "VSMain", "PSMain");


	Light::SetDirLight(DirectX::XMFLOAT3(0.6f, -0.6f, 0.6f), DirectX::XMFLOAT3(1, 1, 1));

	////コンピュートシェーダー作成
	//compute_shader = std::make_unique<shader>();
	//compute_shader->CreateCS(L"Shader/ssao_cs.hlsl", "CSMain");
	//compute_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForSSAO));

	{
		////シェーダの出力先の作成
		//D3D11_TEXTURE2D_DESC desc = {};
		//desc.Width = 1280;
		//desc.Height = 720;
		//desc.MipLevels = 1;
		//desc.ArraySize = 1;
		//desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//desc.SampleDesc.Count = 1;
		//desc.SampleDesc.Quality = 0;
		//desc.Usage = D3D11_USAGE_DEFAULT;
		//desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		//desc.CPUAccessFlags = 0;
		//desc.MiscFlags = 0;
		//auto hr = FRAMEWORK.getDevice()->CreateTexture2D(&desc, nullptr, Texture2D.GetAddressOf());
		//if (FAILED(hr)) {
		//	throw std::runtime_error("コンピュータシェーダの出力先用のID3D11Texture2Dの作成に失敗");
		//}

		////★toshiteru
		////hr = FRAMEWORK.getDevice()->CreateUnorderedAccessView(Texture2D.Get(), nullptr, mpScreenUAV.GetAddressOf());
		////if (FAILED(hr)) {
		////	throw std::runtime_error("コンピュータシェーダの出力先用のID3D11Texture2DのUnorderedAccessViewの作成に失敗");
		////}

		////D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		////srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		////srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		////srvDesc.Texture2D.MostDetailedMip = 0;
		////srvDesc.Texture2D.MipLevels = 1;
		////hr = FRAMEWORK.getDevice()->CreateShaderResourceView(Texture2D.Get(), &srvDesc, cssrv.GetAddressOf());
		////if (FAILED(hr)) {
		////	throw std::runtime_error("コンピュータシェーダの出力先用のID3D11Texture2DのUnorderedAccessViewの作成に失敗");
		////}


		////★monsho
		//D3D11_SHADER_RESOURCE_VIEW_DESC	srvDesc;
		//ZeroMemory(&srvDesc, sizeof(srvDesc));
		//srvDesc.Format = desc.Format;
		//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		//srvDesc.Texture2D.MostDetailedMip = 0;
		//srvDesc.Texture2D.MipLevels = desc.MipLevels;
		//hr = FRAMEWORK.getDevice()->CreateShaderResourceView(Texture2D.Get(), &srvDesc, cssrv.GetAddressOf());
		//if (FAILED(hr))
		//{
		//	return;
		//}

		//// UAVを作成する
		//D3D11_UNORDERED_ACCESS_VIEW_DESC	uavDesc;
		//ZeroMemory(&uavDesc, sizeof(uavDesc));
		//uavDesc.Format = desc.Format;
		//uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		//uavDesc.Texture2D.MipSlice = 0;
		//hr = FRAMEWORK.getDevice()->CreateUnorderedAccessView(Texture2D.Get(), &uavDesc, mpScreenUAV.GetAddressOf());
		//if (FAILED(hr))
		//{
		//	return;
		//}
	}

	//テクスチャ生成
	texture[0] = std::make_unique<Texture>();
	texture[1] = std::make_unique<Texture>();
	//texture[1]->Load(L"images/hdr_test4.jpg");

	//スプライト
	spr = std::make_unique<sprite>();


	//player
	player = std::make_unique<my_mesh>();
	player->initialize();
	player->load("FBX/Player/Player_Res.bin");
	player->setAllMaterialTex(L"FBX/Texture/Player/Player_B.png");
	player->scale = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	anim = std::make_unique<animation>("");
	anim->add(player->_model.get(), "FBX/Player/player_idle2.bin", "Idle");
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
	for (int i = 0; i < kernelSize; ++i)
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

void scene75::Update(float elapsedTime)
{
	HRESULT hr;
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
		ImGui::Checkbox("ssao on/off", &status->ssao_flg);
		ImGui::Image(tex1, ImVec2(250, 250));
		ImGui::Image(tex2, ImVec2(250, 250));
		ImGui::Image(tex3, ImVec2(250, 250));
		ImGui::Image(tex4, ImVec2(250, 250));
		ImGui::NewLine();

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
	ImGui::End();
#endif
	//view = cssrv.Get();
	//{
	//	//シェーダー実行
	//	FRAMEWORK.getImmediateContext()->CSSetShader(compute_shader.get()->cs.Get(), nullptr, 0);//シェーダーセット
	//	//FRAMEWORK.getImmediateContext()->CSSetShaderResources(0, 1, texture->GetShaderResourceViewPointer());//SRVセット
	//	FRAMEWORK.getImmediateContext()->CSSetUnorderedAccessViews(0, 1, &g_pBufUAV, nullptr);//UAVセット

	//	//スレッドグループをいくつ立てるか指定
	//	FRAMEWORK.getImmediateContext()->Dispatch(1280/1, 720/1, 1);

	//	//無効化
	//	ID3D11UnorderedAccessView*  pNullUAVs[1] = { nullptr };
	//	//ID3D11ShaderResourceView*   pNullSRVs[1] = { nullptr };
	//	ID3D11Buffer* pNullCBs[1] = { nullptr };
	//	FRAMEWORK.getImmediateContext()->CSSetShader(nullptr, nullptr, 0);
	//	FRAMEWORK.getImmediateContext()->CSSetUnorderedAccessViews(0, 1, pNullUAVs, nullptr);
	//	//FRAMEWORK.getImmediateContext()->CSSetShaderResources(0, 1, pNullSRVs);
	//}

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
	tex4 = m_blur->getSceneTexture()->GetShaderResourceView();

	timer++;
	if (timer > UINT_MAX)
		timer = 0;

	//更新
	player->pos = DirectX::XMFLOAT3(0.0f, -5.0f, status->player_pos_z);
	anim->update(player->_model.get(), true);
	anim->set("Idle");
	scene_manager::getInstance().SceneJump();
}

//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void scene75::Render(float elapsed_time)
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
	//指定色で画面クリア
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
	FRAMEWORK.getImmediateContext()->ClearRenderTargetView(framework::getInstance().getRenderTargetView(), ClearColor);//何色でクリアするか
	//深度ステンシルビュークリア
	FRAMEWORK.getImmediateContext()->ClearDepthStencilView(framework::getInstance().getDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);//クリアする値
	//レンダーターゲットビューを登録
	FRAMEWORK.getImmediateContext()->OMSetRenderTargets(1, &framework::getInstance().render_target_view, framework::getInstance().getDepthStencilView());//レンダーターゲットにもDepthStencilView教えないとダメ
	//ブレンドステート
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0xFFFFFFFF);

	//オブジェクト描画
	player->render(fbx_m_shader.get(), m_camera.get(), Light::LightDir);
	stage->render(fbx_m_shader.get(), m_camera.get(), Light::LightDir);

	//ground[0]->render(FRAMEWORK.getImmediateContext(), m_shader.get(), m_camera.get());
	//ground[1]->render(FRAMEWORK.getImmediateContext(), m_shader.get(), m_camera.get());
	//ground[2]->render(FRAMEWORK.getImmediateContext(), m_shader.get(), m_camera.get());

	//g_bufferへの書きだし
	m_deffered->begine(f_buffer.get());
	player->render(m_deffered->getGBufferShaderEX(), m_camera.get(), Light::LightDir);
	stage->render(m_deffered->getGBufferShaderEX(), m_camera.get(), Light::LightDir);
	//ground[0]->render(FRAMEWORK.getImmediateContext(), m_deffered->getGBufferShader(), m_camera.get());
	//ground[1]->render(FRAMEWORK.getImmediateContext(), m_deffered->getGBufferShader(), m_camera.get());
	//ground[2]->render(FRAMEWORK.getImmediateContext(), m_deffered->getGBufferShader(), m_camera.get());
	m_deffered->end(f_buffer.get());


	if (status->ssr_flg)
	{
#if PSSSR
		//スペキュラマップ
		f_buffer->swapRenderTargetViewBegin(specularmap_tex.get());
		stage->render(fbx_m_shader.get(), m_camera.get(), Light::LightDir);
		f_buffer->swapRenderTargetViewEnd();

		//非線形深度マップ
		f_buffer->createDepthMapBegin(depth_tex.get());
		player->render(fbx_m_shader.get(), m_camera.get(), Light::LightDir);
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
		m_deffered->getGbuffer()->tex_buffer[1]->Set(1);//法線マップ
		m_deffered->getGbuffer()->tex_buffer[0]->Set(2);//シーンカラー
													   //pbr_tex->Set(3);
		specularmap_tex->Set(3);
		spr->render(ssr_shader.get());
		f_buffer->swapRenderTargetViewEnd();

		{//ブラー掛けるならコッチ
		 //m_blur->setSceneTex(result_tex.get());
		 //m_blur->render(f_buffer.get(), 2.0f, nullptr, false, true);
		}

		////ブラー
		////composite//specular反映
		//result_tex->Set(0);
		////m_blur->getResultTexture()->Set(0);//ブラー掛けるならコッチ
		//specularmap_tex->Set(1);
		//m_deffered->getGbuffer()->tex_buffer[0]->Set(2);
		//spr->render(ssr_composite_shader.get());

		//結果を直接描画
		spr->render(_2d_shader.get(), result_tex.get());

#else
		/////////////////////////////////////////////////////////////////////////////////

		//スペキュラマップ
		f_buffer->swapRenderTargetViewBegin(specularmap_tex.get());
		stage->render(fbx_m_shader.get(), m_camera.get(), Light::LightDir);
		f_buffer->swapRenderTargetViewEnd();

		//非線形深度マップ
		f_buffer->createDepthMapBegin(depth_tex.get());
		player->render(fbx_m_shader.get(), m_camera.get(), Light::LightDir);
		stage->render(fbx_m_shader.get(), m_camera.get(), Light::LightDir);
		f_buffer->createDepthMapEnd();

		//SSR開始
		ConstantBufferForSSR cbssr;
		cbssr.raytrace_max_length = status->max_length;
		cbssr.raytrace_max_thickness = status->max_thickness;
		cbssr.reflection_enhancer = status->reflection_enhancer;
		cbssr.timer = timer;
		m_ssao_cs->updateConstantBuffer(cbssr);

		//SSR結果描画
		//swapRenderTargetViewBeginの中でSRVnullセットするからテクスチャのセットはその後で行う↓
		//m_defferd->getGbuffer()->tex_buffer[3]->Set(0);//深度マップ



		//srvセット
		ID3D11ShaderResourceView *srvs[] =
		{
			depth_tex->GetShaderResourceView(),
			m_deffered->getGbuffer()->tex_buffer[1]->GetShaderResourceView(),//法線マップ
			m_deffered->getGbuffer()->tex_buffer[0]->GetShaderResourceView(),//シーンカラー
			specularmap_tex->GetShaderResourceView(),
		};
		m_ssr_cs->setSRVs(srvs,4);
		m_ssr_cs->render();

		result_tex->SetShaderResourceView(m_ssr_cs->getSRV());
#if BLUR
		{//ブラー掛けるならコッチ
		 m_blur->setSceneTex(result_tex.get());
		 m_blur->render(f_buffer.get(), 2.0f, nullptr, false, true);
		}
		//ブラーあり
		spr->render(_2d_shader.get(), m_blur->getResultTexture());

#else
		//ブラーなし
		spr->render(_2d_shader.get(), result_tex.get());
#endif
#endif
	}



	if (status->ssao_flg)
	{
		cbssao.hemi_radius = status->hemi_radius;
		cbssao.kernel_size = status->kernel_size;
		cbssao.bias = status->bias;

#if PSSSAO
		//ps
		{
			UpdateConstantBuffer(ssao_shader.get(), cbssao);


			f_buffer->swapRenderTargetViewBegin(ssao_tex.get());
			//swapRenderTargetViewBeginの中でSRVnullセットするからテクスチャのセットはその後で行う
			m_deffered->getGbuffer()->tex_buffer[2]->Set(1);//world_pos
			m_deffered->getGbuffer()->tex_buffer[1]->Set(2);//world_normal
			spr->render(ssao_shader.get(), m_deffered->getGbuffer()->tex_buffer[0].get());
			f_buffer->swapRenderTargetViewEnd();

			m_blur->setSceneTex(ssao_tex.get());
			m_blur->render(f_buffer.get(), status->variance, nullptr, false, false, 4, 5);

		}
#else

		//cs
		{
			//FRAMEWORK.getImmediateContext()->UpdateSubresource(compute_shader->ConstantBufferOriginal.Get(), 0, NULL, &cbssao, 0, 0);
			//FRAMEWORK.getImmediateContext()->CSSetConstantBuffers(2, 1, compute_shader->ConstantBufferOriginal.GetAddressOf());

			////実行するシェーダをGPUに設定する
			//FRAMEWORK.getImmediateContext()->CSSetShader(compute_shader.get()->cs.Get(), nullptr, 0);

			////シェーダのscreenとして扱うリソースを設定する。
			//std::array<ID3D11UnorderedAccessView*, 1> ppUAVs = { { this->mpScreenUAV.Get(), } };
			//std::array<UINT, 1> initCounts = { { 0u, } };
			//FRAMEWORK.getImmediateContext()->CSSetUnorderedAccessViews(0, static_cast<UINT>(ppUAVs.size()), ppUAVs.data(), initCounts.data());
			//
			////srvセット
			//ID3D11ShaderResourceView *srvs[] =
			//{
			//	m_deffered->getGbuffer()->tex_buffer[0]->GetShaderResourceView(),
			//	m_deffered->getGbuffer()->tex_buffer[2]->GetShaderResourceView(),
			//	m_deffered->getGbuffer()->tex_buffer[1]->GetShaderResourceView(),
			//};
			//FRAMEWORK.getImmediateContext()->CSSetShaderResources(0, 3, srvs);

			////CSの実行
			////AMD製ならグループスレッドは64の倍数
			////NVIDIA製なら32の倍数が最速
			//FRAMEWORK.getImmediateContext()->Dispatch(1280 / 32, 720 / 32, 1);

			//ID3D11ShaderResourceView*	pReses = NULL;
			//FRAMEWORK.getImmediateContext()->CSSetShaderResources(0, 1, &pReses);
			//FRAMEWORK.getImmediateContext()->CSSetShaderResources(1, 1, &pReses);
			//FRAMEWORK.getImmediateContext()->CSSetShaderResources(2, 1, &pReses);
			//ID3D11UnorderedAccessView*	pUAV = NULL;
			//FRAMEWORK.getImmediateContext()->CSSetUnorderedAccessViews(0, 1, &pUAV, (UINT*)&pUAV);

			//ssao_tex->SetShaderResourceView(cssrv.Get());
			//_blur->setSceneTex(ssao_tex.get());
			//_blur->render(f_buffer.get(), status->variance, nullptr, false, false, 4, 5);

			////////////////////////////


			m_ssao_cs->updateConstantBuffer(cbssao);
			//srvセット
			ID3D11ShaderResourceView *srvs[] =
			{
				m_deffered->getGbuffer()->tex_buffer[0]->GetShaderResourceView(),
				m_deffered->getGbuffer()->tex_buffer[2]->GetShaderResourceView(),
				m_deffered->getGbuffer()->tex_buffer[1]->GetShaderResourceView(),
			};
			m_ssao_cs->setSRVs(srvs,3);
			m_ssao_cs->render();

			ssao_tex->SetShaderResourceView(m_ssao_cs->getSRV());
			m_blur->setSceneTex(ssao_tex.get());
			m_blur->render(f_buffer.get(), status->variance, nullptr, false, false, 4, 5);
		}

#endif


		FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_MULTIPLY), nullptr, 0xFFFFFFFF);
		spr->render(_2d_shader.get(), ssao_tex.get());
	}

}