#pragma once

#include <windows.h>
#include <crtdbg.h>
#include <d3d11.h>
#include <wrl.h>

#if defined( DEBUG ) || defined( _DEBUG )
#define _ASSERT_EXPR_A(expr, msg) \
	(void)((!!(expr)) || \
	(1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", msg)) || \
	(_CrtDbgBreak(), 0))
#else
#define  _ASSERT_EXPR_A(expr, expr_str) ((void)0)
#endif

inline LPWSTR hr_trace(HRESULT hr)
{
	LPWSTR msg;
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&msg), 0, NULL);
	return msg;
}

class benchmark
{
	LARGE_INTEGER ticks_per_second;
	LARGE_INTEGER start_ticks;
	LARGE_INTEGER current_ticks;

public:
	benchmark()
	{
		QueryPerformanceFrequency(&ticks_per_second);
		QueryPerformanceCounter(&start_ticks);
		QueryPerformanceCounter(&current_ticks);
	}
	virtual ~benchmark() = default;
	benchmark(benchmark&) = delete;
	benchmark& operator=(benchmark&) = delete;

	void begin()
	{
		QueryPerformanceCounter(&start_ticks);
	}
	float end()
	{
		QueryPerformanceCounter(&current_ticks);
		return static_cast<float>(current_ticks.QuadPart - start_ticks.QuadPart) / static_cast<float>(ticks_per_second.QuadPart);
	}
};

#pragma warning(disable : 26812)
struct live_device_object_debugger
{
	//Microsoft::WRL::ComPtr<ID3D11Debug> debug;
	//live_device_object_debugger(ID3D11Device* device)
	//{
	//	HRESULT hr = device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(debug.GetAddressOf()));
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}
	//~live_device_object_debugger()
	//{
	//	HRESULT hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_IGNORE_INTERNAL);
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}
	//HRESULT report(D3D11_RLDO_FLAGS flags = D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL)
	//{
	//	//D3D11_RLDO_SUMMARY = 0x1,
	//	//D3D11_RLDO_DETAIL = 0x2,
	//	//D3D11_RLDO_IGNORE_INTERNAL = 0x4
	//	HRESULT hr = debug->ReportLiveDeviceObjects(flags);
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//	return hr;
	//}
};