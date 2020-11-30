#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include "framework.h"
#include "frame_buffer.h"
#include "texture.h"


class g_buffer
{
public:
	//�[�x�}�b�v�̃o�C�A�X�̂����ŋߋ����ł����������`��ł��Ȃ�
	std::unique_ptr<Texture> tex_buffer[4];//color,normal,position,depth�̏�
	std::unique_ptr<Texture> tex_buffer2[2];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srvs[4];
	g_buffer();
	~g_buffer();

	//void psSetGBuffer();
	void begine(frame_buffer *f_buffer,bool dsv_flg = true);//g_buffer:color,normal,position���쐬
	void end(frame_buffer *f_buffer);

	void ks_kd_begine(frame_buffer *f_buffer);//diffuse,specular���쐬
	void ks_kd_end(frame_buffer *f_buffer);
	//setter
	void setTex(Texture* tex, int slot) {*tex_buffer[slot] = tex;}//color,normal,position,depth�̏�//��肭�@�\���Ȃ�
	void setSRV(ID3D11ShaderResourceView* srv,int slot) { srvs[slot] = srv; }
};