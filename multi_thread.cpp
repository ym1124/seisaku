//#include "multi_thread.h"
#include "framework.h"
#include <thread>

//使うのやめたらメモリリーク200byteが２個と124byteが1個減った
//imguiと相性悪い

void scene_load::Initialize()
{
	//img = std::make_unique<sprite>(FRAMEWORK.getDevice(), L"n64.png");
	//font = std::make_unique<sprite>(FRAMEWORK.getDevice(), L"n64.png");

	//ここでスレッドを作って、ロードするスレッドとロード中描画と更新するスレッドに分かれる
	std::thread thread(LoadingThread, this);
	thread.detach();//ブロックを出た後も立てたスレッドが処理を続けるようにするのに必要
}

void scene_load::Update(float elapsedTime)
{
	//****ロード中にこれを行う*****//
	if (nextScene->initialized)//次のシーンの初期化が完了していたら・・・
	{
		/*if (SceneManager::Instance().temp == nullptr)
		{
		SceneManager::Instance().temp = SceneManager::Instance().currentScene;
		SceneManager::Instance().currentScene = nextScene.get();
		}*/

		//SceneManager::Instance().ChangeScene(this);

		scene_manager::getInstance().currentScene.release();//現在ManagerのCurrentに入ってるSceneLoadの所有権を解放する！！ポインタの破棄はしてない！！
		scene_manager::getInstance().currentScene.reset(nextScene.get());//ManagerのCurrentにSceneLoadが持っている次のシーンを入れる
	}

	// ローディング演出処理
	timer -= elapsedTime;
	if (timer < 0)
	{
		timer = 0.1f;

		strcpy_s(message, "Now Loading");
		count = (count + 1) % 3;
		for (int i = 0; i < count; ++i)
		{
			strcat_s(message, ".");
		}
	}
}

void scene_load::Render(float elapsedTime)
{
	//****ロード中にこれを行う*****//

	font = std::make_unique<sprite>();
	img = std::make_unique<sprite>();

	float screenWidth = static_cast<FLOAT>(FRAMEWORK.getInstance().SCREEN_WIDTH);
	float screenHeight = static_cast<FLOAT>(FRAMEWORK.getInstance().SCREEN_HEIGHT);

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = screenWidth;
	viewport.Height = screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	FRAMEWORK.getInstance().getImmediateContext()->RSSetViewports(1, &viewport);

	FLOAT color[] = { 0.4f, 0.4f, 0.9f, 1.0f };
	ID3D11RenderTargetView* rtv = FRAMEWORK.getInstance().getRenderTargetView();
	ID3D11DepthStencilView* dsv = FRAMEWORK.getInstance().getDepthStencilView();
	FRAMEWORK.getInstance().getImmediateContext()->ClearRenderTargetView(rtv, color);
	FRAMEWORK.getInstance().getImmediateContext()->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	FRAMEWORK.getInstance().getImmediateContext()->OMSetRenderTargets(1, &rtv, dsv);


	//ID3D11BlendState* bs = framework.GetBlendState(Blender::BS_ALPHA);
	//context->OMSetBlendState(bs, nullptr, 0xFFFFFFFF);

	//Now Loading…描画
	//font->TextOut(FRAMEWORK.getInstance().getImmediateContext(), message, 200, 300, 32, 32);
	//img->render(FRAMEWORK.getImmediateContext(), 100, 100, 100, 100, 100, 100, 100, 100, 1, 1, 1, 1, 1.0);
}

void scene_load::LoadingThread(scene_load* Lscene)
{
	//次のシーンを初期化
	Lscene->nextScene->Initialize();//	こっち通してる間にSceneLoadのUpdateとRenderをやる
	Lscene->nextScene->initialized = true;
}
