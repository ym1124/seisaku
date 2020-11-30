#include <random>
//
#include "ssao.h"
#include "framework.h"
#include "camera.h"
#include "g_buffer.h"
#include "deffered.h"
#include "texture.h"
#include "shader.h"
#include "frame_buffer.h"
#include "blur.h"
#include "sprite.h"



ssao::ssao(deffered *deffered)
{
	ssao_shader = std::make_unique<shader>();
	ssao_shader->Create(L"ssao_v3.fx", "VSMain", "PSMain");
	ssao_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForSSAO));
	ssao_tex = std::make_unique<Texture>();
	ssao_deffered = deffered;
	_blur = std::make_unique<blur>();
	spr = std::make_unique<sprite>();

	//半球カーネル//使うのはxyz部分だけ
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
	std::default_random_engine generator;
	const int kernelSize = 64;
	DirectX::XMFLOAT4 kernel[kernelSize];
	for (int i = 0; i <kernelSize; ++i)
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

void ssao::render(frame_buffer *f_buffer,shader *_2d_shader,float hemi_radius, float bias, float variance,bool synthesize)
{
	cbssao.hemi_radius = hemi_radius;
	cbssao.bias = bias;
	cbssao.kernel_size = 64;
	UpdateConstantBuffer(ssao_shader.get(), cbssao);

	//ssaoテクスチャ
	ssao_deffered->getGbuffer()->tex_buffer[2]->Set(1);//world_pos
	ssao_deffered->getGbuffer()->tex_buffer[1]->Set(2);//world_normal
	f_buffer->swapRenderTargetViewBegin(ssao_tex.get());
	spr->render(ssao_shader.get(), ssao_deffered->getGbuffer()->tex_buffer[0].get());
	f_buffer->swapRenderTargetViewEnd();

	//ブラー掛けた
	_blur->setSceneTex(ssao_tex.get());
	_blur->render(f_buffer, variance, nullptr, false, false, FRAMEWORK.SM::LINEAR_CLAMP, FRAMEWORK.BS::BS_MULTIPLY);//result_flg=true

	////ssao_tex.get()->Set(1);
	////spr->render(ssao_mul_shader.get(), tex_buffer[0].get());

	//if(synthesize)
	//	spr->render(_2d_shader, ssao_deffered->getGbuffer()->tex_buffer[0].get());
	////合成(元シーンは他で描画する方がいい)
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_MULTIPLY), nullptr, 0xFFFFFFFF);
	spr->render(_2d_shader, ssao_tex.get());
}


//****************シーンテクスチャもらえる場合*********************
void ssao::setGBuffer(std::unique_ptr<Texture> buffer[4])
{
	for (int i = 0; i < 4; i++)
	{
		ssao_deffered->getGbuffer()->setSRV(buffer[i].get()->GetShaderResourceView(), i);
	}
}

void ssao::setDeffered(deffered* def)
{
	ssao_deffered = def;
}
