#pragma once

#include <d3d11.h>

//ShaderResourceView
struct ShaderResourceViewData
{
	wchar_t						filename[256];
	ID3D11ShaderResourceView* shader_resource_view;
	D3D11_TEXTURE2D_DESC		texDesc;
	int							count;

	ShaderResourceViewData() : shader_resource_view(nullptr), count(0)
	{
		ZeroMemory(filename, sizeof(filename));
	}

	void Release(bool bForce = false)
	{
		//	解放済みを確認　終了します
		if (count <= 0)	return;
		//	残機を削減後、削除命令の必要性を検討します
		if (--count <= 0)	bForce = true;

		//	削除命令をを確認、Releaseします
		if (bForce)
		{
			shader_resource_view->Release();
			ZeroMemory(this, sizeof(ShaderResourceViewData));	//	念の為に完全消去します
		}
	}
};




//Shader関連
struct VertexShadersData
{
	char						filename[256];
	ID3D11VertexShader* vertex_shader;
	ID3D11InputLayout* input_layout;
	int							count;

	VertexShadersData() : vertex_shader(nullptr), input_layout(nullptr), count(0)
	{
		ZeroMemory(filename, sizeof(filename));
	}

	void Release(bool bForce = false)
	{
		//	解放済みを確認　終了します
		if (count <= 0)	return;
		//	残機を削減後、削除命令の必要性を検討します
		if (--count <= 0)	bForce = true;

		//	削除命令をを確認、Releaseします
		if (bForce)
		{
			vertex_shader->Release();
			input_layout->Release();
			ZeroMemory(this, sizeof(VertexShadersData));	//	念の為に完全消去します
		}
	}
};

struct PixelShadersData
{
	char						filename[256];
	ID3D11PixelShader* pixel_shader;
	int							count;

	PixelShadersData() : pixel_shader(nullptr), count(0)
	{
		ZeroMemory(filename, sizeof(filename));
	}

	void Release(bool bForce = false)
	{
		//	解放済みを確認　終了します
		if (count <= 0)	return;
		//	残機を削減後、削除命令の必要性を検討します
		if (--count <= 0)	bForce = true;

		//	削除命令をを確認、Releaseします
		if (bForce)
		{
			pixel_shader->Release();
			ZeroMemory(this, sizeof(PixelShadersData));	//	念の為に完全消去します
		}
	}
};

struct GeometryShadersData
{
	char						filename[256];
	ID3D11GeometryShader* geometry_shader;
	int							count;

	GeometryShadersData() : geometry_shader(nullptr), count(0)
	{
		ZeroMemory(filename, sizeof(filename));
	}

	void Release(bool bForce = false)
	{
		//	解放済みを確認　終了します
		if (count <= 0)	return;
		//	残機を削減後、削除命令の必要性を検討します
		if (--count <= 0)	bForce = true;

		//	削除命令をを確認、Releaseします
		if (bForce)
		{
			geometry_shader->Release();
			ZeroMemory(this, sizeof(GeometryShadersData));	//	念の為に完全消去します
		}
	}
};

struct HullShadersData
{
	char						filename[256];
	ID3D11HullShader* hull_shader;
	int							count;

	HullShadersData() : hull_shader(nullptr), count(0)
	{
		ZeroMemory(filename, sizeof(filename));
	}

	void Release(bool bForce = false)
	{
		//	解放済みを確認　終了します
		if (count <= 0)	return;
		//	残機を削減後、削除命令の必要性を検討します
		if (--count <= 0)	bForce = true;

		//	削除命令をを確認、Releaseします
		if (bForce)
		{
			hull_shader->Release();
			ZeroMemory(this, sizeof(HullShadersData));	//	念の為に完全消去します
		}
	}
};

struct DomainShadersData
{
	char						filename[256];
	ID3D11DomainShader* domain_shader;
	int							count;

	DomainShadersData() : domain_shader(nullptr), count(0)
	{
		ZeroMemory(filename, sizeof(filename));
	}

	void Release(bool bForce = false)
	{
		//	解放済みを確認　終了します
		if (count <= 0)	return;
		//	残機を削減後、削除命令の必要性を検討します
		if (--count <= 0)	bForce = true;

		//	削除命令をを確認、Releaseします
		if (bForce)
		{
			domain_shader->Release();
			ZeroMemory(this, sizeof(DomainShadersData));	//	念の為に完全消去します
		}
	}
};

struct ComputeShadersData
{
	char						filename[256];
	ID3D11ComputeShader* compute_shader;
	int							count;

	ComputeShadersData() : compute_shader(nullptr), count(0)
	{
		ZeroMemory(filename, sizeof(filename));
	}

	void Release(bool bForce = false)
	{
		//	解放済みを確認　終了します
		if (count <= 0)	return;
		//	残機を削減後、削除命令の必要性を検討します
		if (--count <= 0)	bForce = true;

		//	削除命令をを確認、Releaseします
		if (bForce)
		{
			compute_shader->Release();
			ZeroMemory(this, sizeof(ComputeShadersData));	//	念の為に完全消去します
		}
	}
};
