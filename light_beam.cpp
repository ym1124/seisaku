#include "light_beam.h"
#include "framework.h"
#include "frame_buffer.h"
#include "shader.h"
#include "sprite.h"

light_beam::light_beam()
{
	//�V�F�[�_�[
	irradiation_shader = std::make_unique<shader>();
	irradiation_shader->Create(L"irradiation.fx", "VSMain", "PSMain");
	irradiation_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForIrradiation));

	bright_shader = std::make_unique<shader>();
	bright_shader->Create(L"brightness_extraction.fx", "VSMain", "PSMain");
	bright_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForBrightnessExtraction));

	_2d_shader = std::make_unique<shader>();
	_2d_shader->Create(L"2d.fx", "VSMain", "PSMain");

	//�X�v���C�g
	spr = std::make_unique<sprite>();

	//�V�[���p�̃e�N�X�`��
	scene_tex = std::make_unique<Texture>();

	//�P�x���o�e�N�X�`��
	bright_tex = std::make_unique<Texture>();

	//�u���[�p�e�N�X�`���o�b�t�@
	for (auto &it : blur_tex)
	{
		it = std::make_unique<Texture>();
	}
	//��䊕`���p�̃e�N�X�`��
	for (auto &it : irradiation_tex)
	{
		it = std::make_unique<Texture>();
	}
}

void light_beam::setSceneTexBegine(frame_buffer *f_buffer)
{
	f_buffer->swapRenderTargetViewBegin(scene_tex.get(), Light::LightDir, FRAMEWORK.BS::BS_ALPHA, DirectX::XMFLOAT2(1280, 720), DirectX::XMFLOAT4(0, 0, 0, 1), true);
}

void light_beam::setSceneTexEnd(frame_buffer *f_buffer)
{
	f_buffer->swapRenderTargetViewEnd();
}

void light_beam::setSceneTexture(Texture* scene_tex)
{
	this->scene_tex->SetShaderResourceView(scene_tex->GetShaderResourceView());
}

void light_beam::render(frame_buffer *f_buffer, float threshold, DirectX::XMFLOAT2 screen_size)
{
	//���P�x���o��
	ConstantBufferForBrightnessExtraction cbbe;
	cbbe.threshold = threshold;
	UpdateConstantBuffer(bright_shader.get(), cbbe);
	f_buffer->swapRenderTargetViewBegin(bright_tex.get(), Light::LightDir, FRAMEWORK.BS::BS_ALPHA, DirectX::XMFLOAT2(1280, 720),DirectX::XMFLOAT4(0, 0, 0, 1), true);
	spr->render(bright_shader.get(), scene_tex.get(), 0, 0, screen_size.x, screen_size.y, 0, 0, screen_size.x, screen_size.y);
	f_buffer->swapRenderTargetViewEnd();

	//������
	float attenuation = 0.925f; //0.9f~0.95f�͈̔�

	//�p�x�I�t�Z�b�g
	float rad_offset = DirectX::XM_PI / 4.0f;

	// �����_�[�^�[�Q�b�g�T�C�Y�̋t��
	DirectX::XMFLOAT2 inv_size(1.0f / irradiation_tex[0]->GetWidth(), 1.0f / irradiation_tex[0]->GetHeight());

	//Texture* temp_tex = new Texture();
	//temp_tex->SetShaderResourceView(tex.get()->GetShaderResourceView());

	//�~���S�������Č�䊂��o��
	for (int j = 0; j < 4; ++j)
	{
		//���炷��������
		DirectX::XMFLOAT2 dir(cosf((DirectX::XM_PI * 2.0f) * 0.25f * j + rad_offset), sinf((DirectX::XM_PI * 2.0f) * 0.25f * j + rad_offset));

		//3�p�X�u���[
		for (int i = 0; i < 3; ++i)
		{
			//�u���[�̃I�t�Z�b�g�v�Z
			ConstantBufferForIrradiation cbi;
			for (auto s = 0; s < 16; ++s)
			{
				auto b = powf(4.0f, float(i));
				cbi.Offset[s].x = dir.x * (b * s) * inv_size.x;
				cbi.Offset[s].y = dir.y * (b * s) * inv_size.y;
				cbi.Offset[s].z = powf(attenuation, (b * s));
			}
			UpdateConstantBuffer(irradiation_shader.get(), cbi);

			//�e�N�X�`���Ƀu���[
			f_buffer->swapRenderTargetViewBegin(blur_tex[i].get(), Light::LightDir, FRAMEWORK.BS::BS_ALPHA, DirectX::XMFLOAT2(1280, 720),DirectX::XMFLOAT4(0, 0, 0, 1), true);
			spr->render(irradiation_shader.get(), bright_tex.get(), 0, 0, 1280, 720, 0, 0, 1280, 720);
			f_buffer->swapRenderTargetViewEnd();
		}

		//�P�����̌�䊂����Z����
		f_buffer->swapRenderTargetViewBegin(irradiation_tex[j].get(), Light::LightDir, FRAMEWORK.BS::BS_ADD, DirectX::XMFLOAT2(1280, 720), DirectX::XMFLOAT4(0, 0, 0, 1), true);
		for (auto &it : blur_tex)
		{
			spr->render(_2d_shader.get(), blur_tex[0].get(), 0, 0, 1280, 720, 0, 0, 1280, 720);
		}
		f_buffer->swapRenderTargetViewEnd();
	}

	//4�{�̌�䊂ƌ��摜���Z����
	//spr->render(_2d_shader.get(), scene_tex.get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);//���摜�̏ォ�珑���ꍇ�͂�����L����
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ADD), nullptr, 0xFFFFFFFF);
	spr->render(_2d_shader.get(), irradiation_tex[0].get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
	spr->render(_2d_shader.get(), irradiation_tex[1].get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
	spr->render(_2d_shader.get(), irradiation_tex[2].get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
	spr->render(_2d_shader.get(), irradiation_tex[3].get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
}