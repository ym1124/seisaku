#include "blur.h"
#include "framework.h"
#include "sprite.h"
#include "texture.h"
#include "frame_buffer.h"
#include "shader.h"

blur::blur()
{
	//�V�F�[�_�[�n����
	blur_shader_horizontal = std::make_unique<shader>();
	blur_shader_horizontal->Create(L"Shader/gaussian_blur_horizontal");
	blur_shader_horizontal->CreateConstantBufferOriginal(sizeof(ConstantBufferForGaussianBlur));

	blur_shader_vertical = std::make_unique<shader>();
	blur_shader_vertical->Create(L"Shader/gaussian_blur_vertical");
	blur_shader_vertical->CreateConstantBufferOriginal(sizeof(ConstantBufferForGaussianBlur));

	//�e�N�X�`���E�X�v���C�g����
	scene_tex = std::make_unique<Texture>();
	blur_tex = std::make_unique<Texture>();
	result_tex = std::make_unique<Texture>();

	spr = std::make_unique<sprite>();
}

void  blur::setSceneTexBegine(frame_buffer *f_buffer)
{
	f_buffer->swapRenderTargetViewBegin(scene_tex.get(), Light::LightDir, FRAMEWORK.BS::BS_ALPHA, DirectX::XMFLOAT2(1280, 720),DirectX::XMFLOAT4(0, 0, 0, 1), true);
}

void  blur::setSceneTexEnd(frame_buffer *f_buffer)
{
	f_buffer->swapRenderTargetViewEnd();
}

void blur::setSceneTex(Texture* tex)
{
	scene_tex->SetShaderResourceView(tex->GetShaderResourceView());
}

void blur::render(frame_buffer *f_buffer, float variance, Texture *tex, bool bloom_flg, bool result_flg, int sampler_type, int blend_type, int weight_num)
{
	//�K�E�X�u���[�v�Z
	CalcGaussWeights(variance*variance, weight_array, weight_num);

	//�K�E�X�u���[�ݒ�
	ConstantBufferForGaussianBlur cbgb;
	for (int i = 0; i < 8; i++)
	{
		cbgb.Weights[i] = weight_array[i];
	}
	cbgb.Size.x = FRAMEWORK.SCREEN_WIDTH;
	cbgb.Size.y = FRAMEWORK.SCREEN_HEIGHT;
	cbgb.Offset.x = 16.0f / FRAMEWORK.SCREEN_WIDTH;
	cbgb.Offset.y = 16.0f / FRAMEWORK.SCREEN_HEIGHT;
	UpdateConstantBuffer(blur_shader_horizontal.get(), cbgb);

	//********************************************************
	//	f_buffer->swapRenderTargetViewBegin(scene_tex.get(), Light::LightDir);
	//_blur->render(f_buffer.get(), status->variance);
	//f_buffer->swapRenderTargetViewEnd();
	//���̌`�ɂȂ��IMGUI�\������Ȃ��Ȃ��Ă��܂��̂ŋC��t����
	//��result_flg��true�Ȃ猋�ʂ��o�͂ł���
	//**********************************************************
	//�������Ƀu���[
	if (tex == nullptr)//���Z�b�g�Ȃ玩���̎����Ă��Ƀu���[
	{
		f_buffer->swapRenderTargetViewBegin(blur_tex.get(), Light::LightDir, FRAMEWORK.BS::BS_ALPHA, DirectX::XMFLOAT2(1280, 720),DirectX::XMFLOAT4(0, 0, 0, 1), true);
		spr->render(blur_shader_horizontal.get(), scene_tex.get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 1.0f, 0, sampler_type);
		f_buffer->swapRenderTargetViewEnd();
	}
	else//�w�肳�ꂽ�e�N�X�`��
	{
		f_buffer->swapRenderTargetViewBegin(blur_tex.get(), Light::LightDir, FRAMEWORK.BS::BS_ALPHA, DirectX::XMFLOAT2(1280, 720),DirectX::XMFLOAT4(0, 0, 0, 1), true);
		spr->render(blur_shader_horizontal.get(), tex, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 1.0f, 0, sampler_type);
		f_buffer->swapRenderTargetViewEnd();
	}

	if (!bloom_flg)
	{
		if (result_flg)
		{
			f_buffer->swapRenderTargetViewBegin(result_tex.get(), Light::LightDir, blend_type, DirectX::XMFLOAT2(1280, 720),DirectX::XMFLOAT4(0, 0, 0, 1), true);
			spr->render(blur_shader_vertical.get(), blur_tex.get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 1.0f, 0, sampler_type);
			f_buffer->swapRenderTargetViewEnd();
		}
		else
		{
			//�������Ƀu���[�����摜�ɏc�����u���[�����Č��摜�ƍ���
			UpdateConstantBuffer(blur_shader_vertical.get(), cbgb);
			FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(blend_type), nullptr, 0xFFFFFFFF);
			spr->render(blur_shader_vertical.get(), blur_tex.get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT,1.0f,0,sampler_type);
		}
	}

	else
	{
		if (result_flg)
		{
			f_buffer->swapRenderTargetViewBegin(result_tex.get(), Light::LightDir, blend_type, DirectX::XMFLOAT2(1280, 720), DirectX::XMFLOAT4(0, 0, 0, 1), true);
			spr->render(blur_shader_vertical.get(), blur_tex.get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 1.0f, 0, sampler_type);
			f_buffer->swapRenderTargetViewEnd();
		}
		else
		{
			//�������Ƀu���[�����摜�ɏc�����u���[�����Č��摜�Ɖ��Z����(�u���[���Ȃ炱����)
			UpdateConstantBuffer(blur_shader_vertical.get(), cbgb);
			FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ADD), nullptr, 0xFFFFFFFF);
			spr->render(blur_shader_vertical.get(), blur_tex.get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 1.0f, 0, sampler_type);
		}
	}
}

void blur::render(frame_buffer *f_buffer, float variance, Texture_d *tex, bool result_flg, int weight_num)
{
	//�K�E�X�u���[�v�Z
	CalcGaussWeights(variance*variance, weight_array, weight_num);

	//�K�E�X�u���[�ݒ�
	ConstantBufferForGaussianBlur cbgb;
	for (int i = 0; i < 8; i++)
	{
		cbgb.Weights[i] = weight_array[i];
	}
	cbgb.Size.x = FRAMEWORK.SCREEN_WIDTH;
	cbgb.Size.y = FRAMEWORK.SCREEN_HEIGHT;
	cbgb.Offset.x = 16.0f / FRAMEWORK.SCREEN_WIDTH;
	cbgb.Offset.y = 16.0f / FRAMEWORK.SCREEN_HEIGHT;
	UpdateConstantBuffer(blur_shader_horizontal.get(), cbgb);

	//********************************************************
	//	f_buffer->swapRenderTargetViewBegin(scene_tex.get(), Light::LightDir);
	//_blur->render(f_buffer.get(), status->variance);
	//f_buffer->swapRenderTargetViewEnd();
	//���̌`�ɂȂ��IMGUI�\������Ȃ��Ȃ��Ă��܂��̂ŋC��t����
	//��result_flg��true�Ȃ猋�ʂ��o�͂ł���
	//**********************************************************

	//�w�肳�ꂽ�e�N�X�`��

	f_buffer->swapRenderTargetViewBegin(blur_tex.get(), Light::LightDir, FRAMEWORK.BS::BS_ALPHA, DirectX::XMFLOAT2(1280, 720), DirectX::XMFLOAT4(0, 0, 0, 1), true);
	spr->render(blur_shader_horizontal.get(), tex, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
	f_buffer->swapRenderTargetViewEnd();

	if (result_flg)
	{
		f_buffer->swapRenderTargetViewBegin(result_tex.get(), Light::LightDir, FRAMEWORK.BS::BS_ALPHA, DirectX::XMFLOAT2(1280, 720), DirectX::XMFLOAT4(0, 0, 0, 1), true);
		spr->render(blur_shader_vertical.get(), blur_tex.get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
		f_buffer->swapRenderTargetViewEnd();
	}
	else
	{
		//�������Ƀu���[�����摜�ɏc�����u���[�����Č��摜�ƍ���
		UpdateConstantBuffer(blur_shader_vertical.get(), cbgb);
		FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0xFFFFFFFF);
		spr->render(blur_shader_vertical.get(), blur_tex.get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
	}
}