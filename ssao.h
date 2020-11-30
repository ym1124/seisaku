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
//						float zfar = 10000.0f;//�J������zfar�ƈ�v
//						float ao_power = 165.0f;
//						float variance = 3.0f;
//�@�@�@�@�@�@�@ ���񂭂炢�̃p�����[�^�[�ł��傤�ǂ��������ɂȂ�
//                     �����̃N���X�̓��C���J�����̍쐬��(make_unique)�ɍ쐬����I�I�I
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
	//ssao(const ssao&) {};//�R�s�[�R���X�g���N�^
	~ssao() {};
	void render(frame_buffer *f_buffer, shader *_2d_shader, float hemi_radius, float bias,float variance, bool synthesize =false);//synthesize����Ό��V�[���ƍ������Ă���`�悷��//false�Ȃ�e�̂�
	
	//�Z�b�g����Ȃ�
	//void createGBufferBegine(frame_buffer *f_buffer);//�쐬
	//void createGBufferEnd(frame_buffer *f_buffer);

	//���ɂ���Ȃ�
	void setGBuffer(std::unique_ptr<Texture> buffer[4]);//���炦��Ȃ�R�b�`
	void setDeffered(deffered* def);

	//getter
	shader* getSSAOShader() { return ssao_shader.get(); }
	//shader* getGBufferShader() { return ssao_deffered->getGBufferShader(); }
	Texture* getSSAOTex() { return ssao_tex.get(); }
};