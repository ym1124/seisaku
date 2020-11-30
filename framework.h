#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <dxgidebug.h>
#include "misc.h"
#include "high_resolution_timer.h"
#include "sampler.h"

////////////////////////////////////
#include <memory>
#include  <string>
//#include "sprite.h"
//#include "camera.h"
//#include "static_mesh.h"
//#include "static_object.h"
//#include "collision.h"
//#include "texture.h"
//#include "bloom.h"
//#include "light_beam.h"
//#include "billboard.h"
//#include "hit_fx.h"
//#include "frame_buffer.h"
//#include "environment_cubemap.h"
//#include "ir_map.h"
//#include "cube_mesh.h"
//#include "g_buffer.h"
//#include "gpu_particle.h"
//#include "liner_depth.h"
//#include "deffered.h"
//#include "ssao.h"
//#include "shadow_map.h"
//#include "dissolve.h"
//#include "mesh.h"
//#include "animation.h"
//#include "ray.h"
//#include "sphere.h"
//#include "hit.h"
//#include "ray_material.h"
//#include "ray_light.h"
//
//class environment_cubemap;
//class irmap;
//class deffered;
//class g_buffer;
//class ssao;


///////////////////////////////////

//imgui
#define USE_IMGUI 1

#if USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

#pragma comment(lib,"d3d11.lib")

class framework
{
public:
	//mouse m;
	int game_timer = 0;
	CONST HWND hwnd;
	static CONST LONG SCREEN_WIDTH = 1280;
	static CONST LONG SCREEN_HEIGHT = 720;

	ID3D11Device* device;
	ID3D11Debug* debug;
	ID3D11DeviceContext* immediate_context;
	IDXGISwapChain* swap_chain;
	ID3D11RenderTargetView* render_target_view;
	ID3D11DepthStencilView* depth_stencil_view;

	static const int DEPTH_STENCIL_TYPE = 3;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[DEPTH_STENCIL_TYPE];

	static const int RASTERIZE_TYPE = 4;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[RASTERIZE_TYPE];

	static const int BLEND_TYPE = 9;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[BLEND_TYPE];

	static const int SAMPLER_TYPE = 8;
	sampler* sampler_states[SAMPLER_TYPE];

	//blender* blend;//ブレンド

	char message[256] = { 0 };

	D3D_DRIVER_TYPE	 g_DriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL	g_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	framework(HWND hwnd) : hwnd(hwnd)
	{
	}
	~framework()
	{
		//解放
		//m.ReleaseDInput();
		//m.ReleaseDInputMouse();
		release();
		//リークチェック
		//checkLeak();
	}
	int run()
	{
		MSG msg = {};

		if (!initialize()) return 0;

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				//m.GetMouseState(hwnd);
				timer.tick();
				calculate_frame_stats();
				update(timer.time_interval());
				render(timer.time_interval());
			}
		}
#if USE_IMGUI
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif
		return static_cast<int>(msg.wParam);
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		//imgui
#if USE_IMGUI
		//ウィンドウサイズとか変えるのに必要
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		{
			return 1;
		}
#endif

		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case WM_ENTERSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
			timer.stop();
			break;
		case WM_EXITSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
			timer.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool initialize();
	void update(float elapsed_time/*Elapsed seconds from last frame*/);
	void render(float elapsed_time/*Elapsed seconds from last frame*/);
	void release();

private:
	high_resolution_timer timer;
	void calculate_frame_stats()
	{
		// Code computes the average frames per second, and also the 
		// average time it takes to render one frame.  These stats 
		// are appended to the window caption bar.
		static int frames = 0;
		static float time_tlapsed = 0.0f;

		frames++;

		// Compute averages over one second period.
		if ((timer.time_stamp() - time_tlapsed) >= 1.0f)
		{
			float fps = static_cast<float>(frames); // fps = frameCnt / 1
			float mspf = 1000.0f / fps;
			std::ostringstream outs;
			outs.precision(6);
			outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
			SetWindowTextA(hwnd, outs.str().c_str());

			// Reset for next average.
			frames = 0;
			time_tlapsed += 1.0f;
		}
	}
	public:
		static framework& getInstance(HWND hwnd = nullptr)
		{
			static framework instance(hwnd);
			return instance;
		}
		inline static HWND getHWND() { return framework::getInstance().hwnd; };
		ID3D11Device* getDevice() { return device; }//ゲッター
		ID3D11DeviceContext* getImmediateContext() { return immediate_context; }//ゲッター
		ID3D11RenderTargetView* getRenderTargetView() const { return render_target_view;}//immediateContext->OMGetRenderTargetにする//これだとダブルバッファの片方になる
		ID3D11RenderTargetView* const* getRenderTargetViewAdress() const { return &render_target_view; }
		ID3D11DepthStencilView* getDepthStencilView() const { return depth_stencil_view; }
		ID3D11DepthStencilView* const* getDepthStencilViewAddress()const{ return &depth_stencil_view; }
		ID3D11RasterizerState* getRasterizerState(int type) { return rasterizer_states[type].Get(); }
		ID3D11DepthStencilState* getDephtStencilState(int type) {return depth_stencil_states[type].Get();}
		//blender* getBlendState() { return blend; }
		ID3D11BlendState* getBlendState(int type) { return blend_states[type].Get(); }
		ID3D11SamplerState* getSamplerState(int type) { return sampler_states[type]->getSamplerState(); }
		ID3D11SamplerState** getSamplerStateAdress(int type) { return sampler_states[type]->getSamplerStateAddress(); }
		

		IDXGISwapChain* getSwapChain()const { return swap_chain; }

		HRESULT createRasterizerStates();
		HRESULT createDephtStencilStates();
		HRESULT createBlendStates();
		void createSamplerStates(float r = 0, float g = 0, float b = 0, float a = 1.0f);//9/4 a=0から変更

		HRESULT checkLeak();

		//void changeRenderTargetView(Texture *texture, sprite *sprite, DirectX::XMFLOAT4 light, blender::BLEND_STATE blend_type = blender::BLEND_STATE::BS_ALPHA);
		//void changeRenderTargetViewEnd();

		DirectX::XMFLOAT4X4 LightViewProjection;

		//RasterizerState
		enum RS{ RS_CULL_BACK, RS_WIRE, RS_CULL_FRONT, RS_CULL_NONE };
		//DepthStencilState
		enum DS{ DS_FALSE, DS_TRUE, DS_WRITE_FALSE };
		//BlendState
		enum BS{ BS_NONE, BS_ALPHA, BS_ADD, BS_SUBTRACT, BS_REPLACE, BS_MULTIPLY, BS_LIGHTEN, BS_DARKEN, BS_SCREEN, BS_END };
		//Sampler
		enum SM{ POINT_BORDER, LINEAR_BORDER, ANISOTROPIC_BORDER, POINT_CLAMP, LINEAR_CLAMP, ANISOTROPIC_CLAMP, LINEAR_WRAP, LINER_BORDER_WHITE};
};
#define FRAMEWORK framework::getInstance()



