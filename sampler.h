#pragma once

#include <d3d11.h>
#include <wrl.h>

class sampler
{
protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state = nullptr;
public:
	sampler(D3D11_SAMPLER_DESC* sd, float r = 0, float g = 0, float b = 0, float a = 0);
	//FILTER��MIN�͏k�����AMAG�͊g�厞�B�Ⴆ��MIN_LINEAR_MAG_POINT�Ȃ�k�����̓o�C���j�A��ԁA�g�厞�̓|�C���g�T���v�����O�B//MIP�̓~�b�v�}�b�v//ANISOTROPIC�͈ٕ������(�����l��)
	//D3D11_TEXTURE_ADDRESS_MODE��uv��1���傫���Ƃ���0��菬�����Ƃ��Ȃǔ͈͊O�̎����w��BWRAP�͓����摜���J��Ԃ��BBORDER�Ȃ�w�肵���F�ŕ`��BCLAMP�Ȃ�0��1�̎��̐F�ŌŒ�B
	//D3D11_COMPARISON_FUNC�͌��f�[�^�Ƃ̔�r�B
	sampler(D3D11_FILTER filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_MODE address_mode = D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_FUNC comparison_func = D3D11_COMPARISON_ALWAYS, float r = 0, float g = 0, float b = 0, float a = 0);
	void set(UINT slot = 0, BOOL flg = TRUE);
	ID3D11SamplerState* getSamplerState() { return sampler_state.Get(); }
	ID3D11SamplerState** getSamplerStateAddress() { return sampler_state.GetAddressOf(); }
};