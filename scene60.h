#pragma once
#include "scene.h"

class sprite;
class shader;

class scene60 :public scene
{
private:
	int timer;
	std::unique_ptr<sprite> spr;

	struct _status
	{
		float sun_pos[2] = { 0.5f,0.3f };
		float sea_base[3] = { 0.1f, 0.19f, 0.22f };
		float ray_origin[3] = { 0.0f, 3.5f, 5.0f };
		float sea_height = 0.6f;
		float sea_freq = 0.16f;
		float choppy = 4.0f;//三角波
		float speed = 0.04f;
		float sea_water_color[3] = { 0.8f, 0.9f, 0.6f };
	};
	std::unique_ptr<_status> status;
public:
	scene60() {}
	virtual ~scene60() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;

	//ストリング
	char message[256] = { 0 };

	//シェーダー
	std::unique_ptr<shader> sky_shader, sea_shader, lens_shader;
};