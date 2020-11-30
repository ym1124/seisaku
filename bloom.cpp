#include "bloom.h"
#include "framework.h"
#include "frame_buffer.h"
#include "shader.h"
#include "sprite.h"

bloom::bloom():blur()
{
	//シェーダー系生成
	bright_shader = std::make_unique<shader>();
	bright_shader->Create(L"brightness_extraction.fx", "VSMain", "PSMain");
	bright_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForBrightnessExtraction));

	//テクスチャ・スプライト生成
	bright_tex = std::make_unique<Texture>();
}

//bloom::bloom()
//{
//	//シェーダー系生成
//	bright_shader = std::make_unique<shader>();
//	bright_shader->Create(L"brightness_extraction.fx", "VSMain", "PSMain");
//	bright_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForBrightnessExtraction));
//
//	blur_shader_horizontal = std::make_unique<shader>();
//	blur_shader_horizontal->Create(L"gaussian_blur.fx", "VSMain1", "PSMain1");
//	blur_shader_horizontal->CreateConstantBufferOriginal(sizeof(ConstantBufferForGaussianBlur));
//
//	blur_shader_vertical = std::make_unique<shader>();
//	blur_shader_vertical->Create(L"gaussian_blur.fx", "VSMain2", "PSMain2");
//	blur_shader_vertical->CreateConstantBufferOriginal(sizeof(ConstantBufferForGaussianBlur));
//
//	//テクスチャ・スプライト生成
//	scene_tex = std::make_unique<Texture>();
//	scene_tex->Create();
//
//	blur_tex = std::make_unique<Texture>();
//	blur_tex->Create();
//
//	bright_tex = std::make_unique<Texture>();
//	bright_tex->Create();
//
//	spr = std::make_unique<sprite>();
//}

//void  bloom::setSceneTexBegine(frame_buffer *f_buffer)
//{
//	f_buffer->swapRenderTargetViewBegin(scene_tex.get(), Light::LightDir, blender::BLEND_STATE::BS_ALPHA, DirectX::XMFLOAT4(0, 0, 0, 1), true);
//}
//void  bloom::setSceneTexEnd(frame_buffer *f_buffer)
//{
//	f_buffer->swapRenderTargetViewEnd();
//}

void bloom::render(frame_buffer *f_buffer , float threshold ,float variance,int weight_num)
{
	//高輝度取り出し
	ConstantBufferForBrightnessExtraction cbbe;
	/*cbbe.threshold = status->threshold;*/
	cbbe.threshold = threshold;
	UpdateConstantBuffer(bright_shader.get(), cbbe);
	f_buffer->swapRenderTargetViewBegin(bright_tex.get(), Light::LightDir, 1/*blender::BLEND_STATE::BS_ALPHA*/, DirectX::XMFLOAT2(1280,720),DirectX::XMFLOAT4(0, 0, 0, 1), true);
	spr->render(bright_shader.get(), scene_tex.get(), 0, 0, FRAMEWORK.SCREEN_WIDTH, FRAMEWORK.SCREEN_HEIGHT, 0, 0, FRAMEWORK.SCREEN_WIDTH, FRAMEWORK.SCREEN_HEIGHT);
	f_buffer->swapRenderTargetViewEnd();

	blur::render(f_buffer, variance, bright_tex.get(),true ,false);
}

kawase_bloom::kawase_bloom():bloom()
{
	createDownSampleTextures(texes, temp, TEXES_NUM);
	//外部からもらえばいい
	//_2d_shader = std::make_unique<shader>();
	//_2d_shader->Create(L"2d.fx", "VSMain", "PSMain");
}

void kawase_bloom::createDownSampleTextures(std::unique_ptr<Texture> texes[], std::unique_ptr<Texture> temp[], int size)
{
	for (int i = 0; i < size; i++)
	{
		int num = pow(2, i);
		texes[i] = std::make_unique<Texture>();
		texes[i]->Create(sCREEN_WIDTH / num, sCREEN_HEIGHT / num);//テクスチャの大きさを小さくしていく
		temp[i] = std::make_unique<Texture>();
		temp[i]->Create(sCREEN_WIDTH / num, sCREEN_HEIGHT / num);//テクスチャの大きさを小さくしていく
	}
}

void kawase_bloom::downSamplingAndBlur(std::unique_ptr<Texture> texes[], std::unique_ptr<Texture> temp[], frame_buffer* f_buffer, ConstantBufferForGaussianBlur cbgb, int size , float threshold,float variance)
{
	////高輝度取り出し
	//ConstantBufferForBrightnessExtraction cbbe;
	///*cbbe.threshold = status->threshold;*/
	//cbbe.threshold = threshold;
	//UpdateConstantBuffer(bright_shader.get(), cbbe);
	//f_buffer->swapRenderTargetViewBegin(bright_tex.get(), Light::LightDir, blender::BLEND_STATE::BS_ALPHA, DirectX::XMFLOAT4(0, 0, 0, 1), true);
	//spr->render(bright_shader.get(), scene_tex.get(), 0, 0, FRAMEWORK.SCREEN_WIDTH, FRAMEWORK.SCREEN_HEIGHT, 0, 0, FRAMEWORK.SCREEN_WIDTH, FRAMEWORK.SCREEN_HEIGHT);
	//f_buffer->swapRenderTargetViewEnd();

	bloom::render(f_buffer, threshold,  variance);

	//サンプリングループ
	for (int i = 0; i < size; i++)
	{
		int num = pow(2, i);
		//画像をセット//元画像で上書きしちゃうので不要
		//texes[i]->SetShaderResourceView(src->GetShaderResourceView());
		//画像を1280,720に戻しつつブラーかけて描画
		//FRAMEWORK.getImmediateContext()->OMSetBlendState(blend.states[blender::BS_ADD].Get(), nullptr, 0xFFFFFFFF);

		texes[i]->SetSize(DirectX::XMFLOAT2(sCREEN_WIDTH, sCREEN_HEIGHT));
		temp[i]->SetSize(DirectX::XMFLOAT2(sCREEN_WIDTH, sCREEN_HEIGHT));

		UpdateConstantBuffer(blur_shader_horizontal.get(), cbgb);
		f_buffer->swapRenderTargetViewBegin(temp[i].get(), DirectX::XMFLOAT4(1, 1, 1, 1), FRAMEWORK.BS::BS_ALPHA);
		//Comptrに変えたら/numする必要なくなった
		spr->render(blur_shader_horizontal.get(), bright_tex.get(), 0, 0, sCREEN_WIDTH/* / num*/, sCREEN_HEIGHT /*/ num*/, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 1.0f, 0, FRAMEWORK.SM::LINEAR_CLAMP);//サンプラーをクランプに指定することで画面端に線が入るのを防ぐ
		f_buffer->swapRenderTargetViewEnd();

		f_buffer->swapRenderTargetViewBegin(texes[i].get(), DirectX::XMFLOAT4(1, 1, 1, 1), FRAMEWORK.BS::BS_ALPHA);
		spr->render(blur_shader_vertical.get(), temp[i].get(), 0, 0, sCREEN_WIDTH /*/ num*/, sCREEN_HEIGHT /*/ num*/, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 1.0f, 0, FRAMEWORK.SM::LINEAR_CLAMP);//サンプラーをクランプに指定することで画面端に線が入るのを防ぐ
		f_buffer->swapRenderTargetViewEnd();
	}
}

void kawase_bloom::render(frame_buffer *f_buffer, shader* _2d_shader, float threshold, float variance, int down_sampling_num,int weight_num)
{
	//ガウスブラー計算
	CalcGaussWeights(variance*variance, weight_array, weight_num);

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

	downSamplingAndBlur(texes, temp, f_buffer, cbgb, TEXES_NUM, threshold,variance);

	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ADD), nullptr, 0xFFFFFFFF);

	//ブラー画像を重ねて描画
	for (int i = 0; i < down_sampling_num; i++)
	{
		spr->render(_2d_shader, texes[i].get(), 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT, 0, 0, sCREEN_WIDTH, sCREEN_HEIGHT,1.0f,0,FRAMEWORK.SM::LINEAR_CLAMP);//サンプラーをクランプに指定することで画面端に線が入るのを防ぐ
	}
}