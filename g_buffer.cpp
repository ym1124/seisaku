#include "g_buffer.h"

class Texture;
class frame_buffer;

g_buffer::g_buffer()
{
	//g_buffer用テクスチャ作成
	for (int i = 0; i < 4; i++)
	{
		tex_buffer[i] = std::make_unique<Texture>(true);
	}
	for (int i = 0; i < 2; i++)
	{
		tex_buffer2[i] = std::make_unique<Texture>(true);
	}
	tex_buffer[0]->Create(1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM);
	tex_buffer[1]->Create(1280, 720, DXGI_FORMAT_R16G16B16A16_FLOAT);
	tex_buffer[2]->Create(1280, 720, DXGI_FORMAT_R16G16B16A16_FLOAT);
	tex_buffer[3]->Create(1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM);
	tex_buffer2[0]->Create();
	tex_buffer2[1]->Create();
}

g_buffer::~g_buffer()
{
	//変更9/11
	//for (int i = 0; i < 4; i++)
	//{
	//	srvs[i]->Release();
	//}
}

//void g_buffer::psSetGBuffer()
//{
//	for (int i = 0; i < 4; i++)
//	{
//		srvs[i] = tex_buffer[i]->GetShaderResourceView();
//	}
//	FRAMEWORK.getImmediateContext()->PSSetShaderResources(7, 4, srvs->GetAddressOf());
//}

void g_buffer::begine(frame_buffer *f_buffer, bool dsv_flg)
{
	f_buffer->createGBufferBegin(tex_buffer, DirectX::XMFLOAT4(0.6f, -0.6f, 0.6f, 0.0f), DirectX::XMFLOAT4(0, 0, 0, 1), dsv_flg);
}

void g_buffer::end(frame_buffer *f_buffer)
{
	f_buffer->createGBufferEnd();
}

void g_buffer::ks_kd_begine(frame_buffer *f_buffer)
{
	f_buffer->createGBufferBegin2(tex_buffer2);
}

void g_buffer::ks_kd_end(frame_buffer *f_buffer)
{
	f_buffer->createGBufferEnd2();
}