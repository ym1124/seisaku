#include "light_beam.h"
#include "framework.h"
#include "frame_buffer.h"
#include "shader.h"
#include "sprite.h"

light_beam::light_beam()
{
	//シェーダー
	irradiation_shader = std::make_unique<shader>();
	irradiation_shader->Create(L"irradiation.fx", "VSMain", "PSMain");
	irradiation_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForIrradiation));

	bright_shader = std::make_unique<shader>();
	bright_shader->Create(L"brightness_extraction.fx", "VSMain", "PSMain");
	bright_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForBrightnessExtraction));

	_2d_shader = std::make_unique<shader>();
	_2d_shader->Create(L"2d.fx", "VSMain", "PSMain");

	//スプライト
	spr = std::make_unique<sprite>();

	//シーン用のテクスチャ
	scene_tex = std::make_unique<Texture>();

	//輝度抽出テクスチャ
	bright_tex = std::make_unique<Texture>();

	//ブラー用テクスチャバッファ
	for (auto &it : blur_tex)
	{
		it = std::make_unique<Texture>();
	}
	//光芒描く用のテクスチャ
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
	//高輝度取り出し
	ConstantBufferForBrightnessExtraction cbbe;
	cbbe.threshold = threshold;
	UpdateConstantBuffer(bright_shader.get(), cbbe);
	f_buffer->swapRenderTargetViewBegin(bright_tex.get(), Light::LightDir, FRAMEWORK.BS::BS_ALPHA, DirectX::XMFLOAT2(1280, 720),DirectX::XMFLOAT4(0, 0, 0, 1), true);
	spr->render(bright_shader.get(), scene_tex.get(), 0, 0, screen_size.x, screen_size.y, 0, 0, screen_size.x, screen_size.y);
	f_buffer->swapRenderTargetViewEnd();

	//減衰率
	float attenuation = 0.925f; //0.9f~0.95fの範囲

	//角度オフセット
	float rad_offset = DirectX::XM_PI / 4.0f;

	// レンダーターゲットサイズの逆数
	DirectX::XMFLOAT2 inv_size(1.0f / irradiation_tex[0]->GetWidth(), 1.0f / irradiation_tex[0]->GetHeight());

	//Texture* temp_tex = new Texture();
	//temp_tex->SetShaderResourceView(tex.get()->GetShaderResourceView());

	//円周４分割して光芒を出す
	for (int j = 0; j < 4; ++j)
	{
		//ずらす方向決定
		DirectX::XMFLOAT2 dir(cosf((DirectX::XM_PI * 2.0f) * 0.25f * j + rad_offset), sinf((DirectX::XM_PI * 2.0f) * 0.25f * j + rad_offset));

		//3パスブラー
		for (int i = 0; i < 3; ++i)
		{
			//ブラーのオフセット計算
			ConstantBufferForIrradiation cbi;
			for (auto s = 0; s < 16; ++s)
			{
				auto b = powf(4.0f, float(i));
				cbi.Offset[s].x = dir.x * (b * s) * inv_size.x;
				cbi.Offset[s].y = dir.y * (b * s) * inv_size.y;
				cbi.Offset[s].z = powf(attenuation, (b * s));
			}
			UpdateConstantBuffer(irradiation_shader.get(), cbi);

			//テクスチャにブラー
			f_buffer->swapRenderTargetViewBegin(blur_tex[i].get(), Light::LightDir, FRAMEWORK.BS::BS_ALPHA, DirectX::XMFLOAT2(1280, 720),DirectX::XMFLOAT4(0, 0, 0, 1), true);
			spr->render(irradiation_shader.get(), bright_tex.get(), 0, 0, 1280, 720, 0, 0, 1280, 720);
			f_buffer->swapRenderTargetViewEnd();
		}

		//１方向の光芒を加算合成
		f_buffer->swapRenderTargetViewBegin(irradiation_tex[j].get(), Light::LightDir, FRAMEWORK.BS::BS_ADD, DirectX::XMFLOAT2(1280, 720), DirectX::XMFLOAT4(0, 0, 0, 1), true);
		for (auto &it : blur_tex)
		{
			spr->render(_2d_shader.get(), blur_tex[0].get(), 0, 0, 1280, 720, 0, 0, 1280, 720);
		}
		f_buffer->swapRenderTargetViewEnd();
	}

	//4本の光芒と元画像加算合成
	//spr->render(_2d_shader.get(), scene_tex.get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);//元画像の上から書く場合はここを有効化
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ADD), nullptr, 0xFFFFFFFF);
	spr->render(_2d_shader.get(), irradiation_tex[0].get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
	spr->render(_2d_shader.get(), irradiation_tex[1].get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
	spr->render(_2d_shader.get(), irradiation_tex[2].get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
	spr->render(_2d_shader.get(), irradiation_tex[3].get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT);
}