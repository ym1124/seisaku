//#include "multi_thread.h"
#include "framework.h"
#include <thread>

//�g���̂�߂��烁�������[�N200byte���Q��124byte��1������
//imgui�Ƒ�������

void scene_load::Initialize()
{
	//img = std::make_unique<sprite>(FRAMEWORK.getDevice(), L"n64.png");
	//font = std::make_unique<sprite>(FRAMEWORK.getDevice(), L"n64.png");

	//�����ŃX���b�h������āA���[�h����X���b�h�ƃ��[�h���`��ƍX�V����X���b�h�ɕ������
	std::thread thread(LoadingThread, this);
	thread.detach();//�u���b�N���o��������Ă��X���b�h�������𑱂���悤�ɂ���̂ɕK�v
}

void scene_load::Update(float elapsedTime)
{
	//****���[�h���ɂ�����s��*****//
	if (nextScene->initialized)//���̃V�[���̏��������������Ă�����E�E�E
	{
		/*if (SceneManager::Instance().temp == nullptr)
		{
		SceneManager::Instance().temp = SceneManager::Instance().currentScene;
		SceneManager::Instance().currentScene = nextScene.get();
		}*/

		//SceneManager::Instance().ChangeScene(this);

		scene_manager::getInstance().currentScene.release();//����Manager��Current�ɓ����Ă�SceneLoad�̏��L�����������I�I�|�C���^�̔j���͂��ĂȂ��I�I
		scene_manager::getInstance().currentScene.reset(nextScene.get());//Manager��Current��SceneLoad�������Ă��鎟�̃V�[��������
	}

	// ���[�f�B���O���o����
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
	//****���[�h���ɂ�����s��*****//

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

	//Now Loading�c�`��
	//font->TextOut(FRAMEWORK.getInstance().getImmediateContext(), message, 200, 300, 32, 32);
	//img->render(FRAMEWORK.getImmediateContext(), 100, 100, 100, 100, 100, 100, 100, 100, 1, 1, 1, 1, 1.0);
}

void scene_load::LoadingThread(scene_load* Lscene)
{
	//���̃V�[����������
	Lscene->nextScene->Initialize();//	�������ʂ��Ă�Ԃ�SceneLoad��Update��Render�����
	Lscene->nextScene->initialized = true;
}
