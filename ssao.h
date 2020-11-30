#pragma once

//#include<memory>
#include <DirectXMath.h>

struct ConstantBufferForSSAO;
class deffered;
class g_buffer;
class Texture;
class shader;
class blur;
class sprite;
class frame_buffer;

//**********************************************************
//                     	float hemi_radius=10.0f;
//						float zfar = 10000.0f;//カメラのzfarと一致
//						float ao_power = 165.0f;
//						float variance = 3.0f;
//　　　　　　　 こんくらいのパラメーターでちょうどいい感じになる
//                     ※このクラスはメインカメラの作成後(make_unique)に作成する！！！
//**********************************************************

class ssao
{
private:
	ConstantBufferForSSAO cbssao;
	std::unique_ptr<Texture> ssao_tex;
	deffered *ssao_deffered;
	std::unique_ptr<shader> ssao_shader;
	std::unique_ptr<blur> _blur;
	std::unique_ptr<sprite> spr;

public:
	ssao(deffered *deffered);
	//ssao(const ssao&) {};//コピーコンストラクタ
	~ssao() {};
	void render(frame_buffer *f_buffer, shader *_2d_shader, float hemi_radius, float bias,float variance, bool synthesize =false);//synthesizeあれば元シーンと合成してから描画する//falseなら影のみ
	
	//セットするなら
	//void createGBufferBegine(frame_buffer *f_buffer);//作成
	//void createGBufferEnd(frame_buffer *f_buffer);

	//既にあるなら
	void setGBuffer(std::unique_ptr<Texture> buffer[4]);//もらえるならコッチ
	void setDeffered(deffered* def);

	//getter
	shader* getSSAOShader() { return ssao_shader.get(); }
	//shader* getGBufferShader() { return ssao_deffered->getGBufferShader(); }
	Texture* getSSAOTex() { return ssao_tex.get(); }
};