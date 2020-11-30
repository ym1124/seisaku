#pragma once
#include "scenes.h"

class scene58 :public scene
{
private:
	int timer;
	std::unique_ptr<sprite> spr;

	struct _status
	{
		float sea_base[3] = { 0.1f, 0.19f, 0.22f };
		float ray_origin[3] = { 0.0f, 3.5f, 5.0f };
		float sea_height = 0.6f;
		float sea_freq = 0.16f;
		float choppy = 4.0f;//�O�p�g
		float speed = 0.04f;
		float sea_water_color[3] = { 0.8f, 0.9f, 0.6f };
	};
	std::unique_ptr<_status> status;
public:
	scene58() {}
	virtual ~scene58() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render(float elapsedTime) override;

	//�X�g�����O
	char message[256] = { 0 };

	//�V�F�[�_�[
	std::unique_ptr<shader> sea_shader;
	//�J����
	std::unique_ptr<main_camera> m_camera;
};