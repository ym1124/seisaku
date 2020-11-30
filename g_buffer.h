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
	//深度マップのバイアスのせいで近距離でしか正しく描画できない
	std::unique_ptr<Texture> tex_buffer[4];//color,normal,position,depthの順
	std::unique_ptr<Texture> tex_buffer2[2];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srvs[4];
	g_buffer();
	~g_buffer();

	//void psSetGBuffer();
	void begine(frame_buffer *f_buffer,bool dsv_flg = true);//g_buffer:color,normal,positionを作成
	void end(frame_buffer *f_buffer);

	void ks_kd_begine(frame_buffer *f_buffer);//diffuse,specularを作成
	void ks_kd_end(frame_buffer *f_buffer);
	//setter
	void setTex(Texture* tex, int slot) {*tex_buffer[slot] = tex;}//color,normal,position,depthの順//上手く機能しない
	void setSRV(ID3D11ShaderResourceView* srv,int slot) { srvs[slot] = srv; }
};