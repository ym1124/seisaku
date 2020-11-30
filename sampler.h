#pragma once

#include <d3d11.h>
#include <wrl.h>

class sampler
{
protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state = nullptr;
public:
	sampler(D3D11_SAMPLER_DESC* sd, float r = 0, float g = 0, float b = 0, float a = 0);
	//FILTERのMINは縮小時、MAGは拡大時。例えばMIN_LINEAR_MAG_POINTなら縮小時はバイリニア補間、拡大時はポイントサンプリング。//MIPはミップマップ//ANISOTROPICは異方性補間(視線考慮)
	//D3D11_TEXTURE_ADDRESS_MODEはuvが1より大きいときや0より小さいときなど範囲外の時を指定。WRAPは同じ画像を繰り返す。BORDERなら指定した色で描画。CLAMPなら0か1の時の色で固定。
	//D3D11_COMPARISON_FUNCは元データとの比較。
	sampler(D3D11_FILTER filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_MODE address_mode = D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_FUNC comparison_func = D3D11_COMPARISON_ALWAYS, float r = 0, float g = 0, float b = 0, float a = 0);
	void set(UINT slot = 0, BOOL flg = TRUE);
	ID3D11SamplerState* getSamplerState() { return sampler_state.Get(); }
	ID3D11SamplerState** getSamplerStateAddress() { return sampler_state.GetAddressOf(); }
};