#pragma once
#include "scene.h"

class sprite;
class shader;

class scene59 :public scene
{
private:
	float timer;
	std::unique_ptr<sprite> spr;

	struct _status
	{
		float camera_distance = 20.0f;
		float target_depth = 1.3f;
		float transmittance = 1.0f;
	};
	std::unique_ptr<_status> status;
public:
	scene59() {}
	virtual ~scene59() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;

	//ストリング
	char message[256] = { 0 };
	//シェーダー
	std::unique_ptr<shader> cloud_shader;
};