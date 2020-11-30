#include "sampler.h"
#include "framework.h"

sampler::sampler(D3D11_SAMPLER_DESC* sd, float r, float g, float b, float a)
{
	//ZeroMemory(&sd, sizeof(sd));
	//sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//�o�C���j�A���
	//sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;//�͈͊O�͎w��F
	//sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	//sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	//sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//float border_colour[4] = { r, g, b, a };
	//memcpy(sd.BorderColor, border_colour, sizeof(float) * 4);
	//sd.MinLOD = 0;
	//sd.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = FRAMEWORK.getDevice()->CreateSamplerState(sd, sampler_state.GetAddressOf());
	assert(SUCCEEDED(hr));
}

sampler::sampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE address_mode, D3D11_COMPARISON_FUNC comparison_func, float r, float g, float b, float a)
{
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = filter;//�o�C���j�A���
	sd.AddressU = address_mode;//�͈͊O�͎w��F
	sd.AddressV = address_mode;
	sd.AddressW = address_mode;
	sd.ComparisonFunc = comparison_func;
	float border_colour[4] = { r, g, b, a };
	memcpy(sd.BorderColor, border_colour, sizeof(float) * 4);
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = FRAMEWORK.getDevice()->CreateSamplerState(&sd, sampler_state.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void sampler::set(UINT Slot, BOOL flg)
{
	if (flg == FALSE) {
		//�_�~�[�Z�b�g����H
	}

	if (sampler_state) {
		FRAMEWORK.getImmediateContext()->PSSetSamplers(Slot, 1, sampler_state.GetAddressOf());
		FRAMEWORK.getImmediateContext()->DSSetSamplers(Slot, 1, sampler_state.GetAddressOf());
	}
}