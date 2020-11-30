#pragma once
#include <DirectXTex.h>
#include <wrl.h>
#include <d3d11.h>
#include <direct.h>
#include "framework.h"
#include "cube_mesh.h"

//事前計算放射照度マップ用構造体
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
	//コンストラクタ
	irmap();
	~irmap() {};
	//コンピュートシェーダー生成
	compute_program createComputeProgram(const Microsoft::WRL::ComPtr<ID3DBlob>& csBytecode);
	//サンプラー生成
	void createSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode);
	//キューブマップ作成
	ir_cube_tex createIrcubeTexture(UINT width, UINT height, DXGI_FORMAT format, UINT levels);
	//CS作成する関数
	HRESULT createTextureUAV(ir_cube_tex &ir_c_tex, UINT mipSlice);
	//csをコンパイルする
	Microsoft::WRL::ComPtr<ID3DBlob> compileShader(const std::string& filename, const std::string& entryPoint, const std::string& profile);
	//実行
	void runComputeShader(cube_mesh* c_mesh);

	//getter
	ir_cube_tex* getIrCubeMap() { return &ict; }
	ID3D11ShaderResourceView* getICTShaderResourceView() { return ict.srv.Get(); }
	ID3D11ShaderResourceView** getICTShaderResourceViewAddress() { return ict.srv.GetAddressOf(); }
	compute_program getComputeProgram() { return cp; }
};

