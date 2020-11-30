#pragma once
#include "scenes.h"
//#include "multi_thread.h"

class scene_load : public scene
{
public:
	std::unique_ptr<scene> nextScene;

	//SceneLoad *temp;
	std::unique_ptr<sprite> img;
	std::unique_ptr<sprite> font;
private:
	//NowLoadingアニメーション用変数
	float timer = 0.0f;
	int count = 0;
	char message[32] = { "aiueo" };

public:
	scene_load(scene *nextScene)
	{
		//ローディング後に遷移するシーン
		//this->nextScene.release();
		this->nextScene.reset(nextScene);
	};

	virtual ~scene_load() { };

	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;

public:
	static void LoadingThread(scene_load* Lscene);
};