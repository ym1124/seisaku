#pragma once

class shader;

class postfx_cs
{
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mpScreenUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cssrv = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> csrtv = nullptr;
	std::unique_ptr<shader> compute_shader;
public:
	postfx_cs(const wchar_t* shader_name, u_int size);
	~postfx_cs() {};

	void render();
	template <typename T>
	void updateConstantBuffer(T cb);
	void setSRVs(ID3D11ShaderResourceView **srvs, int num_views);

	//getter
	ID3D11ShaderResourceView* getSRV() { return cssrv.Get(); }
};