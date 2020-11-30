#pragma once

//#include "Shader.h"
#include "Data.h"

class resource_manager
{
	static const int RESOURCE_MAX = 128;		//	最大スタック数

	static ShaderResourceViewData	srvData[RESOURCE_MAX];
	static VertexShadersData		vsData[RESOURCE_MAX];
	static PixelShadersData			psData[RESOURCE_MAX];
	static GeometryShadersData		gsData[RESOURCE_MAX];
	static HullShadersData			hsData[RESOURCE_MAX];
	static DomainShadersData		dsData[RESOURCE_MAX];
	static ComputeShadersData		csData[RESOURCE_MAX];

	resource_manager() {}
	~resource_manager() { release(); }

public:
	static void release()
	{
		for (int n = 0; n < RESOURCE_MAX; n++)
		{
			srvData[n].Release(true);
			vsData[n].Release(true);
			psData[n].Release(true);
			gsData[n].Release(true);
			hsData[n].Release(true);
			dsData[n].Release(true);
			csData[n].Release(true);
		}
	}

	static bool loadShaderResourceView(ID3D11Device* device, const wchar_t* filename,
		ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC** texDesc);
	static void releaseShaderResourceView(ID3D11ShaderResourceView* srv);

	static bool loadVertexShaders(ID3D11Device* device, const char* filename,
		D3D11_INPUT_ELEMENT_DESC* elementDescs, int numElement,
		ID3D11VertexShader** vs, ID3D11InputLayout** il);
	static void releaseVertexShaders(ID3D11VertexShader* vs, ID3D11InputLayout* il);


	static bool loadPixelShaders(ID3D11Device* device, const char* filename, ID3D11PixelShader** ps);
	static void releasePixelShaders(ID3D11PixelShader* ps);

	static bool loadGeometryShaders(ID3D11Device* device, const char* filename, ID3D11GeometryShader** gs);
	static void releaseGeometryShaders(ID3D11GeometryShader* gs);

	static bool loadHullShaders(ID3D11Device* device, const char* filename, ID3D11HullShader** hs);
	static void releaseHullShaders(ID3D11HullShader* hs);

	static bool loadDomainShaders(ID3D11Device* device, const char* filename, ID3D11DomainShader** ds);
	static void releaseDomainShaders(ID3D11DomainShader* ds);

	static bool loadComputeShaders(ID3D11Device* device, const char* filename, ID3D11ComputeShader** cs);
	static void releaseComputeShaders(ID3D11ComputeShader* cs);
};

