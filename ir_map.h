#pragma once
#include <DirectXTex.h>
#include <wrl.h>
#include <d3d11.h>
#include <direct.h>
#include "framework.h"
#include "cube_mesh.h"

//���O�v�Z���ˏƓx�}�b�v�p�\����
struct ir_cube_tex
{
public:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;
	UINT width, height;
	UINT levels;
};

struct compute_program
{
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> cs;
};

class irmap
{
private:
	ir_cube_tex ict;
	compute_program cp;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> compute_sampler;
public:
	//�R���X�g���N�^
	irmap();
	~irmap() {};
	//�R���s���[�g�V�F�[�_�[����
	compute_program createComputeProgram(const Microsoft::WRL::ComPtr<ID3DBlob>& csBytecode);
	//�T���v���[����
	void createSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode);
	//�L���[�u�}�b�v�쐬
	ir_cube_tex createIrcubeTexture(UINT width, UINT height, DXGI_FORMAT format, UINT levels);
	//CS�쐬����֐�
	HRESULT createTextureUAV(ir_cube_tex &ir_c_tex, UINT mipSlice);
	//cs���R���p�C������
	Microsoft::WRL::ComPtr<ID3DBlob> compileShader(const std::string& filename, const std::string& entryPoint, const std::string& profile);
	//���s
	void runComputeShader(cube_mesh* c_mesh);

	//getter
	ir_cube_tex* getIrCubeMap() { return &ict; }
	ID3D11ShaderResourceView* getICTShaderResourceView() { return ict.srv.Get(); }
	ID3D11ShaderResourceView** getICTShaderResourceViewAddress() { return ict.srv.GetAddressOf(); }
	compute_program getComputeProgram() { return cp; }
};

