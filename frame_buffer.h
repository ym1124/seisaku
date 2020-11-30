#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "define.h"
//#include "texture.h"
//#include "blend.h"
//#include "shader.h"

class shader;
class Texture;
class Texture_d;

class frame_buffer
{
public:
	static const int GBUFFER_MAX = 4;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> render_target_shader_resource_view = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depth_stencil_shader_resource_view = nullptr;//ê[ìxóp
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView2 = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;

public:
	frame_buffer();
	~frame_buffer() {};
	void swapRenderTargetViewBegin(Texture *texture, DirectX::XMFLOAT4 light = DirectX::XMFLOAT4(0.6f,-0.6f,0.6f,0.0f), int blend_type = 1/*BLEND_STATE::BS_ALPHA*/,DirectX::XMFLOAT2 vp_size = DirectX::XMFLOAT2(1280,720), DirectX::XMFLOAT4 clear_color =DirectX::XMFLOAT4(0,0,0,1),bool dsvFlg = true);
	void swapRenderTargetViewEnd();

	////ssao
	//void createSsaoBufferBegine(std::unique_ptr<Texture> texture[2], DirectX::XMFLOAT4 light = DirectX::XMFLOAT4(0.6f, -0.6f, 0.6f, 0.0f), int blend_type = blender::BLEND_STATE::BS_ALPHA, DirectX::XMFLOAT4 clear_color = DirectX::XMFLOAT4(0, 0, 0, 1));
	//void createSsaoBufferEnd();
	////ssr
	//void createSsrBufferBegine(std::unique_ptr<Texture> texture[3], DirectX::XMFLOAT4 light = DirectX::XMFLOAT4(0.6f, -0.6f, 0.6f, 0.0f), int blend_type = blender::BLEND_STATE::BS_ALPHA, DirectX::XMFLOAT4 clear_color = DirectX::XMFLOAT4(0, 0, 0, 1));
	//void createSsrBufferEnd();

	//G-buffer
	//color,normal,position,depth?
	void createGBufferBegin(std::unique_ptr<Texture> texture[GBUFFER_MAX], DirectX::XMFLOAT4 light = DirectX::XMFLOAT4(0.6f, -0.6f, 0.6f, 0.0f),  DirectX::XMFLOAT4 clear_color = DirectX::XMFLOAT4(0, 0, 0, 1), bool dsvFlg = true);
	void createGBufferEnd();
	//diffuse,specular
	void createGBufferBegin2(std::unique_ptr<Texture> texture[2], DirectX::XMFLOAT4 light = DirectX::XMFLOAT4(0.6f, -0.6f, 0.6f, 0.0f),  DirectX::XMFLOAT4 clear_color = DirectX::XMFLOAT4(0, 0, 0, 1), bool dsvFlg = true);
	void createGBufferEnd2();
	//WARNINGÇ≈ÇÈÇØÇ«ñ‚ëËÇ»Ç¢Ç¡ÇƒèëÇ¢ÇƒÇÈÇÃÇ≈ñ≥éã
	//The Pixel Shader expects a Render Target View bound to slot 0, but none is bound.This is OK, as writes of an unbound Render Target View are discarded.It is also possible the developer knows the data will not be used anyway.This is only a problem if the developer actually intended to bind a Render Target View here.
	void createDepthMapBegin(Texture_d *texture, DirectX::XMFLOAT4 light = DirectX::XMFLOAT4(0.6f, -0.6f, 0.6f, 0.0f), DirectX::XMFLOAT2 vp_size = DirectX::XMFLOAT2(sCREEN_WIDTH, sCREEN_HEIGHT));
	void createDepthMapEnd();
	Texture* createDownSample(Texture *tex_input, Texture *tex_output, shader *shader, float width = sCREEN_WIDTH, float height = sCREEN_HEIGHT);
	void release();

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view = nullptr;
};