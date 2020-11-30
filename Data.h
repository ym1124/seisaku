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
		//	����ς݂��m�F�@�I�����܂�
		if (count <= 0)	return;
		//	�c�@���팸��A�폜���߂̕K�v�����������܂�
		if (--count <= 0)	bForce = true;

		//	�폜���߂����m�F�ARelease���܂�
		if (bForce)
		{
			shader_resource_view->Release();
			ZeroMemory(this, sizeof(ShaderResourceViewData));	//	�O�ׂ̈Ɋ��S�������܂�
		}
	}
};




//Shader�֘A
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
		//	����ς݂��m�F�@�I�����܂�
		if (count <= 0)	return;
		//	�c�@���팸��A�폜���߂̕K�v�����������܂�
		if (--count <= 0)	bForce = true;

		//	�폜���߂����m�F�ARelease���܂�
		if (bForce)
		{
			vertex_shader->Release();
			input_layout->Release();
			ZeroMemory(this, sizeof(VertexShadersData));	//	�O�ׂ̈Ɋ��S�������܂�
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
		//	����ς݂��m�F�@�I�����܂�
		if (count <= 0)	return;
		//	�c�@���팸��A�폜���߂̕K�v�����������܂�
		if (--count <= 0)	bForce = true;

		//	�폜���߂����m�F�ARelease���܂�
		if (bForce)
		{
			pixel_shader->Release();
			ZeroMemory(this, sizeof(PixelShadersData));	//	�O�ׂ̈Ɋ��S�������܂�
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
		//	����ς݂��m�F�@�I�����܂�
		if (count <= 0)	return;
		//	�c�@���팸��A�폜���߂̕K�v�����������܂�
		if (--count <= 0)	bForce = true;

		//	�폜���߂����m�F�ARelease���܂�
		if (bForce)
		{
			geometry_shader->Release();
			ZeroMemory(this, sizeof(GeometryShadersData));	//	�O�ׂ̈Ɋ��S�������܂�
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
		//	����ς݂��m�F�@�I�����܂�
		if (count <= 0)	return;
		//	�c�@���팸��A�폜���߂̕K�v�����������܂�
		if (--count <= 0)	bForce = true;

		//	�폜���߂����m�F�ARelease���܂�
		if (bForce)
		{
			hull_shader->Release();
			ZeroMemory(this, sizeof(HullShadersData));	//	�O�ׂ̈Ɋ��S�������܂�
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
		//	����ς݂��m�F�@�I�����܂�
		if (count <= 0)	return;
		//	�c�@���팸��A�폜���߂̕K�v�����������܂�
		if (--count <= 0)	bForce = true;

		//	�폜���߂����m�F�ARelease���܂�
		if (bForce)
		{
			domain_shader->Release();
			ZeroMemory(this, sizeof(DomainShadersData));	//	�O�ׂ̈Ɋ��S�������܂�
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
		//	����ς݂��m�F�@�I�����܂�
		if (count <= 0)	return;
		//	�c�@���팸��A�폜���߂̕K�v�����������܂�
		if (--count <= 0)	bForce = true;

		//	�폜���߂����m�F�ARelease���܂�
		if (bForce)
		{
			compute_shader->Release();
			ZeroMemory(this, sizeof(ComputeShadersData));	//	�O�ׂ̈Ɋ��S�������܂�
		}
	}
};
